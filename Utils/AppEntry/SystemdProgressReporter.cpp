#include "SystemdProgressReporter.h"

#if 0

#if defined(linux) || defined(__linux)

#include <algorithm>
#include <dlfcn.h>
#include <boost/format.hpp>

SystemdProgressReporter::SystemdProgressReporter() :m_libHandle(NULL), m_sd_notify(NULL), m_status(Status_StartPending)
{
	m_libHandle = dlopen("libsystemd.so.0", RTLD_LAZY);
	if(m_libHandle == NULL)
	{
		return;
	}
	m_sd_notify = (int(*)(int, const char*))dlsym(m_libHandle, "sd_notify");
	if(m_sd_notify == NULL)
	{
		dlclose(m_libHandle);
		m_libHandle = NULL;
	}
}

SystemdProgressReporter::SystemdProgressReporter(SystemdProgressReporter &&rhs) noexcept:m_libHandle(rhs.m_libHandle), m_sd_notify(rhs.m_sd_notify), m_status(rhs.m_status)
{
	rhs.m_libHandle = NULL;
	rhs.m_sd_notify = NULL;
	rhs.m_status = Status_StartPending;
}

SystemdProgressReporter::~SystemdProgressReporter()
{
	if(m_libHandle)
	{
		dlclose(m_libHandle);
	}
}

SystemdProgressReporter& SystemdProgressReporter::operator=(SystemdProgressReporter &&rhs) noexcept
{
	std::swap(m_libHandle, rhs.m_libHandle);
	std::swap(m_sd_notify, rhs.m_sd_notify);
	std::swap(m_status, rhs.m_status);
	return *this;
}

void SystemdProgressReporter::IncProgress(int step, int waitHint)
{
#if defined(linux) || defined(__linux)
	std::string msg = (boost::format("EXTEND_TIMEOUT_USEC=%d") % (waitHint * 1000)).str();
	m_sd_notify(0, msg.c_str());
#endif
}

void SystemdProgressReporter::ReportNewStatus(Status newStatus, int waitHint, us32 exitCode)
{
	if (newStatus == Status_StartPending)
	{
		std::string msg = (boost::format("EXTEND_TIMEOUT_USEC=%d") % (waitHint * 1000)).str();
		m_sd_notify(0, msg.c_str());
	}
	else if(newStatus == Status_Running)
	{
		m_sd_notify(0, "READY=1");
	}
	else if(newStatus == Status_StopPending)
	{
		std::string msg = (boost::format("STOPPING=1\nEXTEND_TIMEOUT_USEC=%d") % (waitHint * 1000)).str();
		m_sd_notify(0, msg.c_str());
	}
	m_status = newStatus;
}

#endif

#endif