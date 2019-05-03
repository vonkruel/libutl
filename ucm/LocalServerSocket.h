#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FDstream.h>
#include <libutl/ServerSocket.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Local host server socket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LocalServerSocket : public ServerSocket
{
    UTL_CLASS_DECL(LocalServerSocket, ServerSocket);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param path filesystem path
       \param backlog queue size for unaccepted connections
    */
    LocalServerSocket(const String& path, int backlog = -1);

    virtual FDstream* makeSocket() const;

    /**
       Accept a client connection.
       \return true iff a connection was accepted
       \param socket socket connection to client
       \param clientAddr (optional) client's host address
    */
    virtual bool accept(FDstream* socket, InetHostAddress* clientAddr = nullptr);

    /**
       Listen for connections.
       \param path filesystem path
       \param backlog queue size for unaccepted connections
    */
    void open(const String& path, int backlog = -1);

    /** Set non-blocking mode. */
    void setNonBlocking();

private:
    String _path;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
