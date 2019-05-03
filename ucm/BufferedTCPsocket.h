#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BufferedFDstream.h>
#include <libutl/TCPsocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Buffered TCP socket stream.

   BufferedTCPsocket provides buffering for a TCPsocket, and has the same external interface as
   TCPsocket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BufferedTCPsocket : public BufferedFDstream
{
    UTL_CLASS_DECL(BufferedTCPsocket, BufferedFDstream);

public:
    BufferedTCPsocket(TCPsocket* socket)
    {
        setStream(socket);
    }

    BufferedTCPsocket(int fd)
    {
        setStream(new TCPsocket(fd));
    }

    BufferedTCPsocket(const InetHostAddress& hostAddr, uint16_t port)
    {
        setStream(new TCPsocket(hostAddr, port));
    }

    void
    open(const InetHostAddress& hostAddr, uint16_t port)
    {
        pget()->open(hostAddr, port);
        setBufs();
    }

private:
    void
    init()
    {
        setStream(new TCPsocket());
    }
    void
    deInit()
    {
    }

    const TCPsocket*
    pget() const
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<TCPsocket>(_stream);
    }
    TCPsocket*
    pget()
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<TCPsocket>(_stream);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
