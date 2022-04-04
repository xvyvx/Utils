#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "Concurrent/Timer/DeadlineTimerCache.h"
#include "Concurrent/Timer/SteadyTimerCache.h"
#include "Concurrent/WaitEvent.h"

WaitEvent timerNotifyEvt;

void TimerTimeoutHandler(const boost::system::error_code &err)
{
    BOOST_TEST(!err, "Timer timeout error:" << err.message());
    timerNotifyEvt.Signal();
}

BOOST_AUTO_TEST_SUITE(TimerCacheTest)

BOOST_AUTO_TEST_CASE(DeadlineTimerCacheTest)
{
    ThreadPool::Instance();
    DeadlineTimerCache::Instance().AddToObjectPool(DefaultTimerCacheKey, 10);
    timerNotifyEvt.Reset();
    DeadlineTimerCache::Instance().QueueThreadPoolWorkItemAfter(boost::posix_time::milliseconds(30)
        , &TimerTimeoutHandler);
    BOOST_TEST(timerNotifyEvt.TimedWait(50), "Wait deadline timer event timeout.");
    DeadlineTimerCache::Instance().Destory();
    ThreadPool::Instance().Stop();
    ThreadPool::Destory();
}

BOOST_AUTO_TEST_CASE(SteadyTimerCacheTest)
{
    ThreadPool::Instance();
    SteadyTimerCache::Instance().AddToObjectPool(DefaultTimerCacheKey, 10);
    SteadyTimerCache::ptr_t timer = SteadyTimerCache::Instance().Get(DefaultTimerCacheKey);
    BOOST_TEST((timer.get() != nullptr));
    timerNotifyEvt.Reset();
    boost::system::error_code err;
    timer->expires_from_now(std::chrono::milliseconds(30), err);
    BOOST_TEST(!err, "Begin steady timer error:" << err.message());
    timer->async_wait(TimerTimeoutHandler);
    BOOST_TEST(timerNotifyEvt.TimedWait(50), "Wait steady timer event timeout.");
    SteadyTimerCache::Instance().Destory();
    timer.reset();
    ThreadPool::Instance().Stop();
    ThreadPool::Destory();
}

BOOST_AUTO_TEST_SUITE_END()