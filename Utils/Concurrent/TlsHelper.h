#ifndef TLSHELPER_H
#define TLSHELPER_H

#include <string>
#include <map>
#include <memory>
#include <boost/thread.hpp>
#include "SpinLock.h"

template<typename T> class TlsHelper
{
public:
	static void Set(const std::string &name, T *ptr = nullptr);

	static T* Get(const std::string &name) noexcept;
private:
	static SpinLock<> lock;

	static std::map<std::string, std::unique_ptr<boost::thread_specific_ptr<T>>> pointers;
};

template<typename T> SpinLock<> TlsHelper<T>::lock;

template<typename T> std::map<std::string, std::unique_ptr<boost::thread_specific_ptr<T>>> TlsHelper<T>::pointers;

template<typename T> void TlsHelper<T>::Set(const std::string &name, T *ptr)
{
	SpinLock<>::ScopeLock l(lock);
	auto iter = pointers.find(name);
	if (iter != pointers.end())
	{
		iter->second->reset(ptr);
	}
	else
	{
		auto &pointer = pointers[name];
		pointer.reset(new boost::thread_specific_ptr<T>());
		pointer->reset(ptr);
	}
}

template<typename T> T* TlsHelper<T>::Get(const std::string &name) noexcept
{
	T *ret = nullptr;
	SpinLock<>::ScopeLock l(lock);
	auto iter = pointers.find(name);
	if (iter != pointers.end())
	{
		ret = iter->second->get();
	}
	return ret;
}

#endif /* TLSHELPER_H */