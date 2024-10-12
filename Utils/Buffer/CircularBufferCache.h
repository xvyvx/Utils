#ifndef CIRCULARBUFFERCACHE_H
#define CIRCULARBUFFERCACHE_H

#include "BufferCacheBase.h"
#include "CircularBuffer.h"

extern const char CircularBufferCacheLoggerName[];

class CircularBufferCacheFactory
{
public:
    inline CircularBuffer* CreateObj(size_t requireSize)
    {
        return new CircularBuffer(requireSize);
    }

    inline void FreeObj(CircularBuffer *obj)
    {
        delete obj;
    }
};

/**
 * Circular buffer cache impletemented by @ref BufferCacheBase.
 */
using CircularBufferCache = BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, CircularBufferCacheLoggerName>;

extern template class UTILS_DECL_API ObjectPoolBase
<
    size_t,
    CircularBuffer,
    BufferElementTrait<CircularBuffer>,
    BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, CircularBufferCacheLoggerName>,
    CircularBufferCacheFactory,
    BufferCacheBaseClearFunc<CircularBuffer>,
    CircularBufferCacheLoggerName
>;

extern template class UTILS_DECL_API ObjectPoolElemDeleter
<
    size_t,
    CircularBuffer,
    BufferElementTrait<CircularBuffer>,
    BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, CircularBufferCacheLoggerName>,
    CircularBufferCacheFactory,
    BufferCacheBaseClearFunc<CircularBuffer>,
    CircularBufferCacheLoggerName
>;

extern template class UTILS_DECL_API BufferCacheBase<CircularBuffer, CircularBufferCacheFactory, CircularBufferCacheLoggerName>;

#endif /* CIRCULARBUFFERCACHE_H */