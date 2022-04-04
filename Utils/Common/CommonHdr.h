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

#if defined(_MSC_VER) && defined(UTILS_EXPORTS)
    #define UTILS_DECL_API
    #define UTILS_DEF_API UTILS_EXPORTS_API
#else
    #define UTILS_DECL_API UTILS_EXPORTS_API
    #define UTILS_DEF_API
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

/**
 * Defines an alias representing the unsigned byte.
 */
typedef unsigned char us8;

/**
 * Defines an alias representing the s signed byte.
 */
typedef signed char s8;

/**
 * Defines an alias representing the unsigned two-bytes.
 */
typedef unsigned short us16;

/**
 * Defines an alias representing the signed two-bytes.
 */
typedef signed short s16;

/**
 * Defines an alias representing the unsigned four-bytes.
 */
typedef unsigned int us32;

/**
 * Defines an alias representing the signed four-bytes.
 */
typedef signed int s32;

/**
 * Defines an alias representing the unsigned eight-bytes.
 */
typedef unsigned long long us64;

/**
 * Defines an alias representing the signed eight-bytes.
 */
typedef signed long long s64;

/**
 * Defines an alias representing the float four-bytes.
 * 
 */
typedef float f32;

/**
 * Defines an alias representing the float eight-bytes.
 * 
 */
typedef double f64;

#define IS_LITTLE_ENDIAN

#endif /* COMMONHDR_H */