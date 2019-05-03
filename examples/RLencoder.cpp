#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/RLencoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    if (args.isSet("help"))
    {
        cout << "Usage: " << args(0) << " [-d]" << endl;
        return 0;
    }
    bool compress = !args.isSet("d");
    if (args.printErrors(cerr))
        return 1;

    RLencoder rle(compress ? io_wr : io_rd, compress ? cout : cin, false);

    compress ? rle.copyData(cin) : cout.copyData(rle);

    return 0;
}
