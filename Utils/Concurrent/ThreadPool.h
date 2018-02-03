#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../Common/CommonHdr.h"
#include "../Diagnostics/DiagnosticsHelper.h"
#include "../Log/Log4cplusCustomInc.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace boost {
namespace asio {

template<typename Function, typename... Args> void asio_handler_invoke(Function &func, Args&&...);

}}

class UTILS_EXPORTS_API ThreadPool
{
public:
	template<typename Function, typename... Args> friend void boost::asio::asio_handler_invoke(Function &func, Args&&... args);

	static ThreadPool& Instance()
	{
		if (!instance)
		{
			instance.reset(new ThreadPool());
		}
		return *instance;
	}

	void Stop();

	static void Destory()
	{
		instance.reset();
	}

	~ThreadPool();

	ThreadPool(const ThreadPool&) = delete;

	ThreadPool(ThreadPool&&) = delete;

	ThreadPool& operator=(const ThreadPool&) = delete;

	ThreadPool& operator=(ThreadPool&&) = delete;

	boost::asio::io_context& Context()
	{
		return m_context;
	}

private:
	struct ThreadContext
	{
		bool m_processing;
	};

	ThreadPool();

	ThreadContext* GetCurrentThreadCtx();

	bool CreateThread();

	static void ThreadEntry(ThreadPool *self);

	static std::shared_ptr<ThreadPool> instance;

	std::atomic<int> m_currentThreadCount;

	std::atomic<int> m_threadBusyCount;

	boost::thread_specific_ptr<ThreadContext> m_tls;

	boost::asio::io_context m_context;

	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;

	static int ThreadMinCount;

	static int ThreadMaxCount;

	static log4cplus::Logger log;
};

namespace boost {
namespace asio {

template<typename Function, typename... Args> void asio_handler_invoke(Function &func, Args&&...)
{
	//直接访问tls字段调用get总是返回null指针，估计是thread_specific_ptr使用了无法跨模块边界的变量（比如模板内的全局变量），
	//所以这里使用导出函数处理（由实现模块的函数访问自己对应的thread_specific_ptr实例）
	ThreadPool::ThreadContext *ctx = ThreadPool::instance->GetCurrentThreadCtx();
	if (!ctx->m_processing)
	{
		ctx->m_processing = true;
		int currentBusyCount = ThreadPool::instance->m_threadBusyCount.fetch_add(1, std::memory_order_acquire) + 1;
		//TODO 创建线程需要限制不超过最大数
		if (currentBusyCount >= ThreadPool::instance->m_currentThreadCount.load(std::memory_order_acquire) && ThreadPool::instance->m_currentThreadCount.load(std::memory_order_acquire) < ThreadPool::ThreadMaxCount
			&& DiagnosticsHelper::GetCPUUsage() < 75)
		{
			ThreadPool::instance->CreateThread();
		}
		func();
		ThreadPool::instance->m_threadBusyCount.fetch_sub(1, std::memory_order_acquire);
		ctx->m_processing = false;
	}
	else
	{
		func();
	}
}

}}

template<typename T, typename... Args> void QueueThreadPoolWorkItem(T &&item, Args&&... args)
{
	boost::asio::post(ThreadPool::Instance().Context(), std::bind(std::forward<T>(item), std::forward<Args>(args)...));
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif /* THREADPOOL_H */