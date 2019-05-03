#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Queue.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TDeque, utl::Uint);
UTL_INSTANTIATE_TPL(utl::TDequeIt, utl::Uint);
UTL_INSTANTIATE_TPL(Queue, Uint);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Queue<Uint> queue;
    for (size_t i = 0; i < 4; i++)
    {
        queue.enQ(Uint(i));
        cout << "enQ: " << Uint(i) << endl;
    }
    while (!queue.empty())
    {
        Uint* i = queue.deQ();
        cout << "deQ: " << *i << endl;
        delete i;
    }

    return 0;
}
