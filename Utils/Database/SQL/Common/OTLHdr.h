#ifndef OTLHDR_H
#define OTLHDR_H

#include "../../../Common/CommonHdr.h"

#if defined(USE_ODBC_DATABASE_UTILS)
    #if defined(IS_WINDOWS)
        #define OTL_ODBC
    #elif defined(IS_UNIX)
        #define OTL_ODBC_UNIX
    #else
        #error Unsupportted platform
    #endif
#endif

#if defined(USE_OCI_DATABASE_UTILS)
    #define OTL_ORA11G_R2
#endif

#if defined (UNICODE)
#define OTL_CUSTOM_UNDEF_UNICODE
#undef  UNICODE
#endif

#if defined (_UNICODE)
#define OTL_CUSTOM_UNDEF_UNICODE2
#undef  _UNICODE
#endif

#define OTL_BIGINT s64
#define OTL_UBIGINT us64
#define OTL_EXPLICIT_NAMESPACES
#define OTL_STREAM_POOLING_ON
#define OTL_STL
#define OTL_DESTRUCTORS_DO_NOT_THROW
#include <otl/otlv4.h>

#if defined (OTL_CUSTOM_UNDEF_UNICODE)
#define UNICODE
#endif

#if defined (OTL_CUSTOM_UNDEF_UNICODE2)
#define _UNICODE
#endif

#endif /* OTLHDR_H */