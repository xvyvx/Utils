#include <boost/test/unit_test.hpp>
#include "Diagnostics/DiagnosticsHelper.h"

BOOST_AUTO_TEST_SUITE(DiagnosticsTest)

BOOST_AUTO_TEST_CASE(GetCPUUsageTest)
{
	int usage = DiagnosticsHelper::GetCPUUsage();
	BOOST_TEST((usage >= 0 && usage <= 100));
}

BOOST_AUTO_TEST_SUITE_END()