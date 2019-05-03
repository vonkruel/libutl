#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/FileStream.h>
#include <libutl/IOmux.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    if (args.items() != 4)
    {
        cout << "Usage: " << args(0) << " <infile> <outfile1> <outFile2>" << endl;
        return 1;
    }

    FileStream inFile(args(1), io_rd);
    FileStream outFile1(args(2), fs_clobber);
    FileStream outFile2(args(3), fs_clobber);

    IOmux outMux(io_wr, false);
    outMux.add(outFile1);
    outMux.add(outFile2);

    outMux.copyData(inFile);

    return 0;
}
