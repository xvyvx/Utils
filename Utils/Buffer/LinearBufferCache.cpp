#include "../Common/ObjectPoolBaseImpl.h"
#include "LinearBufferCache.h"

const char LinearBufferCacheLoggerName[] = "LinearBufferCache";

LinearBuffer* LinearBufferCacheFactory::operator()(size_t requireSize)
{
	LinearBuffer *ret = new LinearBuffer();
	ret->reserve(requireSize);
	return ret;
}

LinearBufferCacheFactory LinearBufferCacheFactoryObj;

template class UTILS_EXPORTS_API ObjectPoolBase
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

template class UTILS_EXPORTS_API BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, &LinearBufferCacheFactoryObj, LinearBufferCacheLoggerName>;
