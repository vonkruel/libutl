#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/LZencoder.h>

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

    // determine compression level
    uint_t level = 5;
    if (args.isSet("0"))
        level = 0;
    else if (args.isSet("1"))
        level = 1;
    else if (args.isSet("2"))
        level = 2;
    else if (args.isSet("3"))
        level = 3;
    else if (args.isSet("4"))
        level = 4;
    else if (args.isSet("5"))
        level = 5;
    else if (args.isSet("6"))
        level = 6;
    else if (args.isSet("7"))
        level = 7;
    else if (args.isSet("8"))
        level = 8;
    else if (args.isSet("9"))
        level = 9;

    // ensure no bad arguments given
    if (args.printErrors(cerr))
        return 1;

    LZencoder lze(compress ? io_wr : io_rd, compress ? cout : cin, false, level);

    compress ? lze.copyData(cin) : cout.copyData(lze);

    return 0;
}
