#ifndef IASYNCCHANNELHANDLER_H
#define IASYNCCHANNELHANDLER_H

#include <memory>
#include <boost/system/error_code.hpp>
#include "../../Common/CommonHdr.h"

/**
 * The interface of asynchronous channel callback handler.
 */
class UTILS_EXPORTS_API IAsyncChannelHandler
{
public:

    /**
     * Defines an alias representing the std::shared_ptr<IAsyncChannelHandler>.
     */
    using ptr_t = std::shared_ptr<IAsyncChannelHandler>;

    /**
     * Default constructor
     */
    IAsyncChannelHandler();

    /**
     * Destructor
     */
    virtual ~IAsyncChannelHandler();

    /**
     * The handler to be called when the open operation completes.
     *
     * @param err Result of operation.
     */
    virtual void EndOpen(const boost::system::error_code &err) = 0;

    /**
     * The handler to be called when the read operation completes.
     *
     * @param err Result of operation.
     * @param bytesTransferred Number of bytes read to the buffers.
     * 	If an error occurred,this will be less than the sum of the buffer sizes.
     * @param ctx User defined context data,passed to @ref IAsyncChannel::AsyncReadSome.
     */
    virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) = 0;

    /**
     * The handler to be called when the write operation completes.
     *
     * @param err Result of operation.
     * @param bytesTransferred Number of bytes written from the buffers.
     * @param ctx User defined context data,passed to @ref IAsyncChannel::AsyncWrite.
     */
    virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) = 0;

    /**
     * The handler to be called when the close operation completes.
     *
     * @param err Result of operation.
     */
    virtual void EndClose(const boost::system::error_code &err) = 0;
};

#endif /* IASYNCCHANNELHANDLER_H */