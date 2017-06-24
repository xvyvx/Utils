#ifndef IPROGRESSREPORTER_H
#define IPROGRESSREPORTER_H

#include "../Common/CommonHdr.h"

#if defined(IS_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

enum Status
{
	Status_StartPending = SERVICE_START_PENDING,
	Status_Running = SERVICE_RUNNING,
	Status_StopPending = SERVICE_STOP_PENDING,
	Status_Stoped = SERVICE_STOPPED
};

#else

enum Status
{
	Status_StartPending,
	Status_Running,
	Status_StopPending,
	Status_Stoped
};

#endif

class UTILS_EXPORTS_API IProgressReporter
{
public:
	IProgressReporter();

	virtual ~IProgressReporter();

	virtual void IncProgress(int step, int waitHint) = 0;

	virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0) = 0;

	virtual Status CurrentStatus() = 0;
};

class UTILS_EXPORTS_API NullReport :public IProgressReporter
{
public:
	NullReport():IProgressReporter()
	{
	}

	virtual ~NullReport()
	{
	}

	virtual void IncProgress(int step, int waitHint) override
	{
	}

	virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0) override
	{
	}

	virtual Status CurrentStatus() override
	{
		return Status_Stoped;
	}
};

#endif /* IPROGRESSREPORTER_H */