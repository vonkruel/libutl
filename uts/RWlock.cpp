#include <libutl/libutl.h>
#include <libutl/Mutex.h>
#include <libutl/RWlock.h>
#include <libutl/Thread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::RWlock);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RWlock::haveWriteLock() const
{
    return ((_writeLockOwner != 0) && (_writeLockOwner == Thread::handle()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlock::rdlock()
{
    // already hold write lock -> increment depth and return
    if (haveWriteLock())
    {
        ++_depth;
        return;
    }

// obtain read lock
#if UTL_HOST_TYPE == UTL_HT_UNIX
    UTL_EINTR_LOOP(pthread_rwlock_rdlock(&_rwlock));
#else
    _writeMutex->lock();
    InterlockedIncrement(&_numReaders);
    ResetEvent(_readEvent);
    ASSERTD(_mode != io_wr);
    _mode = io_rd;
    _writeMutex->unlock();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlock::wrlock()
{
    // already hold write lock -> increment depth and return
    auto thisThread = Thread::handle();
    if (_writeLockOwner == thisThread)
    {
        ++_depth;
        return;
    }

// obtain write lock
#if UTL_HOST_TYPE == UTL_HT_UNIX
    UTL_EINTR_LOOP(pthread_rwlock_wrlock(&_rwlock));
#else
    _writeMutex->lock();
    if (_numReaders > 0)
    {
        WaitForSingleObject(_readEvent, INFINITE);
    }
    ASSERTD(_mode != io_rd);
    _mode = io_wr;
#endif

    // make a note that this thread owns the write lock
    _writeLockOwner = thisThread;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RWlock::tryrdlock()
{
    bool res;

    // already hold write lock -> increment depth and exit
    if (haveWriteLock())
    {
        ++_depth;
        return true;
    }

#if UTL_HOST_TYPE == UTL_HT_UNIX
    // try to get read lock
    int err = pthread_rwlock_tryrdlock(&_rwlock);
    res = (err == 0);
#else
    res = _writeMutex->trylock();
    if (res)
    {
        InterlockedIncrement(&_numReaders);
        ResetEvent(_readEvent);
        ASSERTD(_mode != io_wr);
        _mode = io_rd;
        _writeMutex->unlock();
    }
#endif

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RWlock::trywrlock()
{
    bool res;

    // already hold write lock -> increment depth and exit
    auto thisThread = Thread::handle();
    if (_writeLockOwner == thisThread)
    {
        ++_depth;
        return true;
    }

#if UTL_HOST_TYPE == UTL_HT_UNIX
    // try to get write lock
    int err = pthread_rwlock_trywrlock(&_rwlock);
    res = (err == 0);
#else
    if (_writeMutex->trylock())
    {
        if (_numReaders > 0)
        {
            // still have readers, and we can't wait for them to finish
            _writeMutex->unlock();
            res = false;
        }
        else
        {
            ASSERTD(_mode != io_rd);
            _mode = io_wr;
            res = true;
        }
    }
    else
    {
        res = false;
    }
#endif

    if (res)
    {
        // make a note that this thread owns the write lock
        _writeLockOwner = thisThread;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlock::unlock()
{
    // recursively locked after initial write-lock?
    if (haveWriteLock())
    {
        if (_depth > 0)
        {
            --_depth;
            return;
        }
        else
        {
            _writeLockOwner = 0;
        }
    }

// release lock
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_rwlock_unlock(&_rwlock));
#else
    ASSERTD(_mode != uint_t_max);
    if (_mode == io_rd)
    {
        if (InterlockedDecrement(&_numReaders) == 0)
        {
            _mode = uint_t_max;
            SetEvent(_readEvent);
        }
    }
    else
    {
        _mode = uint_t_max;
        _writeMutex->unlock();
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlock::init()
{
    _writeLockOwner = 0;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_rwlock_init(&_rwlock, nullptr));
#else
    _readEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    _writeMutex = new Mutex();
    _numReaders = 0;
    _mode = uint_t_max;
#endif
    _depth = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RWlock::deInit()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_rwlock_destroy(&_rwlock));
#else
    CloseHandle(_readEvent);
    delete _writeMutex;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
