#include <libutl/libutl.h>
#include <limits>
#include <math.h>
#include <libutl/Float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Float);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

Float::Float(double f)
    : Number<double>(f)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Float::compare(const Object& p_rhs) const
{
    auto& f = utl::cast<Float>(p_rhs);

    // normalize into [-1.0, +1.0]
    double lhs = _n;
    double rhs = f._n;
    bool lhsNeg = (lhs < 0.0);
    bool rhsNeg = (rhs < 0.0);
    double lhsABS = lhsNeg ? fabs(lhs) : lhs;
    double rhsABS = rhsNeg ? fabs(rhs) : rhs;
    if (lhsABS < rhsABS)
    {
        lhs /= rhsABS;
        rhs = rhsNeg ? -1.0 : +1.0;
    }
    else
    {
        rhs /= lhsABS;
        lhs = lhsNeg ? -1.0 : +1.0;
    }

    if (lhs < rhs)
    {
        if ((rhs - lhs) <= std::numeric_limits<double>::epsilon())
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if ((lhs - rhs) <= std::numeric_limits<double>::epsilon())
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Float::serialize(Stream& stream, uint_t io, uint_t mode)
{
    Object::serialize(stream, io, mode);
    utl::serialize(_n, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Float::toString() const
{
    return toString(uint_t_max);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Float::toString(uint_t digits) const
{
    char* str = new char[64];
    if (_n == double_t_min)
    {
        strcpy(str, "-INF");
    }
    if (_n == double_t_max)
    {
        strcpy(str, "+INF");
    }
    else
    {
        if (digits == uint_t_max)
        {
            digits = 2 + std::numeric_limits<double>::digits10;
        }
        sprintf(str, "%.*f", (int)digits, _n);
    }
    return String(str, true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Number<double>&
Float::set(const String& str)
{
    char c = str[0];
    if (!isdigit(c) && (c != '-') && (c != '+'))
        goto fail;
    errno = 0;
    _n = strtod(str, nullptr);
    if (errno != 0)
        goto fail;
    return self;
fail:
    _n = 0.0;
    throw IllegalValueEx(str);
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Float
Float::mod(const Float& rhs) const
{
    double d = _n / rhs._n;
    return Float(_n - (d * rhs._n));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
