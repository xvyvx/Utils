#ifndef WINAPPLICATIONBASE_H
#define WINAPPLICATIONBASE_H

#include <memory>
#include <iostream>
#include <fstream>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "IProgressReporter.h"
#include "WinSvcProgressReporter.h"
#include "../Common/PathHelper.h"
#include "../Common/ResGuard.h"
#include "../Common/RunTimeLibraryHelper.h"
#include "../Concurrent/WaitEvent.h"
#include "../Log/Log4cplusCustomInc.h"
#include <boost/algorithm/string/classification.hpp>

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

	int InstallSvc();

	int UninstallSvc();

	int RunSvc();

	int RunNormal();

	static bool WriteSvcNameFile(const std::string &name);

	static std::string ReadSvcNameFile();

	static bool RemoveSvcNameFile();

	static void WINAPI ServiceMain(DWORD dwArgc,LPSTR *lpszArgv);

	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

	static BOOL WINAPI SignalHandler(DWORD dwCtrlType);

	struct ParamEntry
	{
		const char *m_paramName;

		ParamHandler m_handler;
	};

	static constexpr ParamEntry ParamEntries[] =
	{
		{ "help",&SelfType::HelpFunc },
		{ "install",&SelfType::InstallSvc },
		{ "uninstall",&SelfType::UninstallSvc },
		{ "svc",&SelfType::RunSvc }
	};

	static constexpr size_t EntrySize = sizeof(ParamEntries) / sizeof(ParamEntry);

	WaitEvent m_stopEvt;

	boost::program_options::options_description m_optionsDesc;

	boost::program_options::variables_map m_vm;

	std::string m_serviceName;

	std::unique_ptr<IProgressReporter> m_reporter;

	static SelfType *AppInstance;

	static log4cplus::Logger log;
};

template<typename T, T *Proc> typename ApplicationBase<T, Proc>::SelfType *ApplicationBase<T, Proc>::AppInstance = nullptr;

template<typename T, T *Proc> log4cplus::Logger ApplicationBase<T, Proc>::log = log4cplus::Logger::getInstance(T::LogName());

template<typename T, T *Proc> ApplicationBase<T, Proc>::ApplicationBase() : m_stopEvt(), m_optionsDesc("Options"), m_vm(), m_serviceName(), m_reporter(nullptr)
{
	if (AppInstance)
	{
		abort();
	}
	AppInstance = this;
	decltype(m_optionsDesc.add_options()) options = m_optionsDesc.add_options();
	options("help,h", "produce help message");
	options("install,i", "install programme as windows service");
	options("name,n", boost::program_options::value<std::string>(), "optional,set service name when install windows service");
	options("disp-name,d", boost::program_options::value<std::string>(), "optional,set service display name when install windows service");
	options("svc-desc", boost::program_options::value<std::string>(), "optional,set service description when install windows service");
	options("svc-depends", boost::program_options::value< std::vector<std::string> >(), "optional,set service dependencies when install windows service");
	options("uninstall,u", "uninstall installed windows service");
	options("svc", "run programme as windows service(used by SCManager,don't use directly)");
}

template<typename T, T *Proc> ApplicationBase<T,Proc>::~ApplicationBase()
{
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::Run(int argc, char *argv[])
{
	std::string path = PathHelper::AppDeployPath();
	//TODO 自定义路径
	PathHelper::Combine(&path, "Configuration", "log4cplus.properties", 0);
	log4cplus::ConfigureAndWatchThread watchThread(path, 6000);

	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, m_optionsDesc), m_vm);
	boost::program_options::notify(m_vm);

	bool hitted = false;
	int ret = 0;
	for (size_t i = 0; i < EntrySize; ++i)
	{
		if(m_vm.count(ParamEntries[i].m_paramName))
		{
			hitted = true;
			ret = (this->*ParamEntries[i].m_handler)();
			break;
		}
	}
	if(!hitted)
	{
		ret = RunNormal();
	}
	return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::HelpFunc()
{
	std::cout << m_optionsDesc << std::endl;
	return 0;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::InstallSvc()
{
	std::string svcName;
	if(m_vm["name"].empty())
	{
		svcName = T::SvcName();
	}
	else
	{
		svcName = m_vm["name"].as<std::string>();
	}
	if (boost::algorithm::any_of(svcName.begin(), svcName.end(), boost::is_any_of(" /\\")) || svcName.size() > MaxServiceNameLength)
	{
		LOG4CPLUS_ERROR(log, "指定的服务名含有非法字符或长度过长。");
		return 1;
	}

	static auto svcClearFunc = [](SC_HANDLE handle) {CloseServiceHandle(handle); };
	ResGuard<SC_HANDLE, decltype(svcClearFunc)> schSCManager(OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS), svcClearFunc);
	if (schSCManager.get() == nullptr)
	{
		LOG4CPLUS_ERROR_FMT(log, "打开SCManager失败，错误代码：%d。", GetLastError());
		return 1;
	}
	else
	{
		std::string svcDisplayName;
		if (m_vm["disp-name"].empty())
		{
			svcDisplayName = T::SvcDisplayName();
		}
		else
		{
			svcDisplayName = m_vm["disp-name"].as<std::string>();
		}

		LPCSTR depends = NULL;
		std::vector<std::string::value_type> dependsContent;
		if (!m_vm["svc-depends"].empty())
		{
			std::vector<std::string> temp = m_vm["svc-depends"].as< std::vector<std::string> >();
			for (const auto &str : temp)
			{
				dependsContent.insert(dependsContent.end(), str.c_str(), str.c_str() + str.size() + 1);
			}
			if (!dependsContent.empty())
			{
				dependsContent.push_back('\0');
				depends = dependsContent.data();
			}
		}

		if(!WriteSvcNameFile(svcName))
		{
			LOG4CPLUS_ERROR(log, "保存服务名失败。");
			return 1;
		}

		std::string appPath = PathHelper::AppExecutablePath();
		appPath.append(" --svc");
		ResGuard<SC_HANDLE, decltype(svcClearFunc)> schService(CreateServiceA(
			schSCManager.get(),        // SCM database 
			svcName.c_str(),           // name of service 
			svcDisplayName.c_str(),    // service name to display 
			SERVICE_ALL_ACCESS,        // desired access 
			SERVICE_WIN32_OWN_PROCESS, // service type 
			SERVICE_AUTO_START,        // start type 
			SERVICE_ERROR_NORMAL,      // error control type 
			appPath.c_str(),           // path to service's binary 
			NULL,                      // no load ordering group 
			NULL,                      // no tag identifier 
			depends,                      // dependencies 
			NULL,                      // LocalSystem account 
			NULL),                     // no password 
			svcClearFunc);

		if (schService.get() == nullptr)
		{
			LOG4CPLUS_ERROR_FMT(log, "创建服务失败，错误代码：%u。", GetLastError());
			RemoveSvcNameFile();
			return 1;
		}

		SERVICE_DESCRIPTIONA description;
		if (m_vm["svc-desc"].empty())
		{
			description.lpDescription = const_cast<char*>(T::SvcDescription());
		}
		else
		{
			const std::string &tempDesc = m_vm["svc-desc"].as<std::string>();
			if (tempDesc.size() > 1024)
			{
				LOG4CPLUS_INFO(log, "服务安装成功完成（设置描述超过长度限制）。");
				return 0;
			}
			description.lpDescription = const_cast<char*>(tempDesc.c_str());
		}

		if (!::ChangeServiceConfig2A(schService.get(), SERVICE_CONFIG_DESCRIPTION, &description))
		{
			LOG4CPLUS_INFO(log, "服务安装成功完成（设置描述失败）。");
		}
		else
		{
			LOG4CPLUS_INFO(log, "服务安装成功完成。");
		}
		return 0;

	}
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::UninstallSvc()
{
	static auto svcClearFunc = [](SC_HANDLE handle) {CloseServiceHandle(handle); };
	ResGuard<SC_HANDLE, decltype(svcClearFunc)> schSCManager(OpenSCManager(
		nullptr,                 // local computer
		nullptr,                 // ServicesActive database 
		SC_MANAGER_ALL_ACCESS),  // full access rights 
		svcClearFunc);

	if (schSCManager.get() == nullptr)
	{
		LOG4CPLUS_ERROR_FMT(log, "打开SCManager失败，错误代码：%d。", GetLastError());
		return 1;
	}

	std::string svcName = ReadSvcNameFile();
	if(svcName.empty())
	{
		LOG4CPLUS_ERROR(log, "读取服务名失败。");
		return 1;
	}

	ResGuard<SC_HANDLE, decltype(svcClearFunc)> schService(OpenServiceA(schSCManager.get(), svcName.c_str(), SERVICE_STOP | DELETE), svcClearFunc);
	if (schService.get() == nullptr)
	{
		LOG4CPLUS_ERROR_FMT(log, "打开服务失败，错误代码：%d。", GetLastError());
		return 1;
	}

	SERVICE_STATUS status;
	DWORD err;
	if (ControlService(schService.get(), SERVICE_CONTROL_STOP, &status) == 0 && (err = GetLastError()) != ERROR_SERVICE_NOT_ACTIVE)
	{
		LOG4CPLUS_ERROR_FMT(log, "停止服务失败，错误代码：%d。", err);
		return 1;
	}

	if (!DeleteService(schService.get()))
	{
		LOG4CPLUS_ERROR_FMT(log, "删除服务失败，错误代码：%d。", GetLastError());
		return 1;
	}
	else
	{
		RemoveSvcNameFile();
		LOG4CPLUS_INFO(log, "服务删除成功完成。");
		return 0;
	}
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunSvc()
{
	if(!::SetCurrentDirectoryA(PathHelper::AppDeployPath().c_str()))
	{
		LOG4CPLUS_ERROR_FMT(log, "设置当前路径失败，退出执行，错误代码：%d。", GetLastError());
		return 1;
	}

	m_serviceName = ReadSvcNameFile();
	if (m_serviceName.empty() || m_serviceName.size() > MaxServiceNameLength)
	{
		LOG4CPLUS_ERROR(log, "读取服务名失败或服务名过长，退出执行。");
		return 1;
	}

	char name[MaxServiceNameLength + 1] = { 0 };
	RunTimeLibraryHelper::StrCpy(name, MaxServiceNameLength + 1, m_serviceName.c_str());
	SERVICE_TABLE_ENTRYA DispatchTable[] =
	{
		{ name, ServiceMain },
		{ NULL, NULL }
	};
	if (!StartServiceCtrlDispatcherA(DispatchTable))
	{
		LOG4CPLUS_ERROR_FMT(log, "启动服务错误，错误代码：%d", GetLastError());
	}
	return 0;
}

template<typename T, T *Proc> void WINAPI ApplicationBase<T, Proc>::ServiceMain(DWORD dwArgc, LPSTR *lpszArgv)
{
	WinSvcProgressReporter *report = new WinSvcProgressReporter(AppInstance->m_serviceName.c_str(), ServiceCtrlHandler);
	AppInstance->m_reporter.reset(report);
	if (!*report)
	{
		LOG4CPLUS_ERROR_FMT(log, "注册服务控制程序错误，错误代码：%d", GetLastError());
		abort();
	}

	report->ReportNewStatus(Status_StartPending, 30000);
	AppInstance->m_stopEvt = ::CreateEvent(nullptr, false, false, nullptr);
	if (!AppInstance->m_stopEvt)
	{
		LOG4CPLUS_ERROR_FMT(log, "创建控制事件失败，错误代码：%d", GetLastError());
		report->ReportNewStatus(Status_Stoped, 0, 1);
		return;
	}

	bool result = Proc->Startup(*AppInstance->m_reporter, AppInstance->m_vm, log);
	if (!result)
	{
		AppInstance->m_reporter->ReportNewStatus(Status_StopPending, 30000);
		Proc->Exit(*AppInstance->m_reporter, log);
		AppInstance->m_reporter->ReportNewStatus(Status_Stoped, 0);
		return;
	}
	AppInstance->m_reporter->ReportNewStatus(Status_Running, 0);
	AppInstance->m_stopEvt.Wait();
	Proc->Exit(*AppInstance->m_reporter, log);
	AppInstance->m_reporter->ReportNewStatus(Status_Stoped, 0);
}

template<typename T, T *Proc> DWORD WINAPI ApplicationBase<T, Proc>::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	DWORD ret;
	switch(dwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		AppInstance->m_reporter->ReportNewStatus(Status_StopPending, 30000);
		AppInstance->m_stopEvt.Signal();
		ret = NO_ERROR;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		ret = NO_ERROR;
		break;
	default:
		ret = ERROR_CALL_NOT_IMPLEMENTED;
		break;
	}
	return ret;
}

template<typename T, T *Proc> int ApplicationBase<T, Proc>::RunNormal()
{
	if (!::SetConsoleCtrlHandler(SignalHandler, TRUE))
	{
		LOG4CPLUS_ERROR(log, "设置退出处理程序错误，退出执行。");
		::ExitProcess(1);
	}
	m_reporter.reset(new NullReport());
	if(Proc->Startup(*m_reporter, m_vm, log))
	{
		std::string temp;
		std::cin >> temp;
		Proc->Exit(*m_reporter, log);
	}
	else
	{
		Proc->Exit(*m_reporter, log);
	}
	return 0;
}

template<typename T, T *Proc> BOOL WINAPI ApplicationBase<T, Proc>::SignalHandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT || dwCtrlType == CTRL_LOGOFF_EVENT || dwCtrlType == CTRL_SHUTDOWN_EVENT)
	{
		Proc->Exit(*AppInstance->m_reporter, log);
	}
	return TRUE;
}

template<typename T, T *Proc> bool ApplicationBase<T, Proc>::WriteSvcNameFile(const std::string &name)
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	PathHelper::Combine(&path, "Configuration", "svcname.txt", nullptr);
	std::ofstream fs;
	fs.open(path, std::ios::out | std::ios::trunc);
	bool ret = true;
	if(fs)
	{
		fs << name;
		if(fs&&fs.flush())
		{
			fs.close();
		}
		else
		{
			RemoveSvcNameFile();
			ret = false;
		}
	}
	else
	{
		ret = false;
	}
	return ret;
}

template<typename T, T *Proc> std::string ApplicationBase<T, Proc>::ReadSvcNameFile()
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	PathHelper::Combine(&path, "Configuration", "svcname.txt", nullptr);
	std::ifstream fs;
	fs.open(path, std::ios::in);
	std::string ret;
	if (fs)
	{
		fs >> ret;
		fs.close();
	}
	return ret;
}

template<typename T, T *Proc> bool ApplicationBase<T, Proc>::RemoveSvcNameFile()
{
	//TODO 自定义路径
	std::string path = PathHelper::AppDeployPath();
	PathHelper::Combine(&path, "Configuration", "svcname.txt", nullptr);
	boost::filesystem::path filePath(path);
	boost::system::error_code err;
	if(boost::filesystem::exists(filePath,err))
	{
		if(boost::filesystem::remove(filePath, err))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(err)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#endif /* WINAPPLICATIONBASE_H */