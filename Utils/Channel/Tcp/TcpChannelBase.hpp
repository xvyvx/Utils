#ifndef TCPCHANNELBASEIMPL_H
#define TCPCHANNELBASEIMPL_H

#include "TcpChannelBase.h"
#include "../Common/StreamChannelBase.hpp"
#include "../../Concurrent/ThreadPool.h"

template <typename ProtocolTraits, const char *LoggerName>
TcpChannelBase<ProtocolTraits, LoggerName>::TcpChannelBase(const std::string &remoteAddr, us16 remotePort, bool openOnConstruct, us16 localPort, const std::string &localAddr)
    : TcpChannelBase(ProtocolTraits::AddressFromString(remoteAddr), remotePort, openOnConstruct, localPort, ProtocolTraits::AddressFromString(localAddr))
{
}

template <typename ProtocolTraits, const char *LoggerName>
TcpChannelBase<ProtocolTraits, LoggerName>::TcpChannelBase(const typename ProtocolTraits::AddressType &remoteAddr, us16 remotePort, bool openOnConstruct, us16 localPort
    , const typename ProtocolTraits::AddressType &localAddr): StreamChannelBase<ProtocolTraits, LoggerName>(
    std::make_shared<typename ProtocolTraits::StreamType>(ThreadPool::Instance().Context())), m_localEndPoint(localAddr, localPort), m_remoteEndPoint(remoteAddr, remotePort)
    , m_closed(true)
{
    if (openOnConstruct)
    {
        boost::system::error_code error;
        Open(error);
    }
}

template <typename ProtocolTraits, const char *LoggerName>
TcpChannelBase<ProtocolTraits, LoggerName>::~TcpChannelBase()
{
    if (!m_closed)
    {
        boost::system::error_code shutdownErr, closeErr;
        Close(shutdownErr, closeErr);
    }
}

template <typename ProtocolTraits, const char *LoggerName>
void TcpChannelBase<ProtocolTraits, LoggerName>::AsyncOpen(const IAsyncChannelHandler::ptr_t &handler)
{
    SpinLock<>::ScopeLock lock(BaseType::m_lock);
    if (!BaseType::m_stream->is_open())
    {
        boost::system::error_code err;
        Open(err);
        if (err)
        {
            QueueThreadPoolWorkItem([handler = handler, err = err]() { handler->EndOpen(err); });
        }
        else
        {
            BaseType::m_stream->async_connect(m_remoteEndPoint, [handler = handler](const boost::system::error_code &err) { handler->EndOpen(err); });
        }
    }
    else
    {
        BaseType::m_stream->async_connect(m_remoteEndPoint, [handler = handler](const boost::system::error_code &err) { handler->EndOpen(err); });
    }
}

template <typename ProtocolTraits, const char *LoggerName>
void TcpChannelBase<ProtocolTraits, LoggerName>::AsyncClose(
    const IAsyncChannelHandler::ptr_t &handler)
{
    SpinLock<>::ScopeLock lock(BaseType::m_lock);
    boost::system::error_code shutdownErr, closeErr;
    Close(shutdownErr, closeErr);
    QueueThreadPoolWorkItem([handler = handler, err = shutdownErr ? shutdownErr : closeErr]() { handler->EndClose(err); });
}

template <typename ProtocolTraits, const char *LoggerName>
void TcpChannelBase<ProtocolTraits, LoggerName>::Open(boost::system::error_code &error)
{
    StreamChannelBase<ProtocolTraits, LoggerName>::m_stream->open(ProtocolTraits::Protocol(), error);
    if (error)
    {
        LOG4CPLUS_ERROR_FMT(BaseType::log, "打开Tcp通道错误：%s", error.message().c_str());
    }
    else
    {
        if (m_localEndPoint.port() != 0)
        {
            StreamChannelBase<ProtocolTraits, LoggerName>::m_stream->bind(m_localEndPoint, error);
            if (error)
            {
                boost::system::error_code closeErr;
                StreamChannelBase<ProtocolTraits, LoggerName>::m_stream->close(closeErr);
                m_closed = true;
                LOG4CPLUS_ERROR_FMT(BaseType::log, "绑定Tcp通道错误：%s", error.message().c_str());
            }
            else
            {
                m_closed = false;
            }
        }
        else
        {
            m_closed = false;
        }
    }
}

template <typename ProtocolTraits, const char *LoggerName>
void TcpChannelBase<ProtocolTraits, LoggerName>::Close(boost::system::error_code &shutdownErr, boost::system::error_code &closeErr)
{
    if (!m_closed)
    {
        BaseType::m_stream->shutdown(boost::asio::socket_base::shutdown_both, shutdownErr);
        if (shutdownErr)
        {
            LOG4CPLUS_ERROR_FMT(BaseType::log, "停止Tcp通道错误：%s", shutdownErr.message().c_str());
        }
        BaseType::m_stream->close(closeErr);
        if (closeErr)
        {
            LOG4CPLUS_ERROR_FMT(BaseType::log, "关闭Tcp通道错误：%s", closeErr.message().c_str());
        }
        m_closed = true;
    }
}

#endif /* TCPCHANNELBASEIMPL_H */