#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/List.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(TList, Uint);
UTL_INSTANTIATE_TPL(TListIt, Uint);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    TList<Uint> list;
    for (size_t i = 0; i < 8; i++)
    {
        list += Uint(7 - i);
    }
    cout << "--- initial list" << endl;
    list.dump(cout);
    cout << "--- removeFront()" << endl;
    list.removeFront();
    list.dump(cout);
    cout << "--- removeBack()" << endl;
    list.removeBack();
    list.dump(cout);
    cout << "--- sort()" << endl;
    list.sort();
    list.dump(cout);
    return 0;
}
