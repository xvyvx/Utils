#include "SerialPortChannel.h"
#include "../../Concurrent/ThreadPool.h"
#include "../Common/StreamChannelBase.hpp"

const char SerialPortChannelLoggerName[] = "SerialPortChannel";

template class UTILS_DEF_API StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>;

SerialPortChannel::SerialPortChannel(const SerialPortSettings &settings)
    : StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>(std::make_shared<SerialPortTraits::StreamType>(ThreadPool::Instance().Context()))
    , m_settings(settings), m_closed(true)
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
    if (!BaseType::m_stream->open(m_settings.m_deviceName, err))
    {
        m_closed = false;
        SerialPortTraits::StreamType::baud_rate rate(m_settings.m_baudRate);
        SerialPortTraits::StreamType::character_size characterSize(m_settings.m_characterSize);
        if (BaseType::m_stream->set_option(rate, err)
            || BaseType::m_stream->set_option(m_settings.m_flowCtrl, err)
            || BaseType::m_stream->set_option(m_settings.m_parity, err)
            || BaseType::m_stream->set_option(m_settings.m_stopBits, err)
            || BaseType::m_stream->set_option(characterSize, err))
        {
            LOG4CPLUS_ERROR_FMT(BaseType::log, "设置SerialPort通道通讯参数错误：%s", err.message().c_str());
        }
    }
    else
    {
        LOG4CPLUS_ERROR_FMT(BaseType::log, "打开SerialPort通道错误：%s", err.message().c_str());
    }
    QueueThreadPoolWorkItem([handler = handler, err = err]() mutable { handler->EndOpen(err); });
}

void SerialPortChannel::AsyncClose(const IAsyncChannelHandler::ptr_t &handler)
{
    SpinLock<>::ScopeLock lock(BaseType::m_lock);
    boost::system::error_code err;
    Close(err);
    QueueThreadPoolWorkItem([handler = handler, err = err]() mutable { handler->EndClose(err); });
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