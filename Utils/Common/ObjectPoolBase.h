#ifndef OBJECTPOOLBASE_H
#define OBJECTPOOLBASE_H

#include <memory>
#include <list>
#include <vector>
#include <functional>
#include "../Log/Log4cplusCustomInc.h"
#include "../Concurrent/SpinLock.h"

/**
* An object pool element deleter.
*
* @tparam KeyType Object key type used to indetify same objects.
* @tparam ElemType  Element type.
* @tparam ElemTraitType  Element trait type.
* @tparam SelfType  Derived pool type.
* @tparam FactoryType  Factory type used to create elements.
* @tparam ClearFuncType  Function type used to reset object status when returned to pool.
* @tparam LoggerName  C-style string of logger name.
* @tparam FindPredType  Function type used to match key when adding object to pool(default is std::equal_to<KeyType>).
* @tparam GetPredType  Function type used to match key when getting object from pool(default is std::less_equal<KeyType>).
 */
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
    > class ObjectPoolElemDeleter
{
public:
    /**
     * Delete function.
     * 
     * If the pool is destoried,the obj will be deleted.
     *
     * @param obj Returned resource object.
     */
    void operator()(ElemType *obj);
};

/**
* Object pool Implementation.
*
* @tparam KeyType Object key type used to indetify same objects.
* @tparam ElemType  Element type.
* @tparam ElemTraitType  Element trait type.
* @tparam SelfType  Derived pool type.
* @tparam FactoryType  Factory type used to create elements.
* @tparam ClearFuncType  Function type used to reset object status when returned to pool.
* @tparam LoggerName  C-style string of logger name.
* @tparam FindPredType  Function type used to match key when adding object to pool(default is std::equal_to<KeyType>).
* @tparam GetPredType  Function type used to match key when getting object from pool(default is std::less_equal<KeyType>).
*/
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
    friend class ObjectPoolElemDeleter<KeyType, ElemType, ElemTraitType, SelfType, FactoryType, ClearFuncType, LoggerName, FindPredType, GetPredType>;

    using ObjectDeleter = ObjectPoolElemDeleter<KeyType, ElemType, ElemTraitType, SelfType, FactoryType, ClearFuncType, LoggerName, FindPredType, GetPredType>;

    /**
     * Defines an alias representing the pointer whitch is used by caller.
     */
    using ptr_t = std::unique_ptr<ElemType, ObjectDeleter>;

    ObjectPoolBase(const ObjectPoolBase&) = delete;

    ObjectPoolBase(ObjectPoolBase&&) = delete;

    ObjectPoolBase& operator=(const ObjectPoolBase&) = delete;

    ObjectPoolBase& operator=(ObjectPoolBase&&) = delete;

    /**
     * Destructor,will release all objects owned by this pool.
     */
    ~ObjectPoolBase();

    /**
     * Gets the pool instance
     *
     * @return A reference to the global instance of ObjectPoolBase.
     */
    static SelfType& Instance()
    {
        if (!instance)
        {
            instance.reset(new SelfType());
        }
        return *instance;
    }

    /**
     * Destories the global instance of ObjectPoolBase.
     */
    static void Destory()
    {
        instance.reset();
    }

    /**
     * Adds resource object to the object pool.
     *
     * @param requireKey The resource object's key(used to identify same resource object).
     * @param count Number of added resource object.
     */
    void AddToObjectPool(KeyType requireKey, std::size_t count);

    /**
     * Gets a resource object identified by given key
     *
     * @param requireKey Object key.
     *
     * @return A ptr_t(resource object is owned by this pointer).
     */
    ptr_t Get(KeyType requireKey);

protected:

    /**
     * Default constructor
     */
    ObjectPoolBase();

    /**
     * @brief Get logger
     * 
     * @return log4cplus::Logger& logger
     */
    static log4cplus::Logger& Logger()
    {
        return log;
    }


private:
    using ObjectBlocks = struct _ObjectBlocks
    {
        KeyType m_key;  /**< Block key */

        size_t m_allocatedCount = 0;	/**< Total number of allocated objects */

        SpinLock<> m_lock;  /**< Thread sync lock */

        std::vector<ElemType*> m_objects;   /**< Free objects */
    }; /**< Internal object block type */

    std::list<ObjectBlocks> m_blocks;   /**< Internal object blocks */

    static std::shared_ptr<SelfType> instance;	/**< Global pool instance */

    static log4cplus::Logger log;   /**< The logger */
};

#endif /* OBJECTPOOLBASE_H */