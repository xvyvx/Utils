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

extern LinearBufferCacheFactory LinearBufferCacheFactoryObj;

typedef BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, &LinearBufferCacheFactoryObj, LinearBufferCacheLoggerName> LinearBufferCache;

extern template class UTILS_DECL_API ObjectPoolBase
<
	size_t, 
	LinearBuffer, 
	BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, &LinearBufferCacheFactoryObj, LinearBufferCacheLoggerName>, 
	LinearBufferCacheFactory, 
	&LinearBufferCacheFactoryObj, 
	BufferCacheBaseClearFunc<LinearBuffer>, 
	&BufferCacheBaseClearFunc<LinearBuffer>::Instance, 
	BufferCacheBasePredicatorFunc<LinearBuffer>, 
	&BufferCacheBasePredicatorFunc<LinearBuffer>::Instance, 
	LinearBufferCacheLoggerName
>;

extern template class UTILS_DECL_API BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, &LinearBufferCacheFactoryObj, LinearBufferCacheLoggerName>;

#endif /* LINEARBUFFERCACHE_H */