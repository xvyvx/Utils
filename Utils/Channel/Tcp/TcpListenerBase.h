#ifndef TCPLISTENERBASE_H
#define TCPLISTENERBASE_H

#include <set>
#include <utility>
#include <initializer_list>
#include <memory>
#include <boost/asio.hpp>
#include "../../Log/Log4cplusCustomInc.h"
#include "../../Common/CommonHdr.h"
#include "../../Concurrent/SpinLock.h"

template
<
	typename ProtocolTraits, 
	typename AcceptFunc,//=void(*)(std::shared_ptr<boost::asio::ip::tcp::endpoint>&, std::shared_ptr<boost::asio::ip::tcp::socket>&)
	AcceptFunc *AcceptFunction,
	const char *LoggerName
> 
class TcpListenerBase : public std::enable_shared_from_this<TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>>
{
public:
	typedef std::enable_shared_from_this<TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>> BaseType;

	typedef std::shared_ptr<TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>> ptr_t;

	static TcpListenerBase& Instance();

	static void Destory();

	TcpListenerBase(const TcpListenerBase&) = delete;

	TcpListenerBase(TcpListenerBase&&) = delete;

	~TcpListenerBase();

	TcpListenerBase& operator=(const TcpListenerBase&) = delete;

	TcpListenerBase&& operator=(TcpListenerBase&&) = delete;

	bool AddListenEndPoints(std::initializer_list<std::pair<typename ProtocolTraits::AddressType, us16>> localEndPoints);

	bool AddListenEndPoint(const boost::asio::ip::tcp::endpoint &localEndPoint);

	void RemoveListenerEndPoint(const boost::asio::ip::tcp::endpoint &localEndPoint);

protected:
	TcpListenerBase();

private:
	static log4cplus::Logger log;

	static ptr_t instance;

	bool BeginClose();

	std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator AddListenerEndPointUnsafe(const boost::asio::ip::tcp::endpoint &localEndPoint);

	void BeginAccept(std::shared_ptr<boost::asio::ip::tcp::acceptor> listener);

	void EndAccept(std::shared_ptr<boost::asio::ip::tcp::socket> sock, std::shared_ptr<boost::asio::ip::tcp::endpoint> remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::acceptor> listener
		, const boost::system::error_code& error);

	void RemoveListenerEndPointUnsafe(const boost::asio::ip::tcp::endpoint &localEndPoint);

	std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>> m_acceptors;

	volatile bool m_stoped;

	SpinLock<> m_lock;
};

#endif /* TCPLISTENERBASE_H */