#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/host_thread.h>
#include <libutl/NamedObjectMI.h>
#include <libutl/RandUtils.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Thread of execution.

   Thread's main purpose is to encapsulate the native OS's implementation of threads.  A Thread
   object must always be created on the heap (via \c new).  When you create a thread, you can
   either create it in \b detached mode or in \b joinable mode.  The Thread object for a detached
   thread will automatically be deleted when the thread terminates.  The Thread object for a
   joinable thread will not be deleted until you've successfully join()'d on it.

   \author Adam McKee
   \ingroup threads
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Thread
    : public Object
    , public NamedObjectMI
    , protected FlagsMI
{
    UTL_CLASS_DECL_ABC(Thread, Object);
    UTL_CLASS_NO_COPY;

public:
    virtual const Object& getKey() const;

    /** Get a pointer to the current thread's Thread object. */
    static Thread* get();

    /** Get a handle to the current thread. */
    static thread_handle_t handle();

    /** Get the thread's ID. */
    size_t
    id() const
    {
        return _id;
    }

    /** Get the thread's assigned node (NUMA). */
    uint_t
    node() const
    {
        return _node;
    }

    /** Get the thread's assigned node (NUMA). */
    uint_t&
    node()
    {
        return _node;
    }

    /** Determine whether the thread is detached. */
    bool
    isDetached() const
    {
        return getFlag(flg_detached);
    }

    /**
       Cancel the thread.
       After calling cancel(), a call to testCancel() will throw a ThreadCancelledEx.
    */
    void
    cancel()
    {
        setFlag(flg_cancel, true);
    }

    /**
       Test for cancellation. If the thread has been cancelled (with cancel()), throw a
       ThreadCancelledEx - causing termination of the thread.
    */
    static void testCancel();

    /**
       Join on the thread.  The calling thread's execution will be blocked until the thread has
       terminated.  You must make a single call to join() for each non-detached thread you create,
       or you will leak memory.
       \param deleteSelf (optional : true) override if you prefer to delete the object yourself
       \return the thread's return value
    */
    void* join(bool deleteSelf = true);

    /**
       Run the thread.  run() is essentially the thread's main(); it does whatever the thread was
       created to do.  You will need to override this virtual method in your derived class.
       \param arg user-defined thread parameter
       \return the thread's return value
    */
    virtual void* run(void* arg = nullptr) = 0;

    /**
       Start the thread.  After creating a Thread object (via new), you must start it by calling
       start().
       \param arg thread parameter
       \param join whether the thread will be joined
    */
    void start(void* arg = nullptr, bool join = true);

    /** Yield the processor to allow another thread to run. */
    static void yield();

    /** Clean up stored state in preparation for re-use. */
    virtual void pause();

    /** Return a newly created TLS key. */
    static ulong_t tlsNew();

    /** Get the value of a key in TLS. */
    static void* tlsGet(ulong_t key);

    /**
       Set the value of a key in TLS.
       \param key TLS key
       \param value new value for the key in the calling thread
    */
    static void tlsSet(ulong_t key, void* value);

    /** Get the random generator (initialize it if necessary). */
    randutils::mt19937_64_rng* rng();

#if UTL_HOST_TYPE == UTL_HT_UNIX
    void setAffinity(size_t cpusetsize, cpu_set_t* cpuset);
#endif

    static void utl_init();

    static void utl_deInit();

protected:
    virtual void onExit();

#if UTL_HOST_TYPE == UTL_HT_UNIX
    static void* runThread(void* vp_runThreadInfo);
#else
    static ulong_t __stdcall runThread(void* vp_runThreadInfo);
#endif
private:
    void init();
    void deInit();

    void
    setDetached(bool detached)
    {
        setFlag(flg_detached, detached);
    }

private:
    enum flg_t
    {
        flg_detached,
        flg_cancel
    };

private:
    Uint _id;
    uint_t _node;

    randutils::mt19937_64_rng* _rng;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    pthread_t _threadHandle;
    static __thread Thread* _thread;
#else
    void* _threadHandle;
    ulong_t _threadId;
    static ulong_t _objectKey;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   <b>Thread cancelled</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(ThreadCancelledEx, Exception);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
