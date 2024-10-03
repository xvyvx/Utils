#include "DiagnosticsHelper.h"
#include <system_error>

#ifdef IS_WINDOWS

#include <Windows.h>

#define DEVIATION(x, y) (((static_cast<us64>((x).dwHighDateTime) << 32) | (x).dwLowDateTime) - ((static_cast<us64>((y).dwHighDateTime) << 32) | (y).dwLowDateTime))

namespace
{
    FILETIME GlobalIdleTime;

    FILETIME GlobalKernelTime;

    FILETIME GlobalUserTime;

    class DiagnosticsHelperInitializer
    {
    public:
        DiagnosticsHelperInitializer()
        {
            if(!GetSystemTimes(&GlobalIdleTime, &GlobalKernelTime, &GlobalUserTime))
            {
                throw std::system_error(std::error_code(::GetLastError(), std::system_category()));
            }
        }
    } Initializer;
}

int DiagnosticsHelper::GetProcessorUsage()
{
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
    {
        throw std::system_error(std::error_code(::GetLastError(), std::system_category()));
    }
    us64 idleValue = DEVIATION(idleTime, GlobalIdleTime);
    us64 kernelValue = DEVIATION(kernelTime, GlobalKernelTime);
    us64 userValue = DEVIATION(userTime, GlobalUserTime);
    us64 sysValue = kernelValue + userValue;
    if (sysValue == 0)
    {
        return 0;
    }
    int usage = static_cast<int>((sysValue - idleValue) * 100 / sysValue);
    GlobalIdleTime = idleTime;
    GlobalKernelTime = kernelTime;
    GlobalUserTime = userTime;
    return usage;
}

#elif defined(linux) || defined(__linux)

#include <stdio.h>

namespace
{
    us64 GlobalUserTime;

    us64 GlobalNiceTime;

    us64 GlobalSystemTime;

    us64 GlobalIdleTime;

    us64 GlobalIOWaitTime;

    us64 GlobalIrqTime;

    us64 GlobalSoftIrqTime;

    us64 GlobalStealstolenTime;

    us64 GlobalGuestTime;

    void GetProcessorUsageInfo()
    {
        FILE *statFile = fopen("/proc/stat", "r");
        if (statFile == NULL)
        {
            throw std::system_error(std::error_code(errno, std::system_category()));
        }
        char temp[24];
        if(fscanf(statFile, "%s%llu%llu%llu%llu%llu%llu%llu%llu%llu", temp, &GlobalUserTime, &GlobalNiceTime, &GlobalSystemTime, &GlobalIdleTime, &GlobalIOWaitTime
            , &GlobalIrqTime, &GlobalSoftIrqTime, &GlobalStealstolenTime, &GlobalGuestTime) == 0)
        {
            fclose(statFile);
            throw std::system_error(std::error_code(errno, std::system_category()));
        }
        fclose(statFile);
    }

    class DiagnosticsHelperInitializer
    {
    public:
        DiagnosticsHelperInitializer()
        {
            GetProcessorUsageInfo();
        }
    } Initializer;
}

int DiagnosticsHelper::GetProcessorUsage()
{
    us64 oldUserTime = GlobalUserTime;
    us64 oldNiceTime = GlobalNiceTime;
    us64 oldSystemTime = GlobalSystemTime;
    us64 oldIdleTime = GlobalIdleTime;
    us64 oldIOWaitTime = GlobalIOWaitTime;
    us64 oldIrqTime = GlobalIrqTime;
    us64 oldSoftIrqTime = GlobalSoftIrqTime;
    us64 oldStealstolenTime = GlobalStealstolenTime;
    us64 oldGuestTime = GlobalGuestTime;
    us64 oldTotal = oldUserTime + oldNiceTime + oldSystemTime + oldIdleTime + oldIOWaitTime + oldIrqTime + oldSoftIrqTime + oldStealstolenTime
        + oldGuestTime;
    GetProcessorUsageInfo();
    us64 total = GlobalUserTime + GlobalNiceTime + GlobalSystemTime + GlobalIdleTime + GlobalIOWaitTime + GlobalIrqTime + GlobalSoftIrqTime + GlobalStealstolenTime
        + GlobalGuestTime;
    us64 totalValue = total - oldTotal;
    if(totalValue == 0)
    {
        return 0;
    }
    int usage = static_cast<int>((totalValue - (GlobalIdleTime - oldIdleTime)) * 100 / totalValue);
    return usage;
}

#elif defined(__APPLE__)

#include <mach/mach_host.h>

namespace
{
    unsigned long long GlobalTotalTicks;

    unsigned long long GlobalIdleTicks;

    void GetProcessorUsageInfo()
    {
        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        kern_return_t result = host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, reinterpret_cast<host_info_t>(&cpuinfo)
            , &count);
        if(result == KERN_SUCCESS)
        {
            GlobalTotalTicks = 0;
            for(int i = 0; i < CPU_STATE_MAX; i++)
            {
                GlobalTotalTicks += cpuinfo.cpu_ticks[i];
            }
            GlobalIdleTicks = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
        }
        else
        {
            throw std::system_error(std::error_code(result, std::system_category()));
        }
    }

    class DiagnosticsHelperInitializer
    {
    public:
        DiagnosticsHelperInitializer()
        {
            GetProcessorUsageInfo();
        }
    } Initializer;
}

int DiagnosticsHelper::GetProcessorUsage()
{
    us64 oldGlobalTotalTicks = GlobalTotalTicks;
    us64 oldGlobalIdleTicks = GlobalIdleTicks;
    GetProcessorUsageInfo();
    us64 totalValue = GlobalTotalTicks - oldGlobalTotalTicks;
    us64 totalIdleValue = GlobalIdleTicks - oldGlobalIdleTicks;
    if(totalValue == 0)
    {
        return 0;
    }
    int usage = static_cast<int>((totalValue - totalIdleValue) * 100 / totalValue);
    return usage;
}

#else
#error Unsupportted platform
#endif