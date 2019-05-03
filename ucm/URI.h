#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Array;
class BitArray;
class Collection;
class RBtree;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Generic URI (Universal Resource Identifier).

   The various components of a URI are illustrated by example:

   \code
   URI uri("http://user:pass@xyz.com:80/path/to/res.txt?var=val#fragment");
   assert(uri.scheme() == "http");
   assert(uri.username() == "user");
   assert(uri.password() == "pass");
   assert(uri.hostname() == "xyz.com");
   assert(uri.port() == 80);
   assert(uri.path() == "/path/to/";
   assert(uri.filename() == "res");
   assert(uri.extension() == "txt");
   assert(uri.segment(1) == "path");
   assert(uri.segment(2) == "to");
   assert(uri.segment(3) == "resource");
   assert(uri.query() == "var=val");
   assert(uri.queryVar(1) == "var");
   assert(uri.queryVal(1) == "val");
   assert(uri.fragment() == "fragment");
   \endcode

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class URI : public Object
{
    UTL_CLASS_DECL(URI, Object);

public:
    /**
       Constructor.
       \param uri URI
    */
    URI(const String& uri)
    {
        init();
        set(uri);
    }

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    virtual int compare(const Object& rhs) const;

    virtual String toString() const;

    virtual size_t innerAllocatedSize() const;

    /// \name Getters
    //@{
    /** Relative URI? */
    bool
    isRelative() const
    {
        return (_scheme == nullptr);
    }

    /** Get the complete (percent-encoded) URI. */
    String get() const;

    /** Get the scheme. */
    const String&
    scheme() const
    {
        return (_scheme == nullptr) ? emptyString : *_scheme;
    }

    /** Get the authority string. */
    String authority() const;

    /** Get the userinfo string. */
    String userinfo() const;

    /** Get the username. */
    const String&
    username() const
    {
        return (_username == nullptr) ? emptyString : *_username;
    }

    /** Get the password. */
    const String&
    password() const
    {
        return (_password == nullptr) ? emptyString : *_password;
    }

    /** Get the hostname. */
    const String&
    hostname() const
    {
        return (_hostname == nullptr) ? emptyString : *_hostname;
    }

    /** Get the port. */
    uint_t
    port() const
    {
        return _port;
    }

    /** Get the full path (path + filename + extension). */
    String fullPath() const;

    /** Get the path. */
    const String&
    path() const
    {
        return (_path == nullptr) ? emptyString : *_path;
    }

    /** Get the full filename (filename + extension). */
    String fullFilename() const;

    /** Get the filename. */
    const String&
    filename() const
    {
        return (_filename == nullptr) ? emptyString : *_filename;
    }

    /** Get the extension. */
    const String&
    extension() const
    {
        return (_extension == nullptr) ? emptyString : *_extension;
    }

    /** Get the number of segments in the path. */
    size_t numSegments() const;

    /**
       Get the (id)th segment of the path.
       For example, if the path is <code>/foo/bar/</code>
       then segment 1 is <code>foo</code> and segment 2 is <code>bar</code>.
       \return requested segment
       \param id (1-based) id of requested segment
    */
    String segment(size_t id) const;

    /** Get the query string. */
    String query() const;

    /** Get the number of query variables. */
    size_t queryVarsCount() const;

    /**
       Get the (id)th query variable's name and value.
       \param id (1-based) id of requested query variable name/value pair
       \param name stores the name of the requested variable
       \param value stores the value of the requested variable
     */
    void queryVarNameValue(size_t id, String& name, String& value) const;

    /**
       Copy query variables (as utl::Pair objects) into the given collection.
       \param col collection which will receive name/value utl::Pair objects
    */
    void queryVarsCopy(Collection& col) const;

    /** Get the fragment identifier. */
    const String&
    fragment() const
    {
        return (_fragment == nullptr) ? emptyString : *_fragment;
    }
    //@}

    /// \name Setters / Modification
    //@{
    /** Revert to initial/empty state. */
    void clear();

    /** Set the uri. */
    void set(const String& uri);

    /** Append trailing slash to the full path. */
    void appendSlash();

    /** Remove trailing slash from full path. */
    void removeSlash();

    /** Set the scheme. */
    void
    setScheme(const String& scheme)
    {
        delete _scheme;
        _scheme = scheme.clone();
    }

    /**
       Set the "userinfo" (username & password).
       \param username ui.username
       \param password ui.password
    */
    void
    setUserinfo(const String& username, const String& password)
    {
        delete _username;
        _username = username.clone();
        delete _password;
        _password = password.clone();
    }

    /** Set the hostname. */
    void
    setHostname(const String& hostname)
    {
        delete _hostname;
        _hostname = hostname.clone();
    }

    /** Set the port. */
    void
    setPort(uint_t port)
    {
        _port = port;
    }

    /** Set the full path (path + filename + extension). */
    void setFullPath(const String& fp);

    /** Set the path. */
    void
    setPath(const String& path)
    {
        delete _path;
        _path = path.clone();
    }

    /** Add a query variable. */
    void addQueryVariable(const String& name, const String& value);

    /** Set the filename. */
    void
    setFilename(const String& filename)
    {
        delete _filename;
        _filename = filename.clone();
    }

    /** Set the extension. */
    void
    setExtension(const String& extension)
    {
        delete _extension;
        _extension = extension.clone();
    }
    //@}

    /// \name Interoperation with String
    //@{
    /** Get the complete (percent-encoded) URI. */
    operator String() const
    {
        return get();
    }

    /** Get a copy of self with the given string appended to the full path. */
    URI
    operator+(const char* rhs) const
    {
        URI uri(get());
        uri.setFullPath(fullPath() + rhs);
        return uri;
    }

    /** Get a copy of self with the given string appended to the full path. */
    URI
    operator+(const String& rhs) const
    {
        URI uri(get());
        uri.setFullPath(fullPath() + rhs);
        return uri;
    }

    /** Append the given string to the full path. */
    URI&
    operator+=(const char* rhs)
    {
        setFullPath(fullPath() + rhs);
        return self;
    }

    /** Append the given string to the full path. */
    URI&
    operator+=(const String& rhs)
    {
        setFullPath(fullPath() + rhs);
        return self;
    }
    //@}

    /// \name Percent-encoding (aka URL-encoding)
    //@{
    /** Get a percent-encoded representation of the given string. */
    static String percentEncode(const String& str, bool isPath = false);

    /** Get a percent-decoded representation of the given string.  */
    static String percentDecode(const String& str);
    //@}

    /// \name libUTL++ init,deInit
    //@{
    /** Perform global init (called by utl::init()). */
    static void utl_init();

    /** Perform global deInit (called by utl::deInit()). */
    static void utl_deInit();
    //@}
private:
    void init();
    void deInit();

private:
    static BitArray* _unreservedChars;

private:
    String* _scheme;
    String* _username;
    String* _password;
    String* _hostname;
    uint_t _port;
    String* _path;
    String* _filename;
    String* _extension;
    Array* _queryVars;
    String* _fragment;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
