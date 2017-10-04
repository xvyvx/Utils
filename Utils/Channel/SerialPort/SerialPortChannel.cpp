#include "SerialPortChannel.h"
#include <functional>
#include "../../Concurrent/ThreadPool.h"
#include "../Common/StreamChannelBaseImpl.h"

const char SerialPortChannelLoggerName[] = "SerialPortChannel";

template class UTILS_DEF_API StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>;

SerialPortChannel::SerialPortChannel(const SerialPortSettings &settings):
	StreamChannelBase<SerialPortTraits,SerialPortChannelLoggerName>(std::make_shared<SerialPortTraits::StreamType>(ThreadPool::Instance().Service())), m_settings(settings),
	m_closed(false)
{
}

SerialPortChannel::~SerialPortChannel()
{
	if (!m_closed)
	{
		boost::system::error_code err;
		Close(err);
	}
}

void SerialPortChannel::AsyncOpen(const IAsyncChannelHandler::ptr_t &handler)
{
	SpinLock<>::ScopeLock lock(BaseType::m_lock);
	boost::system::error_code err;
	BaseType::m_stream->open(m_settings.m_deviceName, err);
	if(!err)
	{
		SerialPortTraits::StreamType::baud_rate rate(m_settings.m_baudRate);
		BaseType::m_stream->set_option(rate, err);
		if(!err)
		{
			BaseType::m_stream->set_option(m_settings.m_flowCtrl, err);
			if (!err)
			{
				BaseType::m_stream->set_option(m_settings.m_parity, err);
				if (!err)
				{
					BaseType::m_stream->set_option(m_settings.m_stopBits, err);
					if (!err)
					{
						SerialPortTraits::StreamType::character_size characterSize(m_settings.m_characterSize);
						BaseType::m_stream->set_option(characterSize, err);
						if(err)
						{
							LOG4CPLUS_ERROR_FMT(BaseType::log, "设置SerialPort通道数据位错误：%s", err.message().c_str());
						}
					}
					else
					{
						LOG4CPLUS_ERROR_FMT(BaseType::log, "设置SerialPort通道停止位错误：%s", err.message().c_str());
					}
				}
				else
				{
					LOG4CPLUS_ERROR_FMT(BaseType::log, "设置SerialPort通道效验位错误：%s", err.message().c_str());
				}
			}
			else
			{
				LOG4CPLUS_ERROR_FMT(BaseType::log, "设置SerialPort通道流量控制错误：%s", err.message().c_str());
			}
		}
		else
		{
			LOG4CPLUS_ERROR_FMT(BaseType::log, "设置SerialPort通道波特率错误：%s", err.message().c_str());
		}
	}
	else
	{
		LOG4CPLUS_ERROR_FMT(BaseType::log, "打开SerialPort通道错误：%s", err.message().c_str());
	}
	ThreadPool::Instance().QueueWorkItem(std::bind(&IAsyncChannelHandler::EndOpen, handler, err));
}

void SerialPortChannel::AsyncClose(const IAsyncChannelHandler::ptr_t &handler)
{
	SpinLock<>::ScopeLock lock(BaseType::m_lock);
	boost::system::error_code err;
	Close(err);
	ThreadPool::Instance().QueueWorkItem(std::bind(&IAsyncChannelHandler::EndClose, handler, err));
}

void SerialPortChannel::Close(boost::system::error_code &error)
{
	if (!m_closed)
	{
		BaseType::m_stream->close(error);
		if (error)
		{
			LOG4CPLUS_ERROR_FMT(BaseType::log, "关闭SerialPort通道错误：%s", error.message().c_str());
		}
		m_closed = true;
	}
}