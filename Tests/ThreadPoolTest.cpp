#include <boost/test/unit_test.hpp>
#include "Concurrent/ThreadPool.h"
#include "Concurrent/TaskBarrier.h"

TaskBarrier<> GlobalThreadPoolTestBarrier(0);

std::atomic<bool> GlobalThreadPoolTestStopFlag(false);

class MockThreadPool
{
public:
    struct ThreadContext
    {
        bool m_processing;
    };

    static std::shared_ptr<MockThreadPool> instance;

    std::atomic<int> m_currentThreadCount;

    std::atomic<int> m_threadBusyCount;

    boost::thread_specific_ptr<ThreadContext> m_tls;

    boost::asio::io_context m_context;

    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard = boost::asio::make_work_guard(m_context);

    static int ThreadMinCount;

    static log4cplus::Logger log;
};

BOOST_AUTO_TEST_SUITE(ThreadPoolTest)

void ThreadPoolTestWork()
{
    while(!GlobalThreadPoolTestStopFlag.load(std::memory_order_acquire))
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
    }
    GlobalThreadPoolTestBarrier.IncFinishedCount(1);
}

BOOST_AUTO_TEST_CASE(GeneralTest)
{
    int hint = boost::thread::hardware_concurrency();
    int taskCount = hint * 3;
    GlobalThreadPoolTestBarrier.ResetTaskCount(taskCount);
    GlobalThreadPoolTestBarrier.Reset();
    SetInitConcurrentHint(boost::thread::hardware_concurrency());
    //MockThreadPool *pool = reinterpret_cast<MockThreadPool*>(&ThreadPool::Instance());
    ThreadPool::Instance();
    //BOOST_TEST(pool->m_currentThreadCount.load(std::memory_order_acquire) == hint);
    //BOOST_TEST(pool->m_threadBusyCount.load(std::memory_order_acquire) == 0);
    BOOST_TEST_MESSAGE("Pool Creatted.");
    for (int i = 0; i < taskCount; ++i)
    {
        QueueThreadPoolWorkItem(ThreadPoolTestWork);
    }
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    //BOOST_TEST(pool->m_currentThreadCount.load(std::memory_order_acquire) == hint * 3);
    //BOOST_TEST(pool->m_threadBusyCount.load(std::memory_order_acquire) == hint * 3);
    BOOST_TEST_MESSAGE("Post task finished.");
    GlobalThreadPoolTestStopFlag.exchange(true, std::memory_order_release);
    GlobalThreadPoolTestBarrier.WaitAllFinished();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    //BOOST_TEST(pool->m_currentThreadCount.load(std::memory_order_acquire) == hint);
    //BOOST_TEST(pool->m_threadBusyCount.load(std::memory_order_acquire) == 0);
    BOOST_TEST_MESSAGE("Task execute finished.");
    ThreadPool::Instance().Stop();
    ThreadPool::Destory();
    BOOST_TEST_MESSAGE("Pool test finished.");
}

BOOST_AUTO_TEST_SUITE_END()