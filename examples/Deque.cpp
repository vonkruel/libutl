#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Deque.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Deque deque;
    uint64_t n;

    // add [8192,32768) to the back end
    for (n = 8192; n < 32768; ++n)
    {
        deque.pushBack(new Uint(n));
    }

    // add [0,8192) to the front end
    for (n = 8191; n != uint64_t_max; --n)
    {
        deque.pushFront(new Uint(n));
    }

    // try get(size_t)
    for (n = 0; n < 32768; ++n)
    {
        Uint* uint = (Uint*)deque.get(n);
        ASSERT(*uint == n);
    }

    // iterate forward
    DequeIt it;
    n = 0;
    for (it = deque.begin(); it != deque.end(); ++it)
    {
        Uint* uint = (Uint*)*it;
        ASSERT(*uint == n++);
    }
    ++it; // try to move past end
    ASSERT(it == deque.end());

    // iterate backward
    for (;;)
    {
        --it;
        Uint* uint = (Uint*)*it;
        ASSERT(*uint == --n);
        if (it == deque.begin())
            break;
    }
    --it; // try to move before beginning
    ASSERT(it == deque.begin());

    // remove from back end: [16384,32768)
    // .. leaving [0,16384)
    for (n = 32767; n >= 16384; --n)
    {
        AutoPtr<Uint> uint = (Uint*)deque.popBack();
        ASSERT(*uint == n);
    }

    // remove from front end: [0,16384)
    for (n = 0; n < 16384; ++n)
    {
        AutoPtr<Uint> uint = (Uint*)deque.popFront();
        ASSERT(*uint == n);
    }
    ASSERT(deque.empty());

    return 0;
}
