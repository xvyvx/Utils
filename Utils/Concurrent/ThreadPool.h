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

/**
 * Forward declaration of asio_handler_invoke
 *
 * @tparam Function Type of the target function.
 * @tparam Args Type of the arguments.
 * @param [in,out] func Target function.
 * @param [in,out] ...  Target function arguments.
 */
template<typename Function, typename... Args> void asio_handler_invoke(Function &func, Args&&...);

}}

/**
 * Thread pool.
 */
class UTILS_EXPORTS_API ThreadPool
{
public:
	template<typename Function, typename... Args> friend void boost::asio::asio_handler_invoke(Function &func, Args&&... args);

	/**
	 * Gets global instance.
	 *
	 * @return Reference to the global thread pool.
	 */
	static ThreadPool& Instance()
	{
		if (!instance)
		{
			instance.reset(new ThreadPool());
		}
		return *instance;
	}

	/**
	 * Stops all work thread in this pool,return until all thread finished.
	 */
	void Stop();

	/**
	 * Destories thread pool.
	 */
	static void Destory()
	{
		instance.reset();
	}

	/**
	 * Destructor
	 */
	~ThreadPool();

	/**
	 * Copy constructor(deleted)
	 */
	ThreadPool(const ThreadPool&) = delete;

	/**
	 * Move constructor(deleted)
	 */
	ThreadPool(ThreadPool&&) = delete;

	/**
	 * Assignment operator(deleted)
	 */
	ThreadPool& operator=(const ThreadPool&) = delete;

	/**
	 * Move assignment operator(deleted)
	 */
	ThreadPool& operator=(ThreadPool&&) = delete;

	/**
	 * Gets the asio io_context
	 *
	 * @return A reference to a boost::asio::io_context.
	 */
	boost::asio::io_context& Context()
	{
		return m_context;
	}

private:
	/**
	 * Internal thread context,used to store per thread information.
	 */
	struct ThreadContext
	{
		bool m_processing;  /**< True if current is executting. */
	};

	/**
	 * Default constructor
	 */
	ThreadPool();

	/**
	 * Gets current executting thread's context.
	 *
	 * @return Executting thread's context.
	 */
	ThreadContext* GetCurrentThreadCtx();

	/**
	 * Creates a work thread.
	 *
	 * @return True if create succeeds, false if it fails.
	 */
	bool CreateThread();

	/**
	 * Thread work entry.
	 *
	 * @param self Global thread pool object.
	 */
	static void ThreadEntry(ThreadPool *self);

	static std::shared_ptr<ThreadPool> instance;	/**< Global instance. */

	std::atomic<int> m_currentThreadCount;  /**< Number of current threads in the pool. */

	std::atomic<int> m_threadBusyCount; /**< Number of thread whitch is busy. */

	boost::thread_specific_ptr<ThreadContext> m_tls;	/**< tls used to store thread context. */

	boost::asio::io_context m_context;  /**< The asio io_context. */

	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;   /**< io_context's work guard. */

	static int ThreadMinCount;  /**< Number of minimum thread count in the pool. */

	static int ThreadMaxCount;  /**< Number of maximum thread count in the pool. */

	static log4cplus::Logger log;   /**< The logger. */
};

namespace boost {
namespace asio {

/**
 * asio_handler_invoke,use to hook work function execution.
 *
 * @tparam Function Type of the target function.
 * @tparam Args Type of the target function arguments.
 * @param func The target function.
 * @param ...  Target function arguments.
 */
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

/**
 * Queue a work item to thread pool.
 *
 * @tparam T Work function type.
 * @tparam Args Work function arguments.
 * @param item Work function.
 * @param args Variable arguments providing work function arguments.
 */
template<typename T, typename... Args> void QueueThreadPoolWorkItem(T &&item, Args&&... args)
{
	boost::asio::post(ThreadPool::Instance().Context(), std::bind(std::forward<T>(item), std::forward<Args>(args)...));
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif /* THREADPOOL_H */