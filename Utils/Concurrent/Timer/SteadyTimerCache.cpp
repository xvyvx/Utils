#include "SteadyTimerCache.h"
#include "../ThreadPool.h"

const char SteadyTimerCacheLoggerName[] = "SteadyTimerCache";

boost::asio::steady_timer* SteadyTimerCacheFactory::CreateObj(TimerKey key)
{
    boost::asio::steady_timer *ret = new boost::asio::steady_timer(ThreadPool::Instance().Context());
	return ret;
}

void SteadyTimerCacheFactory::FreeObj(boost::asio::steady_timer *obj)
{
    delete obj;
}

template class UTILS_DEF_API ObjectPoolBase
<
    size_t
    , boost::asio::steady_timer
    , TimerElementTrait<boost::asio::steady_timer>
    , TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory, SteadyTimerCacheLoggerName>
    , SteadyTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::steady_timer>
    , SteadyTimerCacheLoggerName
>;

template class UTILS_DEF_API ObjectPoolElemDeleter
<
    size_t
    , boost::asio::steady_timer
    , TimerElementTrait<boost::asio::steady_timer>
    , TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory, SteadyTimerCacheLoggerName>
    , SteadyTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::steady_timer>
    , SteadyTimerCacheLoggerName
>;

template class UTILS_DEF_API TimerCacheBase<boost::asio::steady_timer, SteadyTimerCacheFactory
    , SteadyTimerCacheLoggerName>;