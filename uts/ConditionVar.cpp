#include <libutl/libutl.h>
#include <libutl/ConditionVar.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ConditionVar);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ConditionVar::broadcast()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_cond_broadcast(&_cond));
#else
    EnterCriticalSection(_numWaitersLock);

    // nothing to do if there are no waiters
    bool haveWaiters = (_numWaiters > 0);
    if (!haveWaiters)
    {
        LeaveCriticalSection(_numWaitersLock);
        return;
    }

    // there are waiters...
    _wasBroadcast = 1;

    // wake up all waiters atomically
    ReleaseSemaphore(_sem, _numWaiters, 0);
    LeaveCriticalSection(_numWaitersLock);

    // wait for awakened threads to acquire counting semaphore
    WaitForSingleObject(_waitersDone, INFINITE);
    _wasBroadcast = 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ConditionVar::signal()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_cond_signal(&_cond));
#else
    // are there waiters?
    EnterCriticalSection(_numWaitersLock);
    bool haveWaiters = (_numWaiters > 0);
    LeaveCriticalSection(_numWaitersLock);

    // if there are waiters, revive one of them
    if (haveWaiters)
    {
        ReleaseSemaphore(_sem, 1, 0);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ConditionVar::wait()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    UTL_EINTR_LOOP(pthread_cond_wait(&_cond, &_mutex._mutex));
#else
    EnterCriticalSection(_numWaitersLock);
    ++_numWaiters;
    LeaveCriticalSection(_numWaitersLock);

    // atomically release the mutex and wait on the semaphore
    // until signal() or broadcast() are called by another thread
    SignalObjectAndWait(_mutex, _sem, INFINITE, FALSE);

    // ra-acquire to avoid race
    EnterCriticalSection(_numWaitersLock);

    // not waiting anymore
    --_numWaiters;

    // are we the last waiter?
    bool lastWaiter = _wasBroadcast && (_numWaiters == 0);

    LeaveCriticalSection(_numWaitersLock);

    // If we were the last waiter during this broadcast
    // then let all other threads proceed.
    if (lastWaiter)
    {
        // Atomically signal _waitersDone and wait until
        // _mutex can be acquired (to ensure faireness).
        SignalObjectAndWait(_waitersDone, _mutex, INFINITE, FALSE);
    }
    else
    {
        // regain the external mutex
        WaitForSingleObject(_mutex, INFINITE);
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////

void
ConditionVar::lockMutex()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    _mutex.lock();
#else
    WaitForSingleObject(_mutex, INFINITE);
#endif
}

//////////////////////////////////////////////////////////////////////////////

void
ConditionVar::unlockMutex()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    _mutex.unlock();
#else
    ReleaseMutex(_mutex);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ConditionVar::init()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_cond_init(&_cond, nullptr));
#else
    _mutex = CreateMutex(nullptr, FALSE, nullptr);
    _numWaiters = 0;
    _numWaitersLock = new CRITICAL_SECTION();
    InitializeCriticalSection(_numWaitersLock);
    _sem = CreateSemaphore(nullptr, 0, long_t_max, nullptr);
    _waitersDone = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    _wasBroadcast = 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ConditionVar::deInit()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_cond_destroy(&_cond));
#else
    ASSERTFNP(CloseHandle(_mutex));
    DeleteCriticalSection(_numWaitersLock);
    delete _numWaitersLock;
    ASSERTFNP(CloseHandle(_sem));
    ASSERTFNP(CloseHandle(_waitersDone));
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
