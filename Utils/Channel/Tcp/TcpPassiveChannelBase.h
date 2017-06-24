#ifndef TCPPASSIVECHANNELBASE_H
#define TCPPASSIVECHANNELBASE_H

#include <boost/asio.hpp>
#include "../Common/StreamChannelBase.h"

template<typename ProtocolTraits, const char *LoggerName> class TcpPassiveChannelBase :public StreamChannelBase<ProtocolTraits, LoggerName>
{
public:
	typedef StreamChannelBase<ProtocolTraits, LoggerName> BaseType;

	TcpPassiveChannelBase(std::shared_ptr<typename ProtocolTraits::StreamType> &stream, const boost::asio::ip::tcp::endpoint &remoteEndPoint);

	virtual ~TcpPassiveChannelBase() override;

	virtual void AsyncOpen(const IAsyncChannelHandler::weak_ptr_t &handler) override;

	virtual void AsyncClose(const IAsyncChannelHandler::weak_ptr_t &handler) override;

private:
	void Close(boost::system::error_code &shutdownErr, boost::system::error_code &closeErr);

	boost::asio::ip::tcp::endpoint m_remoteEndPoint;

	volatile bool m_closed;
};

#endif /* TCPPASSIVECHANNELBASE_H */