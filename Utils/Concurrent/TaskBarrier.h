#ifndef TASKBARRIER_H
#define TASKBARRIER_H

#include <atomic>
#include "BlackMagics.h"

template<typename BlackMagicType = BlackMagicFuncPtr, BlackMagicType Func = DefaultBlackMagicFunc> class TaskBarrier
{
public:
	TaskBarrier(unsigned int taskCount) :m_taskCount(taskCount), m_finishedCount(0)
	{
	}

	TaskBarrier(const TaskBarrier&) = delete;

	TaskBarrier(TaskBarrier&&) = delete;

	TaskBarrier& operator=(const TaskBarrier&) = delete;

	TaskBarrier& operator=(TaskBarrier&&) = delete;

	void ResetTaskCount(unsigned int count)
	{
		m_taskCount.exchange(count, std::memory_order_acquire);
	}

	void Reset()
	{
		m_finishedCount.exchange(0, std::memory_order_acquire);
	}

	void IncFinishedCount(unsigned int count)
	{
		m_finishedCount.fetch_add(count, std::memory_order_release);
	}

	void WaitAllFinished()
	{
		for (unsigned k = 0; m_finishedCount.load(std::memory_order_acquire) < m_taskCount.load(std::memory_order_relaxed); ++k)
		{
			Func(k);
		}
	}

private:
	std::atomic<unsigned int> m_taskCount;

	std::atomic<unsigned int> m_finishedCount;
};

#endif /* TASKBARRIER_H */