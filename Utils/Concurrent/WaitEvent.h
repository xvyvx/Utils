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
	WaitEvent(const WaitEvent&) = delete;

	/**
	 * Move constructor(deleted)
	 */
	WaitEvent(const WaitEvent&&) = delete;

	/**
	 * Assignment operator(deleted)
	 */
	WaitEvent& operator=(const WaitEvent&) = delete;

	/**
	 * Move assignment operator(deleted)
	 */
	WaitEvent& operator=(const WaitEvent&&) = delete;

	/**
	 * Waits until caller is signaled.
	 */
	void Wait();

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
#if defined(IS_WINDOWS)
	HANDLE m_evt;   /**< Handle of the windows event. */
#elif defined(IS_UNIX)
	/**
	 * Values that represent wait event status.
	 */
	enum Status
	{
		Status_Normal, /**< Normal status. */
		Status_Signaled, /**< Signaled status. */
		Status_Error /**< Error status. */
	};

	pthread_mutex_t m_mutex;	/**< Handle of the pthread mutex. */

	pthread_cond_t m_cond;  /**< Handle of the pthread condition variable. */

	volatile Status m_status;   /**< Current status of this wait event. */
#else
#error Unsupportted platform.
#endif


};

#endif /* EVENT_H */