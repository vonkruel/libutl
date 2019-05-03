#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BufferedStream.h>
#include <libutl/SSLsocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Buffered SSL socket stream.

   BufferedSSLsocket provides buffering for a SSLsocket, and has the same external interface.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BufferedSSLsocket : public BufferedStream
{
    UTL_CLASS_DECL(BufferedSSLsocket, BufferedStream);

public:
    BufferedSSLsocket(SSLsocket* socket)
    {
        setStream(socket);
    }

    BufferedSSLsocket(const InetHostAddress& hostAddr, uint16_t port)
    {
        setStream(new SSLsocket(hostAddr, port));
    }

    void
    open(const InetHostAddress& hostAddr, uint16_t port)
    {
        pget()->open(hostAddr, port);
    }

private:
    void
    init()
    {
        setStream(new SSLsocket());
    }

    void
    deInit()
    {
    }

    const SSLsocket*
    pget() const
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<SSLsocket>(_stream);
    }

    SSLsocket*
    pget()
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<SSLsocket>(_stream);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
