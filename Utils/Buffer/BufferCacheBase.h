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

	static BufferCacheBaseClearFunc Instance;
};

template<typename T> BufferCacheBaseClearFunc<T> BufferCacheBaseClearFunc<T>::Instance;

template<typename T> class BufferCacheBasePredicatorFunc
{
public:
	inline bool operator()(T *obj, size_t size)
	{
		return size == obj->capacity();
	}

	static BufferCacheBasePredicatorFunc Instance;
};

template<typename T> BufferCacheBasePredicatorFunc<T> BufferCacheBasePredicatorFunc<T>::Instance;

template<typename T, typename FactoryType, FactoryType *Factory, const char *LoggerName> class BufferCacheBase :
	public ObjectPoolBase<size_t, T, BufferCacheBase<T, FactoryType, Factory, LoggerName>, FactoryType, Factory, BufferCacheBaseClearFunc<T>, &BufferCacheBaseClearFunc<T>::Instance, BufferCacheBasePredicatorFunc<T>, &BufferCacheBasePredicatorFunc<T>::Instance, LoggerName>
{
protected:
	friend class ObjectPoolBase<size_t, T, BufferCacheBase<T, FactoryType, Factory, LoggerName>, FactoryType, Factory, BufferCacheBaseClearFunc<T>, &BufferCacheBaseClearFunc<T>::Instance, BufferCacheBasePredicatorFunc<T>, &BufferCacheBasePredicatorFunc<T>::Instance, LoggerName>;

	BufferCacheBase()
	{
	}
};

#endif /* BUFFERCACHEBASE_H */