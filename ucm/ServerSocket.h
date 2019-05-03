#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FDstream.h>
#include <libutl/InetHostAddress.h>
#include <libutl/ServerSocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Local host server socket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ServerSocket : public Object
{
    UTL_CLASS_DECL_ABC(ServerSocket, Object);
    UTL_CLASS_NO_COPY;

public:
    virtual int compare(const Object& rhs) const;

    /** Create a utl::FDstream-derived socket suitable for passing to accept. */
    virtual FDstream* makeSocket() const = 0;

    /**
       Accept a client connection.
       \return true iff a connection was accepted
       \param socket socket connection to client
       \param clientAddr (optional) client address
    */
    virtual bool accept(FDstream* socket, InetHostAddress* clientAddr = nullptr) = 0;

    /** Get the file descriptor. */
    int
    fd() const
    {
        return _fd;
    }

    /** Stop listening for connections. */
    void close();

    /** Set non-blocking mode. */
    void setNonBlocking();

protected:
    int _fd;

private:
    void
    init()
    {
        _fd = -1;
    }
    void
    deInit()
    {
        close();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
