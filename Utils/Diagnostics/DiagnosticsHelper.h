#ifndef DIAGNOSTICSHELPER_H
#define DIAGNOSTICSHELPER_H

#include "../Common/CommonHdr.h"

/**
 * Diagnostics utilities.
 */
class UTILS_EXPORTS_API DiagnosticsHelper
{
public:
	/**
	 * Gets CPU usage since previous call to this function.
	 *
	 * @return The CPU usage(0-100).
	 */
	static int GetCPUUsage();
};

#endif /* DIAGNOSTICSHELPER_H */