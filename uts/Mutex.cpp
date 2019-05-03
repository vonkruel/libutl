#include <libutl/libutl.h>
#include <libutl/Mutex.h>
#include <libutl/Thread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Mutex);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Mutex::haveLock() const
{
    return ((_owner != 0) && (_owner == Thread::handle()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Mutex::trylock()
{
    bool res;

    // already hold root lock -> increment depth
    auto thisThread = Thread::handle();
    if (_owner == thisThread)
    {
        ++_depth;
        return true;
    }

    // note: calling thread does not own this lock

#if UTL_HOST_TYPE == UTL_HT_UNIX
    int err = pthread_mutex_trylock(&_mutex);
    res = (err == 0);
#else
    // try to acquire the lock
    res = (InterlockedCompareExchange(&_count, 1, 0) == 0);
#endif

    // acquired root lock -> record owning thread
    if (res)
        _owner = thisThread;

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Mutex::lock()
{
    // already hold root lock -> just increment depth
    auto thisThread = Thread::handle();
    if (_owner == thisThread)
    {
        ++_depth;
        return;
    }

#if UTL_HOST_TYPE == UTL_HT_UNIX
    UTL_EINTR_LOOP(pthread_mutex_lock(&_mutex));
#else
    // acquire the lock
    if (InterlockedIncrement(&_count) != 1)
    {
        ASSERT(WaitForSingleObject(_sem, uint_t_max) != uint_t_max);
    }
#endif

    // acquired root lock -> record owning thread
    _owner = thisThread;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Mutex::unlock()
{
    // recursively locked -> decrement depth and return
    if (_depth > 0)
    {
        --_depth;
        return;
    }

    // the lock has no owner
    _owner = 0;

#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_mutex_unlock(&_mutex));
#else
    // release the lock
    if (InterlockedDecrement(&_count) > 0)
    {
        ReleaseSemaphore(_sem, 1, nullptr);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Mutex::init()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP);
    ASSERTFNZ(pthread_mutex_init(&_mutex, &attr));
#else
    _count = 0;
    _sem = CreateSemaphore(nullptr, 0, long_t_max, nullptr);
    ASSERT(_sem != nullptr);
#endif

    // recursive locking
    _owner = 0;
    _depth = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Mutex::deInit()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_mutex_destroy(&_mutex));
#else
    ASSERTFNP(CloseHandle(_sem));
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
