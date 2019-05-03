#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Stack.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(TArray, Uint);
UTL_INSTANTIATE_TPL(Stack, Uint);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Stack<Uint> stack;
    for (size_t i = 0; i < 4; i++)
    {
        stack.push(Uint(i));
        cout << "push: " << Uint(i) << endl;
    }
    while (!stack.empty())
    {
        Uint* i = stack.pop();
        cout << "pop: " << *i << endl;
        delete i;
    }

    return 0;
}
