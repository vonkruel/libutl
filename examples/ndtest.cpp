#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/Thread.h>

#undef new
#include <algorithm>
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

class AllocatorThread : public utl::Thread
{
    UTL_CLASS_DECL_ABC(AllocatorThread, utl::Thread);

public:
    AllocatorThread(size_t* sizes, size_t numSizes, size_t numRuns)
    {
        _sizes = sizes;
        _numSizes = numSizes;
        _numRuns = numRuns;
        _ptrs = (void**)malloc(numSizes * sizeof(void*));
    }

    void* run(void*);

protected:
    virtual void oneRun() = 0;

protected:
    size_t* _sizes;
    void** _ptrs;
    size_t _numSizes;
    size_t _numRuns;

private:
    void
    init()
    {
        ABORT();
    }
    void
    deInit()
    {
        free(_ptrs);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
AllocatorThread::run(void*)
{
    for (size_t i = 0; i != _numRuns; ++i)
        oneRun();
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

class MallocFreeThread : public AllocatorThread
{
    UTL_CLASS_DECL(MallocFreeThread, AllocatorThread);
    UTL_CLASS_DEFID;

public:
    MallocFreeThread(size_t* sizes, size_t numSizes, size_t numRuns)
        : AllocatorThread(sizes, numSizes, numRuns)
    {
    }

protected:
    virtual void
    oneRun()
    {
        size_t* sizes = _sizes;
        size_t s = _numSizes;
        size_t* sizesLim = sizes + s;
        void** ptrs = _ptrs;
        void** ptrsLim = ptrs + s;

        // allocate
        size_t* sp = sizes;
        void** pp = ptrs;
        for (; sp != sizesLim; ++sp, ++pp)
        {
            *pp = malloc(*sp);
        }

        // free
        for (pp = ptrs; pp != ptrsLim; ++pp)
        {
            free(*pp);
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class NewDeleteThread : public AllocatorThread
{
    UTL_CLASS_DECL(NewDeleteThread, AllocatorThread);
    UTL_CLASS_DEFID;

public:
    NewDeleteThread(size_t* sizes, size_t numSizes, size_t numRuns)
        : AllocatorThread(sizes, numSizes, numRuns)
    {
    }

protected:
    virtual void
    oneRun()
    {
        size_t* sizes = _sizes;
        size_t s = _numSizes;
        size_t* sizesLim = sizes + s;
        void** ptrs = _ptrs;
        void** ptrsLim = ptrs + s;

        // allocate
        size_t* sp = sizes;
        void** pp = ptrs;
        for (; sp != sizesLim; ++sp, ++pp)
        {
            *pp = new byte_t[*sp];
        }

        // free
        for (pp = ptrs; pp != ptrsLim; ++pp)
        {
            delete[](byte_t*) * pp;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(AllocatorThread);
UTL_CLASS_IMPL(MallocFreeThread);
UTL_CLASS_IMPL(NewDeleteThread);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);

    // check if help was requested
    if (args.isSet("h") || args.isSet("help"))
    {
        cerr << "ndtest [-t threads] [-r runs/thread] [-c count] [-m]" << endl;
        return 1;
    }

    // set # of threads
    size_t numThreads = 1;
    String arg;
    if (args.isSet("t", arg))
    {
        numThreads = Uint(arg);
        if (numThreads < 1)
            numThreads = 1;
        if (numThreads > 256)
            numThreads = 256;
    }

    // set # of runs per thread
    size_t numRuns = 1;
    if (args.isSet("r", arg))
    {
        numRuns = Uint(arg);
        if (numRuns < 1)
            numRuns = 1;
        if (numRuns > KB(64))
            numRuns = KB(64);
    }

    // set # of allocations of each size
    size_t numAllocs = 256;
    if (args.isSet("c", arg))
    {
        numAllocs = Uint(arg);
        if (numAllocs < 1)
            numAllocs = 1;
        if (numAllocs > KB(16))
            numAllocs = KB(16);
    }

    // use malloc/free ?
    bool useMalloc = args.isSet("m");

    // create the array of block sizes
    size_t numSizes = numAllocs * (256 / 8);
    size_t* sizes = (size_t*)malloc(numSizes * sizeof(size_t));
    size_t* sizePtr = sizes;
    for (size_t i = 0; i != numAllocs; ++i)
    {
        for (size_t j = 8; j <= 256; j += 8)
        {
            *sizePtr++ = j;
        }
    }

    // spawn threads
    Array threads(false);
    for (size_t i = 0; i != numThreads; ++i)
    {
        Thread* t;
        if (useMalloc)
            t = new MallocFreeThread(sizes, numSizes, numRuns);
        else
            t = new NewDeleteThread(sizes, numSizes, numRuns);
        threads += t;
        t->start(nullptr, true);
    }
    cout << "threads are started.." << endl;

    // join on threads
    for (size_t i = 0; i != numThreads; ++i)
    {
        Thread* t = (Thread*)threads[i];
        t->join();
    }
    cout << "threads are done!" << endl;

    // clean up (why not?)
    free(sizes);

    return 0;
}
