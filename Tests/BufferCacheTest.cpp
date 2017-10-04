#define BOOST_TEST_MODULE UtilsTest
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

BOOST_AUTO_TEST_SUITE_END()