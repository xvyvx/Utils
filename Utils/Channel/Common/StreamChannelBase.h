#ifndef STREAMCHANNELBASE_H
#define STREAMCHANNELBASE_H

#include "../../Log/Log4cplusCustomInc.h"
#include "IAsyncChannel.h"
#include "../../Concurrent/SpinLock.h"

template<typename StreamTraits, const char *LoggerName> class StreamChannelBase:public std::enable_shared_from_this<StreamChannelBase<StreamTraits, LoggerName>>
	, public IAsyncChannel
{
public:
	StreamChannelBase(const std::shared_ptr<typename StreamTraits::StreamType> &stream);

	virtual void AsyncReadSome(BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::ptr_t &handler, void *ctx = nullptr) override;

	virtual void AsyncWrite(const std::shared_ptr<LinearBuffer> &buf, const IAsyncChannelHandler::ptr_t &handler, void *ctx = nullptr) override;

protected:
	std::shared_ptr<typename StreamTraits::StreamType> m_stream;

	SpinLock<> m_lock;

	static log4cplus::Logger log;

private:
	struct WrReq
	{
		IAsyncChannelHandler::ptr_t m_handler;

		std::shared_ptr<LinearBuffer> m_buf;

		void *m_ctx;

		std::shared_ptr<WrReq> m_next;
	} *m_lastWrReq;

	void EndWrite(std::shared_ptr<WrReq> wrReq, const boost::system::error_code &err, std::size_t bytesTransferred);
};

#endif /* STREAMCHANNELBASE_H */