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
#elif defined(linux) || defined(__linux) || defined(__APPLE__) || defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) \
    || defined(_POSIX_SOURCE)
    #define IS_UNIX
#endif

#include <stddef.h>

/**
 * Defines an alias representing the unsigned byte.
 */
using us8 = unsigned char;

/**
 * Defines an alias representing the s signed byte.
 */
using s8 = signed char;

/**
 * Defines an alias representing the unsigned two-bytes.
 */
using us16 = unsigned short;

/**
 * Defines an alias representing the signed two-bytes.
 */
using s16 = short;

/**
 * Defines an alias representing the unsigned four-bytes.
 */
using us32 = unsigned int;

/**
 * Defines an alias representing the signed four-bytes.
 */
using s32 = int;

/**
 * Defines an alias representing the unsigned eight-bytes.
 */
using us64 = unsigned long long;

/**
 * Defines an alias representing the signed eight-bytes.
 */
using s64 = long long;

/**
 * Defines an alias representing the float four-bytes.
 * 
 */
using f32 = float;

/**
 * Defines an alias representing the float eight-bytes.
 * 
 */
using f64 = double;

#define IS_LITTLE_ENDIAN

#endif /* COMMONHDR_H */