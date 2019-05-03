#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Convert libc's \c errno to an exception.
   \ingroup exception
   \param object associated object
*/
void errToEx(const utl::Object* object = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Convert libc's \c h_errno to an exception.
   \ingroup exception
   \param object associated object
*/
void h_errToEx(const utl::Object* object = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

class String;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare a simple exception type.
   \ingroup exception
   \param exName exception name
   \param baseExName base exception name
*/
#define UTL_EXCEPTION_DECL(exName, baseExName)                                                     \
    class exName : public baseExName                                                               \
    {                                                                                              \
        UTL_CLASS_DECL(exName, baseExName);                                                        \
                                                                                                   \
    public:                                                                                        \
        exName(utl::Object* object);                                                               \
                                                                                                   \
        exName(const utl::Object& object);                                                         \
                                                                                                   \
        exName(const char* name, utl::Object* object)                                              \
            : baseExName(name, object)                                                             \
        {                                                                                          \
        }                                                                                          \
                                                                                                   \
        exName(const char* name, const utl::Object& object)                                        \
            : baseExName(name, object)                                                             \
        {                                                                                          \
        }                                                                                          \
                                                                                                   \
    private:                                                                                       \
        void init();                                                                               \
        void                                                                                       \
        deInit()                                                                                   \
        {                                                                                          \
        }                                                                                          \
    };

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implement a simple exception type.
   \ingroup exception
   \param exNS exception namespace
   \param exName exception name
   \param baseExName base exception name
   \param name human-readable name
*/
#define UTL_EXCEPTION_IMPL(exNS, exName, baseExName, name)                                         \
    UTL_CLASS_IMPL(exNS::exName);                                                                  \
    exNS::exName::exName(utl::Object* object)                                                      \
        : baseExName(name, object)                                                                 \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
    exNS::exName::exName(const utl::Object& object)                                                \
        : baseExName(name, object)                                                                 \
    {                                                                                              \
    }                                                                                              \
                                                                                                   \
    void exNS::exName::init()                                                                      \
    {                                                                                              \
        setName(name);                                                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
// Exception ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Root of UTL++ exception class hierarchy.

   \author Adam McKee
   \ingroup exception
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Exception : public Object
{
    UTL_CLASS_DECL_ABC(Exception, Object);

public:
    /**
       Constructor.
       \param name exception name
       \param object contains information about the exception
    */
    Exception(const char* name, Object* object);

    /**
       Constructor.
       \param name exception name
       \param object contains information about the exception
    */
    Exception(const char* name, const Object& object);

    /**
       Copy another Exception.
    */
    virtual void copy(const Object& rhs);

    /**
       Dump a description of the exception to the given stream.
    */
    virtual void dump(Stream& os) const;

    /** Get the name. */
    const String&
    name() const
    {
        ASSERTD(_name != nullptr);
        return *_name;
    }

    /** Get name pointer. */
    const String*
    namePtr() const
    {
        return _name;
    }

    /** Set the name. */
    void setName(const char* name);

    /** Get the associated object. */
    Object*
    object() const
    {
        return _object;
    }

    /** Take the associated object. */
    Object*
    takeObject()
    {
        Object* obj = _object;
        _object = nullptr;
        return obj;
    }

    /** Set the object to the given object. */
    void setObject(Object* object);

    /** Set the object to a copy of the given object. */
    void setObject(const Object& object);

private:
    void init();
    void deInit();

private:
    const String* _name;
    Object* _object;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Host OS Exceptions //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   <b>Host OS</b> exception (abstract)
   \ingroup exception
*/

UTL_EXCEPTION_DECL(HostOSEx, utl::Exception);

/**
   <b>Access denied</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(AccessDeniedEx, HostOSEx);

/**
   <b>File already exists</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(FileExistsEx, HostOSEx);

/**
   <b>File not found</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(FileNotFoundEx, HostOSEx);

/**
   <b>Interrupted system call</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(InterruptedSyscallEx, HostOSEx);

/**
   <b>Invalid address/descriptor</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(InvalidAddressEx, HostOSEx);

/**
   <b>Non-directory component</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(NonDirectoryComponentEx, HostOSEx);

/**
   <b>Unknown host OS</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(UnknownHostOSEx, HostOSEx);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Stream Exceptions ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   <b>Stream</b> exception (abstract)
   \ingroup exception
*/
UTL_EXCEPTION_DECL(StreamEx, Exception);

/**
   <b>Stream EOF</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(StreamEOFex, StreamEx);

/**
   <b>Stream I/O error</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(StreamErrorEx, StreamEx);

/**
   <b>Stream serialization</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(StreamSerializeEx, StreamEx);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Network Exceptions //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   <b>Network</b> exception (abstract)
   \ingroup exception
*/
UTL_EXCEPTION_DECL(NetworkEx, HostOSEx);

/**
   <b>Connection refused</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(ConnectionRefusedEx, NetworkEx);

/**
   <b>Address in use</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(AddressInUseEx, NetworkEx);

/**
   <b>Address not available</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(AddressNotAvailableEx, NetworkEx);

/**
   <b>Network unreachable</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(NetworkUnreachableEx, NetworkEx);

/**
   <b>Connection reset by peer</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(ConnectionResetEx, NetworkEx);

/**
   <b>Host lookup failure</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(HostLookupEx, NetworkEx);

/**
   <b>Host has no address</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(HostNoAddressEx, NetworkEx);

/**
   <b>Host not found</b> exception
   \ingroup exception
*/
UTL_EXCEPTION_DECL(HostNotFoundEx, NetworkEx);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Misc Exceptions /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Illegal value exception.
   \ingroup exception
*/
UTL_EXCEPTION_DECL(IllegalValueEx, Exception);

/**
   Unknown exception.
   \ingroup exception
*/
UTL_EXCEPTION_DECL(UnknownEx, Exception);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
