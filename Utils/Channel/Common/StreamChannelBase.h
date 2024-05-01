#ifndef STREAMCHANNELBASE_H
#define STREAMCHANNELBASE_H

#include "../../Log/Log4cplusCustomInc.h"
#include "IAsyncChannel.h"
#include "../../Concurrent/SpinLock.h"

/**
* The base class of stream based channel.
*
* @tparam StreamTraits Traits of specific channel class.
* @tparam LoggerName  C-style string of logger name used in base class.
*/
template<typename StreamTraits, const char *LoggerName> class StreamChannelBase:public std::enable_shared_from_this<StreamChannelBase<StreamTraits, LoggerName>>
    , public IAsyncChannel
{
public:

    /**
     * Constructor
     *
     * @param stream Pointer of underlying stream implementation.
     */
    StreamChannelBase(const std::shared_ptr<typename StreamTraits::StreamType> &stream);

    /**
    * {@inheritDoc}
    */
    virtual void AsyncReadSome(BufDescriptor bufs[], size_t bufSize, const IAsyncChannelHandler::ptr_t &handler, void *ctx) override;

    /**
     * {@inheritDoc}
     */
    virtual void AsyncWrite(const std::shared_ptr<LinearBuffer> &buf, size_t sendOffset, size_t sendLen
        , const IAsyncChannelHandler::ptr_t &handler, void *ctx) override;

protected:
    std::shared_ptr<typename StreamTraits::StreamType> m_stream;	/**< Pointer of underlying stream implementation. */

    SpinLock<> m_lock;  /**< Internal spin lock used for thread safe. */

    static log4cplus::Logger log;   /**< The logger */

private:
    /**
     * A write request.
     */
    struct WrReq
    {
        IAsyncChannelHandler::ptr_t m_handler;  /**< Request's callback handler. */

        std::shared_ptr<LinearBuffer> m_buf;	/**< Request's write buffer. */

        size_t m_sendOffset;	/**< Offset of buffer to be writen. */

        size_t m_sendLen;	/**< Byte count to be writen. */

        void *m_ctx;	/**< Request's user defined context data. */

        std::unique_ptr<WrReq> m_next;  /**< Next write request(used as a linked list). */
    } *m_lastWrReq; /**< Last write request in this channel's write linked list. */

    /**
     * The write operation callback handler for internal use.
     *
     * @param wrReq The write request node used in this write request.
     * @param err Result of operation.
     * @param bytesTransferred Number of bytes written from the wrReq.
     */
    void EndWrite(std::unique_ptr<WrReq> &wrReq, const boost::system::error_code &err, std::size_t bytesTransferred);
};

#endif /* STREAMCHANNELBASE_H */