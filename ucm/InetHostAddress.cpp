#include <libutl/libutl.h>
#include <libutl/win32api.h>
#include <libutl/InetHostname.h>
#include <libutl/RBtree.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::InetHostAddress);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostAddress::InetHostAddress(const InetHostname& hostName)
{
    InetHostAddress::copy(hostName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostAddress::InetHostAddress(const void* hostAddr)
{
    set(hostAddr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostAddress::InetHostAddress(uint32_t hostAddr)
{
    set(hostAddr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostAddress::InetHostAddress(uint_t b3, uint_t b2, uint_t b1, uint_t b0)
{
    set(b3, b2, b1, b0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
InetHostAddress::compare(const Object& rhs) const
{
    if (rhs.isA(InetHostAddress))
    {
        auto& ha = utl::cast<InetHostAddress>(rhs);
        if (&ha == this)
            return 0;
        return memcmp(_hostAddr, ha._hostAddr, 4);
    }
    else
    {
        auto& hn = utl::cast<InetHostname>(rhs);
        RBtree addresses;
        hn.addresses(addresses);
        return addresses.has(self) ? 0 : 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::copy(const Object& rhs)
{
    // If rhs is also a InetHostAddress
    if (rhs.isA(InetHostAddress))
    {
        // Copy rhs's members
        auto& ha = utl::cast<InetHostAddress>(rhs);
        if (&ha == this)
            return;
        clear();
        memmove(_hostAddr, ha._hostAddr, 4);
    }
    // Else if rhs is a InetHostname
    else if (rhs.isA(InetHostname))
    {
        auto& hn = utl::cast<InetHostname>(rhs);
        set(hn);
    }
    else
    {
        ABORT();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
InetHostAddress::hash(size_t size) const
{
    auto s = _hostAddr;
    auto sLim = s + 4;
    uint64_t h;
    for (h = 0; s != sLim; ++s)
    {
        h = (h << 8) | (uint64_t)(*s);
    }
    return (size_t)(h % size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::serialize(Stream& stream, uint_t io, uint_t mode)
{
    super::serialize(stream, io, mode);
    utl::serialize(_hostAddr[0], stream, io, mode);
    utl::serialize(_hostAddr[1], stream, io, mode);
    utl::serialize(_hostAddr[2], stream, io, mode);
    utl::serialize(_hostAddr[3], stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
InetHostAddress::toString() const
{
    String res;
    for (uint_t i = 0; i < 4; i++)
    {
        if (i > 0)
        {
            res += '.';
        }
        res += Uint(_hostAddr[i]).toString();
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::clear()
{
    memset(_hostAddr, 0, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t
InetHostAddress::get() const
{
    uint32_t res;
    memmove(&res, _hostAddr, sizeof(uint32_t));
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostname
InetHostAddress::getName() const
{
    return name();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
InetHostAddress::isNil() const
{
    uint32_t nilAddr = 0;
    return (memcmp(_hostAddr, &nilAddr, 4) == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostname
InetHostAddress::name() const
{
    InetHostname res;

    // Get the host address
    struct in_addr addr;
    memmove(&addr, _hostAddr, sizeof(_hostAddr));
    struct hostent* hostEnt = gethostbyaddr((char*)&_hostAddr, sizeof(_hostAddr), AF_INET);

    if (hostEnt == nullptr)
    {
        h_errToEx(this);
    }

    // set the hostname
    res.String::set(hostEnt->h_name);

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::set(const InetHostname& hostName)
{
    // Set to the host address for the host name
    clear();
    copy(hostName.address());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::set(const void* hostAddr)
{
    clear();
    memmove(_hostAddr, hostAddr, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::set(uint32_t hostAddr)
{
    clear();
    memmove(_hostAddr, (void*)&hostAddr, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::set(uint_t b3, uint_t b2, uint_t b1, uint_t b0)
{
    clear();
    _hostAddr[0] = b3;
    _hostAddr[1] = b2;
    _hostAddr[2] = b1;
    _hostAddr[3] = b0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostAddress::init()
{
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
