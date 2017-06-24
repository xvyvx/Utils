#include "WinSvcProgressReporter.h"

#if defined(IS_WINDOWS)

WinSvcProgressReporter::WinSvcProgressReporter(const char *svcName, LPHANDLER_FUNCTION_EX handler) :
	m_statusHandle(RegisterServiceCtrlHandlerExA(svcName, handler, nullptr)),m_status()
{
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_status.dwCurrentState = SERVICE_START_PENDING;
	m_status.dwControlsAccepted = 0;
	m_status.dwWin32ExitCode = NO_ERROR;
	m_status.dwServiceSpecificExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 30;
}

WinSvcProgressReporter::~WinSvcProgressReporter()
{
}

void WinSvcProgressReporter::IncProgress(int step, int waitHint)
{
	m_status.dwCheckPoint += step;
	m_status.dwWaitHint = waitHint;
	SetServiceStatus(m_statusHandle, &m_status);
}

void WinSvcProgressReporter::ReportNewStatus(Status newStatus, int waitHint, us32 exitCode)
{
	m_status.dwCurrentState = newStatus;
	m_status.dwWaitHint = waitHint;
	if (m_status.dwCurrentState == Status_Running || m_status.dwCurrentState == Status_Stoped)
	{
		m_status.dwCheckPoint = 0;
		if(m_status.dwCurrentState == Status_Stoped)
		{
			m_status.dwWin32ExitCode = exitCode;
		}
	}
	if (m_status.dwCurrentState == SERVICE_START_PENDING || m_status.dwCurrentState == SERVICE_STOP_PENDING)
	{
		m_status.dwControlsAccepted = 0;
	}
	else
	{
		m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	}
	SetServiceStatus(m_statusHandle, &m_status);
}

#endif