#include "UnixSignalHelper.h"

#if defined(IS_UNIX)

#include "signal.h"

log4cplus::Logger UnixSignalHelper::log = log4cplus::Logger::getInstance("UnixSignalHelper");

bool UnixSignalHelper::DiscardChildProcessExitInfo()
{
    struct sigaction discardAct;
    discardAct.sa_handler = SIG_IGN;
    sigemptyset(&discardAct.sa_mask);
    discardAct.sa_flags = SA_NOCLDWAIT;
    if(sigaction(SIGCHLD, &discardAct, NULL) != 0)
    {
        LOG4CPLUS_ERROR_FMT(log, "设置子进程退出信息丢弃行为失败（%d）。", errno);
        return false;
    }
    return true;
}

#endif