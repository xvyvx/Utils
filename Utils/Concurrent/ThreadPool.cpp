#include "ThreadPool.h"
#include <functional>

std::shared_ptr<ThreadPool> ThreadPool::instance;

log4cplus::Logger ThreadPool::log = log4cplus::Logger::getInstance("ThreadPool");

ThreadPool::ThreadPool() :m_threads(), m_svr(), m_ioSvrWork(new boost::asio::io_service::work(m_svr))
{
	unsigned int threadCount = boost::thread::hardware_concurrency() + 2;
	while (threadCount)
	{
		m_threads.create_thread(std::bind(ThreadEntry, &m_svr));
		--threadCount;
	}
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::Stop()
{
	instance->m_ioSvrWork.reset();
	instance->m_svr.stop();
	instance->m_threads.join_all();
}

void ThreadPool::ThreadEntry(boost::asio::io_service *svr)
{
	while (!svr->stopped())
	{
		try
		{
			svr->run();
		}
		catch (...)
		{
			LOG4CPLUS_ERROR(log, "线程池执行捕获未知异常。");
		}
	}
}