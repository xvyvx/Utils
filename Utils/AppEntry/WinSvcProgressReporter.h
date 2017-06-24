#ifndef WINPROGRESSREPORTER_H
#define WINPROGRESSREPORTER_H

#include "IProgressReporter.h"

#if defined(IS_WINDOWS)

class UTILS_EXPORTS_API WinSvcProgressReporter :public IProgressReporter
{
public:
	WinSvcProgressReporter(const char *svcName,LPHANDLER_FUNCTION_EX handler);

	virtual ~WinSvcProgressReporter();

	virtual void IncProgress(int step, int waitHint) override;

	virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0) override;

	virtual Status CurrentStatus() override
	{
		return static_cast<Status>(m_status.dwCurrentState);
	}

	operator bool()
	{
		return m_statusHandle != nullptr;
	}

private:
	SERVICE_STATUS_HANDLE m_statusHandle;

	SERVICE_STATUS m_status;
};

#endif

#endif /* WINPROGRESSREPORTER_H */