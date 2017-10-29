#include "../Common/ObjectPoolBaseImpl.h"
#include "LinearBufferCache.h"

const char LinearBufferCacheLoggerName[] = "LinearBufferCache";

LinearBuffer* LinearBufferCacheFactory::operator()(size_t requireSize)
{
	LinearBuffer *ret = new LinearBuffer();
	ret->reserve(requireSize);
	return ret;
}

template class UTILS_DEF_API ObjectPoolBase
<
	size_t, 
	LinearBuffer, 
	BufferElementTrait<LinearBuffer>,
	BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName>, 
	LinearBufferCacheFactory, 
	BufferCacheBaseClearFunc<LinearBuffer>, 
	LinearBufferCacheLoggerName
>;

template class UTILS_DEF_API BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName>;
