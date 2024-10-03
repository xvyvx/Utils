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
	Wait([](){});
}

bool WaitEvent::TimedWait(us32 milliSeconds)
{
	return TimedWait(milliSeconds, [](bool){});
}

void WaitEvent::Signal()
{
	Signal([](){});
}

void WaitEvent::Reset()
{
	Reset([](){});
}

WaitEvent::operator bool() const
{
	return m_status != Status_Error;
}

#elif defined(IS_UNIX)

WaitEvent::WaitEvent() : m_status(Status_Normal)
{
	pthread_condattr_t attr;
	if (pthread_mutex_init(&m_mutex, NULL) != 0)
	{
		m_status = Status_Error;
	}
	else if (pthread_condattr_init(&attr) != 0)
	{
		pthread_mutex_destroy(&m_mutex);
		m_status = Status_Error;
	}
	else if(
#if !defined(__APPLE__)
		pthread_condattr_setclock(&attr, CLOCK_MONOTONIC) != 0 || 
#endif
		pthread_cond_init(&m_cond, &attr) != 0)
	{
		pthread_condattr_destroy(&attr);
		pthread_mutex_destroy(&m_mutex);
		m_status = Status_Error;
	}
	else
	{
		pthread_condattr_destroy(&attr);
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
	Wait([](){});
}

bool WaitEvent::TimedWait(uint32_t milliSeconds)
{
	return TimedWait(milliSeconds, [](bool){});
}

void WaitEvent::Signal()
{
	Signal([](){});
}

void WaitEvent::Reset()
{
	Reset([](){});
}

WaitEvent::operator bool() const
{
	return m_status != Status_Error;
}

#else
#error Unsupportted platform.
#endif