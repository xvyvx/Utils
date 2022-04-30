#ifndef WINSRVHELPER_H
#define WINSRVHELPER_H

#include "CommonHdr.h"

#if defined(IS_WINDOWS)

#include <Windows.h>

/**
 * Windows service helper functions.
 */
class WinSrvHelper
{
public:
    /**
     * Stop specified windows service.
     *
     * @param	srvName		  	Service name.
     * @param	waitForStopped	Wether to block calling thread until service stopped.
     *
     * @return	0-successful，otherwise-error code.
     */
    static DWORD StopService(const char *srvName, bool waitForStopped);

    /**
     * Stop specified windows service.
     *
     * @param	scmHandle	  	Service manager handle.
     * @param	srvHandle	  	Service handle.
     * @note srvHandle parameter need SERVICE_STOP、SERVICE_QUERY_STATUS、SERVICE_ENUMERATE_DEPENDENTS rights.
     * @param	waitForStopped	Wether to block calling thread until service stopped.
     *
     * @return	0-successful，otherwise-error code.
     */
    static DWORD StopService(SC_HANDLE scmHandle, SC_HANDLE srvHandle, bool waitForStopped);

private:
    /**
     * Stop specified windows service's dependencies.
     *
     * @param	scmHandle	  	Service manager handle.
     * @param	srvHandle	  	Service handle.
     * @param	waitForStopped	Wether to block calling thread until service stopped.
     *
     * @return	0-successful，otherwise-error code.
     */
    static DWORD StopDependentServices(SC_HANDLE scmHandle, SC_HANDLE srvHandle, bool waitForStopped);
};

#endif

#endif /* WINSRVHELPER_H */