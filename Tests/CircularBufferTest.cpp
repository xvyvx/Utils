#include <boost/test/unit_test.hpp>
#include "Buffer/CircularBuffer.h"
#include "Buffer/LinearBuffer.h"

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
    const CircularBuffer &constBuf = buf;
    BOOST_TEST((constBuf.size() == 0 && constBuf.capacity() == 512));
    BufDescriptor bufs[2];
    size_t bufSize = buf.free_buffers(bufs);
    BOOST_TEST(bufSize == 1);
    BufDescriptor contentBufs[2];
    size_t contentBufSize = buf.content_buffers(contentBufs);
    BOOST_TEST(contentBufSize == 0);

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
    contentBufSize = buf.content_buffers(contentBufs);
    BOOST_TEST(contentBufSize == 2);
    BOOST_TEST(contentBufs[0].m_size == 256);
    BOOST_TEST(contentBufs[1].m_size == 128);
    buf.pop_front(257);
    bufSize = buf.free_buffers(bufs);
    BOOST_TEST(bufSize == 2);
    BOOST_TEST(bufs[0].m_size == 384);
    BOOST_TEST(bufs[1].m_size == 1);
    BOOST_TEST((buf == m_bufContent), boost::test_tools::per_element());
    contentBufSize = buf.content_buffers(contentBufs);
    BOOST_TEST(contentBufSize == 1);
    BOOST_TEST(contentBufs[0].m_size == 127);
    buf.reserve(513);
    BOOST_TEST((buf == m_bufContent), boost::test_tools::per_element());
    buf.pop_front(126);
    buf.inc_size(387);
    buf.reserve(514);
    BOOST_TEST(buf[0] == 127);
    buf.pop_front(buf.size());
    buf.inc_size(1);
    buf.clear();
    BOOST_TEST(buf.size() == 0);

    buf.inc_size(127);
    LinearBuffer linearBuf(513);
    buf.copy_to(linearBuf, 0, 127);
    for (LinearBuffer::size_type i = 0; i < linearBuf.size(); ++i)
    {
        BOOST_TEST(buf[i] == linearBuf[i]);
    }
    buf.pop_front(126);
    buf.inc_size(387);
    buf.copy_to(linearBuf, 0, 388);
    for (LinearBuffer::size_type i = 0; i < linearBuf.size(); ++i)
    {
        BOOST_TEST(buf[i] == linearBuf[i]);
        BOOST_TEST(constBuf[i] == linearBuf[i]);
    }
}

template<typename T> void CircularBufferIteratorTestCore(T &&iter)
{
    BOOST_TEST(*(++iter) == 2);
    BOOST_TEST(*(--iter) == 1);
    BOOST_TEST(*(iter += 2) == 3);
    BOOST_TEST(iter[0] == 3);
    BOOST_TEST(iter[1] == 4);
    BOOST_TEST(*(iter -= 2) == 1);
    BOOST_TEST(iter[0] == 1);
    BOOST_TEST(iter[1] == 2);
    auto iter2 = iter + 2;
    BOOST_TEST(*iter2 == 3);
    iter2 = iter2 - 2;
    BOOST_TEST(*iter2 == 1);
    bool result = iter2 - iter2 == 0;
    BOOST_TEST(result, "iter2 - iter2 == 0");
    ++iter2;
    result = iter < iter2;
    BOOST_TEST(result, "iter < iter2");
}

template<typename T> void CircularBufferReverseIteratorTestCore(T &&iter)
{
    BOOST_TEST(*(++iter) == 126);
    BOOST_TEST(*(--iter) == 127);
    BOOST_TEST(*(iter += 2) == 125);
    BOOST_TEST(*(iter -= 2) == 127);
    auto iter2 = iter + 2;
    BOOST_TEST(*iter2 == 125);
    iter2 = iter2 - 2;
    BOOST_TEST(*iter2 == 127);
    bool result = iter2 - iter2 == 0;
    BOOST_TEST(result, "iter2 - iter2 == 0");
    ++iter2;
    result = iter < iter2;
    BOOST_TEST(result, "iter < iter2");
}

BOOST_FIXTURE_TEST_CASE(IteratorTest, CircularBufferGeneralTestFixture)
{
    const CircularBuffer &constBuf = m_bufContent;
    CircularBufferIteratorTestCore(m_bufContent.begin());
    CircularBufferIteratorTestCore(constBuf.begin());
    CircularBufferIteratorTestCore(constBuf.cbegin());
    CircularBufferReverseIteratorTestCore(m_bufContent.rbegin());
    CircularBufferReverseIteratorTestCore(constBuf.rbegin());
    CircularBufferReverseIteratorTestCore(constBuf.crbegin());
    BOOST_TEST(*--m_bufContent.end() == 127);
    BOOST_TEST(*--constBuf.end() == 127);
    BOOST_TEST(*--constBuf.cend() == 127);
    BOOST_TEST(*--m_bufContent.rend() == 1);
    BOOST_TEST(*--constBuf.rend() == 1);
    BOOST_TEST(*--constBuf.crend() == 1);
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

struct CircularBufferLogicOperatorTestFixture
{
    CircularBuffer m_buf1;

    CircularBuffer m_buf2;

    CircularBufferLogicOperatorTestFixture()
    {
        m_buf1.reserve(128);
        m_buf1.inc_size(128);
        m_buf2.reserve(128);
        m_buf2.inc_size(128);
        for (size_t i = 0; i < m_buf1.size(); ++i)
        {
            m_buf1[i] = static_cast<us8>(i + 127);
            m_buf2[i] = m_buf1[i];
        }
    }

    ~CircularBufferLogicOperatorTestFixture()
    {
    }
};

BOOST_FIXTURE_TEST_CASE(LogicOperatorTest, CircularBufferLogicOperatorTestFixture)
{
    BOOST_TEST(m_buf1 == m_buf2);
    BOOST_TEST(!(m_buf1 != m_buf2));
    m_buf1.reserve(m_buf1.size() + 1);
    m_buf1.inc_size(1);
    m_buf1[m_buf1.size() - 1] = 255;
    BOOST_TEST(m_buf1 > m_buf2);
    BOOST_TEST(m_buf2 < m_buf1);
    m_buf1.pop_front(1);
    BOOST_TEST(m_buf2 < m_buf1);
    BOOST_TEST(!(m_buf1 < m_buf2));
    BOOST_TEST(!(m_buf1 < m_buf1));
}

BOOST_AUTO_TEST_SUITE_END()