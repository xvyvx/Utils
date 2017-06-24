#ifndef TCPV4LISTENER_H
#define TCPV4LISTENER_H

#include "TcpListenerBase.h"
#include "../Traits/TcpV4Traits.h"

extern const char TcpV4ListenerLoggerName[];

typedef void TcpListenerAcceptFunc(std::shared_ptr<boost::asio::ip::tcp::endpoint>&, std::shared_ptr<boost::asio::ip::tcp::socket>&);

void TcpListenerAcceptEntry(std::shared_ptr<boost::asio::ip::tcp::endpoint> &remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::socket> &sock);

UTILS_EXPORTS_API void SetListenerAcceptFunc(TcpListenerAcceptFunc *func);

typedef TcpListenerBase<TcpV4Traits, TcpListenerAcceptFunc, TcpListenerAcceptEntry, TcpV4ListenerLoggerName> TcpV4Listener;

extern template class UTILS_DECL_API TcpListenerBase<TcpV4Traits, TcpListenerAcceptFunc, TcpListenerAcceptEntry, TcpV4ListenerLoggerName>;

#endif /* TCPV4LISTENER_H */