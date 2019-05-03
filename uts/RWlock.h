#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/host_thread.h>
#include <libutl/NamedObjectMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Mutex;

////////////////////////////////////////////////////////////////////////////////////////////////////
// RWlock //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Read/write lock.

   Multiple readers may run concurrently, but a writer must have exclusive access.  An RWlock can
   be more efficient than a Mutex if reads are much more frequent than writes.

   \author Adam Mckee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RWlock : public Object, public NamedObjectMI
{
    UTL_CLASS_DECL(RWlock, Object);
    UTL_CLASS_NO_COPY;

public:
    /**
       Does the calling thread hold a write-lock?
       \return true if calling thread holds write-lock, false otherwise
    */
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
       Try to acquire read-lock (without blocking).
       \return true if read-lock acquired, false otherwise
    */
    bool tryrdlock();

    /**
       Try to acquire write-lock (without blocking).
       \return true if write-lock acquired, false otherwise
    */
    bool trywrlock();

    /** Release previously acquired lock. */
    void unlock();

private:
    void init();
    void deInit();

private:
#if UTL_HOST_TYPE == UTL_HT_UNIX
    pthread_rwlock_t _rwlock;
#else
    void* _readEvent;
    Mutex* _writeMutex;
    volatile long _numReaders;
    uint_t _mode;
#endif
    thread_handle_t _writeLockOwner;
    size_t _depth;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// RWlockGuard /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Acquire/release a RWlock RAII-style.

   \author Adam McKee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RWlockGuard
{
public:
    /** Default constructor. */
    RWlockGuard()
        : _locked(false)
        , _rwlock(nullptr)
    {
    }

    /**
       Constructor.
       \param rwlock RWlock to lock
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    RWlockGuard(RWlock* rwlock, uint_t mode)
        : _locked(false)
        , _rwlock(rwlock)
    {
        _lock(mode);
    }

    /** Destructor. */
    ~RWlockGuard()
    {
        unlock();
    }

    /**
       Acquire read- or write-lock.
       \param rwlock RWlock to lock
       \param mode (io_rd : read-lock, io_wr : write-lock)
    */
    void
    lock(RWlock* rwlock, uint_t mode)
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
       \param rwlock RWlock to lock
    */
    void
    rdlock(RWlock* rwlock)
    {
        unlock();
        _rwlock = rwlock;
        _rdlock();
    }

    /**
       Acquire write-lock.
       \param rwlock RWlock to lock
    */
    void
    wrlock(RWlock* rwlock)
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
       \param rwlock RWlock to lock
       \param mode (io_rd : read-lock, io_wr : write-lock)
       \return true if lock acquired, false otherwise
    */
    bool
    trylock(RWlock* rwlock, uint_t mode)
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
       \param rwlock RWlock to lock
       \return true if read-lock acquired, false otherwise
    */
    bool
    tryrdlock(RWlock* rwlock)
    {
        unlock();
        _rwlock = rwlock;
        return _tryrdlock();
    }

    /**
       Try to acquire write-lock (without blocking).
       \param rwlock RWlock to lock
       \return true if write-lock acquired, false otherwise
    */
    bool
    trywrlock(RWlock* rwlock)
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
    RWlock* _rwlock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
