#include <libutl/libutl.h>
#include <libutl/FileStream.h>
#include <libutl/HostOS.h>
#include <libutl/Mutex.h>
#include <libutl/Thread.h>

// OpenSSL headers (not under Windows for now)
#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <openssl/err.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RunThreadInfo //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct RunThreadInfo : public Object
{
    UTL_CLASS_DECL(RunThreadInfo, Object);
    UTL_CLASS_DEFID;

public:
    RunThreadInfo(Thread* thread, void* arg)
    {
        _thread = thread;
        _arg = arg;
    }

    void*
    getArg() const
    {
        return _arg;
    }

    Thread*
    getThread() const
    {
        return _thread;
    }

private:
    Thread* _thread;
    void* _arg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Thread /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const Object&
Thread::getKey() const
{
    return _id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Thread*
Thread::get()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    return _thread;
#else
    return static_cast<Thread*>(tlsGet(_objectKey));
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

thread_handle_t
Thread::handle()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    return pthread_self();
#else
    return GetCurrentThread();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::testCancel()
{
    auto thread = get();
    if (thread == nullptr)
        return;
    if (thread->getFlag(flg_cancel))
    {
        throw ThreadCancelledEx();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_CC == UTL_CC_MSVC
#pragma warning(push)
#pragma warning(disable : 4312)
#endif
void*
Thread::join(bool deleteSelf)
{
    ASSERTD(!isDetached());
    void* res;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_join(_threadHandle, &res));
#else
    DWORD dword;
    WaitForSingleObject(_threadHandle, INFINITE);
    GetExitCodeThread(_threadHandle, &dword);
    res = reinterpret_cast<void*>(dword);
    CloseHandle(_threadHandle);
#endif
    if (deleteSelf)
        delete this;
    return res;
}
#if UTL_CC == UTL_CC_MSVC
#pragma warning(pop)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::start(void* arg, bool join)
{
    setDetached(!join);
    auto runThreadInfo = new RunThreadInfo(this, arg);
#if UTL_HOST_TYPE == UTL_HT_UNIX
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, join ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);
    ASSERTFNZ(pthread_create(&_threadHandle, &attr, runThread, runThreadInfo));
    ASSERTD(_threadHandle != 0);
#else
    _threadHandle = CreateThread(nullptr, 0, runThread, runThreadInfo, 0, &_threadId);
    ASSERT(_threadHandle != nullptr);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::yield()
{
    hostOS->yield();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::pause()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ulong_t
Thread::tlsNew()
{
    ulong_t key = 0;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    pthread_key_t oskey;
    ASSERTFNZ(pthread_key_create(&oskey, nullptr));
    key = (ulong_t)oskey;
#else
    key = (ulong_t)TlsAlloc();
#endif
    return key;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
Thread::tlsGet(ulong_t key)
{
    void* value;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    value = pthread_getspecific((pthread_key_t)key);
#else
    value = TlsGetValue((DWORD)key);
#endif
    return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::tlsSet(ulong_t key, void* value)
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(pthread_setspecific((pthread_key_t)key, value));
#else
    TlsSetValue((DWORD)key, value);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

randutils::mt19937_64_rng*
Thread::rng()
{
    if (_rng == nullptr)
    {
        _rng = new randutils::mt19937_64_rng();
    }
    return _rng;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
void
Thread::setAffinity(size_t cpusetsize, cpu_set_t* cpuset)
{
    pthread_setaffinity_np(_threadHandle, cpusetsize, cpuset);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::utl_init()
{
#if UTL_HOST_OS == UTL_OS_WINDOWS
    _objectKey = tlsNew();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::utl_deInit()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::onExit()
{
}

//////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS
DWORD WINAPI
Thread::runThread(void* p_runThreadInfo)
{
    auto runThreadInfo = static_cast<RunThreadInfo*>(p_runThreadInfo);
    auto thread = runThreadInfo->getThread();
    void* arg = runThreadInfo->getArg();
    delete runThreadInfo;

    // remember which thread this is
    tlsSet(_objectKey, thread);

    // run the thread
    try
    {
        thread->run(arg);
    }
    catch (ThreadCancelledEx&)
    {
    }

    // allow the thread to do clean-up
    thread->onExit();

    // delete the thread if it's detached, otherwise it'll get deleted by join()
    if (thread->isDetached())
    {
        CloseHandle(thread->_threadHandle);
        delete thread;
    }
    return 1;
}
#endif // UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
void*
Thread::runThread(void* vp_runThreadInfo)
{
    void* res;
    auto runThreadInfo = static_cast<RunThreadInfo*>(vp_runThreadInfo);
    auto thread = runThreadInfo->getThread();
    void* arg = runThreadInfo->getArg();
    delete runThreadInfo;

    // remember which thread this is
    _thread = thread;

    // run the thread
    try
    {
        res = thread->run(arg);
    }
    catch (ThreadCancelledEx&)
    {
        res = nullptr;
    }

    // allow the thread to do clean-up
    thread->onExit();

    // delete the thread if it's detached (else join() will do it)
    if (thread->isDetached())
        delete thread;

    return res;
}
#endif // UTL_HOST_TYPE == UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::init()
{
    _id = (size_t)this;
    _node = 0;
    _rng = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Thread::deInit()
{
    delete _rng;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
__thread Thread* Thread::_thread;
#else
ulong_t Thread::_objectKey;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::RunThreadInfo);
UTL_CLASS_IMPL_ABC(utl::Thread);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_EXCEPTION_IMPL(utl, ThreadCancelledEx, utl::Exception, "thread cancelled");
