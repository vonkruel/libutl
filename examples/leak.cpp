#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    // write before the beginning of a block
    char* str = new char[16];
    for (int i = 15; i >= -1; i--) // doh!
    {
        str[i] = i;
    }
    // it's detected when we free the memory
    delete[] str;

    // write past the end of a block
    str = new char[16];
    for (uint_t i = 0; i <= 16; i++) // doh!
    {
        str[i] = i;
    }
    // it's detected when we free the memory
    delete[] str;

    cerr << "--- free a block twice" << endl;
    str = new char[16];
    delete[] str;
    delete[] str;

    // leak a couple of blocks
    // (detected when we call memReportLeaks())
    // (UTL_MAIN_RL does this in DEBUG mode)
    str = new char[256];
    strcpy(str, "First leak");
    str = new char[16];
    strcpy(str, "Second leak");

    cerr << endl;

    return 0;
}
