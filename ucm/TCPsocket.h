#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/InetHostAddress.h>
#include <libutl/FDstream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   TCP socket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class TCPsocket : public FDstream
{
    UTL_CLASS_DECL(TCPsocket, FDstream);

public:
    /**
       Constructor.
       \param fd socket file descriptor
    */
    TCPsocket(int fd);

    /**
       Constructor.
       \param hostAddr host address
       \param port port number
    */
    TCPsocket(const InetHostAddress& hostAddr, uint16_t port);

    virtual void checkOK();

    /**
       Open a connection to the given host address on the given port.
       \param hostAddr host address
       \param port port number
    */
    void open(const InetHostAddress& hostAddr, uint16_t port);

#if UTL_HOST_TYPE == UTL_HT_UNIX
    void setTCPnoDelay(bool noDelay);
#else
    virtual void close();

    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max);

    virtual void write(const byte_t* array, size_t num);
#endif
protected:
    virtual void setModes();

private:
    void
    init()
    {
    }
    void
    deInit()
    {
        close();
    }
    void errToEx(const utl::Object* object = nullptr);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
