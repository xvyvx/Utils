#ifndef BUFDESCRIPTOR_H
#define BUFDESCRIPTOR_H

#include "../Common/CommonHdr.h"
#include <vector>
#include "LinearBuffer.h"

typedef struct
{
	us8 *m_beg;

	size_t m_size;
} BufDescriptor;

#endif /* BUFDESCRIPTOR_H */