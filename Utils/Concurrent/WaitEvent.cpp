#include "WaitEvent.h"

#if defined(IS_WINDOWS)

WaitEvent::WaitEvent() : m_status(Status_Normal)
{
	InitializeConditionVariable(&m_cond);
	InitializeCriticalSection(&m_cs);
}

WaitEvent::~WaitEvent()
{
}

void WaitEvent::Wait()
{
	TimedWait(INFINITE);
}

bool WaitEvent::TimedWait(us32 milliSeconds)
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
	LeaveCriticalSection(&m_cs);
	return result;
}

void WaitEvent::Signal()
{
	EnterCriticalSection(&m_cs);
	m_status = Status_Signaled;
	WakeConditionVariable(&m_cond);
	LeaveCriticalSection(&m_cs);
}

WaitEvent::operator bool() const
{
	return m_status != Status_Error;
}

#elif defined(IS_UNIX)

#include <errno.h>

WaitEvent::WaitEvent() : m_status(Status_Normal)
{
	if (pthread_mutex_init(&m_mutex, NULL) != 0)
	{
		m_status = Status_Error;
	}
	else if (pthread_cond_init(&m_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&m_mutex);
		m_status = Status_Error;
	}
}

WaitEvent::~WaitEvent()
{
	if (m_status != Status_Error)
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}
}

void WaitEvent::Wait()
{
	pthread_mutex_lock(&m_mutex);
	while (m_status != Status_Signaled)
	{
		pthread_cond_wait(&m_cond, &m_mutex);
	}
	pthread_mutex_unlock(&m_mutex);
}

bool WaitEvent::TimedWait(us32 milliSeconds)
{
	timespec timeout = {milliSeconds / 1000, milliSeconds % 1000 * 1000000};
	pthread_mutex_lock(&m_mutex);
	bool result = true;
	while (m_status != Status_Signaled)
	{
		int tempResult = pthread_cond_timedwait(&m_cond, &m_mutex, &timeout);
		if (tempResult == ETIMEDOUT)
		{
			result = false;
			break;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return result;
}

void WaitEvent::Signal()
{
	pthread_mutex_lock(&m_mutex);
	m_status = Status_Signaled;
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_mutex);
}

WaitEvent::operator bool() const
{
	return m_status != Status_Error;
}

#else
#error Unsupportted platform.
#endif