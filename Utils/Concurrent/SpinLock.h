#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>
#include "BlackMagics.h"

/**
 * RAII-style Lock guard.
 *
 * @tparam LockType Type of the lock.
 */
template<typename LockType> class LockGuard
{
public:

    /**
     * Constructor,used to add lock.
     *
     * @param [in,out] lock Lock object.
     */
    explicit LockGuard(LockType &lock) :m_lock(lock)
    {
        m_lock.Lock();
    }

    /**
     * Destructor,use to unlock.
     */
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

/**
 * Spin lock class.
 *
 * @tparam BlackMagicType Type of the black magic function type.
 * @tparam BlackMagic	  Instance of BlackMagicType,use to spin.
 */
template<typename BlackMagicType = BlackMagicFuncPtr, BlackMagicType BlackMagic = DefaultBlackMagicFunc> class SpinLock
{
public:
    typedef enum
    {
        Locked, /**< Lock status. */
        Unlocked /**< Unlock status. */
    } LockState;	/**< State of the lock */

    /**
     * Defines an alias representing the scope lock guard.
     */
    typedef LockGuard<SpinLock<BlackMagicType, BlackMagic>> ScopeLock;

    /**
     * Default constructor
     */
    SpinLock() :m_state(Unlocked)
    {
    }

    /**
     * Copy constructor
     *
     * @param rhs The right hand side.
     */
    SpinLock(const SpinLock &rhs) = delete;

    /**
     * Move constructor
     *
     * @param [in,out] rhs The right hand side.
     */
    SpinLock(SpinLock &&rhs) = delete;

    /**
     * Assignment operator
     *
     * @param rhs The right hand side.
     *
     * @return Equal to *this.
     */
    SpinLock& operator=(const SpinLock &rhs) = delete;

    /**
     * Move assignment operator
     *
     * @param [in,out] rhs The right hand side.
     *
     * @return Equal to *this.
     */
    SpinLock& operator=(SpinLock &&rhs) = delete;

    /**
     * Attempts to lock.
     *
     * @return True if it succeeds, false if it fails.
     */
    bool TryLock()
    {
        LockState r = m_state.exchange(Locked, std::memory_order_acquire);
        return r == Unlocked;
    }

    /**
     * Add lock.
     */
    void Lock()
    {
        for (unsigned k = 0; !TryLock(); ++k)
        {
            BlackMagic(k);
        }
    }

    /**
     * Unlock.
     */
    void Unlock()
    {
        m_state.store(Unlocked, std::memory_order_release);
    }

private:
    std::atomic<LockState> m_state; /**< Current status of this lock. */
};

#endif /* SPINLOCK_H */