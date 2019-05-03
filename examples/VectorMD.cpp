#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/VectorMD.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Vector2d<size_t> vect(50, 100);
    size_t i, j, k = 0;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 100; j++)
        {
            vect(i, j) = k++;
        }
    }
    k = 0;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 100; j++)
        {
            ASSERT(vect(i, j) == k);
            k++;
        }
    }
    cout << "all tests passed" << endl;
    return 0;
}
