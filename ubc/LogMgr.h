#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/IOmux.h>
#include <libutl/Mutex.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class LogSetting;
class LogStream;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Manage application event logging.

   Each stream added (via addStream()) has these attributes:

   \arg \b category : Categorizes the logging output.  For example, suppose you have several
   subsystems in your application, and you want to send logging output from each subsystem to
   different streams.  To do this, you would define a category for each software subsystem, and
   associate different streams with each subsystem category.

   \arg \b level : Specifies the relative importance of logging output.  Higher values mean greater
   importance, lower values mean lesser importance.  LogMgr allows you to easily ensure that each
   stream will only receive logging output of a certain level of importance.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LogMgr : public Object, protected FlagsMI
{
    UTL_CLASS_DECL(LogMgr, Object);

public:
    /** See Object::clear(). */
    virtual void clear();

    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Set the \b owner flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /**
       Add a stream that will receive logging output.
       \param stream logging output destination
       \param category the stream will only receive logging output
              for its own category
       \param level the stream will only receive logging output
              for its own level or greater
    */
    void addStream(Stream* stream, uint_t category, uint_t level);

    /**
       Log an application event.  The log entry will be written to all streams that have the same
       category, and whose level is <= the given level.
       \param str output string
       \param category (optional : 0) logging category
       \param level (optional : uint_t_max) logging level
    */
    void put(const String& str, uint_t category = 0, uint_t level = uint_t_max);

    /**
       A front-end for put() which logs a newline after the given string.
       \param str output string
       \param category (optional : 0) logging category
       \param level (optional) logging level
    */
    void
    putLine(const String& str, uint_t category = 0, uint_t level = uint_t_max)
    {
        put(str + '\n', category, level);
    }

    /**
       Set the default level for a category.
       \param category log category
       \param level default level for the category
    */
    void setLevel(uint_t category, uint_t level);

public:
    static void utl_deInit();

private:
    enum flg_t
    {
        flg_owner
    };

private:
    void init();
    void deInit();

private:
    Vector<uint_t> _levels;
    TArray<LogStream>* _logStreams;
    Mutex* _mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Global instance of LogMgr. */
extern LogMgr logMgr;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
