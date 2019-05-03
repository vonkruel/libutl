#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <syslog.h>
#include <libutl/BufferedStream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Syslog stream.

   SyslogStream provides a convenient interface to the system logging facility (syslog) available
   on UNIX-like systems.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SyslogStream : public BufferedStream
{
    UTL_CLASS_DECL(SyslogStream, BufferedStream);
    UTL_CLASS_NO_COPY;

public:
    /**
       Constructor
       \param ident identify originator of log messages
       \param facility see syslog(3)
       \param level see syslog(3)
    */
    SyslogStream(const String& ident, int facility, int level = int_t_max)
    {
        init();
        open(ident, facility, level);
    }

    /**
       Set a new configuration
       \param ident identify originator of log messages
       \param facility see syslog(3)
       \param level see syslog(3)
    */
    void open(const String& ident, int facility, int level = int_t_max);

    /** Close the syslog connection. */
    virtual void close();

    /** Get the facility. */
    int
    getFacility()
    {
        return _facility;
    }

    /** Set the facility. */
    void
    setFacility(int facility)
    {
        _facility = facility;
    }

    /** Get the level. */
    int
    getLevel()
    {
        return _level;
    }

    /** Set the level. */
    void
    setLevel(int level)
    {
        _level = level;
    }

protected:
    virtual void clear();
    virtual void overflow();
    virtual void
    underflow()
    {
        ABORT();
    }

private:
    void
    init()
    {
        clear();
    }
    void
    deInit()
    {
        close();
    }
    void clearSelf();

private:
    String _ident;
    int _facility;
    int _level;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
