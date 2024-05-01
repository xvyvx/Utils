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
        TimerKey
        , T
        , TimerElementTrait<T>
        , TimerCacheBase<T, FactoryType, LoggerName>
        , FactoryType
        , TimerCacheBaseClearFunc<T>
        , LoggerName
    >
{
protected:
    typedef ObjectPoolBase<TimerKey, T, TimerElementTrait<T>, TimerCacheBase<T, FactoryType, LoggerName>, FactoryType
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
     * @brief Timeout handler wrapper that wraps timeout callback functor.
     * 
     * @tparam HandlerType Timeout callback functor type.
     */
    template<typename HandlerType> class TimeoutHandlerWrapper
    {
    public:
        /**
         * @brief Constructor
         * 
         * @param timer timer used to execute timeout handler.
         * @param handler Task function object.
         */
        TimeoutHandlerWrapper(typename BaseType::ptr_t &timer, HandlerType &&handler) : m_timer(std::move(timer))
            , m_handler(std::forward<HandlerType>(handler))
        {
        }

        /**
         * @brief Move constructor
         * 
         * @param rhs Moved object
         */
        TimeoutHandlerWrapper(TimeoutHandlerWrapper &&rhs) noexcept : m_timer(std::move(rhs.m_timer))
            , m_handler(std::forward<HandlerType>(rhs.m_handler))
        {
        }

        /**
         * @brief Move operator=
         * 
         * @param rhs Moved object
         * @return Self object
         */
        TimeoutHandlerWrapper& operator=(TimeoutHandlerWrapper &&rhs) noexcept
        {
            m_timer = std::move(rhs.m_timer);
            m_handler = std::forward<HandlerType>(rhs.m_handler);
        }

        /**
         * @brief Timer callback entry
         * 
         * @param err Timeout error code
         */
        void operator()(const boost::system::error_code &err)
        {
            m_handler(err);
        }

    private:
        typename BaseType::ptr_t m_timer; //timeout timer

        typename std::decay<HandlerType>::type m_handler; //Timeout handler
    };

public:
    /**
     * @brief Queue user request task into thread pool after specified time.
     * 
     * @tparam DurationType Duration time type
     * @tparam FuncType Task function type
     * @param duration specified duration time
     * @param func User request task function object
     */
    template<typename DurationType, typename FuncType> void QueueThreadPoolWorkItemAfter(DurationType duration, FuncType &&func)
    {
        typename BaseType::ptr_t timer = BaseType::Get(DefaultTimerCacheKey);
        if(!timer)
        {
            LOG4CPLUS_ERROR(BaseType::Logger(), "获取Timer失败。");
            QueueThreadPoolWorkItem([handler = std::forward<FuncType>(func)]() mutable
                {
                    boost::system::error_code err(boost::system::errc::resource_unavailable_try_again
                        , boost::system::system_category());
                    handler(err);
                });
            return;
        }
        typename BaseType::ptr_t::pointer rawPtr = timer.get();
        boost::system::error_code err;
        timer->expires_from_now(duration, err);
        if(err)
        {
            LOG4CPLUS_ERROR_FMT(BaseType::Logger(), "设置等待事件错误（%s）。", err.message().c_str());
            QueueThreadPoolWorkItem(
                [handler = std::forward<FuncType>(func), error = err]() mutable
                {
                    handler(error);
                });
            return;
        }
        rawPtr->async_wait(TimeoutHandlerWrapper<FuncType>(timer, std::forward<FuncType>(func)));
    }
};

#endif /* TIMERCACHEBASE_H */