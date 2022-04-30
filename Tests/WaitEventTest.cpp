#include <atomic>
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include "Concurrent/WaitEvent.h"

WaitEvent evt;

std::atomic<bool> flag(true);

BOOST_AUTO_TEST_SUITE(WaitEventTest)

void WaitTestFunc()
{
    evt.Wait();
    flag.store(false, std::memory_order_release);
    while (!flag.load(std::memory_order_acquire))
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
    }
    evt.Signal();
}

BOOST_AUTO_TEST_CASE(GeneralTest)
{
    BOOST_TEST(evt == true);
    boost::thread t(&WaitTestFunc);
    evt.Signal();
    while (flag.load(std::memory_order_acquire))
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(5));
    }
    bool result = evt.TimedWait(1999);
    BOOST_TEST(result == false);
    flag.store(true, std::memory_order_release);
    evt.Wait();
    t.join();
}

BOOST_AUTO_TEST_SUITE_END()