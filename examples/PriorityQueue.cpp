#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/PriorityQueue.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(THeap, Uint);
UTL_INSTANTIATE_TPL(THeapIt, Uint);
UTL_INSTANTIATE_TPL(PriorityQueue, Uint);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    PriorityQueue<Uint> pq;
    for (size_t i = 0; i < 10; i++)
    {
        pq += Uint(i);
        cout << "add: " << Uint(i) << endl;
    }
    cout << "--- priority queue contents (breadth first)" << endl;
    pq.dump(cout);
    while (!pq.empty())
    {
        Uint* i = pq.pop();
        cout << "remove: " << *i << endl;
        delete i;
    }
    return 0;
}
