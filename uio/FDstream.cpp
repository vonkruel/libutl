#include <libutl/libutl.h>
#include <time.h>
#include <libutl/FDstream.h>
#include <libutl/Thread.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <poll.h>
#include <sys/socket.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::FDstream);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

FDstream::FDstream(int fd, uint_t mode)
{
    init();
    setFD(fd, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FDstream::close()
{
    if (_fd >= 0)
    {
        ASSERTFNZ(::close(_fd));
        clear();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
bool
FDstream::isSocket() const
{
    int val;
    socklen_t len = sizeof(val);
    return (getsockopt(_fd, SOL_SOCKET, SO_TYPE, &val, &len) == 0);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
FDstream::isTTY() const
{
    return isatty(_fd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FDstream::setFD(int fd, uint_t mode)
{
    SCOPE_FAIL
    {
        clear();
    };
    close();
    _fd = fd;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    int flags = fcntl(_fd, F_GETFL);
    setFlag(flg_nonBlocking, flags & O_NONBLOCK);
#endif
    setMode(mode);
    setModes();
    setError(false);
    checkOK();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
FDstream::read(byte_t* array, size_t maxBytes, size_t minBytes)
{
    ASSERTD(isInput());

    // check maxBytes, fix minBytes
    if (maxBytes == 0)
        return 0;
    if (minBytes > maxBytes)
        minBytes = maxBytes;

    checkOK();
    byte_t* arrayPtr = array;
    byte_t* arrayMin = array + minBytes;
    byte_t* arrayLim = array + maxBytes;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    if (this->blockingIO())
    {
#endif
        do
        {
            // try to read
            ssize_t num = ::read(_fd, arrayPtr, arrayLim - arrayPtr);

            // an error?
            if (num < 0)
            {
                // interrupted system call -> just try again
                if (errno == EINTR)
                    continue;

                // we're not expecting EAGAIN or EWOULDBLOCK
                ASSERTD(errno != EAGAIN);
                ASSERTD(errno != EWOULDBLOCK);

                // something went wrong...
                throwStreamErrorEx();
            }
            else if (num == 0)
            {
                if (arrayPtr < arrayMin)
                    throwStreamEOFex();
                setEOF(true);
                break;
            }
            else // num > 0
            {
                arrayPtr += num;
            }
        } while (arrayPtr < arrayMin);
#if UTL_HOST_TYPE == UTL_HT_UNIX
    }
    else
    {
        do
        {
            // block until we can read
            blockRead();

            // try to read
            ssize_t num = ::read(_fd, arrayPtr, arrayLim - arrayPtr);

            // an error?
            if (num < 0)
            {
                // interrupted system call -> just try again
                if (errno == EINTR)
                    continue;

                // the call would block?
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    // try again
                    continue;
                }

                // something went wrong...
                throwStreamErrorEx();
            }
            else if (num == 0)
            {
                if (arrayPtr < arrayMin)
                    throwStreamEOFex();
                setEOF(true);
                break;
            }
            else
            {
                arrayPtr += num;
            }
        } while (arrayPtr < arrayMin);
    }
#endif
    return (arrayPtr - array);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FDstream::write(const byte_t* array, size_t num)
{
    ASSERTD(isOutput());
    checkOK();
    const byte_t* arrayPtr = array;
    const byte_t* arrayLim = array + num;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    bool nonBlock = !this->blockingIO();
#endif
    while (arrayPtr < arrayLim)
    {
#if UTL_HOST_TYPE == UTL_HT_UNIX
        // block until we can write
        if (nonBlock)
            blockWrite();
#endif

        // try to write
        ssize_t num = ::write(_fd, arrayPtr, arrayLim - arrayPtr);

        // we wrote something?
        if (num > 0)
        {
            arrayPtr += num;
        }
        else
        {
            // write(2) should not ever return 0
            ASSERTD(num < 0);

            // interrupted system call -> just try again
            if (errno == EINTR)
                continue;

// this shouldn't happen normally (or at all?)
#if UTL_HOST_TYPE == UTL_HT_UNIX
            if (nonBlock && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                continue;
#endif

            // something went wrong...
            throwStreamErrorEx();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
void
FDstream::setBlockingIO(bool v)
{
    if (v == this->blockingIO())
        return;
    int flags = fcntl(_fd, F_GETFL);
    int newFlags = v ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    int err = fcntl(_fd, F_SETFL, newFlags);
    if (err < 0)
        errToEx(getNamePtr());
    setFlag(flg_nonBlocking, !v);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
bool
FDstream::blockRead(uint32_t usec)
{
    struct pollfd pfd;
    pfd.fd = _fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    bool forever = (usec == 0);
    int timeout = forever ? 250 : (int)(usec / 1000);
    do
    {
        UTL_EINTR_LOOP(poll(&pfd, 1, timeout));
        Thread::testCancel();
        if (err == 1)
            return true;
    } while (forever);
    return false;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
bool
FDstream::blockWrite(uint32_t usec)
{
    struct pollfd pfd;
    pfd.fd = _fd;
    pfd.events = POLLOUT | POLLERR | POLLHUP;
    pfd.revents = 0;
    bool forever = (usec == 0);
    int timeout = forever ? 250 : (int)(usec / 1000);
    do
    {
        UTL_EINTR_LOOP(poll(&pfd, 1, timeout));
        Thread::testCancel();
        if (err == 1)
            return true;
    } while (forever);
    return false;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FDstream::clear()
{
    super::clear();
    _fd = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FDstream::setModes()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
