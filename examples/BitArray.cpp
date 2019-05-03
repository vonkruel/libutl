#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BitArray.h>
#include <libutl/BufferedFDstream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE
UTL_APP(Test)
UTL_MAIN_RL(Test)

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    // array of 22 3-bit numbers
    size_t sz = 22;
    BitArray bitArray(sz, 3);

    for (size_t i = 0; i < sz; i++)
    {
        bitArray[i] = i % 8;
    }
    for (size_t i = 0; i < sz; i++)
    {
        cout << "bitArray[" << i << "] = " << bitArray[i] << endl;
    }

    return 0;
}
