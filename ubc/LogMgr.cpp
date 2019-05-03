#include <libutl/libutl.h>
#include <libutl/LogMgr.h>
#include <libutl/RBtree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class LogStream : public Object
{
    UTL_CLASS_DECL(LogStream, Object);
    UTL_CLASS_DEFID;

public:
    LogStream(Stream* stream, uint_t category, uint_t level)
    {
        _stream = stream;
        _category = category;
        _level = level;
    }

    uint_t
    getCategory() const
    {
        return _category;
    }

    uint_t
    getLevel() const
    {
        return _level;
    }

    Stream*
    getStream() const
    {
        return _stream;
    }

private:
    Stream* _stream;
    uint_t _category;
    uint_t _level;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::clear()
{
    MutexGuard g(_mutex);
    RBtree streams(true, false, new AddressOrdering());

    // delete streams if we own them
    for (auto logStream : *_logStreams)
    {
        Stream* stream = logStream->getStream();
        if (isOwner())
            streams += stream;
    }

    _levels.excise();
    _logStreams->excise();
#ifdef DEBUG
    _logStreams->exciseOwnedIts();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::addStream(Stream* stream, uint_t category, uint_t level)
{
    MutexGuard g(_mutex);
    *_logStreams += new LogStream(stream, category, level);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::put(const String& str, uint_t category, uint_t level)
{
    MutexGuard g(_mutex);
    if (level == uint_t_max)
    {
        _levels.grow(category + 1);
        level = _levels[(size_t)category];
    }
    for (auto logStream : *_logStreams)
    {
        uint_t lsCategory = logStream->getCategory();
        if ((lsCategory != uint_t_max) && (lsCategory != category))
        {
            continue;
        }
        if (level >= logStream->getLevel())
        {
            Stream& stream = *logStream->getStream();
            stream << str;
            stream.flush();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::setLevel(uint_t category, uint_t level)
{
    MutexGuard g(_mutex);
    _levels.grow(category + 1);
    _levels[(size_t)category] = level;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::utl_deInit()
{
    logMgr.clear();
    delete logMgr._logStreams;
    logMgr._logStreams = nullptr;
    delete logMgr._mutex;
    logMgr._mutex = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::init()
{
    _levels.setAutoInit(true);
    _logStreams = new TArray<LogStream>();
    _mutex = new Mutex();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LogMgr::deInit()
{
    delete _logStreams;
    delete _mutex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// the global instance
LogMgr logMgr;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::LogStream);
UTL_CLASS_IMPL(utl::LogMgr);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TArray, utl::LogStream);
