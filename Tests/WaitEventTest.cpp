#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include "Concurrent/WaitEvent.h"

WaitEvent evt1;

WaitEvent evt2;

BOOST_AUTO_TEST_SUITE(WaitEventTest)

void WaitTestFunc()
{
    evt1.Wait();
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    evt2.Signal();
}

BOOST_AUTO_TEST_CASE(GeneralTest)
{
    BOOST_TEST(evt1 == true);
    BOOST_TEST(evt2 == true);
    boost::thread t(&WaitTestFunc);
    evt1.Signal();
    bool result = evt2.TimedWait(500);
    BOOST_TEST(result == false);
    evt2.Wait();
    t.join();
}

BOOST_AUTO_TEST_SUITE_END()