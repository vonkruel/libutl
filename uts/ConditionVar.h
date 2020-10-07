#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/host_thread.h>
#include <libutl/Mutex.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Condition variable.

   ConditionVar is a synchronization device that allows a thread to block until a predicate is
   satisfied.  It has an associated Mutex which is used to prevent the race condition where one
   thread calls wait() while another thread simultaneously calls broadcast() or signal().  If all
   threads always acquire the mutex before signaling the condition, this guarantees that the
   condition cannot be signaled (and thus ignored) between the time a thread locks the mutex and
   the time it waits on the condition variable.

   \author Adam Mckee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConditionVar
    : public Object
    , public NamedObjectMI
{
    UTL_CLASS_DECL(ConditionVar, Object);
    UTL_CLASS_NO_COPY;

public:
    /** Wake up all waiting threads. */
    void broadcast();

    /** Wake up a single waiting thread. */
    void signal();

    /**
       Atomically unlock the mutex and wait for the
       predicate to be satisfied.
    */
    void wait();

    /** Lock the associated Mutex. */
    void lockMutex();

    /** Unlock the associated Mutex. */
    void unlockMutex();

private:
    void init();
    void deInit();

private:
#if UTL_HOST_TYPE == UTL_HT_UNIX
    Mutex _mutex;
    pthread_cond_t _cond;
#else
    void* _mutex;
    int _numWaiters;
    struct _CRITICAL_SECTION* _numWaitersLock;
    void* _sem;
    void* _waitersDone;
    uint_t _wasBroadcast;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
