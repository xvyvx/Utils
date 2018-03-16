#ifndef TCPLISTENERBASEIMPL_H
#define TCPLISTENERBASEIMPL_H

#include "TcpListenerBase.h"
#include "../../Concurrent/ThreadPool.h"

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> log4cplus::Logger 
	TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::log = log4cplus::Logger::getInstance(LoggerName);

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	typename TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::ptr_t 
	TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::instance;

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>&TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::Instance()
{
	if (!instance)
	{
		instance.reset(new TcpListenerBase());
	}
	return *instance;
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	void TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::Destory()
{
	if (instance)
	{
		instance->BeginClose();
		instance.reset();
	}
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::TcpListenerBase() :m_acceptors(), m_stoped(false), m_lock()
{
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::~TcpListenerBase()
{
	m_acceptors.clear();
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	bool TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::BeginClose()
{
	bool ret = true;
	boost::system::error_code err;
	SpinLock<>::ScopeLock lock(m_lock);
	if (!m_stoped)
	{
		m_stoped = true;
		for (auto i = m_acceptors.begin(); i != m_acceptors.end(); ++i)
		{
			(*i)->close(err);
			if(err)
			{
				LOG4CPLUS_ERROR(log, "启动关闭监听错误，终结点：" << (*i)->local_endpoint() << "，错误信息：" << err.message());
				if(ret)
				{
					ret = false;
				}
			}
		}
	}
	return ret;
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	bool TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::AddListenEndPoints(
	std::initializer_list<std::pair<typename ProtocolTraits::AddressType, us16>> localEndPoints)
{
	bool ret = true;
	std::vector<std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator> listeners;
	SpinLock<>::ScopeLock lock(m_lock);
	if (m_stoped)
	{
		return ret;
	}
	for (auto i = localEndPoints.begin(); i != localEndPoints.end(); ++i)
	{
		boost::asio::ip::tcp::endpoint temp(i->first, i->second);
		std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator iter = AddListenerEndPointUnsafe(temp);
		if (iter != m_acceptors.end())
		{
			listeners.push_back(iter);
		}
		else
		{
			ret = false;
			std::for_each(listeners.begin(), listeners.end(),
				[&](std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator &listenerIter)->void
			{
				boost::system::error_code err;
				(*listenerIter)->close(err);
				if(err)
				{
					LOG4CPLUS_ERROR(log, "添加监听失败后移除监听错误，终结点：" << (*listenerIter)->local_endpoint() << "，错误信息：" << err.message());
				}
				m_acceptors.erase(listenerIter);
			}
			);
			break;
		}
	}
	return ret;
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	bool TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::AddListenEndPoint(
	const boost::asio::ip::tcp::endpoint &localEndPoint)
{
	bool ret = true;
	SpinLock<>::ScopeLock lock(m_lock);
	if (!m_stoped)
	{
		ret = AddListenerEndPointUnsafe(localEndPoint) != m_acceptors.end();
	}
	return ret;
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	void TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::RemoveListenerEndPoint(const boost::asio::ip::tcp::endpoint &localEndPoint)
{
	SpinLock<>::ScopeLock lock(m_lock);
	if (!m_stoped)
	{
		RemoveListenerEndPointUnsafe(localEndPoint);
	}
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator
	TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::AddListenerEndPointUnsafe(
	const boost::asio::ip::tcp::endpoint &localEndPoint)
{
	std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator ret = m_acceptors.end();
	std::pair<std::set<std::shared_ptr<boost::asio::ip::tcp::acceptor>>::iterator, bool> result = std::make_pair(m_acceptors.end(), false);
	try
	{
		result = m_acceptors.insert(std::make_shared<boost::asio::ip::tcp::acceptor>(ThreadPool::Instance().Context()));
		assert(result.second);
		const std::shared_ptr<boost::asio::ip::tcp::acceptor> &listener = *result.first;
		listener->open(ProtocolTraits::Protocol());
		listener->bind(localEndPoint);
		listener->listen();
		BeginAccept(listener);
		ret = result.first;
	}
	catch (const boost::system::system_error &err)
	{
		LOG4CPLUS_ERROR(log, "添加监听失败，终结点：" << localEndPoint << "，错误信息：" << err.code().message());
		m_acceptors.erase(result.first);
		ret = m_acceptors.end();
	}
	catch (...)
	{
		if (result.second)
		{
			LOG4CPLUS_ERROR(log, "添加监听失败（未知错误），终结点：" << localEndPoint );
			m_acceptors.erase(result.first);
			ret = m_acceptors.end();
		}
	}
	return ret;
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	void TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::BeginAccept(std::shared_ptr<boost::asio::ip::tcp::acceptor> listener)
{
	std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(ThreadPool::Instance().Context()));
	std::shared_ptr<boost::asio::ip::tcp::endpoint> remotePoint(new boost::asio::ip::tcp::endpoint());
	listener->async_accept(*sock, *remotePoint
		, std::bind(&TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::EndAccept
			, BaseType::shared_from_this(), sock, remotePoint, listener, std::placeholders::_1));
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	void TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::EndAccept(std::shared_ptr<boost::asio::ip::tcp::socket> sock
	, std::shared_ptr<boost::asio::ip::tcp::endpoint> remoteEndPoint, std::shared_ptr<boost::asio::ip::tcp::acceptor> listener, const boost::system::error_code& error)
{
	bool listenerDropped = true;
	boost::asio::ip::tcp::acceptor::endpoint_type ep;
	{
		SpinLock<>::ScopeLock lock(m_lock);
		if (!m_stoped)
		{
			auto iter = m_acceptors.find(listener);
			if (iter != m_acceptors.end())
			{
				BeginAccept(listener);
				listenerDropped = false;
				if(error)
				{
					ep = listener->local_endpoint();
				}
			}
		}
	}
	if (!error)
	{
		AcceptFunction(remoteEndPoint, sock);
	}
	else if(!listenerDropped)
	{
		LOG4CPLUS_ERROR(log, "接收连接失败，监听终结点：" << ep << "，错误信息：" << error.message());
	}
}

template<typename ProtocolTraits, typename AcceptFunc, AcceptFunc *AcceptFunction, const char *LoggerName> 
	void TcpListenerBase<ProtocolTraits, AcceptFunc, AcceptFunction, LoggerName>::RemoveListenerEndPointUnsafe(const boost::asio::ip::tcp::endpoint &localEndPoint)
{
	auto iter = std::find_if
		(
			m_acceptors.begin(), m_acceptors.end()
			, [&localEndPoint](const std::shared_ptr<boost::asio::ip::tcp::acceptor> &listener)->bool
	{
		return localEndPoint == listener->local_endpoint();
	}
	);
	if (iter != m_acceptors.end())
	{
		boost::system::error_code err;
		(*iter)->close(err);
		if(err)
		{
			LOG4CPLUS_ERROR(log, "移除监听失败，终结点：" << (*iter)->local_endpoint() << "，错误信息：" << err.message());
		}
		m_acceptors.erase(iter);
	}
}

#endif /* TCPLISTENERBASEIMPL_H */