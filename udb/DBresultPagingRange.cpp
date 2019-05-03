#include <libutl/libutl.h>
#include <libutl/DBresultPagingRange.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::DBresultPagingRange);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingRange::steal(Object& rhs)
{
    auto& pr = utl::cast<DBresultPagingRange>(rhs);
    _connection = pr._connection;
    _cursorName = pr._cursorName;
    _pageSize = pr._pageSize;
    _cursorOpen = pr._cursorOpen;
    pr._cursorOpen = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingRange::deInit()
{
    if (_cursorOpen)
    {
        try
        {
            closeCursor();
        }
        catch (DatabaseEx&)
        {
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingRange::openCursor(const char* query)
{
    ASSERTD(!_cursorOpen);
    auto result = _connection->declareCursor(_cursorName, query);
    _connection->throwOnError(result, true);
    result = _connection->openCursor(_cursorName);
    _connection->throwOnError(result, true);
    _cursorOpen = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingRange::closeCursor()
{
    ASSERTD(_cursorOpen);
    auto result = _connection->closeCursor(_cursorName);
    _cursorOpen = false;
    _connection->throwOnError(result, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
