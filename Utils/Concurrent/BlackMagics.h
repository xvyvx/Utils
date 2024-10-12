#ifndef BLACKMAGICS_H
#define BLACKMAGICS_H

#include "../Common/CommonHdr.h"

/**
 * Black magic function pointer.
 */
using BlackMagicFuncPtr = void (*)(unsigned int);

/**
 * Default black magic function
 *
 * @param count Spin count.
 */
UTILS_EXPORTS_API void DefaultBlackMagicFunc(unsigned count);

#endif /* BLACKMAGICS_H */