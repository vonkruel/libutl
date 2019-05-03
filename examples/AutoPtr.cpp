#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/AutoPtr.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Array array;
    AutoPtr<Object> objectPtr = new Uint();
    AutoPtr<FwdIt> itPtr = array.beginNew();

    // assertion failure (in DEBUG mode)
    itPtr = objectPtr;

    return 0;
}
