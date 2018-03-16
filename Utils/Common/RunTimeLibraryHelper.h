#ifndef RUNTIMELIBRARYHELPER_H
#define RUNTIMELIBRARYHELPER_H

#include <string.h>
#include <utility>

/**
 * C runtime library function wrapper(used to disable vc++ extension warning).
 */
class RunTimeLibraryHelper
{
public:

	/**
	 * Same as sprintf.
	 *
	 * @tparam ...T Variable argument types.
	 * @param [in,out] buf	 printf buffer.
	 * @param bufSize Size of the buffer.
	 * @param format Describes the format to use.
	 * @param arg Variable arguments providing [in,out] The argument.
	 *
	 * @return Same as sprintf.
	 */
	template<typename ...T> static int SPrintF(char *buf, size_t bufSize, const char *format, T&&... arg)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return sprintf_s(buf, bufSize, format, std::forward<T>(arg)...);
#else
		return sprintf(buf, format, std::forward<T>(arg)...);
#endif
	}

	/**
	 * Same as sscanf.
	 *
	 * @tparam ...T Variable argument types.
	 * @param buf sscanf buffer.
	 * @param format Describes the format to use.
	 * @param arg Variable arguments providing [in,out] The argument.
	 *
	 * @return Same as sscanf.
	 */
	template<typename ...T> static int SScanF(const char *buf, const char *format, T&&... arg)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return sscanf_s(buf, format, std::forward<T>(arg)...);
#else
		return sscanf(buf, format, std::forward<T>(arg)...);
#endif
	}

	/**
	 * Same as strcpy.
	 *
	 * @param [in,out] dest Copy destination buffer.
	 * @param  size Buffer size.
	 * @param src Source used to copy.
	 *
	 * @return Same as strcpy.
	 */
	static char* StrCpy(char *dest, size_t size, const char *src)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return strcpy_s(dest, size, src) == 0 ? dest : nullptr;
#else
		return strcpy(dest, src);
#endif
	}

	/**
	 * Same as memcpy.
	 *
	 * @param [in,out] dest Copy destination buffer.
	 * @param numberOfElements Size of the destination buffer.
	 * @param src Source used to copy.
	 * @param count Number of characters to copy.
	 *
	 * @return Same as memcpy.
	 */
	static void* MemCpy(void *dest, size_t numberOfElements, const void *src, size_t count)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		return memcpy_s(dest, numberOfElements, src, count) == 0 ? dest : nullptr;
#else
		return memcpy(dest, src, count);
#endif
	}

	/**
	 * Same as memmove.
	 *
	 * @param [in,out] dest Destination buffer to move.
	 * @param destSize Size of the destination buffer.
	 * @param src Source buffer to move.
	 * @param count Number of characters to move.
	 *
	 * @return Same as memmove.
	 */
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