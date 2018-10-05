#ifndef LOG4CPLUS_CUSTOM_COMMON_INCLUDE_H
#define LOG4CPLUS_CUSTOM_COMMON_INCLUDE_H

#if defined (UNICODE)
#define LOG4CPLUS_CUSTOM_UNDEF_UNICODE
#undef  UNICODE
#endif

#if defined (_UNICODE)
#define LOG4CPLUS_CUSTOM_UNDEF_UNICODE2
#undef  _UNICODE
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored  "-Wdeprecated-declarations"
#endif

#include <log4cplus/logger.h>  
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined (LOG4CPLUS_CUSTOM_UNDEF_UNICODE)
#define UNICODE
#endif

#if defined (LOG4CPLUS_CUSTOM_UNDEF_UNICODE2)
#define _UNICODE
#endif

#endif /* LOG4CPLUS_CUSTOM_COMMON_INCLUDE_H */