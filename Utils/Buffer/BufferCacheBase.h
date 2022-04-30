#ifndef BUFFERCACHEBASE_H
#define BUFFERCACHEBASE_H

#include "../Common/ObjectPoolBase.h"

template<typename T> class BufferCacheBaseClearFunc
{
public:
    inline void operator()(T *obj)
    {
        obj->clear();
    }
};

template<typename T> class BufferElementTrait
{
public:
    static size_t GetKey(const T &obj)
    {
        return obj.capacity();
    }
};

/**
 * A buffer cache base class.
 *
 * @tparam T Generic buffer type.
 * @tparam FactoryType Type of the factory type.
 * @tparam LoggerName C-style string of logger name.
 */
template<typename T, typename FactoryType, const char *LoggerName> class BufferCacheBase :
    public ObjectPoolBase<size_t, T, BufferElementTrait<T>, BufferCacheBase<T, FactoryType, LoggerName>, FactoryType, BufferCacheBaseClearFunc<T>, LoggerName>
{
protected:
    friend class ObjectPoolBase<size_t, T, BufferElementTrait<T>, BufferCacheBase<T, FactoryType, LoggerName>, FactoryType, BufferCacheBaseClearFunc<T>, LoggerName>;

    /**
     * Default constructor
     */
    BufferCacheBase()
    {
    }
};

#endif /* BUFFERCACHEBASE_H */