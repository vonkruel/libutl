#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FDstream.h>
#include <libutl/ServerSocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   TCP server socket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class TCPserverSocket : public ServerSocket
{
    UTL_CLASS_DECL(TCPserverSocket, ServerSocket);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_DEFID;

public:
    /**
       Constructor
       \param hostAddr inet address to bind to
       \param port listen port
       \param backlog queue size for unaccepted connections
    */
    TCPserverSocket(const InetHostAddress* hostAddr, uint16_t port, int backlog = -1);

    virtual FDstream* makeSocket() const;

    /**
       Accept a client connection.
       \return true iff a connection was accepted
       \param socket socket connection to client
       \param clientAddr (optional) client address
    */
    virtual bool accept(FDstream* socket, InetHostAddress* clientAddr = nullptr);

    /**
       Listen for connections.
       \param hostAddr inet address to bind to
       \param port port to listen on
       \param backlog queue size for unaccepted connections
    */
    void open(const InetHostAddress* hostAddr, uint16_t port, int backlog = -1);

private:
    InetHostAddress _hostAddr;
    uint16_t _port;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
