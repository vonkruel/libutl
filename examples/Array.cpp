#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/OStimer.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Array a;

    // try out utl::Array.replace()
    for (size_t i = 0; i < 10; ++i)
    {
        a += new Uint(i);
    }
    a.setKeepSorted(true);
    cout << "initial array:        " << a << endl;
    a.replace(Uint(3), new Uint(8));
    cout << "after replace( 3, 8): " << a << endl;
    a.replace(Uint(2), new Uint(10));
    cout << "after replace( 2,10): " << a << endl;
    a.replace(Uint(8), new Uint(1));
    cout << "after replace( 8, 1): " << a << endl;

    return 0;
}
