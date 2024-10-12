#ifndef LINEARBUFFERCACHE_H
#define LINEARBUFFERCACHE_H

#include "BufferCacheBase.h"
#include "LinearBuffer.h"

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
using LinearBufferCache = BufferCacheBase<LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName>;

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

extern template class UTILS_DECL_API BufferCacheBase
    <
        LinearBuffer, LinearBufferCacheFactory, LinearBufferCacheLoggerName
    >;

/**
 * @brief A wrapper of None Trivial element type vector created from Linear buffer memory pool.
 * 
 * @tparam T None Trivial element type.
 */
template<typename T> class LinearBufferNotTrivialWrapper
{
public:
    friend class LinearBufferCacheHelper; //friend class definition

    /**
     * @brief Move constructor
     * 
     * @param rhs moved object
     */
    LinearBufferNotTrivialWrapper(LinearBufferNotTrivialWrapper &&rhs) noexcept;

    /**
     * @brief Destructor
     * 
     */
    ~LinearBufferNotTrivialWrapper();

    /**
     * @brief Move operator
     * 
     * @param rhs moved object
     * @return LinearBufferNoTrivialWrapper& Self reference.
     */
    LinearBufferNotTrivialWrapper& operator=(LinearBufferNotTrivialWrapper &&rhs) noexcept;

    /**
     * @brief Construct new element on the end of the vector.
     * 
     * @tparam Args Element constructor argument type.
     * @param args Element constructor arguments.
     * @return T& Reference to created element.
     */
    template<typename... Args> T& Construct(Args&&... args);

    /**
     * @brief Index operator.
     * 
     * @param index Position to element in this vector.
     * @return T& Reference to the element.
     */
    T& operator[](size_t index)
    {
        return *(m_ptr + index);
    }

    /**
     * @brief Get the element count of this vector.
     * 
     * @return size_t Constructed element count in this vector.
     */
    size_t Count()
    {
        return m_count;
    }

    /**
     * @brief Get the result wether this vector create successful.
     * 
     * @return true Create successful.
     * @return false Create failed.
     */
    operator bool()
    {
        return m_holder.get() != nullptr;
    }

private:
    LinearBufferNotTrivialWrapper()
    {
    }

    LinearBufferCache::ptr_t m_holder; //buffer resource holder

    T *m_ptr; //First aligned element pointer of alloced vector.

    size_t m_count; //Current constructed element count.
};

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
     * @param [out] holder Pool resource holder.
     * @param [out] allocSize Total size of alloced vector(count of element).
     * @param allocSizeHint Init alloc size hint for vector(count of element).
     * @return T* First element pointer of alloced vector(nullptr if alloc failed).
     */
    template<typename T> static T* AllocVectorFromPool(LinearBufferCache::ptr_t &holder, size_t &allocSize, size_t allocSizeHint)
    {
        holder = LinearBufferCache::Instance().Get(allocSizeHint * sizeof(T));
        if (holder)
        {
            void *ret = holder->data();
            allocSize = holder->capacity();
            ret = std::align(alignof(T), sizeof(T), ret, allocSize);
            if (ret)
            {
                allocSize /= sizeof(T);
                return reinterpret_cast<T*>(ret);
            }
            holder.reset();
        }
        return nullptr;
    }

    /**
     * @brief Create an vector wrapper of T(No trivial type) from Linear buffer memory pool.
     * 
     * @tparam T vector element type.
     * @param allocSizeHint Init alloc size hint for vector(count of element).
     * @return LinearBufferNoTrivialWrapper<T> Created vector wrapper.
     */
    template<typename T> static LinearBufferNotTrivialWrapper<T> AllocVectorFromPool(size_t allocSizeHint)
    {
        LinearBufferNotTrivialWrapper<T> ret;
        ret.m_ptr = AllocVectorFromPool<T>(ret.m_holder, ret.m_count, allocSizeHint);
        ret.m_count = 0;
        return ret;
    }
};

template<typename T> LinearBufferNotTrivialWrapper<T>::LinearBufferNotTrivialWrapper(
    LinearBufferNotTrivialWrapper &&rhs) noexcept : m_holder(std::move(rhs.m_holder)), m_ptr(rhs.m_ptr)
    , m_count(rhs.m_count)
{
    rhs.m_ptr = nullptr;
    rhs.m_count = 0;
}

template<typename T> LinearBufferNotTrivialWrapper<T>::~LinearBufferNotTrivialWrapper()
{
    for(size_t i = 0; i < m_count; ++i)
    {
        (m_ptr + i)->~T();
    }
}

template<typename T> LinearBufferNotTrivialWrapper<T>& LinearBufferNotTrivialWrapper<T>::operator=(
    LinearBufferNotTrivialWrapper &&rhs) noexcept
{
    m_holder.swap(rhs.m_holder);
    std::swap(m_ptr, rhs.m_ptr);
    std::swap(m_count, rhs.m_count);
    return *this;
}

template<typename T> template<typename... Args> T& LinearBufferNotTrivialWrapper<T>::Construct(Args&&... args)
{
    T *ptr = m_ptr + m_count++;
    new(ptr) T(std::forward<Args>(args)...);
    return *ptr;
}

#endif /* LINEARBUFFERCACHE_H */