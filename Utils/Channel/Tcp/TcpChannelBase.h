#ifndef TCPCHANNELBASE_H
#define TCPCHANNELBASE_H

#include <boost/asio.hpp>
#include "../Common/StreamChannelBase.h"

template<typename ProtocolTraits, const char *LoggerName> class TcpChannelBase :public StreamChannelBase<ProtocolTraits, LoggerName>
{
public:
	typedef StreamChannelBase<ProtocolTraits, LoggerName> BaseType;

	TcpChannelBase(const std::string &remoteAddr, us16 remotePort, us16 localPort = 0, const std::string &localAddr = ProtocolTraits::AnyAddressString());

	TcpChannelBase(const typename ProtocolTraits::AddressType &remoteAddr, us16 remotePort, us16 localPort = 0
		, const typename ProtocolTraits::AddressType &localAddr = ProtocolTraits::AnyAddress());

	virtual ~TcpChannelBase() override;

	virtual void AsyncOpen(const IAsyncChannelHandler::ptr_t &handler) override;

	virtual void AsyncClose(const IAsyncChannelHandler::ptr_t &handler) override;

private:
	void Open(boost::system::error_code &error);

	void Close(boost::system::error_code &shutdownErr, boost::system::error_code &closeErr);

	boost::asio::ip::tcp::endpoint m_localEndPoint;

	boost::asio::ip::tcp::endpoint m_remoteEndPoint;

	volatile bool m_closed;
};

#endif /* TCPCHANNELBASE_H */