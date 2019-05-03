#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/DBresultPagingIterator.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBconnection;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Range of result rows for a potentially large result.

   When the result of a database query may contain a large number of rows, DBresultPagingRange
   is used to facilitate iteration over the result rows with a range-based <code>for</code> loop.

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultPagingRange : public Object
{
    UTL_CLASS_DECL(DBresultPagingRange, Object);
    UTL_CLASS_NO_COPY;

public:
    using iterator = DBresultPagingIterator;

public:
    /** Constructor. */
    DBresultPagingRange(DBconnection* connection,
                        const char* query,
                        const char* cursorName = nullptr,
                        uint_t pageSize = 1000)
        : _connection(connection)
        , _cursorName((cursorName == nullptr) ? "defaultCursor" : cursorName)
        , _pageSize(pageSize)
        , _cursorOpen(false)
    {
        openCursor(query);
    }

    virtual void steal(Object& rhs);

    void
    close()
    {
        closeCursor();
    }

    /** Return begin iterator. */
    iterator
    begin() const
    {
        return iterator(_connection, _cursorName, _pageSize);
    }

    /** Return end iterator. */
    iterator
    end() const
    {
        return iterator(_cursorName);
    }

private:
    void
    init()
    {
        ABORT();
    }
    void deInit();

    void openCursor(const char* query);

    void closeCursor();

private:
    DBconnection* _connection;
    String _cursorName;
    uint_t _pageSize;
    bool _cursorOpen;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
