#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <sys/times.h>
#else
#include <libutl/win32api.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Measure CPU time.

   OStimer measures CPU time spent in kernel- and user-space by the invoking process and its
   children.

   \author Adam McKee
   \ingroup hostos
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class OStimer : public Object
{
    UTL_CLASS_DECL(OStimer, Object);
    UTL_CLASS_DEFID;
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

public:
    /** Start the timer. */
    void start();
    /** Stop the timer. */
    void stop();

    /** Get user time (in seconds). */
    double userTime();
    /** Get system time (in seconds). */
    double systemTime();
    /** Get user + system time (in seconds). */
    double totalTime();

#if UTL_HOST_TYPE == UTL_HT_UNIX
public:
    /** Get user time of children (in seconds). */
    double childUserTime();
    /** Get system time of children (in seconds). */
    double childSystemTime();
    /** Get user + system time of children (in seconds). */
    double childTotalTime();

private:
    inline static double clocksToSeconds(clock_t clocks);
    struct tms _tmsStart;
    struct tms _tmsStop;
#else
private:
    inline static double intervalSeconds(FILETIME& lhs, FILETIME& rhs);
    FILETIME _userStart, _userStop;
    FILETIME _systemStart, _systemStop;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
