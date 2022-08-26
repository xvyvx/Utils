#ifndef STEADYTIMERCACHE_H
#define STEADYTIMERCACHE_H

#include "TimerCacheBase.h"

extern const char SteadyTimerCacheLoggerName[];

class SteadyTimerCacheFactory
{
public:
    boost::asio::steady_timer* CreateObj(TimerKey key);

    void FreeObj(boost::asio::steady_timer *obj);
};

/**
 * @brief Steady Timer cache impletemented by @ref TimerCacheBase.
 * 
 */
typedef TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory, SteadyTimerCacheLoggerName> SteadyTimerCache;

extern template class UTILS_DECL_API ObjectPoolBase
<
    TimerKey
    , boost::asio::steady_timer
    , TimerElementTrait<boost::asio::steady_timer>
    , TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory, SteadyTimerCacheLoggerName>
    , SteadyTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::steady_timer>
    , SteadyTimerCacheLoggerName
>;

extern template class UTILS_DECL_API ObjectPoolElemDeleter
<
    TimerKey
    , boost::asio::steady_timer
    , TimerElementTrait<boost::asio::steady_timer>
    , TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory, SteadyTimerCacheLoggerName>
    , SteadyTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::steady_timer>
    , SteadyTimerCacheLoggerName
>;

extern template class UTILS_DECL_API TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory
    , SteadyTimerCacheLoggerName>;

#endif /* STEADYTIMERCACHE_H */