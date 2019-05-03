#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Time.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Time t;

    // output nullptr value
    cout << "Time() = " << t << endl;

    // set to current time
    cout << "t = Time::current()" << endl;
    t = Time::current();

    // write current time (GMT)
    cout << "GMT = " << t.toGMTstring() << endl;

    // write current time (local TZ)
    cout << "local time = " << t << endl;

    // rounding
    t.roundDown(tm_minute);
    cout << "roundDown(tm_minute) = " << t << endl;
    t.roundDown(tm_half_hour);
    cout << "roundDown(tm_half_hour) = " << t << endl;
    t.roundDown(tm_hour);
    cout << "roundDown(tm_hour) = " << t << endl;
    t.roundDown(tm_day);
    cout << "roundDown(tm_day) = " << t << endl;
    t.roundDown(tm_week);
    cout << "roundDown(tm_week) = " << t << endl;

    // addition
    Duration dur("3h 10m");
    t += dur;
    cout << "t + " << dur << " = " << t << endl;
    cout << "date = " << t.toString("$Wn, $Mn $d $yyyy$dst") << endl;
    cout << "time = " << t.toString("$hh:$nn:$ss") << endl;
    dur.set("365d");
    t -= dur;
    cout << "t - " << dur << " = " << t << endl;

    // assignment from time_t
    time_t tv;
    time(&tv);
    t = tv;
    cout << "local time = " << t << endl;

    return 0;
}
