#include <libutl/libutl.h>
#include <libutl/win32api.h>
#include <libutl/Exception.h>
#include <libutl/InetHostname.h>
#include <libutl/Thread.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::Exception);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
errToEx(const Object* object_)
{
    Object* object = utl::clone(object_);
    switch (errno)
    {
    case 0:
        return;
    case EACCES:
        throw AccessDeniedEx(object);
        break;
    case EEXIST:
        throw FileExistsEx(object);
        break;
    case ENOENT:
        throw FileNotFoundEx(object);
        break;
    case EINTR:
        throw InterruptedSyscallEx(object);
        break;
    case EINVAL:
        throw InvalidAddressEx(object);
        break;
    case ENOTDIR:
        throw NonDirectoryComponentEx(object);
        break;
#if UTL_HOST_TYPE == UTL_HT_UNIX
    case ECONNREFUSED:
        throw ConnectionRefusedEx(object);
        break;
    case EADDRINUSE:
        throw AddressInUseEx(object);
        break;
    case EADDRNOTAVAIL:
        throw AddressNotAvailableEx(object);
        break;
    case ENETUNREACH:
        throw NetworkUnreachableEx(object);
        break;
    case ECONNRESET:
        throw ConnectionResetEx(object);
        break;
#endif
    default:
        throw UnknownHostOSEx(object);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
h_errToEx(const Object* object_)
{
    Object* object = utl::clone(object_);
    switch (h_errno)
    {
#if UTL_HOST_TYPE == UTL_HT_UNIX
    // gethostbyname() and friends
    case TRY_AGAIN:
    case NO_RECOVERY:
        throw HostLookupEx(object);
        break;
    case HOST_NOT_FOUND:
        throw HostNotFoundEx(object);
        break;
    case NO_ADDRESS:
        throw HostNoAddressEx(object);
        break;
#else
    // gethostbyname() and friends
    case WSATRY_AGAIN:
    case WSANO_RECOVERY:
        throw HostLookupEx(object);
        break;
    case WSAHOST_NOT_FOUND:
        throw HostNotFoundEx(object);
        break;
    case WSANO_DATA:
        throw HostNoAddressEx(object);
        break;

    // other winsock2
    case WSAEADDRINUSE:
        throw AddressInUseEx(object);
        break;
    case WSAEADDRNOTAVAIL:
        throw AddressNotAvailableEx(object);
        break;
    case WSAECONNREFUSED:
        throw ConnectionRefusedEx(object);
        break;
    case WSAEHOSTUNREACH:
    case WSAENETUNREACH:
        throw NetworkUnreachableEx(object);
        break;
    case WSAECONNRESET:
        throw ConnectionResetEx(object);
        break;
#endif
    default:
        throw UnknownHostOSEx(object);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Exception::Exception(const char* name, Object* object)
{
    init();
    setName(name);
    setObject(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Exception::Exception(const char* name, const Object& object)
{
    init();
    setName(name);
    setObject(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::copy(const Object& rhs)
{
    auto& ex = utl::cast<Exception>(rhs);
    const String* name = ex.namePtr();
    setName((name == nullptr) ? nullptr : name->get());
    setObject(utl::clone(ex._object));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::dump(Stream& os) const
{
    if (_name == nullptr)
    {
        os << "<exception>";
    }
    else
    {
        os << *_name;
    }
    if (_object != nullptr)
    {
        os << ": ";
        os << *_object;
    }
    os << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::setName(const char* name)
{
    delete _name;
    if (name == nullptr)
    {
        _name = nullptr;
    }
    else
    {
        _name = new String(name, false, false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::setObject(Object* object)
{
    delete _object;
    _object = object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::setObject(const Object& object)
{
    delete _object;
    _object = object.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::init()
{
    _name = nullptr;
    _object = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Exception::deInit()
{
    setName(nullptr);
    setObject(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_EXCEPTION_IMPL(utl, HostOSEx, utl::Exception, "unknown host o/s exception");
UTL_EXCEPTION_IMPL(utl, AccessDeniedEx, utl::HostOSEx, "access denied");
UTL_EXCEPTION_IMPL(utl, FileExistsEx, utl::HostOSEx, "file exists");
UTL_EXCEPTION_IMPL(utl, FileNotFoundEx, utl::HostOSEx, "file not found");
UTL_EXCEPTION_IMPL(utl, InterruptedSyscallEx, utl::HostOSEx, "interrupted system call");
UTL_EXCEPTION_IMPL(utl, InvalidAddressEx, utl::HostOSEx, "invalid address/descriptor");
UTL_EXCEPTION_IMPL(utl, NonDirectoryComponentEx, utl::HostOSEx, "non-directory component");
UTL_EXCEPTION_IMPL(utl, UnknownHostOSEx, utl::HostOSEx, "unknown host o/s exception");

// UTL++ exceptions
UTL_EXCEPTION_IMPL(utl, StreamEx, utl::Exception, "stream exception");
UTL_EXCEPTION_IMPL(utl, StreamEOFex, utl::StreamEx, "stream EOF");
UTL_EXCEPTION_IMPL(utl, StreamErrorEx, utl::StreamEx, "stream i/o error");
UTL_EXCEPTION_IMPL(utl, StreamSerializeEx, utl::StreamEx, "serialization error");

// network exceptions
UTL_EXCEPTION_IMPL(utl, NetworkEx, utl::HostOSEx, "network access exception");
UTL_EXCEPTION_IMPL(utl, ConnectionRefusedEx, utl::NetworkEx, "connection refused");
UTL_EXCEPTION_IMPL(utl, AddressInUseEx, utl::NetworkEx, "address in use");
UTL_EXCEPTION_IMPL(utl, AddressNotAvailableEx, utl::NetworkEx, "address not available");
UTL_EXCEPTION_IMPL(utl, NetworkUnreachableEx, utl::NetworkEx, "network unreachable");
UTL_EXCEPTION_IMPL(utl, ConnectionResetEx, utl::NetworkEx, "connection reset by peer");
UTL_EXCEPTION_IMPL(utl, HostLookupEx, utl::NetworkEx, "host lookup failed");
UTL_EXCEPTION_IMPL(utl, HostNoAddressEx, utl::NetworkEx, "host has no address");
UTL_EXCEPTION_IMPL(utl, HostNotFoundEx, utl::NetworkEx, "host not found");

// misc exceptions
UTL_EXCEPTION_IMPL(utl, IllegalValueEx, utl::Exception, "illegal value");
UTL_EXCEPTION_IMPL(utl, UnknownEx, utl::Exception, "unknown exception");
