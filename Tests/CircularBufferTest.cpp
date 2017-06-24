#include <boost/test/unit_test.hpp>
#include "Buffer/CircularBuffer.h"

BOOST_AUTO_TEST_SUITE(CircularBufferTest)

struct CircularBufferGeneralTestFixture
{
	CircularBuffer m_bufContent;

	CircularBufferGeneralTestFixture()
	{
		m_bufContent.reserve(512);
		m_bufContent.inc_size(127);
		for (size_t i = 0; i < 127; ++i)
		{
			m_bufContent[i] = static_cast<us8>(i + 1);
		}
	}

	~CircularBufferGeneralTestFixture()
	{
	}
};

BOOST_FIXTURE_TEST_CASE(GeneralTest, CircularBufferGeneralTestFixture)
{
	CircularBuffer buf(512);
	BufDescriptor bufs[2];
	size_t bufSize = buf.free_buffers(bufs);
	BOOST_TEST(bufSize == 1);

	for (size_t i = 0; i < bufs[0].m_size; ++i)
	{
		bufs[0].m_beg[i] = static_cast<us8>(i);
	}

	buf.inc_size(512);
	buf.pop_front(256);
	buf.inc_size(128);
	bufSize = buf.free_buffers(bufs);
	BOOST_TEST(bufSize == 1);
	BOOST_TEST(bufs[0].m_size == 128);
	buf.pop_front(257);
	bufSize = buf.free_buffers(bufs);
	BOOST_TEST(bufSize == 2);
	BOOST_TEST(bufs[0].m_size == 384);
	BOOST_TEST(bufs[1].m_size == 1);
	BOOST_TEST((buf == m_bufContent), boost::test_tools::per_element());
}

struct CircularBufferCopyCtrlTestFixture
{
	CircularBuffer m_bufContent;

	CircularBufferCopyCtrlTestFixture()
	{
		m_bufContent.reserve(129);
		m_bufContent.inc_size(129);
		for (size_t i = 0; i < 129; ++i)
		{
			m_bufContent[i] = static_cast<us8>(i + 127);
		}
	}

	~CircularBufferCopyCtrlTestFixture()
	{
	}
};

BOOST_FIXTURE_TEST_CASE(CopyCtrlTest, CircularBufferCopyCtrlTestFixture)
{
	CircularBuffer buf(512);
	BufDescriptor bufs[2];
	buf.free_buffers(bufs);
	for (size_t i = 0; i < bufs[0].m_size; ++i)
	{
		bufs[0].m_beg[i] = static_cast<us8>(i);
	}
	buf.inc_size(256);
	buf.pop_front(127);

	CircularBuffer buf2(buf);
	BOOST_TEST((buf2 == m_bufContent), boost::test_tools::per_element());
	CircularBuffer buf3(std::move(buf2));
	BOOST_TEST((buf3 == m_bufContent), boost::test_tools::per_element());

	buf2 = buf3;
	BOOST_TEST((buf2 == m_bufContent), boost::test_tools::per_element());
	buf2 = CircularBuffer(256);
	buf2 = std::move(buf3);
	BOOST_TEST((buf2 == m_bufContent), boost::test_tools::per_element());

	buf2 = buf2;
	BOOST_TEST(buf2.size() == 129);
	BOOST_TEST((buf2 == m_bufContent), boost::test_tools::per_element());
	buf2 = std::move(buf2);
	BOOST_TEST(buf2.size() == 129);
	BOOST_TEST((buf2 == m_bufContent), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()