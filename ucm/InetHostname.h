#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/InetHostAddress.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Collection;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Internet host name.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class InetHostname : public String
{
    UTL_CLASS_DECL(InetHostname, String);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param hostName hostname
    */
    InetHostname(const String& hostName)
        : String(hostName)
    {
    }

    /**
       Constructor.
       \param hostName hostname
    */
    InetHostname(const char* hostName)
        : String(hostName)
    {
    }

    /**
       Constructor.
       \param hostAddr host address (get its hostname)
    */
    InetHostname(const InetHostAddress& hostAddr);

    /** Return the first/primary address this hostname resolves to. */
    InetHostAddress address() const;

    /** Return the first/primary address this hostname resolves to. */
    InetHostAddress
    getAddress() const
    {
        return address();
    }

    /**
       Return all addresses this hostname resolves to.
       \param collection stores result InetHostAddress objects
    */
    void addresses(Collection* collection) const;

    /**
       Get all addresses this hostname resolves to.
       \param collection stores result InetHostAddress objects
    */
    void
    getAddresses(Collection* collection) const
    {
        addresses(collection);
    }

    /**
       Return all aliases for this hostname.
       \param collection stores result InetHostname objects
    */
    void aliases(Collection* collection) const;

    /**
       Get all aliases for this hostname.
       \param collection stores result InetHostname objects
    */
    void
    getAliases(Collection* collection) const
    {
        aliases(collection);
    }

    /** Return the name of the local host. */
    static InetHostname localInetHostname();

    /** Return the FQDN of the local host. */
    static InetHostname localFQDN();

    /** Return the domain name of the local host. */
    static String localDomainName();

    /** Set = the name for the given host address. */
    void set(const InetHostAddress& hostAddr);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
