#ifndef TIMERCACHEBASE_H
#define TIMERCACHEBASE_H

#include "../../Common/ObjectPoolBase.hpp"
#include "../ThreadPool.h"

using TimerKey = int; //Timer object pool key type

constexpr TimerKey DefaultTimerCacheKey = 0; //Timer object pool default key value

/**
 * @brief Timer object which is in the pool clean function.
 * 
 * @tparam T Timer type
 */
template <typename T> class TimerCacheBaseClearFunc
{
public:
    /**
     * @brief Clean specified timer object
     * 
     * @param obj Specified timer object
     */
    inline void operator()(T *obj)
    {
        boost::system::error_code err;
        obj->cancel(err);
    }
};

/**
 * @brief Timer object trait type.
 * 
 * @tparam T Timer type
 */
template <typename T> class TimerElementTrait
{
public:
    /**
     * @brief Get the object pool key value from timer object.
     * 
     * @param obj timer object
     * @return constexpr TimerKey object pool key value
     */
    static constexpr TimerKey GetKey(const T &obj)
    {
        return DefaultTimerCacheKey;
    }
};

/**
 * @brief Timer object pool base implementation.
 * 
 * @tparam T Timer type
 * @tparam FactoryType Timer factory type
 * @tparam *LoggerName Logger name
 */
template <typename T, typename FactoryType, const char *LoggerName> class TimerCacheBase
    : public ObjectPoolBase
    <
        size_t
        , T
        , TimerElementTrait<T>
        , TimerCacheBase<T, FactoryType, LoggerName>
        , FactoryType
        , TimerCacheBaseClearFunc<T>
        , LoggerName
    >
{
protected:
    typedef ObjectPoolBase<size_t, T, TimerElementTrait<T>, TimerCacheBase<T, FactoryType, LoggerName>, FactoryType
        , TimerCacheBaseClearFunc<T>, LoggerName> BaseType;

    friend BaseType;

    /**
     * @brief Default constructor
     * 
     */
    TimerCacheBase()
    {
    }

private:
    /**
     * @brief Timer callback entry
     * 
     * @tparam FuncType Task function type
     * @tparam Args Task function argument types
     * @param err Timer error code
     * @param timer timer
     * @param func Task function object
     * @param args Task function arguments
     */
    template<typename FuncType, typename... Args> static void OnThreadPoolWorkItemDelayTimer(const boost::system::error_code &err
        , typename BaseType::ptr_t &timer, FuncType &func, Args&...args)
    {
        if(err)
        {
            LOG4CPLUS_ERROR_FMT(BaseType::Logger(), "等待队列任务延时错误（%s）。", err.message().c_str());
        }
        QueueThreadPoolWorkItem(std::forward<FuncType>(func), err, std::forward<Args>(args)...);
    }

public:
    /**
     * @brief Queue user request task into thread pool after specified time.
     * 
     * @tparam DurationType Duration time type
     * @tparam FuncType Task function type
     * @tparam Args Task function argument types
     * @param duration specified duration time
     * @param func User request task function object
     * @param args Task function arguments
     */
    template<typename DurationType, typename FuncType, typename... Args> void QueueThreadPoolWorkItemAfter(DurationType duration, FuncType &&func
        , Args&&... args)
    {
        typename BaseType::ptr_t timer = BaseType::Get(DefaultTimerCacheKey);
        if(!timer)
        {
            LOG4CPLUS_ERROR(BaseType::Logger(), "获取Timer失败。");
            boost::system::error_code err(boost::system::errc::resource_unavailable_try_again
                , boost::system::generic_category());
            QueueThreadPoolWorkItem(std::forward<FuncType>(func), err, std::forward<Args>(args)...);
            return;
        }
        typename BaseType::ptr_t::pointer rawPtr = timer.get();
        boost::system::error_code err;
        timer->expires_from_now(duration, err);
        if(err)
        {
            LOG4CPLUS_ERROR_FMT(BaseType::Logger(), "设置等待事件错误（%s）。", err.message().c_str());
            QueueThreadPoolWorkItem(std::forward<FuncType>(func), err, std::forward<Args>(args)...);
            return;
        }
        rawPtr->async_wait(std::bind(&TimerCacheBase::OnThreadPoolWorkItemDelayTimer<FuncType, Args...>
            , std::placeholders::_1, std::move(timer), std::forward<FuncType>(func), std::forward<Args>(args)...));
    }
};

#endif /* TIMERCACHEBASE_H */