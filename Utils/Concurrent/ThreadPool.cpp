#include "ThreadPool.h"
#include <chrono>
#include "BlackMagics.h"

std::shared_ptr<ThreadPool> ThreadPool::instance;

log4cplus::Logger ThreadPool::log = log4cplus::Logger::getInstance("ThreadPool");

int ThreadPool::ThreadMinCount = 0;

int ThreadPool::ThreadMaxCount = 0;

ThreadPool::ThreadPool() :m_currentThreadCount(0), m_threadBusyCount(0), m_tls(), m_context(), m_workGuard(boost::asio::make_work_guard(m_context))
{
	int hint = boost::thread::hardware_concurrency();
	int threadCount = hint * 3;
	if (hint == 1)
	{
		hint = 2;
		threadCount = 4;
	}
	ThreadMinCount = hint;
	ThreadMaxCount = threadCount;
	for (int i = 0; i < hint; ++i)
	{
		CreateThread();
	}
}

ThreadPool::~ThreadPool()
{
	assert(m_currentThreadCount.load(std::memory_order_acquire) == 0);
	Stop();
}

void ThreadPool::Stop()
{
	if (m_currentThreadCount.load(std::memory_order_acquire))
	{
		m_workGuard.reset();
		m_context.stop();
		for (unsigned int i = 0; m_currentThreadCount.load(std::memory_order_acquire); ++i)
		{
			DefaultBlackMagicFunc(i);
		}
	}
}

ThreadPool::ThreadContext* ThreadPool::GetCurrentThreadCtx()
{
	return instance->m_tls.get();
}

bool ThreadPool::CreateThread()
{
	try
	{
		boost::thread t(std::bind(&ThreadPool::ThreadEntry, this));
		t.detach();
	}
	catch(const std::exception &ex)
	{
		LOG4CPLUS_ERROR_FMT(log, "线程池创建线程捕获异常：%s", ex.what());
		return false;
	}
	catch (...)
	{
		LOG4CPLUS_ERROR(log, "线程池创建线程捕获未知异常。");
		return false;
	}
	return true;
}

void ThreadPool::ThreadEntry(ThreadPool *self)
{
	self->m_currentThreadCount.fetch_add(1, std::memory_order_acquire);
	ThreadContext ctx = { false };
	self->m_tls.reset(&ctx);
	bool countDecressed = false;
	while (!self->m_context.stopped())
	{
		try
		{
			size_t count = self->m_context.run_one_for(std::chrono::milliseconds(10));
			if (self->m_context.stopped())
			{
				break;
			}
			if (count == 0 || DiagnosticsHelper::GetCPUUsage() > 90)
			{
				int currentThreadCount = self->m_currentThreadCount.load(std::memory_order_acquire);
				bool exit = false;
				while (currentThreadCount > ThreadMinCount 
					&& !(exit = self->m_currentThreadCount.compare_exchange_weak(currentThreadCount, currentThreadCount - 1, std::memory_order_acquire, std::memory_order_acquire)));
				if(exit)
				{
					countDecressed = true;
					break;
				}
			}
		}
		catch (const std::exception &ex)
		{
			LOG4CPLUS_ERROR_FMT(log, "线程池执行线程捕获异常：%s", ex.what());
		}
		catch (...)
		{
			LOG4CPLUS_ERROR(log, "线程池执行捕获未知异常。");
		}
	}
	if(!countDecressed)
	{
		self->m_currentThreadCount.fetch_sub(1, std::memory_order_release);
	}
	self->m_tls.release();
}