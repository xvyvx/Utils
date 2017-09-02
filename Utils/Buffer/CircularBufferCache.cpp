#include "../Common/ObjectPoolBaseImpl.h"
#include "CircularBufferCache.h"

const char CircularBufferCacheLoggerName[] = "CircularBufferCache";

CircularBufferCacheFactory CircularBufferCacheFactoryObj;

template class UTILS_DEF_API ObjectPoolBase
<
	size_t, 
	CircularBuffer, 
	BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, &CircularBufferCacheFactoryObj, CircularBufferCacheLoggerName>, 
	CircularBufferCacheFactory, 
	&CircularBufferCacheFactoryObj, 
	BufferCacheBaseClearFunc<CircularBuffer>, 
	&BufferCacheBaseClearFunc<CircularBuffer>::Instance, 
	BufferCacheBasePredicatorFunc<CircularBuffer>, 
	&BufferCacheBasePredicatorFunc<CircularBuffer>::Instance, 
	CircularBufferCacheLoggerName
>;

template class UTILS_DEF_API BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, &CircularBufferCacheFactoryObj, CircularBufferCacheLoggerName>;
