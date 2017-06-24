#ifndef STREAMCHANNELBASEIMPL_H
#define STREAMCHANNELBASEIMPL_H

#include "StreamChannelBase.h"

template<typename StreamTraits, const char *LoggerName> log4cplus::Logger StreamChannelBase<StreamTraits, LoggerName>::log 
	= log4cplus::Logger::getInstance(LoggerName);

template<typename StreamTraits, const char *LoggerName> StreamChannelBase<StreamTraits, LoggerName>::StreamChannelBase(
	const std::shared_ptr<typename StreamTraits::StreamType> &stream) :std::enable_shared_from_this<StreamChannelBase<StreamTraits, LoggerName>>(), m_stream(stream)
	, m_lock(), m_rdReq(), m_wrReqs()
{
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::AsyncReadSome(
	BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::weak_ptr_t &handler, void *ctx)
{
	std::vector<boost::asio::mutable_buffer> bufSeq;
	bufSeq.reserve(bufSize);
	for (size_t i = 0; i < bufSize; ++i)
	{
		bufSeq.push_back(boost::asio::buffer(bufs[i].m_beg, bufs[i].m_size));
	}
	SpinLock<>::ScopeLock lock(m_lock);
	if(!m_rdReq.m_handler.expired())
	{
		assert(false);
		return;
	}
	m_rdReq.m_handler = handler;
	m_rdReq.m_ctx = ctx;
	m_stream->async_read_some(std::move(bufSeq)
		, std::bind(&StreamChannelBase<StreamTraits, LoggerName>::EndRead, StreamChannelBase<StreamTraits, LoggerName>::shared_from_this()
			, std::placeholders::_1, std::placeholders::_2));
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::AsyncWrite(const std::shared_ptr<LinearBuffer> &buf
	, const IAsyncChannelHandler::weak_ptr_t &handler, void *ctx)
{
	SpinLock<>::ScopeLock lock(m_lock);
	m_wrReqs.push({ handler,buf,ctx });
	if (m_wrReqs.size() == 1)
	{
		boost::asio::async_write(*m_stream, boost::asio::buffer(buf->data(), buf->size()), std::bind(&StreamChannelBase<StreamTraits, LoggerName>::EndWrite
			, StreamChannelBase<StreamTraits, LoggerName>::shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::EndRead(const boost::system::error_code &err
	, std::size_t bytesTransferred)
{
	assert(!m_rdReq.m_handler.expired());
	RdReq req;
	{
		SpinLock<>::ScopeLock lock(m_lock);
		req = m_rdReq;
		m_rdReq.m_handler.reset();
	}
	IAsyncChannelHandler::ptr_t handler(req.m_handler.lock());
	if (handler)
	{
		handler->EndRead(err, bytesTransferred, req.m_ctx);
	}
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::EndWrite(const boost::system::error_code &err
	, std::size_t bytesTransferred)
{
	WrReq req;
	assert(m_wrReqs.size());
	{
		SpinLock<>::ScopeLock lock(m_lock);
		req = std::move(m_wrReqs.front());
		m_wrReqs.pop();
		if (m_wrReqs.size())
		{
			WrReq &currentReq = m_wrReqs.front();
			boost::asio::async_write(*m_stream, boost::asio::buffer(currentReq.m_buf->data(), currentReq.m_buf->size())
				, std::bind(&StreamChannelBase<StreamTraits, LoggerName>::EndWrite, StreamChannelBase<StreamTraits, LoggerName>::shared_from_this()
					, std::placeholders::_1, std::placeholders::_2));
		}
	}
	IAsyncChannelHandler::ptr_t handler(req.m_handler.lock());
	if (handler)
	{
		handler->EndWrite(err, bytesTransferred, req.m_ctx);
	}
}

#endif /* STREAMCHANNELBASEIMPL_H */