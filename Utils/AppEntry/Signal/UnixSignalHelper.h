#ifndef SYSSIGNALHELPER_H
#define SYSSIGNALHELPER_H

#include "../../Common/CommonHdr.h"

#if defined(IS_UNIX)

#include "../../Log/Log4cplusCustomInc.h"

/**
 * @brief Unix style signal utility functions.
 * 
 */
class UTILS_EXPORTS_API UnixSignalHelper
{
public:
    /**
     * @brief Set the child process exit status info to be discarded when child was exitted.
     *
     * @note Must be called before create child process operation(e.g. before fork call).
     * 
     * @return true-Set operation successful, false-Set operation failed.
     */
    static bool DiscardChildProcessExitInfo();

private:
    static log4cplus::Logger log;   /**< The logger. */
};

#endif

#endif /* SYSSIGNALHELPER_H */