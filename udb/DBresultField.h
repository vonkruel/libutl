#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Database field type.
   \ingroup database
*/
enum db_result_field_t
{
    dbf_string,   /**< string */
    dbf_bool,     /**< boolean. */
    dbf_int16,    /**< signed 16-bit integer. */
    dbf_int32,    /**< signed 32-bit integer. */
    dbf_int64,    /**< signed 64-bit integer. */
    dbf_double,   /**< 64-bit floating point. */
    dbf_decimal,  /**< arbitrary-precision decimal. */
    dbf_dateTime, /**< date and time. */
    dbf_undefined /**< undefined type. */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Field in a DBresult tuple.

   Only a small set of basic types are supported here (see \ref db_result_field_t).

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultField : public Object
{
    UTL_CLASS_DECL(DBresultField, Object);

public:
    /** Constructor. */
    DBresultField(const String& name, db_result_field_t type /*, uint_t size = uint_t_max*/);

    virtual void copy(const Object& rhs);

    virtual int compare(const Object& rhs) const;

    /** Clear/empty the result. */
    void clear();

    /** Get the name. */
    const String&
    name() const
    {
        return _name;
    }

    /** Get the name. */
    String&
    name()
    {
        return _name;
    }

    /** Get the type. */
    db_result_field_t
    type() const
    {
        return _type;
    }

    /** Get the type. */
    db_result_field_t&
    type()
    {
        return _type;
    }

    /** Get the size. */
    /*uint_t
    size() const
    {
        return _size;
    }*/

    /** Get the size. */
    /*uint_t&
    size()
    {
        return _size;
    }*/

private:
    void
    init()
    {
        _type = dbf_undefined;
    }
    void
    deInit()
    {
    }

private:
    String _name;
    db_result_field_t _type;
    //uint_t _size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
