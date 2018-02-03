#include <boost/test/unit_test.hpp>
#include "Concurrent/ThreadPool.h"
#include "Channel/Tcp/TcpV4Listener.h"
#include "Channel/Tcp/TcpV4Channel.h"
#include "Channel/Tcp/TcpV4PassiveChannel.h"
#include "Buffer/CircularBuffer.h"
#include "Concurrent/TaskBarrier.h"

SpinLock<> GlobalAssertLock;

TaskBarrier<> GlobalBarrier(0);

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
					m_channel->AsyncWrite(buf, shared_from_this());
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
				std::shared_ptr<LinearBuffer> buf(new LinearBuffer(512));
				buf->assign(bytesTransferred, 10);
				m_channel->AsyncWrite(buf, shared_from_this());
			}
		}
	}

	virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
	{
		SpinLock<>::ScopeLock lock(GlobalAssertLock);
		BOOST_TEST(!err, "MonkSrvHandler EndWrite called,message:" << err.message() << "\n");
		if (!err)
		{
			m_sendSize += bytesTransferred;
			if (m_sendSize == 100)
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

	virtual void EndClose(const boost::system::error_code &err) override
	{
		SpinLock<>::ScopeLock lock(GlobalAssertLock);
		GlobalBarrier.IncFinishedCount(1);
		BOOST_TEST(!err, "MonkSrvHandler EndClose called,message:" << err.message() << "\n");
	}
};

void TestAcceptFunc(std::shared_ptr<boost::asio::ip::tcp::endpoint> &remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::socket> &sock)
{
	IAsyncChannel::ptr_t channel(new TcpV4PassiveChannel(sock, *remoteEndPoint));
	std::shared_ptr<MonkSrvHandler> handler(new MonkSrvHandler());
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

	GlobalBarrier.ResetTaskCount(2);
	GlobalBarrier.Reset();
	QueueThreadPoolWorkItem(Do);
	{
		std::shared_ptr<MonkHandler> client(new MonkHandler());
		client->m_channel.reset(new TcpV4Channel("127.0.0.1", 8001));
		client->m_channel->AsyncOpen(client);
	}
	GlobalBarrier.WaitAllFinished();

	ThreadPool::Instance().Stop();
	TcpV4Listener::Destory();
	ThreadPool::Destory();
}

BOOST_AUTO_TEST_SUITE_END()