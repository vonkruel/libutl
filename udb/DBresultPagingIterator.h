#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/DBconnection.h>
#include <libutl/DBresult.h>
#include <libutl/DBresultRow.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Paging database query result iterator.

   When the result of a database query may contain a large number of rows, DBresultPagingIterator
   is used to iterate over the result rows.  Result rows are retrieved in pages/chunks to limit
   memory usage while maintaining high efficiency.  The default is to retrieve 1000 rows at a time,
   but you may choose a different page size.

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultPagingIterator : public Object
{
    UTL_CLASS_DECL(DBresultPagingIterator, Object);

public:
    /**
       Initialize.
       \param connection database connection
       \param cursorName database cursor name
       \param pageSize number of result rows retrieved per query
    */
    DBresultPagingIterator(DBconnection* connection, const char* cursorName, uint_t pageSize = 1000)
    {
        _result = nullptr;
        _page = uint64_t_max;
        _row = uint64_t_max;
        _pageSize = pageSize;
        _resultOwner = false;
        _cursorName = cursorName;
        _connection = connection;
        next();
    }

    /**
       Initialize end iterator.
       \param cursorName database cursor name
    */
    DBresultPagingIterator(const char* cursorName)
    {
        _result = nullptr;
        _page = uint64_t_max;
        _row = uint64_t_max;
        _pageSize = uint_t_max;
        _resultOwner = false;
        _cursorName = cursorName;
        _connection = nullptr;
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    /// \name Status Inquiry
    //@{
    /** Is everything OK? */
    bool
    ok() const
    {
        return ((_result != nullptr) && (_result->status() == dbr_ok));
    }

    /** Get the status. */
    db_result_status_t
    status() const
    {
        ASSERTD(_result != nullptr);
        return _result->status();
    }

    /** Get error message. */
    const String&
    errorMsg() const
    {
        ASSERTD(_result != nullptr);
        return _result->errorMsg();
    }
    //@}

    /** Get number of fields. */
    uint_t
    numFields() const
    {
        ASSERTD(_result != nullptr);
        return _result->numFields();
    }

    /** No more result rows? */
    bool
    isEnd() const
    {
        return (_connection == nullptr);
    }

    /** Get the current row. */
    DBresultRow
    row() const
    {
        return DBresultRow(_result, _row);
    }

    /** Get the current row index. */
    uint64_t
    rowIdx() const
    {
        return (_connection == nullptr) ? uint64_t_max : (_page * _pageSize) + _row;
    }

    /** Move forward one row. */
    void next();

    /** Address the current row. */
    DBresultRow operator*() const
    {
        return DBresultRow(_result, _row);
    }

    /** Move forward one row. */
    DBresultPagingIterator&
    operator++()
    {
        next();
        return *this;
    }

public:
    // for STL
    typedef DBresultRow value_type;
    typedef DBresultRow& reference;
    typedef DBresultRow* pointer;
    typedef std::forward_iterator_tag iterator_category;

private:
    void init();
    void deInit();
    void nextPage();
    void setResult(const DBresult* result);

private:
    const DBresult* _result;
    uint64_t _page;
    uint64_t _row;
    uint_t _pageSize;
    bool _resultOwner;
    String _cursorName;
    DBconnection* _connection;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
