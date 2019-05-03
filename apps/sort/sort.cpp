#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/algorithms_inl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE
UTL_APP(Sort)
UTL_MAIN_RL(Sort)

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Sort::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    bool reverse = args.isSet("r");
    if (args.printErrors(cerr))
        return 1;

    // read data from standard input
    Vector<char> dataVect(K(1024));
    char* data = dataVect.get();
    size_t dataPos = 0, dataSize = dataVect.size();
    size_t numStrings = 0;
    bool sol = true;
    while (!cin.eofBlocking())
    {
        // grow if necessary
        if ((dataSize - dataPos) < KB(4))
        {
            dataVect.grow(dataSize * 2);
            dataSize = dataVect.size();
            data = dataVect.get();
        }

        // read up to 4K
        size_t numBytes = cin.read((byte_t*)data + dataPos, KB(4), 0);

        // count strings...
        size_t nextDataPos = dataPos + numBytes;
        for (; dataPos < nextDataPos; ++dataPos)
        {
            if (sol)
            {
                ++numStrings;
                sol = false;
            }
            if (data[dataPos] == '\n')
            {
                sol = true;
                data[dataPos] = '\0';
            }
        }
    }

    Vector<char*> stVect(numStrings);
    char** st = stVect.get();
    size_t stPos = 0;
    size_t i = 0;
    sol = true;
    for (size_t i = 0; i < dataPos; ++i)
    {
        if (sol)
        {
            sol = false;
            st[stPos++] = data + i;
        }
        if (data[i] == '\0')
        {
            sol = true;
        }
    }
    ASSERT(stPos == numStrings);

    // sort the strings (this is the easy part :-)
    if (reverse)
    {
        multiKeyQuickSort<char>(stVect.get(), subtractReverse<char>(), 0U, numStrings);
    }
    else
    {
        multiKeyQuickSort<char>(stVect.get(), subtract<char>(), 0U, numStrings);
    }

    // write the strings to standard output
    for (i = 0; i < numStrings; i++)
    {
        cout.putLine(st[i]);
    }

    return 0;
}
