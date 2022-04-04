#ifndef WINPROGRESSREPORTER_H
#define WINPROGRESSREPORTER_H

#include "IProgressReporter.h"

#if defined(IS_WINDOWS)

/**
 * Windows service progress reporter.
 */
class UTILS_EXPORTS_API WinSvcProgressReporter :public IProgressReporter
{
public:

    /**
     * Constructor
     *
     * @param svcName Name of the service.
     * @param handler Pointer of windows service control function.
     */
    WinSvcProgressReporter(const char *svcName,LPHANDLER_FUNCTION_EX handler);

    /**
     * Destructor
     */
    virtual ~WinSvcProgressReporter();

    /**
     * {@inheritDoc}
     */
    virtual void IncProgress(int step, int waitHint) override;

    /**
     * {@inheritDoc}
     */
    virtual void ReportNewStatus(Status newStatus, int waitHint, us32 exitCode = 0) override;

    /**
     * {@inheritDoc}
     */
    virtual Status CurrentStatus() override
    {
        return static_cast<Status>(m_status.dwCurrentState);
    }

    /**
     * Query this report wether create successful.
     *
     * @return True if create successful,false otherwise.
     */
    operator bool()
    {
        return m_statusHandle != nullptr;
    }

private:
    SERVICE_STATUS_HANDLE m_statusHandle;   /**< Handle of the service status. */

    SERVICE_STATUS m_status;	/**< Current status. */
};

#endif

#endif /* WINPROGRESSREPORTER_H */