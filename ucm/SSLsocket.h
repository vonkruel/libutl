#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/InetHostAddress.h>
#include <libutl/Stream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
    struct bio_st;
    struct ssl_st;
    typedef bio_st BIO;
    typedef ssl_st SSL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Secure socket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SSLsocket : public Stream
{
    UTL_CLASS_DECL(SSLsocket, Stream);

public:
    /**
       Constructor.
       \param hostAddr host address
       \param port port number
    */
    SSLsocket(const InetHostAddress& hostAddr, uint16_t port);

    /**
       Open a connection to the given host address on the given port.
       \param hostAddr host address
       \param port port number
    */
    void open(const InetHostAddress& hostAddr, uint16_t port);

    virtual void close();

    /**
       Did the certificate pass verification?
       The connection can still be used even if the certificate can't be trusted.
    */
    bool certificateOK() const;

    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max);

    virtual void write(const byte_t* array, size_t num);

private:
    void
    init()
    {
        _bio = nullptr;
    }
    void
    deInit()
    {
        close();
    }

private:
    BIO* _bio;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
