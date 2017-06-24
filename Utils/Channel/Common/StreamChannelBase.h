#ifndef STREAMCHANNELBASE_H
#define STREAMCHANNELBASE_H

#include <queue>
#include "../../Log/Log4cplusCustomInc.h"
#include "IAsyncChannel.h"
#include "../../Concurrent/SpinLock.h"

template<typename StreamTraits, const char *LoggerName> class StreamChannelBase:public std::enable_shared_from_this<StreamChannelBase<StreamTraits, LoggerName>>
	, public IAsyncChannel
{
public:
	StreamChannelBase(const std::shared_ptr<typename StreamTraits::StreamType> &stream);

	virtual void AsyncReadSome(BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::weak_ptr_t &handler, void *ctx = nullptr) override;

	virtual void AsyncWrite(const std::shared_ptr<LinearBuffer> &buf, const IAsyncChannelHandler::weak_ptr_t &handler, void *ctx = nullptr) override;

protected:
	std::shared_ptr<typename StreamTraits::StreamType> m_stream;

	SpinLock<> m_lock;

	static log4cplus::Logger log;

private:
	typedef struct
	{
		IAsyncChannelHandler::weak_ptr_t m_handler;

		std::shared_ptr<LinearBuffer> m_buf;

		void *m_ctx;
	} WrReq;

	typedef struct
	{
		IAsyncChannelHandler::weak_ptr_t m_handler;

		void *m_ctx;
	} RdReq;

	void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred);

	void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred);

	RdReq m_rdReq;

	std::queue<WrReq> m_wrReqs;
};

#endif /* STREAMCHANNELBASE_H */