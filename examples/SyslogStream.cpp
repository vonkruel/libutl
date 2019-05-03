#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/SyslogStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    SyslogStream syslog("SyslogStream", LOG_DAEMON, LOG_INFO);
    syslog.setMode(io_wr);
    syslog << "fault detected in AE35 unit" << endl;
    syslog << "oops - the AE35 is actually fine" << endl;
#endif

    // If you don't see the output, check your syslogd configuration,
    // especially to see where messages with facility = LOG_DAEMON and
    // level = LOG_INFO should go.

    return 0;
}
