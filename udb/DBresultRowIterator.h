#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/DBconnection.h>
#include <libutl/DBresult.h>
#include <libutl/DBresultRow.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Database query result iterator.

   DBresultRowIterator provides a convenient way to iterate over the result rows of a query.

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultRowIterator : public Object
{
    UTL_CLASS_DECL(DBresultRowIterator, Object);

public:
    /**
       Initialize.
       \param result query result
       \param rowIdx row index
    */
    DBresultRowIterator(const DBresult* result, uint64_t rowIdx)
    {
        init();
        initialize(result, rowIdx);
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    /**
       Initialize.
       \param result query result
       \param rowIdx row index
    */
    void initialize(const DBresult* result, uint64_t rowIdx);

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
        ASSERTD(_result != nullptr);
        return (_row == _result->numRows());
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
        return _row;
    }

    /** Seek to the given row. */
    void seek(uint64_t row);

    /** Move forward one row. */
    void next();

    /** Address the current row. */
    DBresultRow operator*() const
    {
        return DBresultRow(_result, _row);
    }

    /** Move forward to the next row. */
    DBresultRowIterator&
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
    void
    deInit()
    {
    }

private:
    const DBresult* _result;
    uint64_t _row;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

DBresultRowIterator
DBresult::begin() const
{
    return DBresultRowIterator(this, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DBresultRowIterator
DBresult::end() const
{
    return DBresultRowIterator(this, _numRows);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
