#if defined(linux) || defined(__linux)

#if defined(linux) || defined(__linux)
#include <stdlib.h>
#endif
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include "Concurrent/ThreadPool.h"
#include "Concurrent/WaitEvent.h"
#include "Concurrent/TaskBarrier.h"
#include "Channel/SerialPort/SerialPortChannel.h"

BOOST_AUTO_TEST_SUITE(SerialPortChannelTest)

WaitEvent GlobalSerialPortWaitEvent;

TaskBarrier<> GlobalSerialPortBarrier(2);

class Port1Handler :public std::enable_shared_from_this<Port1Handler>, public IAsyncChannelHandler
{
public:
	IAsyncChannel *m_channel;

	std::shared_ptr<LinearBuffer> m_sendBuf{ new LinearBuffer(12) };

	us8 m_readBuf[12];

	virtual void EndOpen(const boost::system::error_code &err) override
	{
		BOOST_TEST(!err, "Open port1 failed,message:" << err.message());
		GlobalSerialPortWaitEvent.Wait();
		m_sendBuf->assign({ 1,2,3,4,5,6 });
		m_channel->AsyncWrite(m_sendBuf, shared_from_this());
		BufDescriptor readBuf = { m_readBuf,6 };
		m_channel->AsyncReadSome(&readBuf, 1, shared_from_this());
		GlobalSerialPortWaitEvent.Signal();
	}

	virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
	{
		BOOST_TEST(!err, "Read port1 failed,message:" << err.message());
		for (int i = 0; i < 6; ++i)
		{
			BOOST_TEST(m_readBuf[i] == i + 1, "Port1 read content error.");
		}
		m_channel->AsyncClose(shared_from_this());
	}

	virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
	{
		BOOST_TEST(!err, "Write port1 failed,message:" << err.message());
	}

	virtual void EndClose(const boost::system::error_code &err) override
	{
		BOOST_TEST(!err, "Close port1 failed,message:" << err.message());
		GlobalSerialPortBarrier.IncFinishedCount(1);
	}
};

class Port2Handler :public std::enable_shared_from_this<Port2Handler>, public IAsyncChannelHandler
{
public:
	IAsyncChannel * m_channel;

	std::shared_ptr<LinearBuffer> m_sendBuf{ new LinearBuffer(12) };

	us8 m_readBuf[12];

	virtual void EndOpen(const boost::system::error_code &err) override
	{
		BOOST_TEST(!err, "Open port2 failed,message:" << err.message());
		BufDescriptor readBuf = { m_readBuf,6 };
		m_channel->AsyncReadSome(&readBuf, 1, shared_from_this());
		GlobalSerialPortWaitEvent.Signal();
	}

	virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
	{
		BOOST_TEST(!err, "Read port2 failed,message:" << err.message());
		for (int i = 0; i < 6; ++i)
		{
			BOOST_TEST(m_readBuf[i] == i + 1, "Port2 read content error.");
		}
		GlobalSerialPortWaitEvent.Wait();
		m_sendBuf->assign({ 1,2,3,4,5,6 });
		m_channel->AsyncWrite(m_sendBuf, shared_from_this());
	}

	virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) override
	{
		BOOST_TEST(!err, "Write port2 failed,message:" << err.message());
		GlobalSerialPortBarrier.IncFinishedCount(1);
	}

	virtual void EndClose(const boost::system::error_code &err) override
	{
	}
};

BOOST_AUTO_TEST_CASE(GeneralTest)
{
#if defined(linux) || defined(__linux)
	int exitCode = system("socat -d -d pty,raw,nonblock,echo=0,link=./ttyV1 pty,raw,nonblock,echo=0,link=./ttyV2 &");
	BOOST_TEST((WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0));
	while (!boost::filesystem::exists("./ttyV1") || !boost::filesystem::exists("./ttyV2"))
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));
	}
#endif
	ThreadPool::Instance();
	SerialPortSettings setting1 = 
	{
#if defined(IS_WINDOWS)
	"COM1",
#elif defined(linux) || defined(__linux)
	"./ttyV1",
#else
	#error Unsupportted platform.
#endif
		9600,
		boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none),
		boost::asio::serial_port::parity(boost::asio::serial_port::parity::none),
		boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one),
		8
	};
	SerialPortSettings setting2 = setting1;
#if defined(IS_WINDOWS)
	setting2.m_deviceName = "COM2";
#elif defined(linux) || defined(__linux)
	setting2.m_deviceName = "./ttyV2";
#else
	#error Unsupportted platform.
#endif
	SerialPortChannel::ptr_t port1(new SerialPortChannel(setting1));
	Port1Handler::ptr_t handler1(new Port1Handler());
	static_cast<Port1Handler*>(handler1.get())->m_channel = port1.get();
	SerialPortChannel::ptr_t port2(new SerialPortChannel(setting2));
	Port2Handler::ptr_t handler2(new Port2Handler());
	static_cast<Port2Handler*>(handler2.get())->m_channel = port2.get();
	port1->AsyncOpen(handler1);
	port2->AsyncOpen(handler2);
	GlobalSerialPortBarrier.WaitAllFinished();

#if defined(linux) || defined(__linux)
	exitCode = system("killall socat");
	BOOST_TEST((WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0));
#endif

	ThreadPool::Instance().Stop();
	ThreadPool::Destory();
}

BOOST_AUTO_TEST_SUITE_END()

#endif