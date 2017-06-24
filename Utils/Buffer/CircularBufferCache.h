#ifndef CIRCULARBUFFERCACHE_H
#define CIRCULARBUFFERCACHE_H

#include "BufferCacheBase.h"
#include "CircularBuffer.h"

extern const char CircularBufferCacheLoggerName[];

class CircularBufferCacheFactory
{
public:
	inline CircularBuffer* operator()(size_t requireSize)
	{
		return new CircularBuffer(requireSize);
	}
};

extern CircularBufferCacheFactory CircularBufferCacheFactoryObj;

typedef BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, &CircularBufferCacheFactoryObj, CircularBufferCacheLoggerName> CircularBufferCache;

extern template class UTILS_DECL_API ObjectPoolBase
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

extern template class UTILS_DECL_API BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, &CircularBufferCacheFactoryObj, CircularBufferCacheLoggerName>;

#endif /* CIRCULARBUFFERCACHE_H */