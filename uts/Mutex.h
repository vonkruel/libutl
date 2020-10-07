#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/host_thread.h>
#include <libutl/NamedObjectMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Mutex ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   <b>MUT</b>ual <b>EX</b>clusion device.

   A Mutex is useful for protecting shared data structures and implementing critical code sections.

   \author Adam McKee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Mutex
    : public Object
    , public NamedObjectMI
{
    friend class ConditionVar;
    UTL_CLASS_DECL(Mutex, Object);
    UTL_CLASS_NO_COPY;

public:
    /**
       Does the calling thread have the lock?
       \return true if calling thread has the lock, false otherwise
    */
    bool haveLock() const;

    /**
       Try to obtain the lock (without blocking).
       \return true if lock acquired, false otherwise
    */
    bool trylock();

    /** Obtain the lock (blocking if necessary). */
    void lock();

    /** Release the lock. */
    void unlock();

private:
    void init();
    void deInit();

private:
#if UTL_HOST_TYPE == UTL_HT_UNIX
    pthread_mutex_t _mutex;
#else
    HANDLE _sem;
    volatile long _count;
#endif
    thread_handle_t _owner;
    size_t _depth; // recursive locking depth
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// MutexGuard //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Acquire/release a Mutex RAII-style.

   \author Adam McKee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class MutexGuard
{
public:
    /** Default constructor. */
    MutexGuard()
        : _mutex(nullptr)
    {
    }

    /**
       Constructor.
       \param mutex Mutex to lock
    */
    MutexGuard(Mutex* mutex)
        : _mutex(mutex)
    {
        ASSERTD(_mutex != nullptr);
        _mutex->lock();
    }

    /** Destructor. */
    ~MutexGuard()
    {
        unlock();
    }

    /**
       Lock a Mutex.
       \param mutex Mutex to lock
    */
    void
    lock(Mutex* mutex)
    {
        unlock();
        ASSERTD(mutex != nullptr);
        _mutex = mutex;
        _mutex->lock();
    }

    /**
       Lock a new Mutex.
       \param mutex Mutex to lock
       \return true if lock acquired, false otherwise
    */
    bool
    trylock(Mutex* mutex)
    {
        unlock();
        ASSERTD(mutex != nullptr);
        if (mutex->trylock())
        {
            _mutex = mutex;
            return true;
        }
        else
        {
            return false;
        }
    }

    /** Unlock the Mutex. */
    void
    unlock()
    {
        if (_mutex != nullptr)
        {
            _mutex->unlock();
            _mutex = nullptr;
        }
    }

private:
    Mutex* _mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
