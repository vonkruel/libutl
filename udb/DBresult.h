#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BitArray.h>
#include <libutl/DBresultField.h>
#include <libutl/Decimal.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultRowIterator;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Database command status.
   \ingroup database
*/
enum db_result_status_t
{
    dbr_ok,             /**< Command executed successfully */
    dbr_nonfatal_error, /**< Non-fatal error during command execution. */
    dbr_fatal_error,    /**< Fatal error during command execution. */
    dbr_undefined       /**< Undefined status */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_EXCEPTION_DECL(DatabaseEx, Exception);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Database command result.

   An instance of DBresult stores the result of a SQL query executed on the RDBMS server.

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresult : public Object
{
    UTL_CLASS_DECL(DBresult, Object);

public:
    using iterator = DBresultRowIterator;

public:
    /** Initialize a new result. */
    void initialize(uint64_t numRows, uint_t numFields, DBresultField** fields);

    virtual void copy(const Object& rhs);

    /** Delete field and value objects. */
    void clear();

    /** Get the status. */
    db_result_status_t
    status() const
    {
        return _status;
    }

    /** Get the status. */
    db_result_status_t&
    status()
    {
        return _status;
    }

    /** Get the number of rows. */
    uint64_t
    numRows() const
    {
        return _numRows;
    }

    /** Get the number of fields. */
    uint_t
    numFields() const
    {
        return _numFields;
    }

    /** Get the field at the given index. */
    const DBresultField&
    field(uint_t idx) const
    {
        return *_fields[idx];
    }

    /** Get a row index. */
    uint64_t
    rowIndex(uint64_t rowIdx) const
    {
        ASSERTD(rowIdx < _numRows);
        return (rowIdx * _numFields);
    }

    /** Get values pointer. */
    const uint64_t*
    values() const
    {
        return _values;
    }

    /** Get values pointer. */
    uint64_t*
    values()
    {
        return _values;
    }

    /** Get string value at given row and field indices. */
    const char* getString(uint64_t rowIdx, uint_t fieldIdx) const;

    /** Get boolean value at given row and field indices. */
    bool getBool(uint64_t rowIdx, uint_t fieldIdx) const;

    /** Get 32-bit integer value at given row and field indices. */
    int getInt32(uint64_t rowIdx, uint_t fieldIdx) const;

    /** Get 64-bit integer value at given field index. */
    int64_t getInt64(uint64_t rowIdx, uint_t fieldIdx) const;

    /** Get double value at given row and field indices. */
    double getDouble(uint64_t rowIdx, uint_t fieldIdx) const;

    /** Get decimal value at given row and field indices (as a string). */
    const char* getDecimal(uint64_t rowIdx, uint_t fieldIdx) const;

    /** Get date/time value at given row and field indices. */
    time_t getDateTime(uint64_t rowIdx, uint_t fieldIdx) const;

    /**
       Is the given cell null?
       \param cellIdx cell index (rowIdx * numFields + fieldIdx)
    */
    bool
    isNull(uint64_t cellIdx) const
    {
        ASSERTD(cellIdx < (_numRows * _numFields));
        return _nullArray.get(cellIdx);
    }

    /**
       Is the given cell null?
       \param rowIdx row index
       \param fieldIdx field index
    */
    bool
    isNull(uint64_t rowIdx, uint_t fieldIdx) const
    {
        ASSERTD(rowIdx < _numRows);
        ASSERTD(fieldIdx < _numFields);
        auto cellIdx = (rowIdx * _numFields) + fieldIdx;
        return _nullArray.get(cellIdx);
    }

    /**
       Mark the given cell as null.
       \param rowIdx row index
       \param fieldIdx field index
    */
    void
    setNull(uint64_t rowIdx, uint_t fieldIdx)
    {
        ASSERTD(rowIdx < _numRows);
        ASSERTD(fieldIdx < _numFields);
        auto cellIdx = (rowIdx * _numFields) + fieldIdx;
        _nullArray.set(cellIdx, 1);
    }

    /** Get the null-flag array. */
    const BitArray&
    nullArray() const
    {
        return _nullArray;
    }

    /** Get error message. */
    const String&
    errorMsg() const
    {
        return _errorMsg;
    }

    /** Get error message. */
    String&
    errorMsg()
    {
        return _errorMsg;
    }

    /// \name Iterators
    //@{
    inline iterator begin() const;

    inline iterator end() const;
    //@}

private:
    void init();
    void deInit();

private:
    db_result_status_t _status;
    uint64_t _numRows;
    String _errorMsg;
    uint_t _numFields;
    DBresultField** _fields;
    uint64_t* _values;
    BitArray _nullArray;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/DBresultRowIterator.h>
