#ifndef TASKBARRIER_H
#define TASKBARRIER_H

#include <atomic>
#include "BlackMagics.h"

template<typename BlackMagicType = BlackMagicFuncPtr, BlackMagicType Func> class TaskBarrier
{
public:
	TaskBarrier(unsigned int taskCount, BlackMagicType func = DefaultBlackMagicFunc) :m_taskCount(taskCount), m_finishedCount(0), m_func(func)
	{
	}

	TaskBarrier(const TaskBarrier&) = delete;

	TaskBarrier(TaskBarrier&&) = delete;

	TaskBarrier& operator=(const TaskBarrier&) = delete;

	TaskBarrier& operator=(TaskBarrier&&) = delete;

	void Reset()
	{
		m_finishedCount.store(0, std::memory_order_acq_rel);
	}

	void IncFinishedCount(unsigned int count)
	{
		m_finishedCount.fetch_add(count, std::memory_order_release);
	}

	void WaitAllFinished()
	{
		for (unsigned k = 0; m_finishedCount.load(std::memory_order_acquire) < m_taskCount; ++k)
		{
			Func(k);
		}
	}

private:
	unsigned int m_taskCount;

	std::atomic<unsigned int> m_finishedCount;

	BlackMagicType m_func;
};

#endif /* TASKBARRIER_H */