#include <libutl/libutl.h>
#include <libutl/win32api.h>
#include <libutl/Array.h>
#include <libutl/Collection.h>
#include <libutl/Exception.h>
#include <libutl/InetHostname.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::InetHostname);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostname::InetHostname(const InetHostAddress& hostAddr)
{
    set(hostAddr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostAddress
InetHostname::address() const
{
    InetHostAddress res;

    // Get the host address
    struct hostent* hostEnt = gethostbyname(*this);

    // throw an exception if host lookup failed
    if (hostEnt == nullptr)
    {
        h_errToEx(this);
    }

    // Set the address
    res.set(hostEnt->h_addr_list[0]);

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostname::addresses(Collection* collection) const
{
    // collection must exist and own objects
    ASSERTD(collection != nullptr);
    ASSERTD(collection->isOwner());

    // Get the host address
    struct hostent* hostEnt = gethostbyname(*this);

    // throw an exception if host lookup failed
    if (hostEnt == nullptr)
        h_errToEx(this);

    uint_t i;
    for (i = 0; hostEnt->h_addr_list[i] != nullptr; i++)
    {
        *collection += new InetHostAddress(hostEnt->h_addr_list[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostname::aliases(Collection* collection) const
{
    // collection must exist and own objects
    ASSERTD(collection != nullptr);
    ASSERTD(collection->isOwner());

    // Get the host address
    struct hostent* hostEnt = gethostbyname(*this);

    // throw an exception if host lookup failed
    if (hostEnt == nullptr)
        h_errToEx(this);

    uint_t i;
    for (i = 0; hostEnt->h_aliases[i] != nullptr; i++)
    {
        *collection += new InetHostname(hostEnt->h_aliases[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostname
InetHostname::localInetHostname()
{
    char name[64];
    int err = gethostname(name, 64);
    if (err != 0)
        h_errToEx();
    return InetHostname(name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

InetHostname
InetHostname::localFQDN()
{
    InetHostname res = localInetHostname();
    if (res.find(".") != size_t_max)
        return res;
    String domainName = localDomainName();
    if (domainName.empty())
        return res;
    res += ".";
    res += domainName;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
InetHostname::localDomainName()
{
// 1st try: getdomainname()
#if UTL_HOST_TYPE == UTL_HT_UNIX
    char domain[64];
    int err = getdomainname(domain, 64);
    if (err != 0)
        h_errToEx();
    if (String(domain, false) != "(none)")
        return domain;
#endif

    // 2nd try: gethostbyname
    InetHostname name = localInetHostname();
    struct hostent* hostEnt = gethostbyname(name);
    if (hostEnt == nullptr)
        h_errToEx();
    name = hostEnt->h_name;
    size_t dotIdx = name.find(".");
    if (dotIdx != size_t_max)
    {
        return name.subString(dotIdx + 1);
    }

    // failed .. just return empty string
    return String();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
InetHostname::set(const InetHostAddress& hostAddr)
{
    copy(hostAddr.name());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
