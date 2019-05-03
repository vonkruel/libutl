#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef _A
#undef _B
#undef _C
#undef _D

////////////////////////////////////////////////////////////////////////////////////////////////////
// MD5sum //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Store MD5 checksum.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class MD5sum : public Object
{
    UTL_CLASS_DECL(MD5sum, Object);

public:
    MD5sum(const String& str)
    {
        set(str);
    }

    MD5sum(const uint32_t* sum)
    {
        memcpy(_sum, sum, 16);
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual size_t hash(size_t size) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    void set(const String& str);

    virtual String toString() const;

private:
    void
    init()
    {
        memset(_sum, 0, 8);
    }
    void
    deInit()
    {
    }

private:
    uint32_t _sum[4];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// MD5 /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Generate MD5 checksums.

   MD5 is a 128-bit checksum that is used to verify that data has not been accidentally corrupted.
   MD5 isn't secure, so use something else like SHA-2 (e.g. SHA256) if you want a secure hash.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class MD5 : public Object
{
    UTL_CLASS_DECL(MD5, Object);

public:
    virtual void copy(const Object& rhs);

    /** Add the given character. */
    void add(byte_t ch);

    /** Add the given array. */
    void add(const byte_t* array, size_t len);

    /** Reset to initial state. */
    void clear();

    /** Get the 128-bit (16 byte) MD-5 sum. */
    MD5sum get() const;

    /** Compute MD5 for a stream. */
    static MD5sum compute(Stream& is);

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
    void finish();
    void doBlock(const byte_t* buf, size_t len);
    static inline uint32_t revBE(uint32_t w);
    static inline uint32_t revLE(uint32_t w);

private:
    uint32_t _A;
    uint32_t _B;
    uint32_t _C;
    uint32_t _D;
    uint32_t _len[2];
    uint32_t _bufLen;
    byte_t _buf[128];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
