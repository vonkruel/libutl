#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <sys/un.h>
#include <libutl/LocalServerSocket.h>
#include <libutl/LocalSocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::LocalServerSocket);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

LocalServerSocket::LocalServerSocket(const String& path, int backlog)
{
    open(path, backlog);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FDstream*
LocalServerSocket::makeSocket() const
{
    return new LocalSocket();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
LocalServerSocket::accept(FDstream* socket_, InetHostAddress* clientAddr)
{
    auto& socket = utl::cast<LocalSocket>(*socket_);
    UTL_EINTR_LOOP(::accept(_fd, nullptr, nullptr));
    if (err < 0)
        return false;
    int cliFD = err;
    socket.setFD(cliFD);
    if (clientAddr != nullptr)
        clientAddr->set(127, 0, 0, 1);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LocalServerSocket::open(const String& path, int backlog)
{
    // figure out path
    ASSERTD(!path.empty());
    String exceptionString = "unix:" + path;

    int newFD;

    // close the current server socket (if any)
    close();

    // create a socket
    newFD = socket(AF_UNIX, SOCK_STREAM, 0);
    if (newFD < 0)
        errToEx(exceptionString);

    // create the address we will bind to
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    // unlink existing socket
    unlink(path);

    // try to bind
    int err = bind(newFD, (sockaddr*)&addr, sizeof(addr));
    if (err < 0)
    {
        ::close(newFD);
        errToEx(exceptionString);
    }

    // try to listen
    if (backlog < 0)
        backlog = int_t_max;
    err = listen(newFD, backlog);
    if (err < 0)
    {
        ::close(newFD);
        errToEx(exceptionString);
    }

    // switch to rw-rw-rw-
    chmod(path, 0666);

    _fd = newFD;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LocalServerSocket::setNonBlocking()
{
    if (_fd < 0)
        return;
    int flags = fcntl(_fd, F_GETFL);
    int err = fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
    if (err < 0)
    {
        errToEx();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
