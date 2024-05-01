#include <boost/test/unit_test.hpp>
#include "Buffer/LinearBuffer.h"

BOOST_AUTO_TEST_SUITE(LinearBufferTest)

BOOST_AUTO_TEST_CASE(CopyCtrlTest)
{
    LinearBuffer buf512;
    BOOST_TEST(buf512.capacity() == 0);
    buf512.reserve(512);
    BOOST_TEST(buf512.capacity() == 512);
    buf512.assign(500, 10);
    BOOST_TEST(buf512.size() == 500);
    LinearBuffer buf512_2(buf512);
    BOOST_TEST(buf512_2 == buf512, boost::test_tools::per_element());
    buf512_2[0] = 1;
    BOOST_TEST((buf512[0] == 10 && buf512_2[0] == 1));
    buf512_2 = buf512;
    BOOST_TEST(buf512_2 == buf512, boost::test_tools::per_element());
    LinearBuffer buf512_3(std::move(buf512_2));
    BOOST_TEST(buf512_2.capacity() == 0);
    BOOST_TEST(buf512_3 == buf512, boost::test_tools::per_element());
    buf512_2 = std::move(buf512_3);
    BOOST_TEST(buf512_3.capacity() == 0);
    BOOST_TEST(buf512_2 == buf512, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(ReadWriteTest)
{
    LinearBuffer buf512;
    BOOST_TEST(buf512.capacity() == 0);
    
#ifndef NDEBUG
    us8 temp[3] = { 1, 2, 3 };
    std::initializer_list<us8> tempList{ 1, 2, 3 };
    BOOST_CHECK_THROW(buf512.assign(1024, 10), std::out_of_range);
    BOOST_CHECK_THROW(buf512.assign(temp + sizeof(temp) / sizeof(us8), temp), std::out_of_range);
    BOOST_CHECK_THROW(buf512.assign(temp, temp + sizeof(temp) / sizeof(us8)), std::out_of_range);
    BOOST_CHECK_THROW(buf512.assign(tempList), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin(), 10, 10), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin() - 1, 10, 10), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.end() + 1, 10, 10), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin(), tempList.begin(), tempList.end()), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin(), tempList.end(), tempList.begin()), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin() - 1, tempList.begin(), tempList.end()), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.end() + 1, tempList.begin(), tempList.end()), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin(), tempList), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin() - 1, tempList), std::out_of_range);
    BOOST_CHECK_THROW(buf512.insert(buf512.end() + 1, tempList), std::out_of_range);
    BOOST_CHECK_THROW(buf512.erase(buf512.begin()), std::out_of_range);
    BOOST_CHECK_THROW(buf512.erase(buf512.begin(), buf512.end()), std::out_of_range);
#endif

    buf512.reserve(512);
    buf512.assign({ 1,2,3 });
    BOOST_TEST((buf512.size() == 3 && buf512[0] == 1 && buf512[1] == 2 && buf512[2] == 3));
    buf512.clear();
    buf512.assign(511, 10);
    BOOST_TEST(buf512.size() == 511);
#ifndef NDEBUG
    BOOST_CHECK_THROW(buf512.insert(buf512.begin(), buf512.begin(), buf512.begin() + 1), std::range_error);
    BOOST_CHECK_THROW(buf512.insert(buf512.begin(), buf512.begin() - 1, buf512.end() - 10), std::out_of_range);
#endif
    BOOST_TEST((buf512.front() == 10 && buf512.back() == 10 && buf512.at(0) == 10 && buf512[510] == 10 && *buf512.data() == 10));
    buf512.push_back(11);
    BOOST_TEST((buf512.size() == 512 && buf512[511] == 11));
#ifndef NDEBUG
    BOOST_CHECK_THROW(buf512.push_back(11), std::out_of_range);
#endif
    buf512.erase(buf512.begin());
#ifndef NDEBUG
    BOOST_CHECK_THROW(buf512.erase(buf512.begin() - 1), std::out_of_range);
    BOOST_CHECK_THROW(buf512.erase(buf512.end() + 1), std::out_of_range);
#endif
    BOOST_TEST(buf512.size() == 511);
    buf512.erase(buf512.begin(), buf512.end());
    BOOST_TEST((buf512.empty() && buf512.size() == 0));
#ifndef NDEBUG
    BOOST_CHECK_THROW(buf512.resize(1024, 10), std::out_of_range);
#endif
    buf512.resize(5, 100);
    buf512[4] = 99;
    LinearBuffer::size_type index = 5;
    BOOST_TEST(*buf512.rbegin() == 99, "*rbegin() == 99 has failed [" << static_cast<us32>(*buf512.rbegin()) << " == 99]");
    for (auto i = ++buf512.rbegin(); i != buf512.rend(); ++i, --index)
    {
        BOOST_TEST(*i == 100, "*i == 100 has failed [" << static_cast<us32>(*i) << " == 100], index:" << index);
    }
    buf512.pop_back();
    BOOST_TEST(buf512.size() == 4);
    buf512.insert(buf512.begin() + 1, 1);
    BOOST_TEST(buf512[1] == 1);
    buf512.insert(buf512.begin() + 1, 2, 2);
    BOOST_TEST((buf512[1] == 2 && buf512[2] == 2));
    buf512.insert(buf512.begin() + 1, { 3,3,3 });
    BOOST_TEST((buf512[1] == 3 && buf512[2] == 3 && buf512[3] == 3));
    buf512.resize(2, 0);
    BOOST_TEST((buf512[0] == 100 && buf512[1] == 3));
    buf512.clear();
    BOOST_TEST((buf512.empty() && buf512.size() == 0 && buf512.capacity() == 512 && buf512.max_size() == buf512.capacity()));
    buf512.inc_size(512);
    buf512[0] = buf512[256] = buf512[511] = 255;
    BOOST_TEST((buf512.size() == 512 && buf512[0] == buf512[256] && buf512[256] == buf512[511] && buf512[511] == 255));
}

BOOST_AUTO_TEST_SUITE_END()