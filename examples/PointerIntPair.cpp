#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/PointerIntPair.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    long x;
    cout << " &x = " << &x << endl;
    PointerIntPair<long*, 2> pip;
    pip.setPointer(&x);
    pip.setInt(3);
    cout << "ptr = " << pip.getPointer() << endl;
    cout << "int = " << pip.getInt() << endl;
    return 0;
}
