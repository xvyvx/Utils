#include "DeadlineTimerCache.h"
#include "../ThreadPool.h"

const char DeadlineTimerCacheLoggerName[] = "DeadlineTimerCache";

boost::asio::deadline_timer* DeadlineTimerCacheFactory::CreateObj(TimerKey key)
{
    boost::asio::deadline_timer *ret = new boost::asio::deadline_timer(ThreadPool::Instance().Context());
	return ret;
}

void DeadlineTimerCacheFactory::FreeObj(boost::asio::deadline_timer *obj)
{
    delete obj;
}

template class UTILS_DEF_API ObjectPoolBase
<
    TimerKey
    , boost::asio::deadline_timer
    , TimerElementTrait<boost::asio::deadline_timer>
    , TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>
    , DeadlineTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::deadline_timer>
    , DeadlineTimerCacheLoggerName
>;

template class UTILS_DEF_API ObjectPoolElemDeleter
<
    TimerKey
    , boost::asio::deadline_timer
    , TimerElementTrait<boost::asio::deadline_timer>
    , TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory, DeadlineTimerCacheLoggerName>
    , DeadlineTimerCacheFactory
    , TimerCacheBaseClearFunc<boost::asio::deadline_timer>
    , DeadlineTimerCacheLoggerName
>;

template class UTILS_DEF_API TimerCacheBase<boost::asio::deadline_timer, DeadlineTimerCacheFactory
    , DeadlineTimerCacheLoggerName>;