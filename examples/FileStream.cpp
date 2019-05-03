#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/FileStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    if (args.items() != 3)
    {
        cout << "Usage: " << args(0) << " <infile> <outfile>" << endl;
        return 1;
    }

    FileStream inFile(args(1), io_rd);
    FileStream outFile(args(2), fs_clobber);
    outFile.copyData(inFile);

    // inFile and outFile are closed when they go out of scope
    // (destructor takes care of it)
    return 0;
}
