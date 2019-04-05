#ifndef IPROGRESSREPORTER_H
#define IPROGRESSREPORTER_H

#include "../Common/CommonHdr.h"

#if defined(IS_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/**
 * Service/Daemon running status.
 */
enum Status
{
	Status_StartPending = SERVICE_START_PENDING,
	Status_Running = SERVICE_RUNNING,
	Status_StopPending = SERVICE_STOP_PENDING,
	Status_Stoped = SERVICE_STOPPED
};

#else

/**
 * Service/Daemon running status.
 */
enum Status
{
	Status_StartPending,
	Status_Running,
	Status_StopPending,
	Status_Stoped
};

#endif

/**
 * Interface of service/daemon running status reporter.
 */
class UTILS_EXPORTS_API IProgressReporter
{
public:

	/**
	 * Default constructor
	 */
	IProgressReporter();

	/**
	 * Destructor
	 */
	virtual ~IProgressReporter();

	/**
	 * Increment current status progress.
	 *
	 * @param step Amount to increment by.
	 * @param waitHint System service manager wait times after this call(in milliseconds).
	 */
	virtual void IncProgress(int step, int waitHint) = 0;

	/**
	 * Reports a new service running status.
	 *
	 * @param newStatus The new status.
	 * @param waitHint System service manager wait times after this call(in milliseconds).
	 * @param exitCode  (Optional) The exit code if status is Status_Stoped.
	 */
	virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0) = 0;

	/**
	 * Query current status.
	 *
	 * @return Current status.
	 */
	virtual Status CurrentStatus() = 0;
};

/**
 * Empty implementation of IProgressReporter.
 * 
 * This implementation do nothing and @ref CurrentStatus() always return Status_Stoped.
 */
class UTILS_EXPORTS_API NullReport :public IProgressReporter
{
public:

	/**
	 * Default constructor
	 */
	NullReport():IProgressReporter()
	{
	}

	/**
	 * Destructor
	 */
	virtual ~NullReport()
	{
	}

	/**
	 * {@inheritDoc}
	 */
	virtual void IncProgress(int step, int waitHint) override
	{
	}

	/**
	 * {@inheritDoc}
	 */
	virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0) override
	{
	}

	/**
	 * {@inheritDoc}
	 */
	virtual Status CurrentStatus() override
	{
		return Status_Stoped;
	}
};

#endif /* IPROGRESSREPORTER_H */