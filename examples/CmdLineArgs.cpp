#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

static void
trySwitch(Stream& os, CmdLineArgs& args, const String& sw)
{
    String val;
    if (args.isSet(sw, val))
    {
        os << sw << " given";
        if (!val.empty())
        {
            os << ", val = " << val;
        }
        os << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    String val;
    trySwitch(cout, args, "foo");
    trySwitch(cout, args, "bar");
    trySwitch(cout, args, "a");
    trySwitch(cout, args, "z");
    args.printErrors(cerr);
    return 0;
}
