#ifndef TASKBARRIER_H
#define TASKBARRIER_H

#include <atomic>
#include "BlackMagics.h"

/**
 * Task barrier class.
 *
 * @tparam BlackMagicType Type of the black magic function type.
 * @tparam Func Instance of BlackMagicType,used to spin.
 */
template<typename BlackMagicType = BlackMagicFuncPtr, BlackMagicType Func = DefaultBlackMagicFunc> class TaskBarrier
{
public:

    /**
     * Constructor
     *
     * @param taskCount Number of total tasks.
     */
    TaskBarrier(unsigned int taskCount) :m_taskCount(taskCount), m_finishedCount(0)
    {
    }

    /**
     * Copy constructor(deleted)
     */
    TaskBarrier(const TaskBarrier&) = delete;

    /**
     * Move constructor(deleted)
     */
    TaskBarrier(TaskBarrier&&) = delete;

    /**
     * Assignment operator(deleted)
     *
     * @return Equal to *this.
     */
    TaskBarrier& operator=(const TaskBarrier&) = delete;

    /**
     * Move assignment operator(deleted)
     */
    TaskBarrier& operator=(TaskBarrier&&) = delete;

    /**
     * Resets current total task count described by count
     *
     * @param count New total task count.
     */
    void ResetTaskCount(unsigned int count)
    {
        m_taskCount.exchange(count, std::memory_order_acquire);
    }

    /**
     * Resets current finished task count to zero.
     */
    void Reset()
    {
        m_finishedCount.exchange(0, std::memory_order_acquire);
    }

    /**
     * Increment current finished task count.
     *
     * @param count Increase count.
     */
    void IncFinishedCount(unsigned int count)
    {
        m_finishedCount.fetch_add(count, std::memory_order_release);
    }

    /**
     * Get current finished task count.
     *
     * @return Current finished task count.
     */
    unsigned int CurrentFinishedCount()
    {
        return m_finishedCount.load(std::memory_order_acquire);
    }

    /**
     * Wait all task to be finished.
     */
    void WaitAllFinished()
    {
        for (unsigned k = 0; m_finishedCount.load(std::memory_order_acquire) < m_taskCount.load(std::memory_order_relaxed); ++k)
        {
            Func(k);
        }
    }

private:
    std::atomic<unsigned int> m_taskCount;  /**< Number of total tasks. */

    std::atomic<unsigned int> m_finishedCount;  /**< Number of current finished tasks. */
};

#endif /* TASKBARRIER_H */