#ifndef APPLICATIONBASE_H
#define APPLICATIONBASE_H

#include "../Common/CommonHdr.h"

#if defined(IS_WINDOWS)
#include "WinApplicationBase.h"
#elif defined(IS_UNIX)
#include "UnixApplicationBase.h"
#else
#error Unsupport platform.
#endif

#endif /* APPLICATIONBASE_H */