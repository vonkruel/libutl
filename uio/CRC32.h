#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Generate CRC-32 checksums.

   A CRC-32 is a checksum that is used to verify the integrity of data.  A matching CRC-32 does
   <b>not guarantee</b> the data is uncorrupted, but it provides a very high degree of confidence.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class CRC32 : public Object
{
    UTL_CLASS_DECL(CRC32, Object);

public:
    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual String toString() const;

    /** Update the CRC-32 code for a given byte. */
    void add(byte_t ch);

    /** Clear/reset the CRC-32 code. */
    void
    clear()
    {
        _crc = uint32_t_max;
    }

    /** Get the CRC-32 code. */
    uint_t
    get() const
    {
        return ~_crc;
    }

private:
    void
    init()
    {
        clear();
    }
    void
    deInit()
    {
    }

private:
    uint32_t _crc;
    static const uint32_t table[256];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
