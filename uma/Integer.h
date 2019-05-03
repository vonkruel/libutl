#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Number.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Integer value.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class Integer : public Number<T>
{
    UTL_CLASS_DECL_TPL(Integer, T, Number<T>);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param i initial value
    */
    Integer(T i)
        : Number<T>(i)
    {
    }

    virtual size_t hash(size_t size) const;

    /** Get the result of \b self modulo \b rhs. */
    Integer<T>
    mod(const Integer<T>& rhs) const
    {
        return Integer<T>(this->_n % rhs._n);
    }

    /** Determine whether self is divisible by the given number. */
    bool
    isDivisibleBy(T num) const
    {
        return utl::isDivisibleBy(this->_n, num);
    }

    /**
       Determine the smallest number \b n, s.t. \b n is a multiple of \b self,
       and \c n >= \b target.
    */
    inline Integer<T>
    nextMultiple(T target) const
    {
        return Integer<T>(utl::nextMultiple(this->_n, target));
    }

    void
    set(T n)
    {
        super::set(n);
    }

    /** Set from a decimal string representation. */
    virtual Number<T>& set(const String& str);

    /** Set from a binary string representation. */
    Integer<T>& setBinary(const String& str);

    /** Set from a hexadecimal string representation. */
    Integer<T>& setHex(const String& str);

    /**
       Get a binary string representation of the number.
       \return binary string representation
       \param numDigits number of digits
    */
    String toBinary(uint_t numDigits = 0) const;

    /**
       Get octal representation of the number.
       \return binary string representation
       \param numDigits number of digits
    */
    String toOctal(uint_t numDigits = 0) const;

    /**
       Get a binary string representation of the number.
       \return binary string representation
       \param numDigits number of digits
    */
    String toDecimal(uint_t numDigits = 0) const;

    /**
       Get a hexadecimal string representation of the number.
       \return hex string representation
       \param numDigits number of digits
    */
    String toHex(uint_t numDigits = 0) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
size_t
Integer<T>::hash(size_t size) const
{
    return (size_t)(this->_n % size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Number<T>&
Integer<T>::set(const String& str)
{
    char c = str[0];
    if (!isdigit(c) && (c != '-') && (c != '+'))
        goto fail;
    if (std::is_signed<T>())
    {
        this->_n = strtol(str, nullptr, 0);
    }
    else
    {
        this->_n = strtoul(str, nullptr, 0);
    }
    if (errno == EINVAL)
        goto fail;
    return self;
fail:
    this->_n = 0;
    throw IllegalValueEx(str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Integer<T>&
Integer<T>::setBinary(const String& str)
{
    if (!isdigit(str[0]))
        goto fail;
    if (std::is_signed<T>())
    {
        this->_n = strtol(str, nullptr, 2);
    }
    else
    {
        this->_n = strtoul(str, nullptr, 2);
    }
    if (errno == EINVAL)
        goto fail;
    return self;
fail:
    this->_n = 0;
    throw IllegalValueEx(str);
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Integer<T>&
Integer<T>::setHex(const String& str)
{
    if (!isxdigit(str[0]))
        goto fail;
    if (std::is_signed<T>())
    {
        this->_n = strtol(str, nullptr, 16);
    }
    else
    {
        this->_n = strtoul(str, nullptr, 16);
    }
    if (errno == EINVAL)
        goto fail;
    return self;
fail:
    this->_n = 0;
    throw IllegalValueEx(str);
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
String
Integer<T>::toBinary(uint_t numDigits) const
{
    uint64_t n = (this->_n < 0) ? -1 * this->_n : this->_n;
    if (numDigits == 0)
    {
        numDigits = 64;
        while (((n & (1ULL << (numDigits - 1))) == 0) && (numDigits > 1))
        {
            --numDigits;
        }
    }

    char* res = new char[numDigits + 2];
    char* resChar = res;
    if (this->_n < 0)
    {
        *(resChar++) = '-';
    }
    uint64_t mask = ((uint64_t)1 << (numDigits - 1));
    while (mask != 0)
    {
        if ((n & mask) == 0)
        {
            *resChar++ = '1';
        }
        else
        {
            *resChar++ = '0';
        }
        mask >>= 1;
    }
    *resChar = '\0';
    ASSERTD((resChar + 1 - res) <= (numDigits + 2));
    return String(res, true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
String
Integer<T>::toDecimal(uint_t numDigits) const
{
    if (numDigits == 0)
        return this->toString();
    String fmt = "%0*";
    fmt += Printf<T>::formatType;
    char buf[64];
    sprintf(buf, fmt, (int)numDigits, this->_n);
    return buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
String
Integer<T>::toOctal(uint_t numDigits) const
{
    String fmt = "%";
    if (numDigits == 0)
    {
        fmt += Printf<T>::formatLength;
        fmt += 'o';
        return this->toString(fmt);
    }
    fmt += "0*";
    fmt += Printf<T>::formatLength;
    fmt += 'o';
    char buf[64];
    sprintf(buf, fmt, (int)numDigits, this->_n);
    return buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
String
Integer<T>::toHex(uint_t numDigits) const
{
    String fmt = "%";
    if (numDigits == 0)
    {
        fmt += Printf<T>::formatLength;
        fmt += 'x';
        return this->toString(fmt);
    }
    fmt += "0*";
    fmt += Printf<T>::formatLength;
    fmt += 'x';
    char buf[64];
    sprintf(buf, fmt, (int)numDigits, this->_n);
    return buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::Integer, T);
