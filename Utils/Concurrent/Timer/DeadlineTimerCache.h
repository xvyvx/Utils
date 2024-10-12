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
using DeadlineTimerCache = TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>;

extern template class UTILS_DECL_API ObjectPoolBase
<
    TimerKey
    , boost::asio::deadline_timer
    , TimerElementTrait<boost::asio::deadline_timer>
    , TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>
    , DeadlineTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::deadline_timer>
    , DeadlineTimerCacheLoggerName
>;

extern template class UTILS_DECL_API ObjectPoolElemDeleter
<
    TimerKey
    , boost::asio::deadline_timer
    , TimerElementTrait<boost::asio::deadline_timer>
    , TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>
    , DeadlineTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::deadline_timer>
    , DeadlineTimerCacheLoggerName
>;

extern template class UTILS_DECL_API TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory
    , DeadlineTimerCacheLoggerName>;

#endif /* DEADLINETIMERCACHE_H */