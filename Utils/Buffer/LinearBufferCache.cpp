#include "../Common/ObjectPoolBase.hpp"
#include "LinearBufferCache.h"

const char LinearBufferCacheLoggerName[] = "LinearBufferCache";

LinearBuffer* LinearBufferCacheFactory::CreateObj(size_t requireSize)
{
    LinearBuffer *ret = new LinearBuffer();
    ret->reserve(requireSize);
    return ret;
}

void LinearBufferCacheFactory::FreeObj(LinearBuffer *obj)
{
    delete obj;
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

template class UTILS_DEF_API ObjectPoolElemDeleter
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
