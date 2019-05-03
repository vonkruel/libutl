#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/host_thread.h>
#include <libutl/NamedObjectMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Mutex;

////////////////////////////////////////////////////////////////////////////////////////////////////
// RWlockLF ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Read/write lock (lock-free implementation).

   RWlockLF has the same interface as RWlock, but it tries to give better performance for readers
   when writers are very infrequent.  When no writer is active, readers only pay the cost of one
   atomic-add operation (using ACQUIRE memory model) for synchronization.  When readers & writers
   have to wait for a writer, they use HostOS::yield() to give priority to other processes and
   threads that want to run during their "busy-waiting" loop.

   \author Adam Mckee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RWlockLF : public Object, public NamedObjectMI
{
    UTL_CLASS_DECL(RWlockLF, Object);
    UTL_CLASS_NO_COPY;

public:
    /** Return true iff the calling thread holds a write lock. */
    bool haveWriteLock() const;

    /**
       Acquire read- or write-lock.
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    void
    lock(uint_t mode)
    {
        (mode == io_rd) ? rdlock() : wrlock();
    }

    /** Acquire read-lock. */
    void rdlock();

    /** Acquire write-lock. */
    void wrlock();

    /** Release previously acquired lock. */
    void unlock();

    /**
       Try to acquire read- or write-lock (without blocking).
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    bool
    trylock(uint_t mode)
    {
        return (mode == io_rd) ? tryrdlock() : trywrlock();
    }

    /**
       Try to acquire a read-lock (without blocking).
       \return true if read-lock acquired, false otherwise
    */
    bool tryrdlock();

    /**
       Try to acquire a write-lock (without blocking).
       \return true if write-lock acquired, false otherwise
    */
    bool trywrlock();

private:
    void init();

    void
    deInit()
    {
    }

    thread_handle_t
    owner() const
    {
        return _writeLockOwner.load(std::memory_order_relaxed);
    }

private:
    char pad1[UTL_ARCH_CACHE_LINE_SIZE - sizeof(void*)];
    std::atomic<thread_handle_t> _writeLockOwner;
    size_t _depth;
    char pad2[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
    std::atomic_size_t _numActive;
    char pad3[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// RWlockLFguard ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Acquire/release a RWlockLF RAII-style.

   \author Adam McKee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RWlockLFguard
{
public:
    /** Default constructor. */
    RWlockLFguard()
        : _locked(false)
        , _rwlock(nullptr)
    {
    }

    /**
       Constructor.
       \param rwlock RWlockLF to lock
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    RWlockLFguard(RWlockLF* rwlock, uint_t mode)
        : _locked(false)
        , _rwlock(rwlock)
    {
        _lock(mode);
    }

    /** Destructor. */
    ~RWlockLFguard()
    {
        unlock();
    }

    /**
       Acquire read- or write-lock.
       \param rwlock RWlockLF to lock
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    void
    lock(RWlockLF* rwlock, uint_t mode)
    {
        unlock();
        _rwlock = rwlock;
        _lock(mode);
    }

    /**
       Acquire read- or write-lock.
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    void
    lock(uint_t mode)
    {
        (mode == io_rd) ? rdlock() : wrlock();
    }

    /**
       Acquire read-lock.
       \param rwlock RWlockLF to lock
    */
    void
    rdlock(RWlockLF* rwlock)
    {
        unlock();
        _rwlock = rwlock;
        _rdlock();
    }

    /**
       Acquire write-lock.
       \param rwlock RWlockLF to lock
    */
    void
    wrlock(RWlockLF* rwlock)
    {
        unlock();
        _rwlock = rwlock;
        _wrlock();
    }

    /** Acquire read-lock. */
    void
    rdlock()
    {
        unlock();
        _rdlock();
    }

    /** Acquire write-lock. */
    void
    wrlock()
    {
        unlock();
        _wrlock();
    }

    /**
       Try to acquire read- or write-lock (without blocking).
       \param rwlock RWlockLF to lock
       \param mode (io_rd : read-lock, io_wr : write-lock)
       \return true if lock acquired, false otherwise
    */
    bool
    trylock(RWlockLF* rwlock, uint_t mode)
    {
        unlock();
        _rwlock = rwlock;
        return _trylock(mode);
    }

    /**
       Try to acquire read- or write-lock (without blocking).
       \param mode (io_rd : read-lock, io_wr : write-lock)
       \return true if lock acquired, false otherwise
    */
    bool
    trylock(uint_t mode)
    {
        return (mode == io_rd) ? tryrdlock() : trywrlock();
    }

    /**
       Try to acquire read-lock (without blocking).
       \param rwlock RWlockLF to lock
       \return true if read-lock acquired, false otherwise
    */
    bool
    tryrdlock(RWlockLF* rwlock)
    {
        unlock();
        _rwlock = rwlock;
        return _tryrdlock();
    }

    /**
       Try to acquire write-lock (without blocking).
       \param rwlock RWlockLF to lock
       \return true if write-lock acquired, false otherwise
    */
    bool
    trywrlock(RWlockLF* rwlock)
    {
        unlock();
        _rwlock = rwlock;
        return _trywrlock();
    }

    /**
       Try to acquire read-lock (without blocking).
       \return true if read-lock acquired, false otherwise
    */
    bool
    tryrdlock()
    {
        unlock();
        return _tryrdlock();
    }

    /**
       Try to acquire write-lock (without blocking).
       \return true if write-lock acquired, false otherwise
    */
    bool
    trywrlock()
    {
        unlock();
        return _trywrlock();
    }

    /** Release the lock. */
    inline void
    unlock()
    {
        if (_locked)
            _unlock();
    }

private:
    void
    _lock(uint_t mode)
    {
        (mode == io_rd) ? _rdlock() : _wrlock();
    }

    void
    _rdlock()
    {
        ASSERTD(_rwlock != nullptr);
        ASSERTD(!_locked);
        _rwlock->rdlock();
        _locked = true;
    }

    void
    _wrlock()
    {
        ASSERTD(_rwlock != nullptr);
        ASSERTD(!_locked);
        _rwlock->wrlock();
        _locked = true;
    }

    bool
    _trylock(uint_t mode)
    {
        return (mode == io_rd) ? _tryrdlock() : _trywrlock();
    }

    bool
    _tryrdlock()
    {
        ASSERTD(_rwlock != nullptr);
        ASSERTD(!_locked);
        if (_rwlock->tryrdlock())
            _locked = true;
        return _locked;
    }

    bool
    _trywrlock()
    {
        ASSERTD(_rwlock != nullptr);
        ASSERTD(!_locked);
        if (_rwlock->trywrlock())
            _locked = true;
        return _locked;
    }

    void
    _unlock()
    {
        ASSERTD(_rwlock != nullptr);
        ASSERTD(_locked);
        _rwlock->unlock();
        _locked = false;
    }

private:
    bool _locked;
    RWlockLF* _rwlock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
