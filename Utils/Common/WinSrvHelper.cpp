#include "WinSrvHelper.h"

#if defined(IS_WINDOWS)

#include <memory>
#include <functional>
#include "ResGuard.h"

DWORD WinSrvHelper::StopService(const char* srvName, bool waitForStopped)
{
	static auto svcClearFunc = [](SC_HANDLE handle) {CloseServiceHandle(handle); };
	ResGuard<SC_HANDLE, decltype(svcClearFunc)> schSCManager(OpenSCManager(
		nullptr,                 // local computer
		nullptr,                 // ServicesActive database 
		SC_MANAGER_ALL_ACCESS),  // full access rights 
		svcClearFunc);
	if (schSCManager.get() == nullptr)
	{
		return GetLastError();
	}

	ResGuard<SC_HANDLE, decltype(svcClearFunc)> schService(OpenServiceA(schSCManager.get(), srvName, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS)
		, svcClearFunc);
	if (schService.get() == nullptr)
	{
		return GetLastError();
	}
	return StopService(schSCManager.get(), schService.get(), waitForStopped);
}

DWORD WinSrvHelper::StopService(SC_HANDLE scmHandle, SC_HANDLE srvHandle, bool waitForStopped)
{
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	if (!QueryServiceStatusEx(srvHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
	{
		return GetLastError();
	}
	if (ssp.dwCurrentState == SERVICE_STOPPED || (ssp.dwCurrentState == SERVICE_STOP_PENDING && !waitForStopped))
	{
		return 0;
	}
	while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		Sleep(1000);
		if (!QueryServiceStatusEx(srvHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
		{
			return GetLastError();
		}
		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			return 0;
		}
	}
	DWORD result = StopDependentServices(scmHandle, srvHandle, true);
	if (result)
	{
		return result;
	}
	if (!ControlService(srvHandle, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp))
	{
		return GetLastError();
	}

	// Wait for the service to stop.
	if (waitForStopped)
	{
		while (ssp.dwCurrentState != SERVICE_STOPPED)
		{
			Sleep(1000);
			if (!QueryServiceStatusEx(srvHandle, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
			{
				return GetLastError();
			}

			if (ssp.dwCurrentState == SERVICE_STOPPED)
				break;
		}
	}
	return 0;
}

DWORD WinSrvHelper::StopDependentServices(SC_HANDLE scmHandle, SC_HANDLE srvHandle, bool waitForStopped)
{
	DWORD dwBytesNeeded;
	DWORD dwCount;
	ENUM_SERVICE_STATUS ess;

	// Pass a zero-length buffer to get the required buffer size.
	if (EnumDependentServices(srvHandle, SERVICE_ACTIVE, NULL, 0, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent
		// services, so do nothing.
		return 0;
	}
	else
	{
		DWORD err = GetLastError();
		if (err != ERROR_MORE_DATA)
			return err; // Unexpected error

		// Allocate a buffer for the dependencies.
		auto deleter = std::bind(::HeapFree, GetProcessHeap(), 0, std::placeholders::_1);
		std::unique_ptr<void,decltype(deleter)> tempDependencies(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded), deleter);
		LPENUM_SERVICE_STATUS lpDependencies = (LPENUM_SERVICE_STATUS)tempDependencies.get();

		if (!lpDependencies)
			return ERROR_OUTOFMEMORY;

		// Enumerate the dependencies.
		if (!EnumDependentServices(srvHandle, SERVICE_ACTIVE, lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
			return GetLastError();

		for (DWORD i = 0; i < dwCount; i++)
		{
			ess = *(lpDependencies + i);
			// Open the service.
			static auto svcClearFunc = [](SC_HANDLE handle) {CloseServiceHandle(handle); };
			ResGuard<SC_HANDLE, decltype(svcClearFunc)> hDepService(OpenService(scmHandle, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS)
				, svcClearFunc);

			if (!hDepService.get())
				return GetLastError();

			DWORD result = StopService(scmHandle, hDepService.get(), waitForStopped);
			if(result)
			{
				return result;
			}
		}
	}
	return 0;
}

#endif