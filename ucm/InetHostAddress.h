#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Exception.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class InetHostname;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Internet host address.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class InetHostAddress : public Object
{
    UTL_CLASS_DECL(InetHostAddress, Object);

public:
    /**
       Constructor.
       \param hostName to get address for
    */
    InetHostAddress(const InetHostname& hostName);

    /**
       Constructor.
       \param hostAddr host address (big-endian)
    */
    InetHostAddress(const void* hostAddr);

    /**
       Constructor.
       \param hostAddr 32-bit host address
    */
    InetHostAddress(uint32_t hostAddr);

    /**
       Constructor.
       \param b3 byte 3 of host address (MSB)
       \param b2 byte 2 of host address
       \param b1 byte 1 of host address
       \param b0 byte 0 of host address (LSB)
    */
    InetHostAddress(uint_t b3, uint_t b2, uint_t b1, uint_t b0);

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual size_t hash(size_t size) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Return a String representation (ie "127.0.0.1"). */
    virtual String toString() const;

    /** Clear the address (zero it out). */
    void clear();

    /** Get the address. */
    uint32_t get() const;

    /** Return the name of the host with self's address. */
    InetHostname getName() const;

    /** Determine whether the address is defined. */
    bool isNil() const;

    /** Return the name of the host with self's address. */
    InetHostname name() const;

    /** Set to the address of the given host. */
    void set(const InetHostname& hostName);

    /**
       Set the address.
       \param hostAddr host address (big-endian)
    */
    void set(const void* hostAddr);

    /**
       Set the address.
       \param hostAddr 32-bit host address
    */
    void set(uint32_t hostAddr);

    /**
       Set the address.
       \param b3 byte 3 of host address (MSB)
       \param b2 byte 2 of host address
       \param b1 byte 1 of host address
       \param b0 byte 0 of host address (LSB)
    */
    void set(uint_t b3, uint_t b2, uint_t b1, uint_t b0);

    /** Clear the address (zero it out). */
    void
    setNil()
    {
        clear();
    }

private:
    void init();

    void
    deInit()
    {
    }

private:
    byte_t _hostAddr[4];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/InetHostname.h>
