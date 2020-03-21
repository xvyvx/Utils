#ifndef EVENT_H
#define EVENT_H

#include "../Common/CommonHdr.h"
#if defined(IS_WINDOWS)
#include <Windows.h>
#elif defined(IS_UNIX)
#include <pthread.h>
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
	 * Waits until caller is signaled or timeout is elapsed.
	 *
	 * @param milliSeconds timeout value(in millisecond).
	 * 
	 * @return True-successful wait the event,
	 * False-the time-out interval has elapsed before another thread has attempted to signal the sleeping thread.
	 */
	bool TimedWait(us32 milliSeconds);

	/**
	 * Signals the wait thread.
	 * 
	 * If there isn't any thread called @ref WaitEvent::Wait(),this function has no effects.
	 */
	void Signal();

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

#endif /* EVENT_H */