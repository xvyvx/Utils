#include "../Common/ObjectPoolBaseImpl.h"
#include "CircularBufferCache.h"

const char CircularBufferCacheLoggerName[] = "CircularBufferCache";

template class UTILS_DEF_API ObjectPoolBase
<
	size_t, 
	CircularBuffer, 
	BufferElementTrait<CircularBuffer>,
	BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, CircularBufferCacheLoggerName>, 
	CircularBufferCacheFactory, 
	BufferCacheBaseClearFunc<CircularBuffer>, 
	CircularBufferCacheLoggerName
>;

template class UTILS_DEF_API BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, CircularBufferCacheLoggerName>;
