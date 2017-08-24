#include "PathHelper.h"
#include <stdio.h>
#include <stdarg.h>
#include <boost/smart_ptr.hpp>
#include "CommonHdr.h"
#include "../Concurrent/SpinLock.h"

SpinLock<> PathHelperLock;

std::string AppDeployPathStr;

std::string AppExecutablePathStr;

void InitPathStrings();

const std::string& PathHelper::AppDeployPath()
{
	{
		SpinLock<>::ScopeLock lock(PathHelperLock);
		if(AppDeployPathStr.empty())
		{
			InitPathStrings();
		}
	}
	return AppDeployPathStr;
}

const std::string& PathHelper::AppExecutablePath()
{
	{
		SpinLock<>::ScopeLock lock(PathHelperLock);
		if(AppExecutablePathStr.empty())
		{
			InitPathStrings();
		}
	}
	return AppExecutablePathStr;
}

#if defined(IS_WINDOWS)

#include <Windows.h>

#define FILE_SYSTEM_SEPARATOR_CHAR '\\'

#if defined(FORCE_EXECUTION_CHAR_SET_UTF8)
typedef wchar_t CharType;
#define GetModuleFileNameFunc GetModuleFileNameW
#else
typedef char CharType;
#define GetModuleFileNameFunc GetModuleFileNameA
#endif

void InitPathStrings()
{
	HMODULE handle=GetModuleHandleA(NULL);
	if(handle)
	{
		SetLastError(ERROR_SUCCESS);
		try
		{
			DWORD bufferSize=MAX_PATH;
			boost::scoped_array<CharType> buffer(new CharType[bufferSize]);
			DWORD result= GetModuleFileNameFunc(handle,buffer.get(),bufferSize);
			while(result!=0&&GetLastError()==ERROR_INSUFFICIENT_BUFFER)
			{
				bufferSize*=2;
				buffer.reset(new CharType[bufferSize]);
				result= GetModuleFileNameFunc(handle,buffer.get(),bufferSize);
			}
			if(result!=0)
			{
#if defined(FORCE_EXECUTION_CHAR_SET_UTF8)
				int requireSize = 0;
				if ((requireSize = ::WideCharToMultiByte(CP_UTF8, 0, buffer.get(), result, NULL, 0, NULL, NULL)) != 0)
				{
					boost::scoped_array<char> temp(new char[requireSize]);
					if ((result = ::WideCharToMultiByte(CP_UTF8, 0, buffer.get(), result, temp.get(), requireSize, NULL, NULL)) != 0)
					{
						AppDeployPathStr.assign(temp.get(), result);
						AppExecutablePathStr.assign(temp.get(), result);
					}
				}
#else
				AppDeployPathStr.assign(buffer.get(),result);
				AppExecutablePathStr.assign(buffer.get(),result);
#endif
				std::string::size_type index = AppDeployPathStr.rfind('\\');
				assert(index!=std::string::npos);
				AppDeployPathStr.erase(index+1);
			}
		}
		catch(...)
		{
			AppDeployPathStr.clear();
			AppExecutablePathStr.clear();
		}
		FreeLibrary(handle);
	}
}

bool PathHelper::IsAbsolutePath(const char *path, size_t size)
{
	return size >= 3 && path[1] == ':' && path[2] == FILE_SYSTEM_SEPARATOR_CHAR;
}

#elif defined(IS_UNIX)

#include <unistd.h>

#define FILE_SYSTEM_SEPARATOR_CHAR '/'

//获取路径依赖proc文件系统
void InitPathStrings()
{
	try
	{
		ssize_t bufferSize=256;
		boost::scoped_array<char> buffer(new char[bufferSize]);
		ssize_t result=readlink("/proc/self/exe",buffer.get(),bufferSize);
		while(result>bufferSize)
		{
			bufferSize*=2;
			buffer.reset(new char[bufferSize]);
			result=readlink("/proc/self/exe",buffer.get(),bufferSize);
		}
		if(result>=0)
		{
			AppDeployPathStr.assign(buffer.get(),result);
			AppExecutablePathStr.assign(buffer.get(),result);
			std::string::size_type index = AppDeployPathStr.rfind('/');
			assert(index!=std::string::npos);
			AppDeployPathStr.erase(index+1);
		}
	}
	catch(...)
	{
		AppDeployPathStr.clear();
		AppExecutablePathStr.clear();
	}
}

bool PathHelper::IsAbsolutePath(const char *path, size_t size)
{
	return size >= 1 && path[0] == FILE_SYSTEM_SEPARATOR_CHAR;
}

#else

#error Unsupportted platform.

#endif

void PathHelper::Combine(std::string *result,...)
{
	va_list list;
	va_start(list,result);
	const char *start=va_arg(list,const char*);
	while(start)
	{
		if(result->back()!=FILE_SYSTEM_SEPARATOR_CHAR)
		{
			result->push_back(FILE_SYSTEM_SEPARATOR_CHAR);
		}
		result->append(start);
		start=va_arg(list,const char*);
	}
	va_end(list);
}