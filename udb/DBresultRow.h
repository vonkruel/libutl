#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresult;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Database query result row.

   DBresultRow addresses a particular row of a query result.

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultRow : public Object
{
    UTL_CLASS_DECL(DBresultRow, Object);

public:
    /**
       Initialize.
       \param result the query result
       \param row index of result row
    */
    DBresultRow(const DBresult* result, uint64_t row)
    {
        init(result, row);
    }

    virtual void copy(const utl::Object& rhs);

    virtual void steal(utl::Object& rhs);

    virtual int compare(const utl::Object& rhs) const;

    /** Get number of fields. */
    uint_t numFields() const;

    /** Is the given field nullptr? */
    bool isNull(uint_t field) const;

    /** Get values pointer for this row. */
    const uint64_t*
    values() const
    {
        return _values;
    }

    /** Get raw value at given field index. */
    uint64_t
    field(uint_t fieldIdx) const
    {
        ASSERTD(!isNull(fieldIdx));
        return _values[(uint64_t)fieldIdx];
    }

    /** Get string value at given field index. */
    const char* getString(uint_t fieldIdx) const;

    /** Get boolean value at given field index. */
    bool getBool(uint_t fieldIdx) const;

    /** Get 32-bit integer value at given field index. */
    int getInt32(uint_t fieldIdx) const;

    /** Get 64-bit integer value at given field index. */
    int64_t getInt64(uint_t fieldIdx) const;

    /** Get double value at given field index. */
    double getDouble(uint_t fieldIdx) const;

    /** Get decimal value at given field index. */
    const char* getDecimal(uint_t fieldIdx) const;

    /** Get date/time value at given field index. */
    time_t getDateTime(uint_t fieldIdx) const;

private:
    void
    init()
    {
        _result = nullptr;
        _values = nullptr;
    }
    void init(const DBresult* result, uint64_t row);
    void
    deInit()
    {
    }

private:
    const DBresult* _result;
    const uint64_t* _values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
