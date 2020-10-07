#include <libutl/libutl.h>
#include <libutl/win32api.h>
#include <libutl/OStimer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::OStimer);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
OStimer::start()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    times(&_tmsStart);
#else
    FILETIME dummy;
    GetProcessTimes((HANDLE)-1, &dummy, &dummy, &_systemStart, &_userStart);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
OStimer::stop()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    times(&_tmsStop);
#else
    FILETIME dummy;
    GetProcessTimes((HANDLE)-1, &dummy, &dummy, &_systemStop, &_userStop);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::userTime()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    return clocksToSeconds(_tmsStop.tms_utime - _tmsStart.tms_utime);
#else
    return intervalSeconds(_userStart, _userStop);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::systemTime()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    return clocksToSeconds(_tmsStop.tms_stime - _tmsStart.tms_stime);
#else
    return intervalSeconds(_systemStart, _systemStop);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::totalTime()
{
    return userTime() + systemTime();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::childUserTime()
{
    return clocksToSeconds(_tmsStop.tms_cutime - _tmsStart.tms_cutime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::childSystemTime()
{
    return clocksToSeconds(_tmsStop.tms_cstime - _tmsStart.tms_cstime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::childTotalTime()
{
    return clocksToSeconds((_tmsStop.tms_cutime - _tmsStart.tms_cutime) +
                           (_tmsStop.tms_cstime + _tmsStart.tms_cstime));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::clocksToSeconds(clock_t clocks)
{
    return ((double)clocks / (double)sysconf(_SC_CLK_TCK));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#else // UTL_HOST_TYPE != UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

double
OStimer::intervalSeconds(FILETIME& lhs, FILETIME& rhs)
{
    uint64_t& lhsClocks = (uint64_t&)lhs;
    uint64_t& rhsClocks = (uint64_t&)rhs;
    uint64_t clocks = rhsClocks - lhsClocks;
    double seconds = clocks / (uint64_t)10000000;
    uint64_t remainder = clocks % (uint64_t)10000000;
    seconds += ((double)remainder / (double)10000000);
    return seconds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;