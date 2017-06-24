#include "TcpV4Listener.h"
#include "TcpListenerBaseImpl.h"

const char TcpV4ListenerLoggerName[] = "TcpV4Listener";

TcpListenerAcceptFunc *acceptFunc = nullptr;

void SetListenerAcceptFunc(TcpListenerAcceptFunc *func)
{
	acceptFunc = func;
}

void TcpListenerAcceptEntry(std::shared_ptr<boost::asio::ip::tcp::endpoint> &remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::socket> &sock)
{
	acceptFunc(remoteEndPoint, sock);
}

template class UTILS_EXPORTS_API TcpListenerBase<TcpV4Traits, TcpListenerAcceptFunc, TcpListenerAcceptEntry, TcpV4ListenerLoggerName>;