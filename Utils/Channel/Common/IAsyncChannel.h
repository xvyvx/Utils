#ifndef IASYNCCHANNEL_H
#define IASYNCCHANNEL_H

#include <memory>
#include "IAsyncChannelHandler.h"
#include "../../Buffer/BufferDescriptor.h"

class UTILS_EXPORTS_API IAsyncChannel
{
public:
	typedef std::shared_ptr<IAsyncChannel> ptr_t;

	IAsyncChannel();

	virtual ~IAsyncChannel();

	virtual void AsyncOpen(const IAsyncChannelHandler::weak_ptr_t &handler) = 0;

	virtual void AsyncReadSome(BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::weak_ptr_t &handler, void *ctx = nullptr) = 0;

	virtual void AsyncWrite(const std::shared_ptr<LinearBuffer> &buf, const IAsyncChannelHandler::weak_ptr_t &handler, void *ctx = nullptr) = 0;

	virtual void AsyncClose(const IAsyncChannelHandler::weak_ptr_t &handler) = 0;
};

#endif /* IASYNCCHANNEL_H */