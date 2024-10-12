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

/**
 * A TCP listener base class.
 *
 * @tparam ProtocolTraits Type of the protocol traits.
 * @tparam AcceptFunc Type of the accept function.
 * @tparam AcceptFunction Instance of the accept function.
 * 	The function signature of predicator must be:
 * @code void(std::shared_ptr<boost::asio::ip::tcp::endpoint>&, //remote end point
 * 		 std::shared_ptr<boost::asio::ip::tcp::socket>& //acceptted socket
 * 	) @endcode
 * @tparam LoggerName The logger name used in base class.
 */
template
<
    typename ProtocolTraits, 
    typename AcceptFunc,
    AcceptFunc *AcceptFunction,
    const char *LoggerName
> 
class TcpListenerBase : public std::enable_shared_from_this<TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>>
{
public:

    /**
     * Defines an alias representing type of the base class.
     */
    using BaseType = std::enable_shared_from_this<TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>>;

    /**
     * Defines an alias representing the pointer to self.
     */
    using ptr_t = std::shared_ptr<TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>>;

    /**
     * Gets the global instance.
     *
     * @return Global instance refernece.
     */
    static TcpListenerBase& Instance();

    /**
     * Destories the global instance.
     */
    static void Destory();

    /**
     * Copy constructor(deleted)
     */
    TcpListenerBase(const TcpListenerBase&) = delete;

    /**
     * Move constructor(deleted)
     */
    TcpListenerBase(TcpListenerBase&&) = delete;

    /**
     * Destructor
     */
    ~TcpListenerBase();

    /**
     * Assignment operator(deleted)
     *
     * @return Equal to *this.
     */
    TcpListenerBase& operator=(const TcpListenerBase&) = delete;

    /**
     * Move assignment operator(deleted)
     */
    TcpListenerBase&& operator=(TcpListenerBase&&) = delete;

    /**
     * Adds listen end points.
     *
     * @param localEndPoints The local end points.
     *
     * @return True if it succeeds, false if it fails.
     */
    bool AddListenEndPoints(std::initializer_list<std::pair<typename ProtocolTraits::AddressType, us16>> localEndPoints);

    /**
     * Adds a listen end point
     *
     * @param localEndPoint The local end point.
     *
     * @return True if it succeeds, false if it fails.
     */
    bool AddListenEndPoint(const boost::asio::ip::tcp::endpoint &localEndPoint);

    /**
     * Removes the listener end point described by localEndPoint
     *
     * @param localEndPoint The local end point.
     */
    void RemoveListenerEndPoint(const boost::asio::ip::tcp::endpoint &localEndPoint);

protected:

    /**
     * Default constructor
     */
    TcpListenerBase();

private:
    static log4cplus::Logger log;   /**< The logger. */

    static ptr_t instance;  /**< Global instance. */

    /**
     * Begin a close operation.
     *
     * @return True if it succeeds, false if it fails.
     */
    bool BeginClose();

    /**
     * Adds listener end point implementation(thread unsafe).
     *
     * @param localEndPoint The local end point to be added.
     *
     * @return Iterator point to added end point when success,otherwise return a end iterator.
     */
    std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator AddListenerEndPointUnsafe(const boost::asio::ip::tcp::endpoint &localEndPoint);

    /**
     * Begin an accept operation.
     *
     * @param listener Global listener instance.
     */
    void BeginAccept(std::shared_ptr<boost::asio::ip::tcp::acceptor> listener);

    /**
     * Internal end accept callback.
     *
     * @param sock The acceptted sock.
     * @param remoteEndPoint The remote end point.
     * @param listener Global listener instance.
     * @param error Accpet operation result.
     */
    void EndAccept(std::shared_ptr<boost::asio::ip::tcp::socket> sock, std::shared_ptr<boost::asio::ip::tcp::endpoint> remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::acceptor> listener
        , const boost::system::error_code& error);

    /**
     * Removes the listener end point implementation(thread unsafe)
     *
     * @param localEndPoint Local end point to be removed.
     */
    void RemoveListenerEndPointUnsafe(const boost::asio::ip::tcp::endpoint &localEndPoint);

    std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>> m_acceptors;  /**< Listeners. */

    volatile bool m_stoped; /**< True if stoped. */

    SpinLock<> m_lock;  /**< Internal lock used for thread safe. */
};

#endif /* TCPLISTENERBASE_H */