#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FDstream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Local host socket.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LocalSocket : public FDstream
{
    UTL_CLASS_DECL(LocalSocket, FDstream);

public:
    /**
       Constructor.
       \param fd socket file descriptor
    */
    LocalSocket(int fd);

    /**
       Constructor.
       \param path filesystem path
    */
    LocalSocket(const String& path);

    /**
       Open a connection to the given host address on the given port.
       \param path filesystem path
    */
    void open(const String& path);

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
