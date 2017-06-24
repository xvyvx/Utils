#include "WaitEvent.h"

#if defined(IS_WINDOWS)

WaitEvent::WaitEvent()
{
	m_evt = CreateEvent(NULL, FALSE, FALSE, NULL);
}

WaitEvent::~WaitEvent()
{
	CloseHandle(m_evt);
}

void WaitEvent::Wait()
{
	WaitForSingleObject(m_evt, INFINITE);
}

void WaitEvent::Signal()
{
	SetEvent(m_evt);
}

WaitEvent::operator bool() const
{
	return m_evt != NULL;
}

#elif defined(IS_UNIX)

WaitEvent::WaitEvent():m_status(Status_Normal)
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
	m_status = Status_Normal;
	pthread_mutex_unlock(&m_mutex);

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