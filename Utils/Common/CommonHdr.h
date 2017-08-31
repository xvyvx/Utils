#ifndef COMMONHDR_H
#define COMMONHDR_H

#if defined(_MSC_VER)
	#ifdef UTILS_EXPORTS
	#define UTILS_EXPORTS_API __declspec(dllexport)
	#elif UTILS_IMPORTS
	#define UTILS_EXPORTS_API __declspec(dllimport)
	#else
	#define UTILS_EXPORTS_API
	#endif
#elif defined(__GNUC__)
	#ifdef UTILS_EXPORTS
	#define UTILS_EXPORTS_API __attribute__ ((visibility ("default")))
	#elif UTILS_IMPORTS
	#define UTILS_EXPORTS_API __attribute__ ((visibility ("default")))
	#else
	#define UTILS_EXPORTS_API
	#endif
#endif

#if defined(UTILS_EXPORTS)
#define UTILS_DECL_API
#else
#define UTILS_DECL_API UTILS_EXPORTS_API
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	#define IS_WINDOWS
	#if defined(_WIN64)
	#define IS_WINDOWS_64
	#endif
#elif defined(linux) || defined(__linux) || defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
	#define IS_UNIX
#endif

#include <stddef.h>

typedef unsigned char us8;
typedef signed char s8;
typedef unsigned short us16;
typedef signed short s16;
typedef unsigned int us32;
typedef signed int s32;
typedef unsigned long long us64;
typedef signed long long s64;

#define IS_LITTLE_ENDIAN

#endif /* COMMONHDR_H */