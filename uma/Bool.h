#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Boolean value.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Bool : public Object
{
    UTL_CLASS_DECL(Bool, Object);

public:
    /**
       Constructor.
       \param val initial value
    */
    Bool(bool val)
    {
        set(val);
    }

    /**
       Constructor.
       \param str string representation ("true"/"yes", "false"/"no", "on/off")
    */
    Bool(const String& str)
    {
        set(str);
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Get a string representation ("true" or "false"). */
    virtual String toString() const;

    /** Get the value. */
    bool
    get() const
    {
        return _b;
    }

    /**
       Set the value from a string representation.
       \param str string representation ("true"/"yes", "false"/"no", "on/off")
    */
    void set(const String& str);

    /** Set the value. */
    void
    set(bool val)
    {
        _b = val;
    }

    /** Conversion to \b bool. */
    operator bool() const
    {
        return _b;
    }

    /** Conversion to \b bool. */
    operator bool()
    {
        return _b;
    }

private:
    void
    init()
    {
        _b = false;
    }
    void
    deInit()
    {
    }

private:
    bool _b;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
