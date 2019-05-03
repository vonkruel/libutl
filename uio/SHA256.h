#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class SHA256sum;

////////////////////////////////////////////////////////////////////////////////////////////////////
// SHA256 //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compute SHA256 cryptographic hash.

   SHA256 is part of the SHA-2 family of secure hashes created by the NSA.  SHA256 is believed
   to be much more secure than SHA1.  "Secure" means it's nearly impossible to figure out what data
   to put in a message to make the machinery arrive at a particular result.

   This implementation is reasonably efficient.  It could be faster, but I don't know how.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SHA256 : public Object
{
    UTL_CLASS_DECL(SHA256, Object);
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

public:
    /** Reset to initial state. */
    void clear();

    /** Process the provided data. */
    void process(const byte_t* data, size_t dataLen);

    /** Get the 256-bit hash. */
    void finalize(SHA256sum& sum);

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
    void transform(const byte_t* data, size_t numBlocks = 1);

private:
    byte_t _data[64];
    byte_t* _dataPtr;
    byte_t* _dataLim;
    uint64_t _bitLen;
    uint32_t _h[8];
    static uint32_t k[64];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SHA256sum ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Store a SHA256 sum.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SHA256sum : public Object
{
    UTL_CLASS_DECL(SHA256sum, Object);
    UTL_CLASS_DEFID;

public:
    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual size_t hash(size_t size) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode);

    /** Set from the internal state of the SHA256 encoder. */
    void set(const uint32_t* hash);

    /** Set from a string representation (64 hexadecimal digits). */
    void set(const String& str);

    /** Provide string representation as 64 hexadecimal digits. */
    virtual String toString() const;

private:
    uint32_t _h[8];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
