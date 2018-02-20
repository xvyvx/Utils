#ifndef IASYNCCHANNEL_H
#define IASYNCCHANNEL_H

#include <memory>
#include "IAsyncChannelHandler.h"
#include "../../Buffer/BufferDescriptor.h"

/**
 * The interface of asynchronous communication channels.
 */
class UTILS_EXPORTS_API IAsyncChannel
{
public:
	/**
	 * Defines an alias representing the std::shared_ptr<IAsyncChannel>.
	 */
	typedef std::shared_ptr<IAsyncChannel> ptr_t;

	/**
	 * Default constructor.
	 */
	IAsyncChannel();

	/**
	 * Destructor.
	 */
	virtual ~IAsyncChannel();

	/**
	 * Start an asynchronous open operation on the communication channel,all operation error will reportted in the callback handler.
	 *
	 * @param The handler to be called when the open operation completes.
	 */
	virtual void AsyncOpen(const IAsyncChannelHandler::ptr_t &handler) = 0;

	/**
	 * Start an asynchronous read operation on the communication channel,all operation error will reportted in the callback handler.
	 *
	 * @param bufs The read buf array.
	 * @param bufSize Size of the read buf array.
	 * @param handler The handler to be called when the read operation completes.
	 * @param ctx (Optional) user defined context data.
	 * 
	 * @note The read operation may not read all of the requested number of bytes.
	 */
	virtual void AsyncReadSome(BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::ptr_t &handler, void *ctx = nullptr) = 0;

	/**
	 * Start an asynchronous write operation on the communication channel,all operation error will reportted in the callback handler.
	 *
	 * @param buf The write buf array.
	 * @param handler The handler to be called when the write operation completes.
	 * @param [in,out] ctx (Optional) user defined context data.
	 * 
	 * @note The write operation will write all of the requested number of bytes(or an error occurred) before call the handler. This function support
	 * multiple calls before previous operation finished(following request will be queued).
	 */
	virtual void AsyncWrite(const std::shared_ptr<LinearBuffer> &buf, const IAsyncChannelHandler::ptr_t &handler, void *ctx = nullptr) = 0;

	/**
	 * Start an asynchronous close operation on the communication channel,all operation error will reportted in the callback handler.
	 *
	 * @param handler The handler to be called when the close operation completes.
	 */
	virtual void AsyncClose(const IAsyncChannelHandler::ptr_t &handler) = 0;
};

#endif /* IASYNCCHANNEL_H */