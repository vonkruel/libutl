#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <libutl/Number.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Floating point number.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Float : public Number<double>
{
    UTL_CLASS_DECL(Float, Number<double>);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param val initial value
    */
    Float(double val);

    /**
       Constructor
       \param str string representation of initial value
    */
    Float(const String& str)
    {
        set(str);
    }

    virtual int compare(const Object& rhs) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual String toString() const;

    virtual String
    toString(const char* fmt) const
    {
        return super::toString(fmt);
    }

    /**
       Get a string representation.
       \return string representation
       \param digits number of digits of precision (uint_t_max for maximum)
    */
    String toString(uint_t digits) const;

    String
    toString(int digits) const
    {
        ASSERTD(digits >= 0);
        return toString(static_cast<uint_t>(digits));
    }

    void
    set(double n)
    {
        super::set(n);
    }

    virtual Number<double>& set(const String& str);

    /** Determine whether the number is an integer (has no fractional component). */
    bool
    isInt() const
    {
        return (_n == floor(_n));
    }

    /** Get the absolute value of self. */
    Float
    abs() const
    {
        return Float(fabs(_n));
    }

    /** Get the result of \b self modulo \b rhs. */
    Float mod(const Float& rhs) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
