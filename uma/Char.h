#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Integer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Character value.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Char : public Integer<char>
{
    UTL_CLASS_DECL(Char, Integer<char>);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param val initial value
    */
    Char(char val)
        : Integer<char>(val)
    {
    }

    /**
       Constructor.
       \param val initial value
    */
    Char(int val)
        : Integer<char>((char)val)
    {
    }

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Determine whether character is alpha-numeric: [a-zA-Z0-9]. */
    bool
    isAlnum() const
    {
        return isalnum(_n);
    }

    /** Determine whether character is alphabetic: [a-zA-Z]. */
    bool
    isAlpha() const
    {
        return isalpha(_n);
    }

    /** Determine whether character is 7-bit ASCII. */
    bool
    isAscii() const
    {
        return isascii(_n);
    }

    /** Determine whether character is a control character. */
    bool
    isCntrl() const
    {
        return iscntrl(_n);
    }

    /** Determine whether character is a digit: [0-9]. */
    bool
    isDigit() const
    {
        return isdigit(_n);
    }

    /** Determine whether character is printable (excluding space). */
    bool
    isGraph() const
    {
        return isgraph(_n);
    }

    /** Determine whether character is lowercase: [a-z]. */
    bool
    isLower() const
    {
        return islower(_n);
    }

    /** Determine whether character is printable (including space). */
    bool
    isPrint() const
    {
        return isprint(_n);
    }

    /**
       Determine whether character is printable, but not a space or
       alpha-numeric.
    */
    bool
    isPunct() const
    {
        return ispunct(_n);
    }

    /** Determine whether character is white-space. */
    bool
    isSpace() const
    {
        return isspace(_n);
    }

    /** Determine whether character is upper-case: [A-Z]. */
    bool
    isUpper() const
    {
        return isupper(_n);
    }

    /** Determine whether character is a hex digit: [0-9a-fA-F]. */
    bool
    isHexDigit() const
    {
        return isxdigit(_n);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
