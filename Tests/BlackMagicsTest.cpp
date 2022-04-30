#include <boost/test/unit_test.hpp>
#include "Concurrent/BlackMagics.h"

BOOST_AUTO_TEST_SUITE(BlackMagicsTest)

BOOST_AUTO_TEST_CASE(GeneralTest)
{
    for (int i = 0; i < 100; ++i)
    {
        DefaultBlackMagicFunc(i);
    }
}

BOOST_AUTO_TEST_SUITE_END()