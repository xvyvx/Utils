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

class UTILS_EXPORTS_API WaitEvent
{
public:
	WaitEvent();

	~WaitEvent();

	WaitEvent(const WaitEvent&) = delete;

	WaitEvent(const WaitEvent&&) = delete;

	WaitEvent& operator=(const WaitEvent&) = delete;

	WaitEvent& operator=(const WaitEvent&&) = delete;

	void Wait();

	void Signal();

	operator bool() const;

private:
#if defined(IS_WINDOWS)
	HANDLE m_evt;
#elif defined(IS_UNIX)
	enum Status
	{
		Status_Normal,
		Status_Signaled,
		Status_Error
	};

	pthread_mutex_t m_mutex;

	pthread_cond_t m_cond;

	volatile Status m_status;
#else
#error Unsupportted platform.
#endif


};

#endif /* EVENT_H */