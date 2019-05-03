#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/LogMgr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    logMgr.setOwner(false);
    logMgr.addStream(cout, 0, 1);
    logMgr.addStream(cerr, 0, 2);
    logMgr.setLevel(0, 1);
    logMgr.putLine("only goes to stdout");
    logMgr.setLevel(0, 2);
    logMgr.putLine("goes to stdout and stderr");
    return 0;
}
