#ifndef EVENT_H
#define EVENT_H

#include <type_traits>
#include <utility>
#include "../Common/CommonHdr.h"
#if defined(IS_WINDOWS)
#include <Windows.h>
#elif defined(IS_UNIX)
#include <pthread.h>
#include <inttypes.h>
#else
#error Unsupportted platform.
#endif

/**
 * WaitEvent used for threads sync.
 */
class UTILS_EXPORTS_API WaitEvent
{
public:
    /**
     * Default constructor,set status to normal.
     */
    WaitEvent();

    /**
     * Destructor
     */
    ~WaitEvent();

    /**
     * Copy constructor(deleted)
     */
    WaitEvent(const WaitEvent &) = delete;

    /**
     * Move constructor(deleted)
     */
    WaitEvent(const WaitEvent &&) = delete;

    /**
     * Assignment operator(deleted)
     */
    WaitEvent &operator=(const WaitEvent &) = delete;

    /**
     * Move assignment operator(deleted)
     */
    WaitEvent &operator=(const WaitEvent &&) = delete;

    /**
     * Waits until caller is signaled.
     */
    void Wait();

    /**
     * @brief Waits until caller is signaled.
     *
     * postWaitAct is executed before return to caller.
     * 
     * @tparam T Action type which is executed after wait operation( must has define operator() ).
     * operator signature:
     * @code{.cpp}
     * void operator()();
     * @endcode 
     * @param postWaitAct function object which is executed before return to caller.
     */
    template<typename T> void Wait(T &&postWaitAct);

    /**
     * Waits until caller is signaled or timeout is elapsed.
     *
     * @param milliSeconds timeout value(in millisecond).
     * 
     * @return True-successful wait the event,
     * False-the time-out interval has elapsed before another thread has attempted to signal the sleeping thread.
     */
    bool TimedWait(us32 milliSeconds);

    /**
     * @brief Waits until caller is signaled or timeout is elapsed.
     *
     * postWaitAct is executed before return to caller.
     * 
     * @tparam T Action type which is executed after wait operation( must has define operator() ).
     * operator signature:
     * @code{.cpp}
     * void operator()(bool); //same meaning as return value
     * @endcode 
     * @param milliSeconds timeout value(in millisecond).
     * @param postWaitAct function object which is executed before return to caller.
     * @return True-successful wait the event,
     * False-the time-out interval has elapsed before another thread has attempted to signal the sleeping thread.
     */
    template<typename T> bool TimedWait(uint32_t milliSeconds, T &&postWaitAct);

    /**
     * Signals the wait thread.
     * 
     * If there isn't any thread called @ref WaitEvent::Wait(),this function has no effects.
     */
    void Signal();

    /**
     * @brief Signals the wait thread.
     *
     * preSignalAct is executed before signal.
     * If there isn't any thread called @ref WaitEvent::Wait(),this function has no effects.
     * 
     * @tparam T Action type which is executed before signal operation( must has define operator() ).
     * operator signature:
     * @code{.cpp}
     * bool operator()(); //signal operation will not be executed when return false.
     * @endcode 
     * @param preSignalAct function object which is executed before signal operation.
     */
    template<typename T> void Signal(T &&preSignalAct);

    /**
     * @brief Reset event to normal status.
     * 
     */
    void Reset();

    /**
     * @brief Reset event to normal status.
     *
     * preResetAct is executed before reset operation.
     * 
     * @tparam T Action type which is executed before reset operation( must has define operator() ).
     * operator signature:
     * @code{.cpp}
     * void operator()();
     * @endcode 
     * @param preResetAct function object which is executed before reset operation.
     */
    template<typename T> void Reset(T &&preResetAct);

    /**
     * Query this wait evnet is error status.
     *
     * @return True if it is not in error status,otherwise false.
     */
    operator bool() const;

private:
    /**
     * Values that represent wait event status.
     */
    enum Status
    {
        Status_Normal,   /**< Normal status. */
        Status_Signaled, /**< Signaled status. */
        Status_Error	 /**< Error status. */
    };
    
#if defined(IS_WINDOWS)
    CONDITION_VARIABLE m_cond; /**< Handle of the windows CONDITION_VARIABLE. */

    CRITICAL_SECTION m_cs; /**< Handle of the windows CRITICAL_SECTION. */
#elif defined(IS_UNIX)
    pthread_mutex_t m_mutex; /**< Handle of the pthread mutex. */

    pthread_cond_t m_cond; /**< Handle of the pthread condition variable. */
#else
#error Unsupportted platform.
#endif

    volatile Status m_status; /**< Current status of this wait event. */
};

template<typename T> constexpr inline bool WaitEventExecPreAct(T &&preAct
	, typename std::enable_if<std::is_void<decltype(preAct())>::value, bool>::type  /*dummy*/ = true)
{
	preAct();
	return true;
}

template<typename T> inline bool WaitEventExecPreAct(T &&preAct
	, typename std::enable_if<!std::is_void<decltype(preAct())>::value, bool>::type  /*dummy*/ = true)
{
	return preAct();
}

#if defined(IS_WINDOWS)

template<typename T> void WaitEvent::Wait(T &&postWaitAct)
{
	TimedWait(INFINITE, [&](bool){ postWaitAct(); });
}

template<typename T> bool WaitEvent::TimedWait(us32 milliSeconds, T &&postWaitAct)
{
	EnterCriticalSection(&m_cs);
	bool result = true;
	while (m_status != Status_Signaled)
	{
		BOOL tempResult = SleepConditionVariableCS(&m_cond, &m_cs, milliSeconds);
		if (!tempResult && ::GetLastError() == ERROR_TIMEOUT)
		{
			result = false;
			break;
		}
	}
	if(result)
	{
		m_status = Status_Normal;
	}
	postWaitAct(result);
	LeaveCriticalSection(&m_cs);
	return result;
}

template<typename T> void WaitEvent::Signal(T &&preSignalAct)
{
	EnterCriticalSection(&m_cs);
	if(WaitEventExecPreAct(std::forward<T>(preSignalAct)))
	{
		m_status = Status_Signaled;
		WakeConditionVariable(&m_cond);
	}
	LeaveCriticalSection(&m_cs);
}

template<typename T> void WaitEvent::Reset(T &&preResetAct)
{
	EnterCriticalSection(&m_cs);
	preResetAct();
	m_status = Status_Normal;
	LeaveCriticalSection(&m_cs);
}

#elif defined(IS_UNIX)

#include <assert.h>
#include <errno.h>

template<typename T> void WaitEvent::Wait(T &&postWaitAct)
{
	pthread_mutex_lock(&m_mutex);
	while (m_status != Status_Signaled)
	{
		pthread_cond_wait(&m_cond, &m_mutex);
	}
	m_status = Status_Normal;
	postWaitAct();
	pthread_mutex_unlock(&m_mutex);
}

template<typename T> bool WaitEvent::TimedWait(uint32_t milliSeconds, T &&postWaitAct)
{
	timespec timeout;
    clock_gettime(CLOCK_MONOTONIC, &timeout);
	timeout.tv_sec += milliSeconds / 1000;
	timeout.tv_nsec += milliSeconds % 1000 * 1000000;
	if(timeout.tv_nsec >= 1000 * 1000000)
	{
		timeout.tv_nsec -= 1000 * 1000000;
		timeout.tv_sec += 1;
	}
	pthread_mutex_lock(&m_mutex);
	bool result = true;
	while (m_status != Status_Signaled)
	{
		int tempResult = pthread_cond_timedwait(&m_cond, &m_mutex, &timeout);
		if (tempResult == ETIMEDOUT)
		{
			assert(m_status != Status_Signaled);
			result = false;
			break;
		}
	}
	if(result)
	{
		m_status = Status_Normal;
	}
	postWaitAct(result);
	pthread_mutex_unlock(&m_mutex);
	return result;
}

template<typename T> void WaitEvent::Signal(T &&preSignalAct)
{
	pthread_mutex_lock(&m_mutex);
	if(WaitEventExecPreAct(std::forward<T>(preSignalAct)))
	{
		m_status = Status_Signaled;
		pthread_cond_signal(&m_cond);
	}
	pthread_mutex_unlock(&m_mutex);
}

template<typename T> void WaitEvent::Reset(T &&preResetAct)
{
	pthread_mutex_lock(&m_mutex);
	preResetAct();
	m_status = Status_Normal;
	pthread_mutex_unlock(&m_mutex);
}

#else
#error Unsupportted platform.
#endif

#endif /* EVENT_H */