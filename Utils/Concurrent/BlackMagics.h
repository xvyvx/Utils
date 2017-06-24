#ifndef BLACKMAGICS_H
#define BLACKMAGICS_H

#include "../Common/CommonHdr.h"

typedef void(*BlackMagicFuncPtr)(unsigned);

UTILS_EXPORTS_API void DefaultBlackMagicFunc(unsigned count);

#endif /* BLACKMAGICS_H */