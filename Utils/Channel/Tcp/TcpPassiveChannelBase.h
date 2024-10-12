#ifndef TCPPASSIVECHANNELBASE_H
#define TCPPASSIVECHANNELBASE_H

#include <boost/asio.hpp>
#include "../Common/StreamChannelBase.h"

/**
 * A TCP passive channel base class.
 *
 * @tparam ProtocolTraits Type of the protocol traits.
 * @tparam LoggerName Logger name used in base class.
 */
template<typename ProtocolTraits, const char *LoggerName> class TcpPassiveChannelBase :public StreamChannelBase<ProtocolTraits, LoggerName>
{
public:

    /**
     * Defines an alias representing type of the base class.
     */
    using BaseType = StreamChannelBase<ProtocolTraits, LoggerName>;

    /**
     * Constructor
     *
     * @param [in,out] stream Basic stream object.
     * @param remoteEndPoint Remote end point connectted from.
     */
    TcpPassiveChannelBase(std::shared_ptr<typename ProtocolTraits::StreamType> &stream, const boost::asio::ip::tcp::endpoint &remoteEndPoint);

    /**
     * Destructor
     */
    virtual ~TcpPassiveChannelBase() override;

    /**
     * {@inheritDoc}
     */
    virtual void AsyncOpen(const IAsyncChannelHandler::ptr_t &handler) override;

    /**
     * {@inheritDoc}
     */
    virtual void AsyncClose(const IAsyncChannelHandler::ptr_t &handler) override;

private:

    /**
     * Closes this channel.
     *
     * @param [out] shutdownErr The shutdown error.
     * @param [out] closeErr The close error.
     */
    void Close(boost::system::error_code &shutdownErr, boost::system::error_code &closeErr);

    boost::asio::ip::tcp::endpoint m_remoteEndPoint;	/**< The remote end point connectted from. */

    volatile bool m_closed; /**< True if closed. */
};

#endif /* TCPPASSIVECHANNELBASE_H */