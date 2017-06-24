#ifndef UNIXAPPLICATIONBASE_H
#define UNIXAPPLICATIONBASE_H

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <bsd/libutil.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "../Concurrent/WaitEvent.h"
#include "IProgressReporter.h"
#include "../Common/PathHelper.h"
#include "../Log/Log4cplusCustomInc.h"

template<typename T, T *Proc> class ApplicationBase
{
public:
	typedef ApplicationBase<T, Proc> SelfType;

	static constexpr size_t MaxServiceNameLength = 31;

	static SelfType& Instance()
	{
		return *AppInstance;
	}

	ApplicationBase();

	~ApplicationBase();

	int Run(int argc, char *argv[]);

	const boost::program_options::variables_map& CmdVariables()
	{
		return m_vm;
	}

	const std::string& ServiceName()
	{
		return m_serviceName;
	}

private:
	typedef int (SelfType::*ParamHandler)();

	ApplicationBase(const ApplicationBase&) = delete;

	ApplicationBase(ApplicationBase&&) = delete;

	ApplicationBase& operator=(const ApplicationBase&) = delete;

	ApplicationBase& operator=(ApplicationBase&&) = delete;

	int HelpFunc();

	int InstallDaemon();

	int UninstallDaemon();

	int UninstallDaemonImpl(const std::string &svcName);

	int RunSvc();

	int RunNormal();

	bool Initialize(int argc, char *argv[]);

	static void SignalHandler(int sigNum);

	int RedirectStream();

	static bool WriteFile(const std::string &content, const std::string &path);

	static std::string ReadFile(const std::string &path);

	static bool RemoveFile(const std::string &path);

	struct ParamEntry
	{
		const char *m_paramName;

		ParamHandler m_handler;
	};

	static constexpr size_t EntrySize = 4;

	//如果之保留内联的初始化写法，clang编译通过，但是链接时出现未定义错误（odr use?），gcc、vc++（WinApplicationBase）无问题，所以仍需要外部定义
	//同时由于如果外部没有定义时出现了链接时未定义错误，因此说明constexpr未起作用（odr use），所以这里虽然是编译时定义的，但是加不加constexpr都无所谓
	static ParamEntry ParamEntries[EntrySize] =
	{
		{ "help",&SelfType::HelpFunc },
		{ "install",&SelfType::InstallDaemon },
		{ "uninstall",&SelfType::UninstallDaemon },
		{ "svc",&SelfType::RunSvc }
	};

	boost::program_options::options_description m_optionsDesc;

	boost::program_options::variables_map m_vm;

	pidfh *m_pidFile;

	WaitEvent m_evt;

	std::string m_serviceName;

	std::unique_ptr<IProgressReporter> m_reporter;

	static SelfType *AppInstance;

	static log4cplus::Logger log;
};

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::SelfType *ApplicationBase<T, Proc>::AppInstance = nullptr;

template<typename T, T *Proc> log4cplus::Logger ApplicationBase<T, Proc>::log = log4cplus::Logger::getInstance(T::LogName());

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::ParamEntry ApplicationBase<T, Proc>::ParamEntries[EntrySize];

template<typename T, T *Proc> ApplicationBase<T, Proc>::ApplicationBase() : m_optionsDesc("Options"), m_vm(), m_pidFile(nullptr), m_evt(), m_serviceName(), m_reporter(nullptr)
{
	if (AppInstance)
	{
		abort();
	}
	AppInstance = this;
	decltype(m_optionsDesc.add_options()) options = m_optionsDesc.add_options();
	options("help,h", "produce help message");
	options("install,i", "install programme as unix daemon");
	options("name,n", boost::program_options::value<std::string>(), "optional,set daemon name when install unix daemon");
	options("disp-name,d", boost::program_options::value<std::string>(), "optional,set daemon description when install unix daemon");
	options("start-order", boost::program_options::value<int>(), "optional,set daemon script start order number");
	options("stop-order", boost::program_options::value<int>(), "optional,set daemon script stop order number");
	options("uninstall,u", "uninstall installed unix daemon");
	options("svc", "run programme as unix daemon(used by daemon script,don't use directly)");
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
	if (!Initialize(argc, argv))
	{
		return 1;
	}

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
	if (!m_evt)
	{
		LOG4CPLUS_ERROR(log, "创建控制事件失败。");
		return 1;
	}
	int ret = 0;
	m_reporter.reset(new NullReport());
	if (Proc->Startup(*m_reporter, m_vm, log))
	{
		m_evt.Wait();
		Proc->Exit(*m_reporter, log);
	}
	else
	{
		ret = 1;
		Proc->Exit(*m_reporter, log);
	}
	return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunSvc()
{
	if (!m_evt)
	{
		LOG4CPLUS_ERROR(log, "创建控制事件失败。");
		return 1;
	}
	int ret = RedirectStream();
	if (ret != 0)
	{
		return ret;
	}

	std::string pidFilePath("/var/run/");
	pidFilePath.append(m_serviceName);
	pidFilePath.append(".pid");
	m_pidFile = pidfile_open(pidFilePath.c_str(), 0600, nullptr);
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
			m_evt.Wait();
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

template<typename T, T *Proc> int ApplicationBase<T, Proc>::HelpFunc()
{
	std::cout << m_optionsDesc << std::endl;
	return 0;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::InstallDaemon()
{
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

	boost::system::error_code err;
	bool svcExists = boost::filesystem::exists("/etc/init.d/" + svcName, err);
	if (svcExists&&UninstallDaemonImpl(svcName) != 0)
	{
		LOG4CPLUS_ERROR(log, "卸载已存在服务失败。");
		return 1;
	}
	int startOrder = 80, stopOrder = 1;
	if (!m_vm["start-order"].empty())
	{
		startOrder = m_vm["start-order"].template as<int>();
	}
	if (!m_vm["stop-order"].empty())
	{
		stopOrder = m_vm["stop-order"].template as<int>();
	}

	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	std::string skeletonPath = path;
	PathHelper::Combine(&skeletonPath, "Configuration", "skeleton", nullptr);
	boost::filesystem::path filePath(skeletonPath);
	if (!boost::filesystem::exists(filePath, err))
	{
		LOG4CPLUS_ERROR(log, "Daemon模板脚本文件不存在。");
		return 1;
	}
	std::string skeletonStr = ReadFile(skeletonPath);
	if (skeletonStr.empty())
	{
		LOG4CPLUS_ERROR(log, "读取Daemon模板脚本文件失败。");
		return 1;
	}
	//TODO 自定义路径
	std::string daemonPath = PathHelper::AppExecutablePath();
	std::string scriptContent = (boost::format("\
#! /bin/sh\n\
### BEGIN INIT INFO\n\
# Provides:          %s\n\
# Required-Start:    $remote_fs $syslog\n\
# Required-Stop:     $remote_fs $syslog\n\
# Default-Start:     2 3 4 5\n\
# Default-Stop:      0 1 6\n\
# Short-Description: %s daemon script\n\
### END INIT INFO\n\
\n\
OPERATION=$1\n\
set - e\n\
\n\
DESC=\"%s\"\n\
NAME=%s\n\
DAEMON=%s\n\
DAEMON_ARGS=\"--svc\"\n\
DAEMON_DIRECTORY=%s\n\
WAIT_STOP_TIMEOUT=300\n") % svcName%svcName%svcDisplayName%svcName%daemonPath%path).str();
	scriptContent.append(skeletonStr);
	//TODO 自定义路径
	std::string tempScriptPath = path;
	PathHelper::Combine(&tempScriptPath, "Configuration", "tempscript", nullptr);
	if(!WriteFile(scriptContent, tempScriptPath))
	{
		LOG4CPLUS_ERROR(log, "保存临时脚本文件失败。");
		return 1;
	}
	std::string installScriptPath = path;
	PathHelper::Combine(&installScriptPath, "Configuration", (boost::format("install.sh \"%s\" \"%s\" %02d %02d") % svcName % tempScriptPath % startOrder % stopOrder).str().c_str()
		, nullptr);
	int exitCode = system(installScriptPath.c_str());
	if (exitCode != 0)
	{
		RemoveFile(tempScriptPath);
		LOG4CPLUS_ERROR_FMT(log, "执行安装脚本失败，退出码：%d", exitCode);
		return 1;
	}
	//TODO 自定义路径
	std::string nameFilePath = path;
	PathHelper::Combine(&nameFilePath, "Configuration", "svcname.txt", nullptr);
	if(!WriteFile(svcName, nameFilePath))
	{
		RemoveFile(tempScriptPath);
		LOG4CPLUS_ERROR(log, "保存服务名失败。");
		return 1;
	}
	RemoveFile(tempScriptPath);
	LOG4CPLUS_INFO(log, "服务安装过程成功完成。");
	return 0;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::UninstallDaemon()
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	std::string nameFilePath = path;
	PathHelper::Combine(&nameFilePath, "Configuration", "svcname.txt", nullptr);
	std::string svcName = ReadFile(nameFilePath);
	if (svcName.empty())
	{
		LOG4CPLUS_ERROR(log, "读取服务名失败。");
		return 1;
	}
	return UninstallDaemonImpl(svcName);
}

template<typename T,T *Proc> int ApplicationBase<T,Proc>::UninstallDaemonImpl(const std::string &svcName)
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	std::string nameFilePath = path;
	PathHelper::Combine(&nameFilePath, "Configuration", "svcname.txt", nullptr);

	//TODO 自定义路径
	std::string uninstallScriptPath = path;
	PathHelper::Combine(&uninstallScriptPath, "Configuration", (boost::format("uninstall.sh %s") % svcName).str().c_str(), nullptr);
	int exitCode = system(uninstallScriptPath.c_str());
	if (exitCode != 0)
	{
		LOG4CPLUS_ERROR_FMT(log, "执行卸载脚本失败，退出码：%d。", exitCode);
		return 1;
	}

	RemoveFile(nameFilePath);
	LOG4CPLUS_INFO(log, "服务删除过程成功完成。");
	return 0;
}

template<typename T,T *Proc> bool ApplicationBase<T,Proc>::Initialize(int argc, char *argv[])
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	if (chdir(path.c_str()) != 0)
	{
		LOG4CPLUS_ERROR_FMT(log, "设置当前路径失败，退出执行，错误代码：%d", errno);
		return false;
	}
	PathHelper::Combine(&path, "Configuration", "log4cplus.properties", 0);
	log4cplus::ConfigureAndWatchThread watchThread(path, 6000);

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
	m_serviceName = ReadFile(nameFilePath);
	if (m_serviceName.empty())
	{
		m_serviceName = T::SvcName();
	}

	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_optionsDesc), m_vm);
	boost::program_options::notify(m_vm);

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
		AppInstance->m_evt.Signal();
		break;
	default:
		//SIGTTIN,SIGTTOU
		break;
	}
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RedirectStream()
{
	int fd = open("/dev/null", O_RDWR);
	if (fd == -1)
	{
		LOG4CPLUS_ERROR(log, "打开/dev/null失败。");
		return 1;
	}
	if (dup2(fd, STDIN_FILENO) == -1 || dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1)
	{
		close(fd);
		LOG4CPLUS_ERROR(log, "重定向失败。");
		return 1;
	}
	close(fd);
	return 0;
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

#endif /* UNIXAPPLICATIONBASE_H */