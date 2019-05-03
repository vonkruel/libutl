#include <libutl/libutl.h>
#include <libutl/HostOS.h>
#include <libutl/RWlockLF.h>
#include <libutl/Thread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::RWlockLF);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RWlockLF::haveWriteLock() const
{
    auto owner = this->owner();
    return ((owner != 0) && (owner == Thread::handle()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlockLF::rdlock()
{
    // already hold write lock -> increment depth and return
    if (haveWriteLock())
    {
        ++_depth;
        return;
    }

    // obtain read lock
    while (true)
    {
        // unsynchronized wait for no active writer
        while (_writeLockOwner.load(std::memory_order_relaxed) != 0)
        {
            hostOS->yield();
        }

        // if wrlock() bumped _numActive first, we'll see its write to _writeLockOwner
        _numActive.fetch_add(1, std::memory_order_acquire);

        // no writer?
        if (_writeLockOwner.load(std::memory_order_relaxed) == 0)
        {
            break;
        }
        else
        {
            _numActive.fetch_sub(1, std::memory_order_relaxed);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlockLF::wrlock()
{
    // already hold write lock -> increment depth and return
    if (haveWriteLock())
    {
        ++_depth;
        return;
    }

    // only one thread at a time can pass this while block
    auto thisThread = Thread::handle();
    thread_handle_t cur = 0;
    while (!_writeLockOwner.compare_exchange_strong(cur, thisThread, std::memory_order_relaxed,
                                                    std::memory_order_relaxed))
    {
        cur = 0;
        hostOS->yield();
    }

    // increment _numActive
    // .. if we bumped _numActive before rdlock(), it will see our write to _writeLockOwner
    _numActive.fetch_add(1, std::memory_order_release);

    // wait until we're the only active thread
    // .. we will see everything the previous writers did
    while (_numActive.load(std::memory_order_acquire) > 1)
    {
        hostOS->yield();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RWlockLF::tryrdlock()
{
    // already hold write lock -> increment depth and succeed
    if (haveWriteLock())
    {
        ++_depth;
        return true;
    }

    // there's a writer -> fail
    if (_writeLockOwner.load(std::memory_order_relaxed) != 0)
        return false;

    // if wrlock() bumped _numActive first, we'll see its write to _writeLockOwner
    _numActive.fetch_add(1, std::memory_order_acquire);

    // no writer?
    if (_writeLockOwner.load(std::memory_order_relaxed) == 0)
    {
        return true;
    }
    else
    {
        _numActive.fetch_sub(1, std::memory_order_relaxed);
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RWlockLF::trywrlock()
{
    // already hold write lock -> increment depth and succeed
    if (haveWriteLock())
    {
        ++_depth;
        return true;
    }

    // try to acquire the lock without waiting
    auto thisThread = Thread::handle();
    thread_handle_t cur = 0;
    if (!_writeLockOwner.compare_exchange_strong(cur, thisThread, std::memory_order_relaxed,
                                                 std::memory_order_relaxed))
    {
        return false;
    }

    // increment _numActive
    // .. this thread will see what previous writers did
    // .. if we bumped _numActive before rdlock(), it will see our write to _writeLockOwner
    if (_numActive.fetch_add(1, std::memory_order_acq_rel) == 0)
    {
        return true;
    }
    else
    {
        _writeLockOwner.store(0, std::memory_order_relaxed);
        _numActive.fetch_sub(1, std::memory_order_relaxed);
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlockLF::unlock()
{
    if (haveWriteLock())
    {
        if (_depth > 0)
        {
            --_depth;
        }
        else
        {
            _writeLockOwner.store(0, std::memory_order_relaxed);
            // sync with rdlock(), wrlock(): subsequent readers+writers will see what we did
            _numActive.fetch_sub(1, std::memory_order_release);
        }
    }
    else
    {
        // no synchronization for completed reader -> just decrement _numActive
        _numActive.fetch_sub(1, std::memory_order_relaxed);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlockLF::init()
{
    _writeLockOwner.store(0, std::memory_order_relaxed);
    _numActive.store(0, std::memory_order_relaxed);
    _depth = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
