#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/BinaryData.h>
#include <libutl/Stream.h>
#include <libutl/StringVars.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpResponse;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   HTTP request.

   This is not a full-featured solution for making HTTP requests.  If it happens to have all the
   features you need, you may want to use it.  I'll extend its functionality over time as
   needed in my own projects.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpRequest : public Object
{
    UTL_CLASS_DECL(HttpRequest, Object);

public:
    /** Constructor. */
    HttpRequest(const char* request)
        : _request(request)
    {
        init();
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    /** Add a unique header. */
    void
    header(const char* name, const char* value)
    {
        header(name, String(value, false));
    }

    /** Add a unique header. */
    void header(const char* name, const String& value);

    /** Specify a body for the request. */
    void body(BinaryData* body, const char* type);

    /** Send the request and obtain the server's response. */
    virtual HttpResponse* execute(Stream& stream) const;

private:
    void
    init()
    {
        _headersHT.setOwner(false);
        _body = nullptr;
    }
    void
    deInit()
    {
        delete _body;
    }

private:
    String _request;
    Array _headersArray;
    Hashtable _headersHT;
    BinaryData* _body;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   HTTP response.

   The result of executing a HttpRequest.

   \author Adam McKee
  \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpResponse : public Object
{
    friend class HttpRequest;
    UTL_CLASS_DECL(HttpResponse, Object);

public:
    /** Constructor. */
    HttpResponse(uint16_t code, const char* text)
        : _statusCode(code)
        , _statusText(text)
    {
        _body = nullptr;
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    /** Get the status code. */
    const uint16_t
    statusCode()
    {
        return _statusCode;
    }

    /** Get the status text. */
    const String&
    statusText()
    {
        return _statusText;
    }

    /** Get the value of the given header. */
    const String&
    header(const char* name) const
    {
        return header(String(name, false));
    }

    /** Get the value of the given header. */
    const String& header(const String& name) const;

    /** Get the collection of headers. */
    const utl::Collection&
    headers() const
    {
        return _headers;
    }

    /** Print the headers to a stream. */
    void printHeaders(Stream& os) const;

    /** Get the response body. */
    const BinaryData*
    body() const
    {
        return _body;
    }

private:
    void
    init()
    {
        _statusCode = 0;
        _body = nullptr;
    }
    void
    deInit()
    {
        delete _body;
    }
    void header(String* name, String* value);
    void
    body(BinaryData* body)
    {
        delete _body;
        _body = body;
    }

private:
    uint16_t _statusCode;
    String _statusText;
    Hashtable _headers;
    BinaryData* _body;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
