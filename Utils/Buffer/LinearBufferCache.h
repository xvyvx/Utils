#ifndef LINEARBUFFERCACHE_H
#define LINEARBUFFERCACHE_H

#include "BufferCacheBase.h"
#include "BufferDescriptor.h"

extern const char LinearBufferCacheLoggerName[];

class LinearBufferCacheFactory
{
public:
    LinearBuffer* CreateObj(size_t requireSize);

    void FreeObj(LinearBuffer *obj);
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

extern template class UTILS_DECL_API ObjectPoolElemDeleter
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

/**
 * @brief Linear buffer memory pool helper functions.
 *
 */
class LinearBufferCacheHelper
{
public:
    /**
     * @brief Alloc an vector of T from Linear buffer memory pool.
     *
     * @tparam T vector element type.
     * @param holder Pool resource holder.
     * @param allocSize Total size of alloced vector.
     * @param allocSizeHint Init alloc size hint for vector.
     * @return T* First element pointer of alloced vector(nullptr if alloc failed).
     */
    template<typename T> static T* AllocVectorFromPool(LinearBufferCache::ptr_t &holder, size_t &allocSize, size_t allocSizeHint = 4096)
    {
        holder = LinearBufferCache::Instance().Get(allocSizeHint);
        if (holder)
        {
            void *ret = holder->data();
            allocSize = holder->capacity();
            ret = std::align(alignof(T), sizeof(T), ret, allocSize);
            if (ret)
            {
                return reinterpret_cast<T*>(ret);
            }
            holder.reset();
        }
        return nullptr;
    }
};

#endif /* LINEARBUFFERCACHE_H */