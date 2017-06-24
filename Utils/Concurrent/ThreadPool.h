#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../Common/CommonHdr.h"
#include "../Log/Log4cplusCustomInc.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class UTILS_EXPORTS_API ThreadPool
{
public:
	static ThreadPool& Instance()
	{
		if (!instance)
		{
			instance.reset(new ThreadPool());
		}
		return *instance;
	}

	static void Stop();

	static void Destory()
	{
		instance.reset();
	}

	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;

	ThreadPool(ThreadPool&&) = delete;

	ThreadPool& operator=(const ThreadPool&) = delete;

	ThreadPool& operator=(ThreadPool&&) = delete;

	template<typename T> void QueueWorkItem(T &&item)
	{
		m_svr.post(std::forward<T>(item));
	}

	boost::asio::io_service& Service()
	{
		return m_svr;
	}

private:
	ThreadPool();

	static void ThreadEntry(boost::asio::io_service*);

	static std::shared_ptr<ThreadPool> instance;

	boost::thread_group m_threads;

	boost::asio::io_service m_svr;

	std::unique_ptr<boost::asio::io_service::work> m_ioSvrWork;

	static log4cplus::Logger log;
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif /* THREADPOOL_H */