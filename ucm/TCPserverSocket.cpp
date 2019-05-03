#include <libutl/libutl.h>
#include <libutl/win32api.h>
#include <libutl/FDstream.h>
#include <libutl/InetHostname.h>
#include <libutl/String.h>
#include <libutl/Uint.h>
#include <libutl/TCPserverSocket.h>
#include <libutl/TCPsocket.h>

#if UTL_HOST_OS == UTL_OS_WINDOWS
#include <libutl/win32api.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::TCPserverSocket);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

TCPserverSocket::TCPserverSocket(const InetHostAddress* hostAddr, uint16_t port, int backlog)
{
    open(hostAddr, port, backlog);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FDstream*
TCPserverSocket::makeSocket() const
{
    return new TCPsocket();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
TCPserverSocket::accept(FDstream* socket, InetHostAddress* clientAddr)
{
    struct sockaddr_in hostAddr;
    socklen_t hostAddrLen = sizeof(hostAddr);
    UTL_EINTR_LOOP(::accept(_fd, (sockaddr*)&hostAddr, &hostAddrLen));
    if (err < 0)
        return false;
    int cliFD = err;
    socket->setFD(cliFD);
    if (clientAddr != nullptr)
        clientAddr->set(&hostAddr.sin_addr);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPserverSocket::open(const InetHostAddress* hostAddr, uint16_t port, int backlog)
{
    // figure out host address
    ASSERTD(hostAddr != nullptr);
    _hostAddr = *hostAddr;
    if (_hostAddr.isNil())
        _hostAddr.set(InetHostname::localInetHostname());

    String exceptionString = _hostAddr.toString() + ":" + Uint(port).toString();

    int newFD;

    // close the current server socket (if any)
    close();

    // set the new port
    _port = port;

    // create a socket
    newFD = socket(AF_INET, SOCK_STREAM, 0);
    if (newFD < 0)
        errToEx(exceptionString);

    // create the address we will bind to
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = _hostAddr.get();
    addr.sin_port = htons(_port);

    // try to bind
    int reuseAddr = 1;
    setsockopt(newFD, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddr, sizeof(int));
    int err = bind(newFD, (sockaddr*)&addr, sizeof(addr));
    if (err < 0)
        goto error;

    // try to listen
    if (backlog < 0)
        backlog = int_t_max;
    err = listen(newFD, backlog);
    if (err < 0)
        goto error;

    // success
    _fd = newFD;
    return;
error:
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(::close(newFD));
#else
    ASSERTFNZ(::closesocket(newFD));
#endif
    errToEx(exceptionString);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
