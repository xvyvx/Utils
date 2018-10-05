#ifndef UNIXAPPLICATIONBASE_H
#define UNIXAPPLICATIONBASE_H

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
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
#include "../Concurrent/WaitEvent.h"
#include "IProgressReporter.h"
#include "SystemdProgressReporter.h"
#include "../Common/PathHelper.h"
#include "../Log/Log4cplusCustomInc.h"

/**
 * Base class for Unix systemV/Linux systemd based application.
 *
 * @tparam T Application specific type.
 * 	T must implete following function:
 * @code
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
	typedef ApplicationBase<T, Proc> SelfType;

	static constexpr size_t MaxServiceNameLength = 31;  /**< The maximum service name length */

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

private:
	/**
	 * Defines an alias representing the internal handler.
	 */
	typedef int (SelfType::*ParamHandler)();

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
	 * @return True if it succeeds, false if it fails.
	 */
	bool Initialize();

	/**
	 * Signal handler entry.
	 *
	 * @param sigNum The signal number.
	 */
	static void SignalHandler(int sigNum);

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

	std::unique_ptr<WaitEvent> m_evt;   /**< WaitEvent for internal use.*/

	std::string m_serviceName;  /**< Installed service name. */

	DaemonType m_svcType;   /**< Installed daemon type. */

	std::unique_ptr<IProgressReporter> m_reporter;  /**< Pointer of progress reporter. */

	static SelfType *AppInstance;   /**< Global instance. */

	static log4cplus::Logger log;   /**< The logger. */
};

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::SelfType *ApplicationBase<T, Proc>::AppInstance = nullptr;

template<typename T, T *Proc> log4cplus::Logger ApplicationBase<T, Proc>::log = log4cplus::Logger::getInstance(T::LogName());

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::ParamEntry ApplicationBase<T, Proc>::ParamEntries[EntrySize];

template<typename T, T *Proc> ApplicationBase<T, Proc>::ApplicationBase() : m_optionsDesc("Options"), m_vm(), m_pidFile(nullptr), m_evt(), m_serviceName(), m_svcType(DaemonType_None)
	, m_reporter(nullptr)
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
	options("disp-name,d", boost::program_options::value<std::string>(), "optional,set daemon description when install unix daemon");
	options("start-order", boost::program_options::value<int>(), "optional,set daemon script start order number(legacy systemV only)");
	options("stop-order", boost::program_options::value<int>(), "optional,set daemon script stop order number(legacy systemV only)");
	options("svc-depends", boost::program_options::value<std::string>(), "optional,set daemon dependencies when install unix daemon(non legacy systemV or systemd only)");
	options("uninstall,u", "uninstall installed unix daemon");
	options("svc-systemV", "run programme as unix daemon(used by daemon script,don't use directly)");
	options("svc-systemd", "run programme as unix daemon(used by systemd service manager,don't use directly)");
}

template<typename T, T *Proc> ApplicationBase<T, Proc>::~ApplicationBase()
{
	if (m_pidFile)
	{
		pidfile_remove(m_pidFile);
		m_pidFile = nullptr;
	}
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

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunNormal()
{
	if (!Initialize())
	{
		return 1;
	}
	if (!m_evt)
	{
		LOG4CPLUS_ERROR(log, "创建控制事件失败。");
		return 1;
	}
	int ret = 0;
	m_reporter.reset(new NullReport());
	if (Proc->Startup(*m_reporter, m_vm, log))
	{
		m_evt->Wait();
		Proc->Exit(*m_reporter, log);
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
	if (daemon(1, 0) != 0)
	{
		return 1;
	}
	if (!Initialize())
	{
		return 2;
	}
	if (!m_evt)
	{
		LOG4CPLUS_ERROR(log, "创建控制事件失败。");
		return 2;
	}

	int ret = 0;
	std::string pidFilePath("/var/run/");
	pidFilePath.append(m_serviceName);
	pidFilePath.append(".pid");
	m_pidFile = pidfile_open(pidFilePath.c_str(), 0644, nullptr);
	if (m_pidFile == NULL)
	{
		if (errno == EEXIST)
		{
			LOG4CPLUS_ERROR_FMT(log, "指定的Daemon已经运行，退出执行，Daemon名称：%s。", m_serviceName.c_str());
		}
		else
		{
			LOG4CPLUS_ERROR_FMT(log, "创建Pid文件失败，退出执行，Daemon名称：%s，错误代码：%d", m_serviceName.c_str(), errno);
		}
		ret = 1;
	}
	if(ret==0)
	{
		ret = pidfile_write(m_pidFile);
		if (ret == -1)
		{
			LOG4CPLUS_ERROR_FMT(log, "写入Pid文件失败，退出执行，Daemon名称：%s，错误代码：%d", m_serviceName.c_str(), errno);
		}
	}

	if (ret == 0)
	{
		m_reporter.reset(new NullReport());
		if (Proc->Startup(*m_reporter, m_vm, log))
		{
			m_evt->Wait();
			Proc->Exit(*m_reporter, log);
			AppInstance->m_reporter->ReportNewStatus(Status_Stoped, 0);
		}
		else
		{
			ret = 1;
			Proc->Exit(*m_reporter, log);
		}
	}
	return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunSvcSystemd()
{
	SystemdProgressReporter *report = new SystemdProgressReporter();
	m_reporter.reset(report);
	if (!*report)
	{
		return 1;
	}
	m_reporter->ReportNewStatus(Status_StartPending, 30000);
	if (!Initialize())
	{
		m_reporter->ReportNewStatus(Status_StopPending, 0, 1);
		return 1;
	}
	if (!m_evt)
	{
		m_reporter->ReportNewStatus(Status_StopPending, 0, 1);
		LOG4CPLUS_ERROR(log, "创建控制事件失败。");
		return 1;
	}
	int ret = 0;
	if (Proc->Startup(*m_reporter, m_vm, log))
	{
		m_reporter->ReportNewStatus(Status_Running, 0);
		m_evt->Wait();
		Proc->Exit(*m_reporter, log);
	}
	else
	{
		m_reporter->ReportNewStatus(Status_StopPending, 0, 1);
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
	if (!Initialize())
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
	std::string svcDisplayName;
	if (m_vm["disp-name"].empty())
	{
		svcDisplayName = T::SvcDisplayName();
	}
	else
	{
		svcDisplayName = m_vm["disp-name"].template as<std::string>();
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
		depends.assign(" ");
		depends.append(m_vm["svc-depends"].template as<std::string>());
	}

	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	std::string daemonPath = PathHelper::AppExecutablePath();
	std::string installScriptPath = path;
	PathHelper::Combine(&installScriptPath, "Configuration", (boost::format("install.sh %s \"%s\" \"%s\" \"%s\" \"%s\" %02d %02d \"%s\"") % installStr % svcName % svcDisplayName
		% daemonPath % path % startOrder % stopOrder % depends).str().c_str(), nullptr);
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
	if (!Initialize())
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

template<typename T,T *Proc> bool ApplicationBase<T,Proc>::Initialize()
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	PathHelper::Combine(&path, "Configuration", "log4cplus.properties", 0);
	log4cplus::ConfigureAndWatchThread watchThread(path, 6000);

	if (chdir(PathHelper::AppDeployPath().c_str()) != 0)
	{
		LOG4CPLUS_ERROR_FMT(log, "设置当前路径失败，退出执行，错误代码：%d", errno);
		return false;
	}

	struct sigaction act;
	act.sa_handler = ApplicationBase<T, Proc>::SignalHandler;
	if (sigemptyset(&act.sa_mask) == -1)
	{
		LOG4CPLUS_ERROR_FMT(log, "初始化信号掩码失败，退出执行，错误代码：%d", errno);
		return false;
	}
	act.sa_flags = 0;
	act.sa_restorer = 0;
	if (sigaction(SIGTERM, &act, nullptr) == -1 || sigaction(SIGINT, &act, nullptr) == -1 || sigaction(SIGQUIT, &act, nullptr) == -1
		|| sigaction(SIGTTIN, &act, nullptr) == -1|| sigaction(SIGTTOU, &act, nullptr) == -1)
	{
		LOG4CPLUS_ERROR_FMT(log, "注册信号处理程序失败，退出执行，错误代码：%d", errno);
		return false;
	}

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
	m_evt.reset(new WaitEvent());
	return true;
}

template<typename T,T *Proc> void ApplicationBase<T,Proc>::SignalHandler(int sigNum)
{
	switch (sigNum)
	{
	case SIGTERM:
	case SIGINT:
	case SIGQUIT:
		AppInstance->m_reporter->ReportNewStatus(Status_StopPending, 30000);
		AppInstance->m_evt->Signal();
		break;
	default:
		//SIGTTIN,SIGTTOU
		break;
	}
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