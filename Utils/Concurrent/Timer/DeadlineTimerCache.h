#ifndef DEADLINETIMERCACHE_H
#define DEADLINETIMERCACHE_H

#include "TimerCacheBase.h"

extern const char DeadlineTimerCacheLoggerName[];

class DeadlineTimerCacheFactory
{
public:
    boost::asio::deadline_timer* CreateObj(TimerKey key);

    void FreeObj(boost::asio::deadline_timer *obj);
};

/**
 * @brief Deadline Timer cache impletemented by @ref TimerCacheBase.
 * 
 */
typedef TimerCacheBase<boost::asio::steady_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName> DeadlineTimerCache;

extern template class UTILS_DECL_API ObjectPoolBase
<
    size_t
    , boost::asio::steady_timer
    , TimerElementTrait<boost::asio::steady_timer>
    , TimerCacheBase<boost::asio::steady_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>
    , DeadlineTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::steady_timer>
    , DeadlineTimerCacheLoggerName
>;

extern template class UTILS_DECL_API ObjectPoolElemDeleter
<
    size_t
    , boost::asio::steady_timer
    , TimerElementTrait<boost::asio::steady_timer>
    , TimerCacheBase<boost::asio::steady_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>
    , DeadlineTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::steady_timer>
    , DeadlineTimerCacheLoggerName
>;

extern template class UTILS_DECL_API TimerCacheBase<boost::asio::steady_timer, DeadlineTimerCacheFactory
    , DeadlineTimerCacheLoggerName>;

#endif /* DEADLINETIMERCACHE_H */