#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include "../ucc/ConcurrentQueue.h"
#include "../ucc/ConcurrentQueue_mutex.h"
#include "../ucc/RingBuffer.h"
#include <libutl/Thread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

template class ConcurrentQueue<size_t>;
template class ConcurrentQueue_mutex<size_t>;
template class RingBuffer<size_t, (size_t)-1>;
template class std::function<void(size_t)>;

////////////////////////////////////////////////////////////////////////////////////////////////////

class CQthread : public utl::Thread
{
public:
    CQthread(ConcurrentQueue<size_t>& q, size_t count, size_t iters, cpu_set_t* cpus)
        : _q(q)
        , _count(count)
        , _iters(iters)
        , _cpus(cpus)
    {
    }

    virtual void* run(void*);

private:
    ConcurrentQueue<size_t>& _q;
    size_t _count;
    size_t _iters;
    cpu_set_t* _cpus;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CQMthread : public utl::Thread
{
public:
    CQMthread(ConcurrentQueue_mutex<size_t>& q, size_t count, size_t iters, cpu_set_t* cpus)
        : _q(q)
        , _count(count)
        , _iters(iters)
        , _cpus(cpus)
    {
    }

    virtual void* run(void*);

private:
    ConcurrentQueue_mutex<size_t>& _q;
    size_t _count;
    size_t _iters;
    cpu_set_t* _cpus;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RBthread : public utl::Thread
{
public:
    RBthread(RingBuffer<size_t, (size_t)-1>& q, size_t count, size_t iters, cpu_set_t* cpus)
        : _q(q)
        , _count(count)
        , _iters(iters)
        , _cpus(cpus)
    {
    }

    virtual void* run(void*);

private:
    RingBuffer<size_t, (size_t)-1>& _q;
    size_t _count;
    size_t _iters;
    cpu_set_t* _cpus;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
CQthread::run(void*)
{
    setAffinity(sizeof(cpu_set_t), _cpus);
    for (size_t iter = 0; iter < _iters; ++iter)
    {
        size_t count = _count;
        while (count-- > 0)
            _q.enQ(count);
        count = _count;
        size_t val;
        while (count-- > 0)
            _q.deQ(val);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
CQMthread::run(void*)
{
    setAffinity(sizeof(cpu_set_t), _cpus);
    for (size_t iter = 0; iter != _iters; ++iter)
    {
        size_t count = _count;
        while (count-- > 0)
            _q.enQ(count);
        count = _count;
        size_t val;
        while (count-- > 0)
            _q.deQ(val);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
RBthread::run(void*)
{
    setAffinity(sizeof(cpu_set_t), _cpus);
    for (size_t iter = 0; iter != _iters; ++iter)
    {
        size_t count = _count;
        while (count-- > 0)
            _q.enQ(count);
        count = _count;
        size_t val;
        while (count-- > 0)
            _q.deQ(val);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

enum structure_t
{
    cq,
    cqm,
    rb
};
char* structureName[] = {"utl::ConcurrentQueue", "utl::ConcurrentQueue_mutex", "utl::RingBuffer"};

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    // read arguments
    if (argc != 6)
    {
        cerr << "RingBuffer <'cq'|'cqm'|'rb'> <nodes> <threads> <count_in_mb> <iterations>" << endl;
        return 1;
    }
    String structureStr = argv[1];
    uint_t nodes = Uint(argv[2]);
    uint_t threads = Uint(argv[3]);
    uint_t count = Uint(argv[4]).get() * 1048576;
    uint_t iters = Uint(argv[5]);
    structure_t structure;
    if (structureStr == "cq")
        structure = cq;
    else if (structureStr == "cqm")
        structure = cqm;
    else
        structure = rb;
    structureStr = structureName[structure];
    cout << "testing " << structureStr << " (nodes=" << nodes << ", threads=" << threads
         << ", count=" << count << ", iters=" << iters << ")" << endl;

    cpu_set_t cpus, nodeCPUs[nodes];
    uint_t i;
    for (i = 0; i != nodes; ++i)
    {
        CPU_ZERO(nodeCPUs + i);
    }
    sched_getaffinity(getpid(), sizeof(cpu_set_t), &cpus);
    uint_t numCPUs = CPU_COUNT(&cpus);
    uint_t cpusPerNode = numCPUs / nodes;
    uint_t cpu = 0;
    for (i = 0; i != nodes; ++i)
    {
        for (uint_t j = 0; j != cpusPerNode; ++j)
        {
            CPU_SET(cpu++, nodeCPUs + i);
        }
    }

    ConcurrentQueue<size_t> cqQ;
    ConcurrentQueue_mutex<size_t> cqmQ;
    RingBuffer<size_t, (size_t)-1>* rbQ = nullptr;
    if (structure == rb)
        rbQ = new RingBuffer<size_t, (size_t)-1>(count);

    Thread* t[threads];
    size_t node = 0;
    for (size_t i = 0; i != threads; ++i)
    {
        if (structure == cq)
            t[i] = new CQthread(cqQ, count / threads, iters, nodeCPUs + node);
        else if (structure == cqm)
            t[i] = new CQMthread(cqmQ, count / threads, iters, nodeCPUs + node);
        else
            t[i] = new RBthread(*rbQ, count / threads, iters, nodeCPUs + node);
        if (++node == nodes)
            node = 0;
    }
    for (size_t i = 0; i != threads; ++i)
    {
        t[i]->start();
    }
    for (size_t i = 0; i != threads; ++i)
    {
        t[i]->join();
    }
    delete rbQ;
    return 0;
}
