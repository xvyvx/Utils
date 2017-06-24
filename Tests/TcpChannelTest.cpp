#include <boost/test/unit_test.hpp>
#include "Concurrent/ThreadPool.h"
#include "Channel/Tcp/TcpV4Listener.h"
#include "Channel/Tcp/TcpV4Channel.h"
#include "Channel/Tcp/TcpV4PassiveChannel.h"
#include "Buffer/CircularBuffer.h"

SpinLock<> GlobalAssertLock;

BOOST_AUTO_TEST_SUITE(TcpChannelTest)

	class MonkHandler :public std::enable_shared_from_this<MonkHandler>, public IAsyncChannelHandler
	{
	public:
		IAsyncChannel *m_channel = nullptr;

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
				std::shared_ptr<LinearBuffer> buf(new LinearBuffer(512));
				buf->assign(10, 10);
				IAsyncChannelHandler::weak_ptr_t self(shared_from_this());
				m_channel->AsyncWrite(buf, self);
			}
		}

		virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
		{
			SpinLock<>::ScopeLock lock(GlobalAssertLock);
			BOOST_TEST(!err, "MonkHandler EndRead called,message:" << err.message());
			if (!err)
			{
				m_readBuf->inc_size(bytesTransferred);
				for (size_t i = 0; i < m_readBuf->size(); ++i)
				{
					bool flag = (*m_readBuf)[i] == 10;
					BOOST_TEST(flag, "MonkHandler received buf content, index:" << i);
					if (!flag)
					{
						break;
					}
				}
				IAsyncChannelHandler::weak_ptr_t self(shared_from_this());
				m_channel->AsyncClose(self);
			}
		}

		virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
		{
			SpinLock<>::ScopeLock lock(GlobalAssertLock);
			BOOST_TEST(!err, "MonkHandler EndWrite called,message:" << err.message());
			if (!err)
			{
				BufDescriptor bufs[2];
				size_t bufSize = m_readBuf->free_buffers(bufs);
				IAsyncChannelHandler::weak_ptr_t self(shared_from_this());
				m_channel->AsyncReadSome(bufs, bufSize, self);
			}
		}

		virtual void EndClose(const boost::system::error_code &err) override
		{
			SpinLock<>::ScopeLock lock(GlobalAssertLock);
			BOOST_TEST(!err, "MonkHandler EndClose called,message:" << err.message());
		}
	};

class MonkSrvHandler :public std::enable_shared_from_this<MonkSrvHandler>, public IAsyncChannelHandler
{
public:
	static std::shared_ptr<MonkSrvHandler> TestSession;

	IAsyncChannel::ptr_t m_channel;

	std::shared_ptr<CircularBuffer> m_readBuf{ new CircularBuffer(512) };

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
			IAsyncChannelHandler::weak_ptr_t self(shared_from_this());
			m_channel->AsyncReadSome(bufs, bufSize, self);
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
				std::shared_ptr<LinearBuffer> buf(new LinearBuffer(512));
				buf->assign(10, 10);
				IAsyncChannelHandler::weak_ptr_t self(shared_from_this());
				m_channel->AsyncWrite(buf, self);
			}
		}
	}

	virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
	{
		SpinLock<>::ScopeLock lock(GlobalAssertLock);
		BOOST_TEST(!err, "MonkSrvHandler EndWrite called,message:" << err.message() << "\n");
		if (!err)
		{
			IAsyncChannelHandler::weak_ptr_t self(shared_from_this());
			m_channel->AsyncClose(self);
		}
	}

	virtual void EndClose(const boost::system::error_code &err) override
	{
		SpinLock<>::ScopeLock lock(GlobalAssertLock);
		BOOST_TEST(!err, "MonkSrvHandler EndClose called,message:" << err.message() << "\n");
	}
};

std::shared_ptr<MonkSrvHandler> MonkSrvHandler::TestSession;

void TestAcceptFunc(std::shared_ptr<boost::asio::ip::tcp::endpoint> &remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::socket> &sock)
{
	IAsyncChannel::ptr_t channel(new TcpV4PassiveChannel(sock, *remoteEndPoint));
	std::shared_ptr<MonkSrvHandler> handler(new MonkSrvHandler());
	MonkSrvHandler::TestSession = handler;
	handler->m_channel = channel;
	channel->AsyncOpen(handler);
}

	void Do()
	{
	}

BOOST_AUTO_TEST_CASE(GeneralTest)
{
	ThreadPool::Instance();
	SetListenerAcceptFunc(TestAcceptFunc);
	TcpV4Listener::Instance().AddListenEndPoints(
		{
			{ boost::asio::ip::address_v4::from_string("0.0.0.0"),8001 },
			{ boost::asio::ip::address_v4::from_string("0.0.0.0"),8002 }
		});

	ThreadPool::Instance().QueueWorkItem(Do);
	IAsyncChannel::ptr_t channel(new TcpV4Channel("127.0.0.1", 8001));
	IAsyncChannelHandler::ptr_t monkObj(new MonkHandler());
	static_cast<MonkHandler*>(monkObj.get())->m_channel = channel.get();
	channel->AsyncOpen(monkObj);
	boost::this_thread::sleep(boost::posix_time::seconds(5));

	MonkSrvHandler::TestSession.reset();
	channel.reset();
	ThreadPool::Stop();
	TcpV4Listener::Destory();
	ThreadPool::Destory();
}

BOOST_AUTO_TEST_SUITE_END()