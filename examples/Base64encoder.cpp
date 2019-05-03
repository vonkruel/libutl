#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Base64encoder.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>

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

    Base64encoder base64(compress ? io_wr : io_rd, compress ? cout : cin, false);

    compress ? base64.copyData(cin) : cout.copyData(base64);

    return 0;
}
