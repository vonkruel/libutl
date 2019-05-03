#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Number.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Fixed-point decimal number.

   This simple implementation uses a signed 64-bit integer as the raw encoding, and provides 6
   decimal digits of precision.  That is, the fractional part of the decimal number may be
   precisely expressed as a whole number of one-millionths.

   Noting that:

   \arg the decimal number is stored in a signed 64-bit integer
   \arg 6 decimal digits of precision are provided
   \arg size of the space of 64-bit values: 18,446,744,073,709,551,616.
   \arg half the above size is: 9,223,372,036,854,775,808.

   We get:

   \arg minimum representable value: -9,223,372,036,854.775808
   \arg maximum representable value: +9,223,372,036,854.775807

   Addition and subtraction are simple: just add or subtract the "raw" underlying integers,
   and the result of that is the correct decimal answer.

   Multiplication and division are very simple too, but they do need a simple shift adjustment
   to restore correct scaling factor, as well as a rounding adjustment to minimize loss of
   precision when extra digits in the result of a calculation are discarded.  For example,
   2/3 will be calculated as 0.666667 (and not 0.666666).

   Note that if you call toString() with digits < 6, the printed number may be rounded up to
   show the closest approximation to the actual stored number in the specified number of digits:

   \code
   Decimal a("0.999999"), b("0.66899");
   cout << a.toString(2) << endl;           // outputs "1.00" !
   cout << b.toString(2) << endl;           // outputs "0.67"
   \endcode

   If you don't want this behavior, specify round=false when calling toString().

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Decimal : public Object
{
    UTL_CLASS_DECL(Decimal, Object);

public:
    /**
       Constructor
       \param n initial value
    */
    Decimal(int64_t n)
    {
        _n = n;
    }

    /**
       Constructor.
       \param str string representation of initial value
    */
    Decimal(const String& str)
    {
        set(str);
    }

    virtual void copy(const Object& rhs);

    virtual int compare(const Object& rhs) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual String toString() const;

    /**
       Convert to string representation.
       \param digits number of digits of precision
       \param round (optional : true) round up to show best approximation (if digits < 6)?
    */
    String toString(uint_t digits, bool round = true) const;

    /** Set from string representation. */
    Decimal& set(const String& str);

    /** Take the absolute value. */
    Decimal&
    abs()
    {
        if (_n < 0)
            _n *= -1;
        return self;
    }

    /** Remove any fractional component. */
    Decimal&
    floor()
    {
        if (_n >= 0)
            _n -= (_n % 1000000);
        else
            _n += (_n % 1000000);
        return self;
    }

    /// \name Math Operators
    //@{
    Decimal
    operator+(const Decimal& rhs) const
    {
        return Decimal(_n + rhs._n);
    }

    Decimal&
    operator+=(const Decimal& rhs)
    {
        _n += rhs._n;
        return self;
    }

    Decimal
    operator-(const Decimal& rhs) const
    {
        return Decimal(_n - rhs._n);
    }

    Decimal&
    operator-=(const Decimal& rhs)
    {
        _n -= rhs._n;
        return self;
    }

    Decimal operator*(const Decimal& rhs) const
    {
        Decimal res(_n);
        res.multiply(rhs._n);
        return res;
    }

    Decimal&
    operator*=(const Decimal& rhs)
    {
        multiply(rhs._n);
        return self;
    }

    Decimal
    operator/(const Decimal& rhs) const
    {
        Decimal res(_n);
        res.divide(rhs._n);
        return res;
    }

    Decimal&
    operator/=(const Decimal& rhs)
    {
        divide(rhs._n);
        return self;
    }

    Decimal
    operator%(const Decimal& rhs) const
    {
        Decimal res(_n);
        res.mod(rhs._n);
        return res;
    }

    Decimal&
    operator++()
    {
        _n += 1000000;
        return self;
    }

    Decimal
    operator++(int)
    {
        Decimal res = self;
        _n += 1000000;
        return res;
    }

    Decimal&
    operator--()
    {
        _n -= 1000000;
        return self;
    }

    Decimal
    operator--(int)
    {
        Decimal res = self;
        _n -= 1000000;
        return res;
    }
    //@}
private:
    void
    init()
    {
        _n = 0;
    }
    void
    deInit()
    {
    }

    Decimal& multiply(int64_t rhs);

    Decimal& divide(int64_t rhs);

    Decimal& mod(int64_t rhs);

private:
    int64_t _n;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
