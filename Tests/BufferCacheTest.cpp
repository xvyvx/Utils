#include <boost/test/unit_test.hpp>
#include "Buffer/CircularBufferCache.h"
#include "Buffer/LinearBufferCache.h"

BOOST_AUTO_TEST_SUITE(BufferCacheTest)

BOOST_AUTO_TEST_CASE(CircularBufferCacheTest)
{
    CircularBufferCache::Instance().AddToObjectPool(512, 30000);
    CircularBufferCache::Instance().AddToObjectPool(1024, 30000);
    CircularBufferCache::Instance().AddToObjectPool(2048, 30000);
    CircularBufferCache::Instance().AddToObjectPool(4096, 30000);
    CircularBufferCache::ptr_t buf512 = CircularBufferCache::Instance().Get(256);
    BOOST_TEST((buf512.get() != nullptr));
    BOOST_TEST((buf512->capacity() == static_cast<size_t>(512)));
    CircularBufferCache::ptr_t buf1024 = CircularBufferCache::Instance().Get(999);
    BOOST_TEST((buf1024.get() != nullptr));
    BOOST_TEST((buf1024->capacity() == static_cast<size_t>(1024)));
    CircularBufferCache::ptr_t buf2048 = CircularBufferCache::Instance().Get(1025);
    BOOST_TEST((buf2048.get() != nullptr));
    BOOST_TEST((buf2048->capacity() == static_cast<size_t>(2048)));
    CircularBufferCache::ptr_t buf4096 = CircularBufferCache::Instance().Get(4096);
    BOOST_TEST((buf4096.get() != nullptr));
    BOOST_TEST((buf4096->capacity() == static_cast<size_t>(4096)));
    CircularBufferCache::ptr_t bufNull = CircularBufferCache::Instance().Get(4097);
    BOOST_TEST((bufNull.get() == nullptr));
    CircularBufferCache::Instance().Destory();
}

BOOST_AUTO_TEST_CASE(LinearBufferCacheTest)
{
    LinearBufferCache::Instance().AddToObjectPool(512, 30000);
    LinearBufferCache::Instance().AddToObjectPool(1024, 30000);
    LinearBufferCache::Instance().AddToObjectPool(2048, 30000);
    LinearBufferCache::Instance().AddToObjectPool(4096, 1);
    LinearBufferCache::ptr_t buf4096 = LinearBufferCache::Instance().Get(4096);
    buf4096 = LinearBufferCache::Instance().Get(4096);
    BOOST_TEST(buf4096.get());
    LinearBufferCache::ptr_t buf4096_2 = LinearBufferCache::Instance().Get(4096);
    BOOST_TEST(buf4096_2.get());

    LinearBufferCache::ptr_t buf512 = LinearBufferCache::Instance().Get(256);
    buf512->push_back(1);
    BOOST_TEST((buf512.get() != nullptr));
    buf512.reset();
    buf512 = LinearBufferCache::Instance().Get(256);
    BOOST_TEST((buf512->size() == 0));
    BOOST_TEST((buf512->capacity() == static_cast<size_t>(512)));
    LinearBufferCache::ptr_t buf1024 = LinearBufferCache::Instance().Get(999);
    BOOST_TEST((buf1024.get() != nullptr));
    BOOST_TEST((buf1024->capacity() == static_cast<size_t>(1024)));
    LinearBufferCache::ptr_t buf2048 = LinearBufferCache::Instance().Get(1025);
    BOOST_TEST((buf2048.get() != nullptr));
    BOOST_TEST((buf2048->capacity() == static_cast<size_t>(2048)));
    buf4096 = LinearBufferCache::Instance().Get(4096);
    BOOST_TEST((buf4096.get() != nullptr));
    BOOST_TEST((buf4096->capacity() == static_cast<size_t>(4096)));
    LinearBufferCache::ptr_t bufNull = LinearBufferCache::Instance().Get(4097);
    BOOST_TEST((bufNull.get() == nullptr));
    LinearBufferCache::Instance().Destory();
}

BOOST_AUTO_TEST_CASE(LinearBufferCacheVectorTest)
{
    struct TestAlloc
    {
        us8 m_val8;

        us32 m_val32;
    };
    LinearBufferCache::Instance().AddToObjectPool(2, 100);
    LinearBufferCache::Instance().AddToObjectPool(8, 100);
    LinearBufferCache::Instance().AddToObjectPool(1024, 100);
    LinearBufferCache::ptr_t buffer;
    size_t allocSize;
    TestAlloc *temp = LinearBufferCacheHelper::AllocVectorFromPool<TestAlloc>(buffer, allocSize, 10);
    BOOST_TEST((temp != nullptr && allocSize == 128));
    BOOST_TEST(buffer.get() != nullptr);
    BOOST_TEST(((reinterpret_cast<size_t>(temp) % std::alignment_of<TestAlloc>::value == 0)
        && (allocSize * sizeof(TestAlloc) == buffer->capacity() - (reinterpret_cast<us8*>(temp) - buffer->data()))));
    temp = LinearBufferCacheHelper::AllocVectorFromPool<TestAlloc>(buffer, allocSize, 4096);
    BOOST_TEST(temp == nullptr);
    BOOST_TEST(buffer.get() == nullptr);
    allocSize = 0;
    temp = LinearBufferCacheHelper::AllocVectorFromPool<TestAlloc>(buffer, allocSize, 128);
    BOOST_TEST((temp != nullptr && allocSize == 128));
    BOOST_TEST(buffer.get() != nullptr);
    BOOST_TEST(((reinterpret_cast<size_t>(temp) % std::alignment_of<TestAlloc>::value == 0)
        && (allocSize * sizeof(TestAlloc) == buffer->capacity() - (reinterpret_cast<us8*>(temp) - buffer->data()))));
}

BOOST_AUTO_TEST_CASE(LinearBufferCacheNoTrivialWrapperTest)
{
    struct TestAlloc
    {
        TestAlloc(size_t val32, size_t &count) : m_val32(val32), m_count(count)
        {
            ++m_count;
        }

        ~TestAlloc()
        {
            --m_count;
        }

        size_t m_val32;

        size_t &m_count;
    };
    LinearBufferCache::Instance().AddToObjectPool(2, 100);
    LinearBufferCache::Instance().AddToObjectPool(8, 100);
    LinearBufferCache::Instance().AddToObjectPool(1024, 100);
    size_t objCount = 0;
    LinearBufferNoTrivialWrapper<TestAlloc> temp = LinearBufferCacheHelper::AllocNoTrivialVectorFromPool<TestAlloc>(
        10);
    BOOST_TEST((temp == true));
    for(size_t i = 0; i < 10; ++i)
    {
        temp.Construct(i, objCount);
        BOOST_TEST((objCount == i + 1 && temp[i].m_val32 == i));
    }
    BOOST_TEST((temp.Count() == 10));
    LinearBufferNoTrivialWrapper<TestAlloc> temp2 = LinearBufferCacheHelper::AllocNoTrivialVectorFromPool<TestAlloc>(
        4096);
    BOOST_TEST((temp2 == false));
    temp2 = std::move(temp);
    BOOST_TEST((temp == false && temp2 == true));
    {
        LinearBufferNoTrivialWrapper<TestAlloc> temp3 = std::move(temp2);
        BOOST_TEST((temp2 == false && temp3 == true));
    }
    BOOST_TEST(objCount == 0);
}

BOOST_AUTO_TEST_SUITE_END()