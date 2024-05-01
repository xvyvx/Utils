#include <boost/test/unit_test.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include "Concurrent/ThreadPool.h"
#include "Channel/Tcp/TcpV4Listener.h"
#include "Channel/Tcp/TcpV4Channel.h"
#include "Channel/Tcp/TcpV4PassiveChannel.h"
#include "Buffer/CircularBuffer.h"
#include "Concurrent/TaskBarrier.h"
#include "Concurrent/WaitEvent.h"

class TestLogFixture
{
public:
    TestLogFixture()
    {
        ::log4cplus::helpers::SharedObjectPtr<::log4cplus::Appender> append(new log4cplus::ConsoleAppender());
        append->setName("console");
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored  "-Wdeprecated-declarations"
#endif
        std::auto_ptr<::log4cplus::Layout> layout(new ::log4cplus::PatternLayout("%D %c[%t] %-5p %m%n"));
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
        append->setLayout(layout);
        ::log4cplus::Logger logger = ::log4cplus::Logger::getRoot();
        logger.addAppender(append);
        logger.setLogLevel(::log4cplus::ERROR_LOG_LEVEL);
    }

    ~TestLogFixture()
    {
    }
};

BOOST_TEST_GLOBAL_FIXTURE(TestLogFixture);

SpinLock<> GlobalAssertLock;

TaskBarrier<> GlobalBarrier(0);

WaitEvent GlobalWaitEvent;

BOOST_AUTO_TEST_SUITE(TcpChannelTest)

class MonkHandler :public std::enable_shared_from_this<MonkHandler>, public IAsyncChannelHandler
{
public:
    IAsyncChannel::ptr_t m_channel;

    std::shared_ptr<CircularBuffer> m_readBuf{ new CircularBuffer(512) };

    ~MonkHandler()
    {
    }

    virtual void EndOpen(const boost::system::error_code &err) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        BOOST_TEST(!err, "MonkHandler EndOpen called,message:" << err.message());
        if(!err)
        {
            for (int i = 0; i < 10; ++i)
            {
                std::shared_ptr<LinearBuffer> buf(new LinearBuffer(512));
                buf->assign(10, 10);
                m_channel->AsyncWrite(buf, 0, buf->size(), shared_from_this());
            }
            BufDescriptor bufs[2];
            size_t bufSize = m_readBuf->free_buffers(bufs);
            m_channel->AsyncReadSome(bufs, bufSize, shared_from_this());
        }
    }

    virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        BOOST_TEST(!err, "MonkHandler EndRead called,message:" << err.message());
        if (!err)
        {
            m_readBuf->inc_size(bytesTransferred);
            bool flag = false;
            for (size_t i = 0; i < m_readBuf->size(); ++i)
            {
                flag = (*m_readBuf)[i] == 10;
                BOOST_TEST(flag, "MonkHandler received buf content, index:" << i);
                if (!flag)
                {
                    break;
                }
            }
            if (!flag || m_readBuf->size() == 100)
            {
                m_channel->AsyncClose(shared_from_this());
            }
            else
            {
                BufDescriptor bufs[2];
                size_t bufSize = m_readBuf->free_buffers(bufs);
                m_channel->AsyncReadSome(bufs, bufSize, shared_from_this());
            }
        }
    }

    virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        BOOST_TEST(!err, "MonkHandler EndWrite called,message:" << err.message());
    }

    virtual void EndClose(const boost::system::error_code &err) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        GlobalBarrier.IncFinishedCount(1);
        BOOST_TEST(!err, "MonkHandler EndClose called,message:" << err.message());
    }
};

class MonkSrvHandler :public std::enable_shared_from_this<MonkSrvHandler>, public IAsyncChannelHandler
{
public:
    IAsyncChannel::ptr_t m_channel;

    std::shared_ptr<CircularBuffer> m_readBuf{ new CircularBuffer(512) };

    size_t m_sendSize{ 0 };

    ~MonkSrvHandler()
    {
    }

    virtual void EndOpen(const boost::system::error_code &err) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        BOOST_TEST(!err, "MonkSrvHandler EndOpen called,message:" << err.message());
        if (!err)
        {
            BufDescriptor bufs[2];
            size_t bufSize = m_readBuf->free_buffers(bufs);
            m_channel->AsyncReadSome(bufs, bufSize, shared_from_this());
        }
    }

    virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        bool flag = false;
        BOOST_TEST(!err, "MonkSrvHandler EndRead called,message:" << err.message());
        if (!err)
        {
            m_readBuf->inc_size(bytesTransferred);
            for (size_t i = 0; i < m_readBuf->size(); ++i)
            {
                flag = (*m_readBuf)[i] == 10;
                BOOST_TEST(flag, "MonkSrvHandler received buf content, index:" << i);
                if (!flag)
                {
                    break;
                }
            }
            if (flag)
            {
                if(m_readBuf->size() == 100)
                {
                    std::shared_ptr<LinearBuffer> buf(new LinearBuffer(512));
                    buf->assign(m_readBuf->size(), 10);
                    m_channel->AsyncWrite(buf, 0, buf->size(), shared_from_this());
                }
                else
                {
                    BufDescriptor bufs[2];
                    size_t bufSize = m_readBuf->free_buffers(bufs);
                    m_channel->AsyncReadSome(bufs, bufSize, shared_from_this());
                }
            }
        }
    }

    virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        BOOST_TEST(!err, "MonkSrvHandler EndWrite called,message:" << err.message() << "\n");
        m_channel->AsyncClose(shared_from_this());
    }

    virtual void EndClose(const boost::system::error_code &err) override
    {
        SpinLock<>::ScopeLock lock(GlobalAssertLock);
        GlobalBarrier.IncFinishedCount(1);
        BOOST_TEST(!err, "MonkSrvHandler EndClose called,message:" << err.message() << "\n");
    }
};

void TestAcceptFunc(std::shared_ptr<boost::asio::ip::tcp::endpoint> &remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::socket> &sock)
{
    if (remoteEndPoint->port() != 8003)
    {
        IAsyncChannel::ptr_t channel(new TcpV4PassiveChannel(sock, *remoteEndPoint));
        std::shared_ptr<MonkSrvHandler> handler(new MonkSrvHandler());
        handler->m_channel = channel;
        channel->AsyncOpen(handler);
    }
    else
    {
        IAsyncChannel::ptr_t channel(new TcpV4PassiveChannel(sock, *remoteEndPoint));
    }
}

void Do()
{
}

class NullHandler :public IAsyncChannelHandler
{
public:
    virtual void EndOpen(const boost::system::error_code &err) override
    {
        GlobalWaitEvent.Signal();
    }

    virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
    }

    virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
    }

    virtual void EndClose(const boost::system::error_code &err) override
    {
    }
};

class BindErrorCallbackHandler :public IAsyncChannelHandler
{
public:
    virtual void EndOpen(const boost::system::error_code &err) override
    {
        BOOST_TEST(err, "Bind error not happened.");
        GlobalWaitEvent.Signal();
    }

    virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
    }

    virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
    {
    }

    virtual void EndClose(const boost::system::error_code &err) override
    {
    }
};

BOOST_AUTO_TEST_CASE(GeneralTest)
{
    ThreadPool::Instance();
    SetListenerAcceptFunc(TestAcceptFunc);
    BOOST_TEST(TcpV4Listener::Instance().AddListenEndPoints(
        {
            { boost::asio::ip::address_v4::from_string("127.0.0.1"),8001 },
            { boost::asio::ip::address_v4::from_string("127.0.0.1"),8002 }
        }), "Add end point error.");
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string("0.0.0.0"), 8009);
    BOOST_TEST(TcpV4Listener::Instance().AddListenEndPoint(ep), "Add test end point error.");
    TcpV4Listener::Instance().RemoveListenerEndPoint(ep);
    BOOST_TEST(!TcpV4Listener::Instance().AddListenEndPoints(
        {
            { boost::asio::ip::address_v4::from_string("127.0.0.1"),9001 },
            { boost::asio::ip::address_v4::from_string("127.0.0.1"),8002 }
        }), "Add same end point error not happened.");

    GlobalBarrier.ResetTaskCount(2);
    GlobalBarrier.Reset();
    QueueThreadPoolWorkItem(Do);
    {
        std::shared_ptr<MonkHandler> client(new MonkHandler());
        client->m_channel.reset(new TcpV4Channel("127.0.0.1", 8001));
        client->m_channel->AsyncOpen(client);
    }
    GlobalBarrier.WaitAllFinished();

    BOOST_TEST(GlobalWaitEvent);
    {
        TcpV4Channel channel("127.0.0.1", 8001, false, 8003, "127.0.0.1");
        IAsyncChannelHandler::ptr_t handler(new NullHandler());
        channel.AsyncOpen(handler);
        GlobalWaitEvent.Wait();
        TcpV4Channel errChannel("127.0.0.1", 8001, false, 8003, "127.0.0.1");
        IAsyncChannelHandler::ptr_t errHandler(new BindErrorCallbackHandler());
        channel.AsyncOpen(handler);
        GlobalWaitEvent.Wait();
    }

    ThreadPool::Instance().Stop();
    TcpV4Listener::Destory();
    ThreadPool::Destory();
}

BOOST_AUTO_TEST_SUITE_END()