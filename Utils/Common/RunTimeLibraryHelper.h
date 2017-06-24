#ifndef RUNTIMELIBRARYHELPER_H
#define RUNTIMELIBRARYHELPER_H

#include <string.h>
#include <utility>

class RunTimeLibraryHelper
{
public:
	template<typename ...T> static int SPrintF(char *buf, size_t bufSize, const char *format, T&&... arg)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return sprintf_s(buf, bufSize, format, std::forward<T>(arg)...);
#else
		return sprintf(buf, format, std::forward<T>(arg)...);
#endif
	}

	template<typename ...T> static int SScanF(const char *buf, const char *format, T&&... arg)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return sscanf_s(buf, format, std::forward<T>(arg)...);
#else
		return sscanf(buf, format, std::forward<T>(arg)...);
#endif
	}

	static char* StrCpy(char *dest, size_t size, const char *src)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return strcpy_s(dest, size, src) == 0 ? dest : nullptr;
#else
		return strcpy(dest, src);
#endif
	}

	static void* MemCpy(void *dest, size_t numberOfElements, const void *src, size_t count)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return memcpy_s(dest, numberOfElements, src, count) == 0 ? dest : nullptr;
#else
		return memcpy(dest, src, count);
#endif
	}

	static void* MemMove(void *dest, size_t destSize, const void *src, size_t count)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return memmove_s(dest, destSize, src, count) == 0 ? dest : nullptr;
#else
		return memmove(dest, src, count);
#endif
	}
};

#endif /* RUNTIMELIBRARYHELPER_H */