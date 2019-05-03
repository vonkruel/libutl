#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_CC != UTL_CC_MSVC

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Decimal.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Decimal);

////////////////////////////////////////////////////////////////////////////////////////////////////

#define ONE_HUNDRED_K 100000ULL
#define ONE_MILLION 1000000ULL
#define TEN_MILLION 10000000ULL

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Decimal::copy(const Object& rhs)
{
    auto& dec = utl::cast<Decimal>(rhs);
    _n = dec._n;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Decimal::compare(const Object& rhs) const
{
    auto& dec = utl::cast<Decimal>(rhs);
    return utl::compare(_n, dec._n);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Decimal::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        String s;
        s.serializeIn(stream, mode);
        set(s);
    }
    else
    {
        toString().serializeOut(stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Decimal::toString() const
{
    return toString(6);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Decimal::toString(uint_t digits, bool round) const
{
    String res;
    static uint64_t mlookup[7] = {1, 10, 100, 1000, 10000, 100000, 1000000};
    if (digits > 6)
        digits = 6;
    bool positive = (_n >= 0);
    uint64_t n = positive ? _n : (-1 * _n);
    uint64_t w = n / ONE_MILLION;
    uint64_t f = n % ONE_MILLION;

    // first write a minus sign (if the number is negative)
    if (!positive)
        res += '-';

    // strip unwanted digits from the fractional part
    // .. (and round up if appropriate)
    uint64_t m = mlookup[6 - digits];
    uint64_t r = f % m;
    if (r != 0)
    {
        f -= r;
        if (round && (r >= (m / 2)))
        {
            f += m;
            if (f >= ONE_MILLION)
            {
                ++w;
                f = 0;
            }
        }
    }

    // finally convert whole and fractional parts to string
    res += Uint(w).toString();
    if (digits == 0)
        return res;
    res += '.';
    String fractStr = Uint(f).toString().padBegin(6, '0').subString(0, digits);
    res += fractStr;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Decimal&
Decimal::set(const String& str)
{
    // set the sign, find the dot
    bool positive = true;
    size_t startIdx = 0, dotIdx = str.find(".");
    char c = str.firstChar();
    if ((c == '-') || (c == '+'))
        startIdx = 1;
    if (c == '-')
        positive = false;

    // divide str into whole and fractional parts
    String wholeStr, fractStr;
    bool roundUp = false;
    if (dotIdx == size_t_max)
    {
        if (startIdx == 0)
            wholeStr = str;
        else
            wholeStr = str.subString(startIdx);
    }
    else
    {
        wholeStr = str.subString(startIdx, dotIdx - startIdx);
        fractStr = str.subString(dotIdx + 1);
        if (fractStr.length() < 6)
        {
            fractStr.padEnd(6, '0');
        }
        else if (fractStr.length() > 6)
        {
            if (fractStr[6] >= '5')
                roundUp = true;
            fractStr[6] = '\0';
            fractStr.lengthInvalidate();
        }
        if (fractStr.firstChar() == '0')
        {
            size_t i, lim = fractStr.length();
            for (i = 0; (i < lim) && (fractStr[i] == '0'); ++i)
                ;
            fractStr = fractStr.subString(i);
        }
    }

    // determine whole component
    uint64_t whole = Uint(wholeStr);

    // determine fractional component
    uint64_t fract = 0;
    if (!fractStr.empty())
    {
        // convert to machine integer
        fract = Uint(fractStr);

        // round up if we decided to do so earlier
        if (roundUp)
        {
            if (++fract == ONE_MILLION)
            {
                ++whole;
                fract = 0;
            }
        }
    }

    // bounds-checking
    if (whole > 9223372036854ULL)
        throw IllegalValueEx(str);
    if (whole == 9223372036854ULL)
    {
        bool ok = positive ? (fract <= 775807ULL) : (fract <= 775808ULL);
        if (!ok)
            throw IllegalValueEx(str);
    }

    // ok, now set _n
    _n = (whole * ONE_MILLION) + fract;
    if (!positive)
        _n *= -1;

    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Decimal&
Decimal::multiply(int64_t p_rhs)
{
    bool lhsPositive = (_n >= 0), rhsPositive = (p_rhs >= 0);
    bool positive = (lhsPositive == rhsPositive);
    __uint128_t lhs = lhsPositive ? _n : -1 * _n;
    __uint128_t rhs = rhsPositive ? p_rhs : -1 * p_rhs;

    // do the calculation (scale the result up 10x)
    lhs *= rhs;
    lhs /= (__uint128_t)ONE_HUNDRED_K;

    // scale back down 10x (possibly round up)
    uint_t rem = lhs % 10;
    bool roundUp = (rem >= 5);
    lhs /= 10;
    if (roundUp)
        ++lhs;

    // take 2's complement if necessary
    _n = lhs;
    if (!positive)
        _n = -1 * _n;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Decimal&
Decimal::divide(int64_t p_rhs)
{
    bool lhsPositive = (_n >= 0), rhsPositive = (p_rhs >= 0);
    bool positive = (lhsPositive == rhsPositive);
    __uint128_t lhs = lhsPositive ? _n : -1 * _n;
    __uint128_t rhs = rhsPositive ? p_rhs : -1 * p_rhs;

    // do the calculation (scaling the result up 10x for rounding)
    lhs *= (__uint128_t)TEN_MILLION;
    lhs /= rhs;

    // scale back down 10x (and possibly round up)
    uint_t rem = lhs % 10;
    bool roundUp = (rem >= 5);
    lhs /= 10;
    if (roundUp)
        ++lhs;

    // times -1 if the result is negative
    _n = lhs;
    if (!positive)
        _n = -1 * _n;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Decimal&
Decimal::mod(int64_t p_rhs)
{
    __int128_t lhs = _n;
    __int128_t rhs = p_rhs;
    lhs %= rhs;
    _n = lhs;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
