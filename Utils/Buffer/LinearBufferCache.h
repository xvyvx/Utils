#ifndef LINEARBUFFERCACHE_H
#define LINEARBUFFERCACHE_H

#include "BufferCacheBase.h"
#include "BufferDescriptor.h"

extern const char LinearBufferCacheLoggerName[];

class LinearBufferCacheFactory
{
public:
	LinearBuffer* operator()(size_t requireSize);
};

/**
 * Linear buffer cache impletemented by @ref BufferCacheBase.
 */
typedef BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName> LinearBufferCache;

extern template class UTILS_DECL_API ObjectPoolBase
<
	size_t, 
	LinearBuffer, 
	BufferElementTrait<LinearBuffer>,
	BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName>, 
	LinearBufferCacheFactory, 
	BufferCacheBaseClearFunc<LinearBuffer>, 
	LinearBufferCacheLoggerName
>;

extern template class UTILS_DECL_API BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName>;

#endif /* LINEARBUFFERCACHE_H */