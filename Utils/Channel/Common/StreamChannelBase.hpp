#ifndef STREAMCHANNELBASEIMPL_H
#define STREAMCHANNELBASEIMPL_H

#include "StreamChannelBase.h"
#include <boost/asio.hpp>

template<typename StreamTraits, const char *LoggerName> log4cplus::Logger StreamChannelBase<StreamTraits, LoggerName>::log 
    = log4cplus::Logger::getInstance(LoggerName);

template<typename StreamTraits, const char *LoggerName> StreamChannelBase<StreamTraits, LoggerName>::StreamChannelBase(
    const std::shared_ptr<typename StreamTraits::StreamType> &stream) :std::enable_shared_from_this<StreamChannelBase<StreamTraits, LoggerName>>(), m_stream(stream)
    , m_lock(), m_lastWrReq(nullptr)
{
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::AsyncReadSome(
    BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::ptr_t &handler, void *ctx)
{
    std::vector<boost::asio::mutable_buffer> bufSeq;
    bufSeq.reserve(bufSize);
    for (size_t i = 0; i < bufSize; ++i)
    {
        bufSeq.push_back(boost::asio::buffer(bufs[i].m_beg, bufs[i].m_size));
    }
    SpinLock<>::ScopeLock lock(m_lock);
    m_stream->async_read_some(std::move(bufSeq), std::bind(&IAsyncChannelHandler::EndRead, handler, std::placeholders::_1, std::placeholders::_2, ctx));
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::AsyncWrite(const std::shared_ptr<LinearBuffer> &buf
    , size_t sendOffset, size_t sendLen, const IAsyncChannelHandler::ptr_t &handler, void *ctx)
{
    SpinLock<>::ScopeLock lock(m_lock);
    if (m_lastWrReq)
    {
        m_lastWrReq->m_next = std::unique_ptr<WrReq>(new WrReq{ handler, buf, sendOffset, sendLen, ctx, nullptr });
        m_lastWrReq = m_lastWrReq->m_next.get();
    }
    else
    {
        std::unique_ptr<WrReq> req(new WrReq{ handler, buf, sendOffset, sendLen, ctx, nullptr });
        m_lastWrReq = req.get();
        boost::asio::async_write(*m_stream, boost::asio::buffer(buf->data() + sendOffset, sendLen), std::bind(&StreamChannelBase<StreamTraits, LoggerName>::EndWrite
            , StreamChannelBase<StreamTraits, LoggerName>::shared_from_this(), std::move(req), std::placeholders::_1, std::placeholders::_2));
    }
}

template<typename StreamTraits, const char *LoggerName> void StreamChannelBase<StreamTraits, LoggerName>::EndWrite(std::unique_ptr<WrReq> &wrReq
    , const boost::system::error_code &err, std::size_t bytesTransferred)
{
    {
        SpinLock<>::ScopeLock lock(m_lock);
        if (wrReq->m_next)
        {
            us8* buffer = wrReq->m_next->m_buf->data() + wrReq->m_next->m_sendOffset;
            size_t bufferSize = wrReq->m_next->m_sendLen;
            boost::asio::async_write(*m_stream, boost::asio::buffer(buffer, bufferSize)
                , std::bind(&StreamChannelBase<StreamTraits, LoggerName>::EndWrite, StreamChannelBase<StreamTraits, LoggerName>::shared_from_this(), std::move(wrReq->m_next)
                , std::placeholders::_1, std::placeholders::_2));
        }
        else
        {
            m_lastWrReq = nullptr;
        }
    }
    wrReq->m_handler->EndWrite(err, bytesTransferred, wrReq->m_ctx);
}

#endif /* STREAMCHANNELBASEIMPL_H */