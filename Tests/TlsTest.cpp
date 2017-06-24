#include <boost/test/unit_test.hpp>
#include "Concurrent/TlsHelper.h"

BOOST_AUTO_TEST_SUITE(TlsTest)

BOOST_AUTO_TEST_CASE(GeneralTest)
{
	std::string *result = TlsHelper<std::string>::Get("test1");
	BOOST_TEST(!result);
	TlsHelper<std::string>::Set("test1", new std::string("hello"));
	result = TlsHelper<std::string>::Get("test1");
	BOOST_TEST(*result == "hello");

	int *result2 = TlsHelper<int>::Get("test1");
	BOOST_TEST(!result2);
	TlsHelper<int>::Set("test1", new int(100));
	result2 = TlsHelper<int>::Get("test1");
	BOOST_TEST(*result2 == 100);
}

BOOST_AUTO_TEST_SUITE_END()