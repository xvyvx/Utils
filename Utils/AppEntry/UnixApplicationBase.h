#ifndef UNIXAPPLICATIONBASE_H
#define UNIXAPPLICATIONBASE_H

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <bsd/libutil.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include "../Common/RunTimeLibraryHelper.h"
#include "IProgressReporter.h"
#include "SystemdProgressReporter.h"
#include "Signal/SignalDispatcher.hpp"
#include "../Common/PathHelper.h"
#include "../Log/Log4cplusCustomInc.h"

/**
 * Base class for Unix systemV/Linux systemd based application.
 *
 * @tparam T Application specific type.
 * 	T must implete following function:
 * @code {.cpp}
 * 	void InitParameterDescriptions(boost::program_options::options_description_easy_init &desc);
 *  bool InitSignalDispatchDescriptors(); //It is not mandatory to implement this function
 * 	bool Startup(IProgressReporter&, const boost::program_options::variables_map&, log4cplus::Logger&)
 * 	bool Exit(IProgressReporter&, log4cplus::Logger&);
 * @endcode
 * @tparam Proc Instance of T.
 */
template<typename T, T *Proc> class ApplicationBase
{
public:
    /**
     * Defines an alias representing type of the self
     */
    using SelfType = ApplicationBase<T, Proc>;

    static constexpr size_t MaxServiceNameLength = 31;  /**< The maximum service name length */

    /**
     * Begin application exit process.
     *
     * Only begin a exit request,do not block to wait exit
     *
     * @returns 0-request successful,otherwise failed.
     */
    static us32 ExitApplication();

    /**
     * Gets the global instance.
     *
     * @return Global instance.
     */
    static SelfType& Instance()
    {
        return *AppInstance;
    }

    /**
     * Default constructor
     */
    ApplicationBase();

    /**
     * Destructor
     */
    ~ApplicationBase();

    /**
     * Main entry of application execution.
     *
     * @param argc Argc passed to main.
     * @param argv Argv passed to main.
     *
     * @return Execute result.
     */
    int Run(int argc, char *argv[]);

    /**
     * Query command arguments passed from command line.
     *
     * @return Command arguments.
     */
    const boost::program_options::variables_map& CmdVariables()
    {
        return m_vm;
    }

    /**
     * Query installed service name.
     *
     * @return Installed service name.
     */
    const std::string& ServiceName()
    {
        return m_serviceName;
    }

    /**
	 * @brief Adding custom signal processing descriptors
	 * 
	 * Nor thread safe, 
     * for multi-threaded programs, needs to be called before the program creates any other threads, 
     * otherwise the calling behaviour is undefined.
	 * 
	 * @tparam SignalDescriptorType Custom signal processing descriptor types
	 * @param descriptors Customised signal processing descriptors
	 * @return bool true-add success, false-add failure 
     * (possible outcomes on failure: add partial handler success, failure results in clearing all signal 
     * distribution processing)
	 */
	template<typename... SignalDescriptorType> bool AddSignalDispatchDescriptors(
        SignalDescriptorType&&... descriptors);

private:
    /**
     * Defines an alias representing the internal handler.
     */
    using ParamHandler = int (SelfType::*)();

    /**
     * Values that represent daemon types
     */
    enum DaemonType
    {
        DaemonType_None, /**< Running application is not installed a daemon. */
        DaemonType_SystemV, /**< Running application is installed as a systemV daemon. */
        DaemonType_Systemd /**< Running application is installed as a systemd daemon. */
    };

    /**
     * Values that represent SysV daemon Initialize results
     */
    enum SysVDaemonIniResult
    {
        SysVDaemonIniResult_DaemonIniSuccessful = 0,	/**<Constant representing the SysV daemon initialize successful */
        SysVDaemonIniResult_Successful = 0, /**< Constant representing the parent process receive child successful notify */
        SysVDaemonIniResult_FirstChildSuccessful,   /**< Constant representing the first child initialize successful option */
        SysVDaemonIniResult_AlreadyExec,	/**< Constant representing the daemon process already execute */
        SysVDaemonIniResult_OpenPIDFileFailed,  /**< Constant representing the parent process open PID file failed */
        SysVDaemonIniResult_OpenIPCFailed,  /**< Constant representing open ipc failed */
        SysVDaemonIniResult_FirstForkFailed,	/**< Constant representing first fork call failed */
        SysVDaemonIniResult_ParentReadPipeFailed,   /**< Constant representing the parent process read pipe failed */
        SysVDaemonIniResult_SetsidFailed,   /**< Constant representing setsid call failed */
        SysVDaemonIniResult_SecondForkFailed,   /**< Constant representing second fork call failed */
        SysVDaemonIniResult_DaemonOpenNullFileFailed,   /**< Constant representing the daemon process open null file descriptor failed */
        SysVDaemonIniResult_DaemonRedirectFailed,	/**< Constant representing the daemon process redirect stdin,stdout,stderr failed */
        SysVDaemonIniResult_DaemonInitFailed,   /**< Constant representing the daemon process initialize failed */
        SysVDaemonIniResult_DaemonWritePIDFileFailed,   /**< Constant representing the daemon process write PID file failed */
        SysVDaemonIniResult_DaemonStartupFailed /**< Constant representing the daemon process call startup failed */
    };

    using SignalDispatcherType = SignalDispatcher<SIGTERM, SIGINT, SIGQUIT, SIGHUP>; //Signal dispatcher type

    /**
     * Copy constructor(deleted)
     */
    ApplicationBase(const ApplicationBase&) = delete;

    /**
     * Move constructor(deleted)
     */
    ApplicationBase(ApplicationBase&&) = delete;

    /**
     * Assignment operator(deleted)
     *
     * @return Equal to *this.
     */
    ApplicationBase& operator=(const ApplicationBase&) = delete;

    /**
     * Move assignment operator(deleted)
     *
     * @return Equal to *this.
     */
    ApplicationBase& operator=(ApplicationBase&&) = delete;

    /**
     * Help message print handler.
     *
     * @return Execute result.
     */
    int HelpFunc();

    /**
     * Install daemon handler.
     *
     * @return Execute result.
     */
    int InstallDaemon();

    /**
     * Uninstall daemon handler.
     *
     * @return Execute result.
     */
    int UninstallDaemon();

    /**
     * Uninstall daemon implementation
     *
     * @param svcName Name of the daemon.
     * @param svcType Type of the daemon.
     *
     * @return Execute result.
     */
    int UninstallDaemonImpl(const std::string &svcName, DaemonType svcType);

    /**
     * SystemV daemon execution entry.
     *
     * @return Execute result.
     */
    int RunSvcSystemV();

    /**
     * Systemd daemon execution entry.
     *
     * @return Execute result.
     */
    int RunSvcSystemd();

    /**
     * Normal execution entry(run as a normal console application).
     *
     * @return Execute result.
     */
    int RunNormal();

    /**
     * Initializes execution environment.
     *
     * @returns Non nullptr if it succeeds, nullptr if it fails.
     */
    std::unique_ptr<log4cplus::ConfigureAndWatchThread> Initialize();

    /**
     * Initializes the daemon install type and name.
     */
    void InitializeDaemonName();

    /**
     * SysV daemon initialize.
     *
     * @param [out] writePipe The pipe file descriptor used to send notify message to parent process.
     *
     * @returns Pipe file descriptor.
     */
    int SysVDaemonInitialize(int* writePipe);

    /**
     * SysV daemon child process initialize.
     *
     * @param writePipe The pipe file descriptor used to send notify message to parent process.
     *
     * @returns Pipe file descriptor.
     */
    int SysVDaemonChildInitialize(int writePipe);

    /**
	 * @brief Initialize custom command line parameter descriptor
	 * 
	 * @tparam ProcType Application specific type.
	 * @tparam *ProcInstance Instance of ProcType
	 * @param desc command line parameter descriptor
	 */
	template<typename ProcType = T, ProcType *ProcInstance = Proc> void InitCustomParameterDescriptions(
		boost::program_options::options_description_easy_init &desc
		, typename std::enable_if
		<
			std::is_void
			<
				decltype(ProcInstance->InitParameterDescriptions(*new boost::program_options::options_description_easy_init(nullptr)))
			>::value
			, int
		>::type);
	
	/**
	 * @brief Initialize custom command line parameter descriptor
	 * 
	 * @param ... No use
	 */
	void InitCustomParameterDescriptions(...);

	/**
	 * @brief Initialising custom signal processing
	 * 
	 * @tparam ProcType Application specific type.
	 * @tparam *ProcInstance Instance of ProcType
	 * @return constexpr bool true-execute successful，false-execute failed
	 */
	template<typename ProcType = T, ProcType *ProcInstance = Proc> bool InitializeCustomSignalHandler(
		typename std::enable_if<std::is_integral<decltype(ProcInstance->InitSignalDispatchDescriptors())>::value, int>::type);

	/**
	 * @brief Initialising custom signal processing
	 * 
	 * @param ... No use
	 * @return constexpr bool true-execute successful，false-execute failed
	 */
	constexpr bool InitializeCustomSignalHandler(...);

    /**
	 * @brief Handler for receiving the exit signal.
	 * 
	 */
	void OnExitSignal();

    /**
     * Writes a notify message to pipe.
     *
     * @param pipe The pipe file descriptor used to send notify message to parent process.
     * @param msg  The notify message.
     */
    static void WriteNotifyPipeMsg(int pipe, us8 msg);

    /**
     * Writes daemon install information file.
     *
     * @param content File content.
     * @param path Location of the file.
     *
     * @return True if it succeeds, false if it fails.
     */
    static bool WriteFile(const std::string &content, const std::string &path);

    /**
     * Reads daemon install information file.
     *
     * @param path Location of the file.
     *
     * @return Content of the file or empty if read failed.
     */
    static std::string ReadFile(const std::string &path);

    /**
     * Gets a child process execute result.
     *
     * @param exitCode The system function return value.
     * @param ctx	   log context text.
     *
     * @return True if child process execute succeeds, false if it fails.
     */
    static bool GetChildExecResult(int exitCode, const char *ctx);

    /**
     * Removes the daemon install information file.
     *
     * @param path Location of the file.
     *
     * @return True if it succeeds, false if it fails.
     */
    static bool RemoveFile(const std::string &path);

    /**
     * Delay execute when running as daemon(unit:seconds).
     *
     * @param reporter progress report obj.
     */
    static void DelayExec(IProgressReporter *reporter);

    /**
     * A parameter entry.
     */
    struct ParamEntry
    {
        const char *m_paramName;	/**< Name of the parameter. */

        ParamHandler m_handler; /**< The handler. */
    };

    static constexpr size_t EntrySize = 5;  /**< Size of the entry */

    //如果之保留内联的初始化写法，clang编译通过，但是链接时出现未定义错误（odr use?），gcc、vc++（WinApplicationBase）无问题，所以仍需要外部定义
    //同时由于如果外部没有定义时出现了链接时未定义错误，因此说明constexpr未起作用（odr use），所以这里虽然是编译时定义的，但是加不加constexpr都无所谓
    static ParamEntry ParamEntries[EntrySize] = 
    {
        { "help",&SelfType::HelpFunc },
        { "install",&SelfType::InstallDaemon },
        { "uninstall",&SelfType::UninstallDaemon },
        { "svc-systemV",&SelfType::RunSvcSystemV },
        { "svc-systemd",&SelfType::RunSvcSystemd }
    };

    boost::program_options::options_description m_optionsDesc;  /**< Information describing the command line arguments. */

    boost::program_options::variables_map m_vm; /**< Command line arguments. */

    pidfh *m_pidFile;   /**< The PID file pointer if execute as systemV daemon. */

    std::string m_serviceName;  /**< Installed service name. */

    DaemonType m_svcType;   /**< Installed daemon type. */

    std::unique_ptr<IProgressReporter> m_reporter;  /**< Pointer of progress reporter. */

    SignalDispatcherType m_sigDispatcher; /**< Signal dispatch manager. */

    static SelfType *AppInstance;   /**< Global instance. */

    static log4cplus::Logger log;   /**< The logger. */
};

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::SelfType *ApplicationBase<T, Proc>::AppInstance = nullptr;

template<typename T, T *Proc> log4cplus::Logger ApplicationBase<T, Proc>::log = log4cplus::Logger::getInstance(T::LogName());

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::ParamEntry ApplicationBase<T, Proc>::ParamEntries[EntrySize];

template<typename T, T *Proc> ApplicationBase<T, Proc>::ApplicationBase() : m_optionsDesc("Options"), m_vm(), m_pidFile(nullptr), m_serviceName(), m_svcType(DaemonType_None)
    , m_reporter(nullptr), m_sigDispatcher()
{
    if (AppInstance)
    {
        abort();
    }
    AppInstance = this;
    decltype(m_optionsDesc.add_options()) options = m_optionsDesc.add_options();
    options("help,h", "produce help message");
    options("install,i", boost::program_options::value<std::string>(), "install programme as unix daemon(alternative values:systemV,systemd)");
    options("name,n", boost::program_options::value<std::string>(), "optional,set daemon name when install unix daemon");
    options("svc-desc", boost::program_options::value<std::string>(), "optional,set service description when install unix daemon");
    options("start-order", boost::program_options::value<int>(), "optional,set daemon script start order number(legacy systemV only)");
    options("stop-order", boost::program_options::value<int>(), "optional,set daemon script stop order number(legacy systemV only)");
    options("svc-depends", boost::program_options::value< std::vector<std::string> >(), "optional,set daemon dependencies when install unix daemon(non legacy systemV or systemd only)");
    options("exec-delay", boost::program_options::value<unsigned int>(), "optional,set daemon execute delay time when startup(in seconds).");
    options("uninstall,u", "uninstall installed unix daemon");
    options("svc-systemV", "run programme as unix daemon(used by daemon script,don't use directly)");
    options("svc-systemd", "run programme as unix daemon(used by systemd service manager,don't use directly)");
    options("svc-exec-delay", boost::program_options::value<unsigned int>(), "Unix daemon execute delay time(in seconds,used by daemon script or systemd service manager,don't use directly)");
    Proc->InitParameterDescriptions(options);
}

template<typename T, T *Proc> ApplicationBase<T, Proc>::~ApplicationBase()
{
    if (m_pidFile)
    {
        pidfile_remove(m_pidFile);
        m_pidFile = nullptr;
    }
}

template <typename T, T *Proc> template<typename... SignalDescriptorType> bool
    ApplicationBase<T, Proc>::AddSignalDispatchDescriptors(SignalDescriptorType&&... descriptors)
{
	return m_sigDispatcher.AddDispatchedSignals(std::forward<SignalDescriptorType>(descriptors)...);
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::Run(int argc, char *argv[])
{
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_optionsDesc), m_vm);
    boost::program_options::notify(m_vm);
    bool hitted = false;
    int ret = 0;
    for (size_t i = 0; i < EntrySize; ++i)
    {
        if (m_vm.count(ParamEntries[i].m_paramName))
        {
            hitted = true;
            ret = (this->*ParamEntries[i].m_handler)();
            break;
        }
    }
    if (!hitted)
    {
        ret = RunNormal();
    }
    return ret;
}

template<typename T, T *Proc> us32 ApplicationBase<T, Proc>::ExitApplication()
{
    return kill(getpid(), SIGTERM);
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunNormal()
{
    sigset_t signalSet;
    std::unique_ptr<log4cplus::ConfigureAndWatchThread> logWatchThread = Initialize(&signalSet);
    if (!logWatchThread)
    {
        return 1;
    }
    int ret = 0;
    m_reporter.reset(new NullReport());
    if (Proc->Startup(*m_reporter, m_vm, log))
    {
        int signalNum = -1;
		while (m_sigDispatcher.DispatchSignals(signalNum) != SignalDispatcherType::ExitSignalDispatchSuccessful) { }
    }
    else
    {
        ret = 1;
        Proc->Exit(*m_reporter, log);
    }
    return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunSvcSystemV()
{
    InitializeDaemonName();
    std::string pidFilePath("/var/run/");
    pidFilePath.append(m_serviceName);
    pidFilePath.append(".pid");
    m_pidFile = pidfile_open(pidFilePath.c_str(), 0644, nullptr);
    if (m_pidFile == NULL)
    {
        return errno == EEXIST ? SysVDaemonIniResult_AlreadyExec : SysVDaemonIniResult_OpenPIDFileFailed;
    }

    int notifyPipe;
    int result = SysVDaemonInitialize(&notifyPipe);
    if (result != SysVDaemonIniResult_DaemonIniSuccessful)
    {
        return result;
    }
    auto pipeDeleter = [](int* pipeDesc) {close(*pipeDesc); };
    std::unique_ptr<int, decltype((pipeDeleter))> pipeGuard(&notifyPipe, pipeDeleter);

    std::unique_ptr<log4cplus::ConfigureAndWatchThread> logWatchThread = Initialize();
    if (!logWatchThread)
    {
        WriteNotifyPipeMsg(notifyPipe, SysVDaemonIniResult_DaemonInitFailed);
        return SysVDaemonIniResult_DaemonInitFailed;
    }
    int ret = pidfile_write(m_pidFile);
    if (ret == -1)
    {
        WriteNotifyPipeMsg(notifyPipe, SysVDaemonIniResult_DaemonWritePIDFileFailed);
        LOG4CPLUS_ERROR_FMT(log, "写入Pid文件失败，退出执行，Daemon名称：%s，错误代码：%d", m_serviceName.c_str(), errno);
        ret = SysVDaemonIniResult_DaemonWritePIDFileFailed;
    }

    if (ret == 0)
    {
        m_reporter.reset(new NullReport());
        DelayExec(m_reporter.get());
        if (Proc->Startup(*m_reporter, m_vm, log))
        {
            WriteNotifyPipeMsg(notifyPipe, SysVDaemonIniResult_Successful);
            pipeGuard.reset();
            int signalNum = 1;
			while (m_sigDispatcher.DispatchSignals(signalNum) != SignalDispatcherType::ExitSignalDispatchSuccessful)
            { }
        }
        else
        {
            WriteNotifyPipeMsg(notifyPipe, SysVDaemonIniResult_DaemonStartupFailed);
            ret = SysVDaemonIniResult_DaemonStartupFailed;
            Proc->Exit(*m_reporter, log);
        }
    }
    return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunSvcSystemd()
{
    InitializeDaemonName();
    SystemdProgressReporter *report = new SystemdProgressReporter();
    m_reporter.reset(report);
    if (!*report)
    {
        return 1;
    }
    m_reporter->ReportNewStatus(Status_StartPending, 30000);
    std::unique_ptr<log4cplus::ConfigureAndWatchThread> logWatchThread = Initialize();
    if (!logWatchThread)
    {
        m_reporter->ReportNewStatus(Status_StopPending, 30000, 1);
        return 1;
    }
    DelayExec(report);
    int ret = 0;
    if (Proc->Startup(*m_reporter, m_vm, log))
    {
        m_reporter->ReportNewStatus(Status_Running, 0);
        int signalNum = 1;
		while (m_sigDispatcher.DispatchSignals(signalNum) != SignalDispatcherType::ExitSignalDispatchSuccessful)
        { }
    }
    else
    {
        m_reporter->ReportNewStatus(Status_StopPending, 30000, 1);
        ret = 1;
        Proc->Exit(*m_reporter, log);
    }
    return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::HelpFunc()
{
    std::cout << m_optionsDesc << std::endl;
    return 0;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::InstallDaemon()
{
    InitializeDaemonName();
    std::unique_ptr<log4cplus::ConfigureAndWatchThread> logWatchThread = Initialize();
    if (!logWatchThread)
    {
        return 1;
    }
    if (m_svcType != DaemonType_None)
    {
        if (UninstallDaemonImpl(m_serviceName, m_svcType) != 0)
        {
            LOG4CPLUS_ERROR(log, "卸载已存在服务失败。");
            return 1;
        }
    }

    std::string svcName;
    if (m_vm["name"].empty())
    {
        svcName = T::SvcName();
    }
    else
    {
        svcName = m_vm["name"].template as<std::string>();
    }
    if (boost::algorithm::any_of(svcName.begin(), svcName.end(), boost::is_any_of(" /\\")) || svcName.size() > MaxServiceNameLength)
    {
        LOG4CPLUS_ERROR(log, "指定的服务名含有非法字符或长度过长。");
        return 1;
    }
    std::string svcDesc;
    if (m_vm["svc-desc"].empty())
    {
        svcDesc = T::SvcDescription();
    }
    else
    {
        svcDesc = m_vm["svc-desc"].template as<std::string>();
    }

    std::string installStr = m_vm["install"].template as<std::string>();
    if(installStr != "systemV" && installStr != "systemd")
    {
        LOG4CPLUS_ERROR(log, "指定了无效的服务安装方式。");
        return 1;
    }
    int startOrder = 80, stopOrder = 20;
    std::string depends("");
    if (installStr == "systemV")
    {
        if (!m_vm["start-order"].empty())
        {
            startOrder = m_vm["start-order"].template as<int>();
        }
        if (!m_vm["stop-order"].empty())
        {
            stopOrder = m_vm["stop-order"].template as<int>();
        }
    }
    if (!m_vm["svc-depends"].empty())
    {
        std::vector<std::string> temp = m_vm["svc-depends"].as< std::vector<std::string> >();
        for (const auto &str : temp)
        {
            depends.push_back(' ');
            depends.insert(depends.end(), str.c_str(), str.c_str() + str.size());
        }
    }

    char delayTimeStr[48] = { 0 };
    if (!m_vm["exec-delay"].empty())
    {
        unsigned int delayTime = m_vm["exec-delay"].as<unsigned int>();
        if (delayTime > 0 && delayTime < 9999)
        {
            RunTimeLibraryHelper::SPrintF(delayTimeStr, sizeof(delayTimeStr) / sizeof(char), " --svc-exec-delay=%u", delayTime);
        }
        else if (delayTime != 0)
        {
            LOG4CPLUS_ERROR_FMT(log, "延迟执行时间范围错误，丢弃改参数，参数值：%u。", delayTime);
        }
    }

    //TODO 自定义路径
    std::string path = PathHelper::AppDeployPath();
    std::string daemonPath = PathHelper::AppExecutablePath();
    std::string installScriptPath = path;
    PathHelper::Combine(&installScriptPath, "Configuration", (boost::format("install.sh %s \"%s\" \"%s\" \"%s\" \"%s\" %02d %02d \"%s\" \"%s\"") % installStr % svcName % svcDesc
        % daemonPath % path % startOrder % stopOrder % depends % delayTimeStr).str().c_str(), nullptr);
    int exitCode = system(installScriptPath.c_str());
    if (!GetChildExecResult(exitCode, "执行安装脚本"))
    {
        return 1;
    }
    //TODO 自定义路径
    std::string nameFilePath = path;
    PathHelper::Combine(&nameFilePath, "Configuration", "svcname.txt", nullptr);
    std::string fileContent(svcName);
    fileContent.append("\n");
    fileContent.append(installStr);
    if(!WriteFile(fileContent, nameFilePath))
    {
        LOG4CPLUS_ERROR(log, "保存服务名失败。");
        return 1;
    }
    LOG4CPLUS_INFO(log, "服务安装过程成功完成。");
    return 0;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::UninstallDaemon()
{
    InitializeDaemonName();
    std::unique_ptr<log4cplus::ConfigureAndWatchThread> logWatchThread = Initialize();
    if (!logWatchThread)
    {
        return 1;
    }
    if (m_serviceName.empty() || (m_svcType != DaemonType_SystemV && m_svcType != DaemonType_Systemd))
    {
        LOG4CPLUS_ERROR(log, "读取已安装服务信息失败。");
        return 1;
    }
    return UninstallDaemonImpl(m_serviceName, m_svcType);
}

template<typename T,T *Proc> int ApplicationBase<T,Proc>::UninstallDaemonImpl(const std::string &svcName, DaemonType svcType)
{
    //TODO 自定义路径
    std::string path = PathHelper::AppDeployPath();
    std::string nameFilePath = path;
    PathHelper::Combine(&nameFilePath, "Configuration", "svcname.txt", nullptr);

    //TODO 自定义路径
    std::string uninstallScriptPath = path;
    PathHelper::Combine(&uninstallScriptPath, "Configuration", (boost::format("uninstall.sh %s %s") % (svcType == DaemonType_SystemV ? "systemV" : "systemd") % svcName).str().c_str(), nullptr);
    int exitCode = system(uninstallScriptPath.c_str());
    if (!GetChildExecResult(exitCode, "执行卸载脚本"))
    {
        return 1;
    }

    RemoveFile(nameFilePath);
    LOG4CPLUS_INFO(log, "服务删除过程成功完成。");
    return 0;
}

template<typename T,T *Proc> std::unique_ptr<log4cplus::ConfigureAndWatchThread> ApplicationBase<T,Proc>::Initialize()
{
    if (chdir(PathHelper::AppDeployPath().c_str()) != 0)
    {
        return std::unique_ptr<log4cplus::ConfigureAndWatchThread>();
    }

    //TODO 自定义路径
    std::string path = PathHelper::AppDeployPath();
    PathHelper::Combine(&path, "Configuration", "log4cplus.properties", 0);
    std::unique_ptr<log4cplus::ConfigureAndWatchThread> watchThread(new log4cplus::ConfigureAndWatchThread(path, 6000));

    if(!m_sigDispatcher.AddIgnoredSignals({ SIGTTIN, SIGTTOU }) || !m_sigDispatcher.AddDispatchedSignals(
		MakeSignalDescriptor(SIGTERM, [this]() mutable { OnExitSignal(); }, SignalDispatchType_Direct)
		, MakeSignalDescriptor(SIGINT, [this]() mutable { OnExitSignal(); }, SignalDispatchType_Direct)
		, MakeSignalDescriptor(SIGQUIT, [this]() mutable { OnExitSignal(); }, SignalDispatchType_Direct)
		, MakeSignalDescriptor(SIGHUP, [this]() mutable { OnExitSignal(); }, SignalDispatchType_Direct))
		|| !InitializeCustomSignalHandler(0))
	{
        watchThread.reset();
		return std::unique_ptr<log4cplus::ConfigureAndWatchThread>();
	}

    return watchThread;
}

template<typename T, T* Proc> void ApplicationBase<T, Proc>::InitializeDaemonName()
{
    //TODO 自定义路径
    std::string nameFilePath = PathHelper::AppDeployPath();
    PathHelper::Combine(&nameFilePath, "Configuration", "svcname.txt", nullptr);
    std::istringstream fileStream(ReadFile(nameFilePath));
    std::string tempType;
    fileStream >> m_serviceName >> tempType;
    if (m_serviceName.empty())
    {
        m_serviceName = T::SvcName();
    }
    if (tempType == "systemV")
    {
        m_svcType = DaemonType_SystemV;
    }
    else if (tempType == "systemd")
    {
        m_svcType = DaemonType_Systemd;
    }
}

template<typename T, T* Proc> int ApplicationBase<T, Proc>::SysVDaemonInitialize(int* writePipe)
{
    int pipeDesc[2];
    if (pipe(pipeDesc) != 0)
    {
        return SysVDaemonIniResult_OpenIPCFailed;
    }
    pid_t result = fork();
    if (result == -1)
    {
        close(pipeDesc[0]);
        close(pipeDesc[1]);
        return SysVDaemonIniResult_FirstForkFailed;
    }
    if (result != 0)
    {
        close(pipeDesc[1]);
        pidfile_close(m_pidFile);
        m_pidFile = nullptr;
        int exitStatus;
        waitpid(result, &exitStatus, 0);
        us8 childResult = 0;
        ssize_t readBytes = read(pipeDesc[0], &childResult, 1);
        close(pipeDesc[0]);
        return readBytes == -1 || readBytes == 0 ? SysVDaemonIniResult_ParentReadPipeFailed : childResult;
    }
    else
    {
        close(pipeDesc[0]);
        *writePipe = pipeDesc[1];
        return SysVDaemonChildInitialize(pipeDesc[1]);
    }
}

template<typename T, T* Proc> int ApplicationBase<T, Proc>::SysVDaemonChildInitialize(int writePipe)
{
    pid_t result = setsid();
    if (result == -1)
    {
        WriteNotifyPipeMsg(writePipe, SysVDaemonIniResult_SetsidFailed);
        close(writePipe);
        return SysVDaemonIniResult_SetsidFailed;
    }
    result = fork();
    if (result == -1)
    {
        WriteNotifyPipeMsg(writePipe, SysVDaemonIniResult_SecondForkFailed);
        close(writePipe);
        return SysVDaemonIniResult_SecondForkFailed;
    }
    if (result != 0)
    {
        close(writePipe);
        pidfile_close(m_pidFile);
        m_pidFile = nullptr;
        return SysVDaemonIniResult_FirstChildSuccessful;
    }
    else
    {
        int nullDesc = open("/dev/null", O_RDWR);
        if (nullDesc == -1)
        {
            WriteNotifyPipeMsg(writePipe, SysVDaemonIniResult_DaemonOpenNullFileFailed);
            close(writePipe);
            return SysVDaemonIniResult_DaemonOpenNullFileFailed;
        }
        if (dup2(nullDesc, STDIN_FILENO) == -1 || dup2(nullDesc, STDOUT_FILENO) == -1 || dup2(nullDesc, STDERR_FILENO) == -1)
        {
            WriteNotifyPipeMsg(writePipe, SysVDaemonIniResult_DaemonRedirectFailed);
            close(writePipe);
            close(nullDesc);
            return SysVDaemonIniResult_DaemonRedirectFailed;
        }
        close(nullDesc);
        umask(0);
    }
    return SysVDaemonIniResult_DaemonIniSuccessful;
}

template <typename T, T *Proc> template<typename ProcType, ProcType *ProcInstance>
void ApplicationBase<T, Proc>::InitCustomParameterDescriptions(boost::program_options::options_description_easy_init &desc
	, typename std::enable_if
	<
		std::is_void
		<
			decltype(ProcInstance->InitParameterDescriptions(*new boost::program_options::options_description_easy_init(nullptr)))
		>::value
		, int
	>::type)
{
	Proc->InitParameterDescriptions(desc);
}

template <typename T, T *Proc> void ApplicationBase<T, Proc>::InitCustomParameterDescriptions(...)
{
}

template <typename T, T *Proc> template<typename ProcType, ProcType *ProcInstance> bool ApplicationBase<T, Proc>::InitializeCustomSignalHandler(
	typename std::enable_if<std::is_integral<decltype(ProcInstance->InitSignalDispatchDescriptors())>::value, int>::type)
{
	return ProcInstance->InitSignalDispatchDescriptors();
}

template <typename T, T *Proc> constexpr bool ApplicationBase<T, Proc>::InitializeCustomSignalHandler(...)
{
	return true;
}

template <typename T, T *Proc> void ApplicationBase<T, Proc>::OnExitSignal()
{
	m_reporter->ReportNewStatus(Status_StopPending, 30000);
	Proc->Exit(*m_reporter, log);
}

template<typename T, T* Proc> void ApplicationBase<T, Proc>::WriteNotifyPipeMsg(int pipe, us8 msg)
{
    write(pipe, &msg, 1);
}

template<typename T, T *Proc> bool ApplicationBase<T, Proc>::WriteFile(const std::string &content,const std::string &path)
{
    std::ofstream fs;
    fs.open(path, std::ios::out | std::ios::trunc);
    bool ret = true;
    if (fs)
    {
        fs << content;
        if (fs&&fs.flush())
        {
            fs.close();
        }
        else
        {
            RemoveFile(path);
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}

template<typename T, T *Proc> std::string ApplicationBase<T, Proc>::ReadFile(const std::string &path)
{
    std::ifstream fs;
    fs.open(path, std::ios::in | std::ios::binary);
    std::string ret;
    fs.seekg(0, std::ios::end);
    std::streamsize length = fs.tellg();
    fs.seekg(0, std::ios::beg);
    if (fs&&length)
    {
        ret.assign(length, 0);
        fs.read(const_cast<char*>(ret.data()), length);
        fs.close();
    }
    return ret;
}

template<typename T, T *Proc> bool ApplicationBase<T, Proc>::RemoveFile(const std::string &path)
{
    boost::filesystem::path filePath(path);
    boost::system::error_code err;
    if (boost::filesystem::exists(filePath, err))
    {
        if (boost::filesystem::remove(filePath, err))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (err)
    {
        return false;
    }
    else
    {
        return true;
    }
}

template<typename T, T *Proc> void ApplicationBase<T, Proc>::DelayExec(IProgressReporter *reporter)
{
    unsigned int delayTime = 0;
    if (!AppInstance->m_vm["svc-exec-delay"].empty())
    {
        delayTime = AppInstance->m_vm["svc-exec-delay"].template as<unsigned int>();
    }
    if (delayTime)
    {
        reporter->IncProgress(0, static_cast<int>(delayTime) * 1000);
        boost::this_thread::sleep_for(boost::chrono::seconds(delayTime));
    }
}

template<typename T, T *Proc> bool ApplicationBase<T, Proc>::GetChildExecResult(int exitCode, const char *ctx)
{
    if (exitCode != -1)
    {
        if (WIFEXITED(exitCode))
        {
            exitCode = WEXITSTATUS(exitCode);
            if (exitCode != 0)
            {
                LOG4CPLUS_ERROR_FMT(log, "%s失败，退出码：%d", ctx, exitCode);
                return false;
            }
        }
        else if(WIFSIGNALED(exitCode))
        {
            LOG4CPLUS_ERROR_FMT(log, "%s未正常退出（未捕获信号），终止信号：%d", ctx, WTERMSIG(exitCode));
            return false;
        }
        else
        {
            LOG4CPLUS_ERROR_FMT(log, "%s未知错误，退出码：%d", ctx, exitCode);
            return false;
        }
    }
    else
    {
        LOG4CPLUS_ERROR_FMT(log, "system%s启动失败。", ctx);
        return false;
    }
    return true;
}

#endif /* UNIXAPPLICATIONBASE_H */