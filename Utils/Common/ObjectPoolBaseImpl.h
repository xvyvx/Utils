#ifndef OBJECTPOOLBASEIMPL_H
#define OBJECTPOOLBASEIMPL_H

#include "ObjectPoolBase.h"
#include <assert.h>
#include <algorithm>

#define OBJECT_POOL_BASE_TEMPLATE template<typename KeyType, typename ElemType, typename SelfType, typename FactoryType, FactoryType *Factory, typename ClearFuncType, ClearFuncType *ClearFunc, typename PredicatorType, PredicatorType *PredicatorFunc, const char *LoggerName>
#define OBJECT_POOL_BASE_FULL_TYPE_NAME ObjectPoolBase<KeyType, ElemType, SelfType, FactoryType, Factory, ClearFuncType, ClearFunc, PredicatorType, PredicatorFunc, LoggerName>

OBJECT_POOL_BASE_TEMPLATE log4cplus::Logger OBJECT_POOL_BASE_FULL_TYPE_NAME::log = log4cplus::Logger::getInstance(LoggerName);

OBJECT_POOL_BASE_TEMPLATE std::shared_ptr<OBJECT_POOL_BASE_FULL_TYPE_NAME> OBJECT_POOL_BASE_FULL_TYPE_NAME::instance;

OBJECT_POOL_BASE_TEMPLATE OBJECT_POOL_BASE_FULL_TYPE_NAME::ObjectPoolBase() : m_blocks()
{
}

OBJECT_POOL_BASE_TEMPLATE OBJECT_POOL_BASE_FULL_TYPE_NAME::~ObjectPoolBase()
{
	for (auto &blocks : m_blocks)
	{
		for (auto obj : blocks.m_objects)
		{
			delete obj;
		}
	}
}

OBJECT_POOL_BASE_TEMPLATE void OBJECT_POOL_BASE_FULL_TYPE_NAME::AddToObjectPool(KeyType requireKey, std::size_t count)
{
	typename std::list<ObjectBlocks>::iterator target = std::find_if(m_blocks.begin(), m_blocks.end(),
		[requireKey](const ObjectBlocks &val)->bool
	{
		return val.m_key == requireKey;
	}
	);
	ObjectBlocks *currentBlock;
	if (target == m_blocks.end())
	{
		m_blocks.emplace_back();
		currentBlock = &m_blocks.back();
		currentBlock->m_key = requireKey;
		currentBlock->m_objects.reserve(count);
	}
	else
	{
		currentBlock = &*target;
		currentBlock->m_objects.reserve(currentBlock->m_allocatedCount + count);
	}
	size_t i = 0;
	try
	{
		for (; i < count; ++i)
		{
			currentBlock->m_objects.push_back((*Factory)(requireKey));
		}
		currentBlock->m_allocatedCount += count;
	}
	catch (...)
	{
		currentBlock->m_allocatedCount += i;
		throw;
	}
}

OBJECT_POOL_BASE_TEMPLATE typename OBJECT_POOL_BASE_FULL_TYPE_NAME::ptr_t OBJECT_POOL_BASE_FULL_TYPE_NAME::Get(KeyType requireKey)
{
	typename std::list<ObjectBlocks>::iterator target = std::find_if(m_blocks.begin(), m_blocks.end(),
		[requireKey](const ObjectBlocks &val)->bool
	{
		return val.m_key >= requireKey;
	}
	);
	if (target == m_blocks.end())
	{
		return ptr_t();
	}
	else if (target->m_objects.size() == 0)
	{
		SpinLock<>::ScopeLock lock(target->m_lock);
		size_t allocSize = target->m_allocatedCount;
		try
		{
			AddToObjectPool(target->m_key, allocSize);
		}
		catch (const std::bad_alloc&)
		{
		}
		if (target->m_objects.size() == 0)
		{
			LOG4CPLUS_DEBUG_FMT(log, "对象池再申请失败，请求Key：%zu，匹配Key：%zu，申请个数：%zu。", static_cast<size_t>(requireKey), static_cast<size_t>(target->m_key)
				, allocSize);
			return ptr_t();
		}
		else
		{
			LOG4CPLUS_DEBUG_FMT(log, "对象池再申请成功，请求Key：%zu，匹配Key：%zu，当前空闲个数：%zu。", static_cast<size_t>(requireKey)
				, static_cast<size_t>(target->m_key), target->m_objects.size());
			ptr_t result(target->m_objects.back(), OBJECT_POOL_BASE_FULL_TYPE_NAME::ReleaseObject);
			target->m_objects.pop_back();
			return result;
		}
	}
	else
	{
		SpinLock<>::ScopeLock lock(target->m_lock);
		ptr_t result(target->m_objects.back(), OBJECT_POOL_BASE_FULL_TYPE_NAME::ReleaseObject);
		target->m_objects.pop_back();
		return result;
	}
}

OBJECT_POOL_BASE_TEMPLATE void OBJECT_POOL_BASE_FULL_TYPE_NAME::ReleaseObject(ElemType *obj)
{
	if (!instance)
	{
		delete obj;
	}
	else if (obj)
	{
		(*ClearFunc)(obj);
		typename std::list<ObjectBlocks>::iterator target = instance->m_blocks.end();
		for(typename std::list<ObjectBlocks>::iterator i=instance->m_blocks.begin();i!=instance->m_blocks.end();++i)
		{
			if ((*PredicatorFunc)(obj, i->m_key))
			{
				target = i;
				break;
			}
		}
		assert(target != instance->m_blocks.end());
		SpinLock<>::ScopeLock lock(target->m_lock);
		//这里不会失败，因为vector存指针的内存之前在构建池或是临时增加池大小时加上去了，只要不显示调用方法缩小空间，这部分空间就不会还回去（如果没加上去就不会有这个对象指针了）
		target->m_objects.push_back(obj);
	}
}

#endif /* OBJECTPOOLBASEIMPL_H */