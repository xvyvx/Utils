#ifndef OBJECTPOOLBASE_H
#define OBJECTPOOLBASE_H

#include <memory>
#include <list>
#include <vector>
#include <functional>
#include "../Log/Log4cplusCustomInc.h"
#include "../Concurrent/SpinLock.h"

template<
	typename KeyType, 
	typename ElemType, 
	typename ElemTraitType,
	typename SelfType, 
	typename FactoryType,
	typename ClearFuncType,
	const char *LoggerName,
	typename FindPredType = std::equal_to<KeyType>,
	typename GetPredType = std::less_equal<KeyType>
	> class ObjectPoolBase
{
public:
	typedef std::shared_ptr<ElemType> ptr_t;

	ObjectPoolBase(const ObjectPoolBase&) = delete;

	ObjectPoolBase(ObjectPoolBase&&) = delete;

	ObjectPoolBase& operator=(const ObjectPoolBase&) = delete;

	ObjectPoolBase& operator=(ObjectPoolBase&&) = delete;

	~ObjectPoolBase();

	static ObjectPoolBase& Instance()
	{
		if (!instance)
		{
			instance.reset(new SelfType());
		}
		return *instance;
	}

	static void Destory()
	{
		instance.reset();
	}

	void AddToObjectPool(KeyType requireKey, std::size_t count);

	ptr_t Get(KeyType requireKey);

protected:
	ObjectPoolBase();


private:
	typedef struct
	{
		KeyType m_key;

		size_t m_allocatedCount = 0;

		SpinLock<> m_lock;

		std::vector<ElemType*> m_objects;
	} ObjectBlocks;

	std::list<ObjectBlocks> m_blocks;

	static std::shared_ptr<ObjectPoolBase> instance;

	static void ReleaseObject(ElemType *obj);

	static log4cplus::Logger log;
};

#endif /* OBJECTPOOLBASE_H */