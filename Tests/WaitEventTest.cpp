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
    boost::timer::cpu_timer timer;
    bool result = evt.TimedWait(500);
    timer.stop();
    boost::timer::cpu_times elapsedTime = timer.elapsed();
    BOOST_TEST((result == false && elapsedTime.wall / 1000000 >= 500 && elapsedTime.wall / 1000000 <= 600));
    flag.store(true, std::memory_order_release);
    evt.Wait();
    t.join();
}

BOOST_AUTO_TEST_SUITE_END()