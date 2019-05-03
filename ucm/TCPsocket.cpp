#include <libutl/libutl.h>
#include <libutl/String.h>
#include <libutl/TCPsocket.h>

#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <netinet/tcp.h>
#else
#include <libutl/win32api.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::TCPsocket);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

TCPsocket::TCPsocket(int fd)
{
    setFD(fd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TCPsocket::TCPsocket(const InetHostAddress& hostAddr, uint16_t port)
{
    open(hostAddr, port);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPsocket::checkOK()
{
    super::checkOK();

    int i_error;
    socklen_t errorSize = sizeof(i_error);
    getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char*)&i_error, &errorSize);
    if (i_error != 0)
        throwStreamErrorEx();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPsocket::open(const InetHostAddress& hostAddr, uint16_t port)
{
    int fd;

    // address must make sense
    ASSERTD(!hostAddr.isNil());

    // close any currently open connection
    close();

    // create a socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        errToEx(hostAddr.toString());
    }

    // create the address we will connect to
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = hostAddr.get();
    addr.sin_port = htons(port);

    // try to connect
    int err = connect(fd, (struct sockaddr*)&addr, sizeof(sockaddr_in));

// something went wrong?
#if UTL_HOST_TYPE == UTL_HT_UNIX
    if (err < 0)
    {
        ::close(fd);
        errToEx(hostAddr.toString());
    }
#else
    if (err == SOCKET_ERROR)
    {
        SCOPE_EXIT
        {
            ::closesocket(fd);
        };
        h_errToEx(hostAddr.toString());
    }
#endif

    setName(hostAddr);
    setFD(fd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
void
TCPsocket::setTCPnoDelay(bool noDelay_)
{
    // enable TCP_NODELAY
    int noDelay = noDelay_ ? 1 : 0;
    int err = setsockopt(_fd, SOL_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay));
    if (err < 0)
        errToEx(getNamePtr());
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPsocket::close()
{
    if (_fd < 0)
        return;
    int res = closesocket(_fd);
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
TCPsocket::read(byte_t* array, size_t maxBytes, size_t minBytes)
{
    ASSERTD(isInput());

    // check maxBytes, fix minBytes
    if (maxBytes == 0)
        return 0;
    if (minBytes > maxBytes)
        minBytes = maxBytes;

    byte_t* arrayLim = array + maxBytes;
    byte_t* arrayMin = array + minBytes;

    // read into the buffer
    byte_t* arrayPtr = array;
    do
    {
        int num = recv(static_cast<SOCKET>(_fd), reinterpret_cast<char*>(arrayPtr),
                       arrayLim - arrayPtr, 0);

        // error?
        if (num == SOCKET_ERROR)
            h_errToEx();

        // eof?
        if (num == 0)
        {
            if (arrayPtr < arrayMin)
                throwStreamEOFex();
            setEOF(true);
            break;
        }

        // we got something
        ASSERTD(num > 0);
        arrayPtr += num;
    } while (arrayPtr < arrayMin);
    return arrayPtr - array;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPsocket::write(const byte_t* array, size_t num)
{
    ASSERTD(isOutput());
    ASSERTD(num >= 0);
    const byte_t* arrayLim = array + num;
    const byte_t* cur = array;
    while (cur < arrayLim)
    {
        auto curCharPtr = const_cast<char*>(reinterpret_cast<const char*>(cur));
        int num = send(static_cast<SOCKET>(_fd), curCharPtr, arrayLim - cur, 0);

        // error?
        if (num == SOCKET_ERROR)
            h_errToEx();

        cur += num;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPsocket::setModes()
{
    super::setModes();

    // disable SO_KEEPALIVE
    int keepAlive = 0;
    int err = setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(int));
    if (err < 0)
        errToEx(getNamePtr());

    // disable SO_LINGER
    struct linger lgr;
    lgr.l_onoff = 0;
    lgr.l_linger = 0;
    err = setsockopt(_fd, SOL_SOCKET, SO_LINGER, (char*)&lgr, sizeof(lgr));
    if (err < 0)
        errToEx(getNamePtr());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TCPsocket::errToEx(const Object* object)
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    utl::errToEx(object);
#else
    utl::h_errToEx(object);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
