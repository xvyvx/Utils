#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>
#include "BlackMagics.h"

template<typename LockType> class LockGuard
{
public:
	explicit LockGuard(LockType &lock) :m_lock(lock)
	{
		m_lock.Lock();
	}

	~LockGuard()
	{
		m_lock.Unlock();
	}

	LockGuard(const LockGuard &rhs) = delete;

	LockGuard(LockGuard &&rhs) = delete;

	LockGuard& operator=(const LockGuard &rhs) = delete;

	LockGuard& operator=(LockGuard &&rhs) = delete;

private:
	LockType &m_lock;
};

template<typename BlackMagicType = BlackMagicFuncPtr, BlackMagicType BlackMagic = DefaultBlackMagicFunc> class SpinLock
{
public:
	typedef enum
	{
		Locked,
		Unlocked
	} LockState;

	typedef LockGuard<SpinLock<BlackMagicType, BlackMagic>> ScopeLock;

	SpinLock() :m_state(Unlocked)
	{
	}

	SpinLock(const SpinLock &rhs) = delete;

	SpinLock(SpinLock &&rhs) = delete;

	SpinLock& operator=(const SpinLock &rhs) = delete;

	SpinLock& operator=(SpinLock &&rhs) = delete;

	bool TryLock()
	{
		LockState r = m_state.exchange(Locked, std::memory_order_acquire);
		return r == Unlocked;
	}

	void Lock()
	{
		for (unsigned k = 0; !TryLock(); ++k)
		{
			BlackMagic(k);
		}
	}

	void Unlock()
	{
		m_state.store(Unlocked, std::memory_order_release);
	}

private:
	std::atomic<LockState> m_state;
};

#endif /* SPINLOCK_H */