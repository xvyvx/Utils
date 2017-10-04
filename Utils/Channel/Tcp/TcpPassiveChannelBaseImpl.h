#ifndef TCPPASSIVECHANNELBASEIMPL_H
#define TCPPASSIVECHANNELBASEIMPL_H

#include "TcpPassiveChannelBase.h"
#include "../../Concurrent/ThreadPool.h"
#include "../Common/StreamChannelBaseImpl.h"

template<typename ProtocolTraits, const char *LoggerName> TcpPassiveChannelBase<ProtocolTraits, LoggerName>::TcpPassiveChannelBase(
	std::shared_ptr<typename ProtocolTraits::StreamType> &stream, const boost::asio::ip::tcp::endpoint &remoteEndPoint):
	StreamChannelBase<ProtocolTraits, LoggerName>(stream), m_remoteEndPoint(remoteEndPoint), m_closed(false)
{
}

template<typename ProtocolTraits, const char *LoggerName> TcpPassiveChannelBase<ProtocolTraits, LoggerName>::~TcpPassiveChannelBase()
{
	if (!m_closed)
	{
		boost::system::error_code shutdownErr, closeErr;
		Close(shutdownErr, closeErr);
	}
}

template<typename ProtocolTraits, const char *LoggerName> void TcpPassiveChannelBase<ProtocolTraits, LoggerName>::AsyncOpen(
	const IAsyncChannelHandler::ptr_t &handler)
{
	boost::system::error_code err;
	ThreadPool::Instance().QueueWorkItem(std::bind(&IAsyncChannelHandler::EndOpen, handler, err));
}

template<typename ProtocolTraits, const char *LoggerName> void TcpPassiveChannelBase<ProtocolTraits, LoggerName>::AsyncClose(
	const IAsyncChannelHandler::ptr_t &handler)
{
	SpinLock<>::ScopeLock lock(BaseType::m_lock);
	boost::system::error_code shutdownErr, closeErr;
	Close(shutdownErr, closeErr);
	ThreadPool::Instance().QueueWorkItem(std::bind(&IAsyncChannelHandler::EndClose, handler, shutdownErr ? shutdownErr : closeErr));
}

template<typename ProtocolTraits, const char *LoggerName> void TcpPassiveChannelBase<ProtocolTraits, LoggerName>::Close(boost::system::error_code &shutdownErr, boost::system::error_code &closeErr)
{
	if (!m_closed)
	{
		BaseType::m_stream->shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdownErr);
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

#endif /* TCPPASSIVECHANNELBASEIMPL_H */