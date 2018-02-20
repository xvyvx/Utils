#ifndef TCPCHANNELBASE_H
#define TCPCHANNELBASE_H

#include <boost/asio.hpp>
#include "../Common/StreamChannelBase.h"

/**
 * Base class of TCP channel.
 *
 * @tparam ProtocolTraits Type of the protocol traits.
 * @tparam LoggerName    The logger name used in base class.
 */
template<typename ProtocolTraits, const char *LoggerName> class TcpChannelBase :public StreamChannelBase<ProtocolTraits, LoggerName>
{
public:

	/**
	 * Defines an alias representing type of the base class.
	 */
	typedef StreamChannelBase<ProtocolTraits, LoggerName> BaseType;

	/**
	 * Constructor
	 *
	 * @param remoteAddr The remote address(string format) used to connect.
	 * @param remotePort The remote port used to connect.
	 * @param openOnConstruct (Optional) True to open on construct.
	 * @param localPort (Optional) The local port used to bind local end point(default is auto select).
	 * @param localAddr (Optional) The local address used to bind local end point(default is to use any address).
	 */
	TcpChannelBase(const std::string &remoteAddr, us16 remotePort, bool openOnConstruct = true, us16 localPort = 0, const std::string &localAddr = ProtocolTraits::AnyAddressString());

	/**
	 * Constructor
	 *
	 * @param remoteAddr The remote address used to connect.
	 * @param remotePort The remote port used to connect.
	 * @param openOnConstruct (Optional) True to open on construct.
	 * @param localPort (Optional) The local port used to bind local end point(default is auto select).
	 * @param localAddr (Optional) The local address used to bind local end point(default is to use any address).
	 */
	TcpChannelBase(const typename ProtocolTraits::AddressType &remoteAddr, us16 remotePort, bool openOnConstruct = true, us16 localPort = 0
		, const typename ProtocolTraits::AddressType &localAddr = ProtocolTraits::AnyAddress());

	/**
	 * Destructor
	 */
	virtual ~TcpChannelBase() override;

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
	 * Opens the this channel.
	 *
	 * @param [out] error Operation result.
	 */
	void Open(boost::system::error_code &error);

	/**
	 * Closes this channel.
	 *
	 * @param [out] shutdownErr The shutdown result.
	 * @param [out] closeErr The close error.
	 */
	void Close(boost::system::error_code &shutdownErr, boost::system::error_code &closeErr);

	boost::asio::ip::tcp::endpoint m_localEndPoint; /**< The local end point used to bind local end point. */

	boost::asio::ip::tcp::endpoint m_remoteEndPoint;	/**< The remote end point used to connect. */

	volatile bool m_closed; /**< True if closed */
};

#endif /* TCPCHANNELBASE_H */