#if defined(linux) || defined(__linux)

#include <boost/test/unit_test.hpp>
#include "AppEntry/Signal/UnixSignalHelper.h"

BOOST_AUTO_TEST_SUITE(UnixSignalHelperTest)

BOOST_AUTO_TEST_CASE(DiscardChildInfoTest)
{
    BOOST_TEST(UnixSignalHelper::DiscardChildProcessExitInfo(), "Discard child info failed.");
}

BOOST_AUTO_TEST_SUITE_END()

#endif