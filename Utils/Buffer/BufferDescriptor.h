#ifndef BUFDESCRIPTOR_H
#define BUFDESCRIPTOR_H

#include "../Common/CommonHdr.h"
#include <vector>
#include "LinearBuffer.h"

typedef struct
{
	us8 *m_beg; /**< The beg pointer of a buffer. */

	size_t m_size;  /**< The size of a buffer. */
} BufDescriptor;	/**< Information describing a buffer. */

#endif /* BUFDESCRIPTOR_H */