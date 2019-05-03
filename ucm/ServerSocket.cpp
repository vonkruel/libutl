#include <libutl/libutl.h>
#include <libutl/ServerSocket.h>
#include <libutl/Uint.h>

#if UTL_HOST_OS == UTL_OS_WINDOWS
#include <libutl/win32api.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::ServerSocket);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
ServerSocket::compare(const Object& rhs) const
{
    auto& ss = utl::cast<ServerSocket>(rhs);
    if (rhs.isA(Uint))
    {
        auto& rhsFD = utl::cast<Uint>(rhs);
        return utl::compare(_fd, (int)rhsFD.get());
    }
    else
    {
        return utl::compare(_fd, ss._fd);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ServerSocket::close()
{
    if (_fd < 0)
        return;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    ASSERTFNZ(::close(_fd));
#else
    ASSERTFNZ(::closesocket(_fd));
#endif
    _fd = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ServerSocket::setNonBlocking()
{
    if (_fd < 0)
        return;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    int flags = fcntl(_fd, F_GETFL);
    int err = fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
    if (err < 0)
        errToEx();
#else
    ABORT();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
