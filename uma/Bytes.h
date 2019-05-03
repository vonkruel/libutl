#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Number of bytes.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Bytes : public Uint
{
    UTL_CLASS_DECL(Bytes, Uint);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param bytes number of bytes
    */
    Bytes(uint64_t bytes)
    {
        set(bytes);
    }

    /**
       Constructor.
       \param str format string
    */
    Bytes(const String& str)
    {
        set(str);
    }

    String toString() const;

    double
    kilobytes() const
    {
        return ((double)_n / KB(1));
    }

    double
    megabytes() const
    {
        return ((double)_n / MB(1));
    }

    double
    gigabytes() const
    {
        return ((double)_n / MB(1024));
    }

public:
    /** One kilobyte. */
    static const Bytes oneKilobyte;
    /** One megabyte. */
    static const Bytes oneMegabyte;
    /** One gigabyte. */
    static const Bytes oneGigabyte;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
