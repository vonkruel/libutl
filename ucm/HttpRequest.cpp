#include <libutl/libutl.h>
#include <libutl/AutoPtr.h>
#include <libutl/HttpRequest.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::HttpRequest);
UTL_CLASS_IMPL(utl::HttpResponse);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// HttpRequest ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
HttpRequest::compare(const Object& rhs) const
{
    auto& req = utl::cast<HttpRequest>(rhs);
    int res;
    res = _request.compare(req._request);
    if (res != 0)
        return res;
    res = _headersArray.compare(req._headersArray);
    if (res != 0)
        return res;
    res = utl::compareNullable(_body, req._body);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HttpRequest::copy(const Object& rhs)
{
    auto& req = utl::cast<HttpRequest>(rhs);
    _request = req._request;
    _headersArray = req._headersArray;
    _headersHT = _headersArray;
    delete _body;
    _body = utl::clone(req._body);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HttpRequest::header(const char* name, const String& value)
{
    ASSERTD(name != nullptr);
    auto pair = utl::cast<Pair>(_headersHT.find(String(name, false)));
    if (pair == nullptr)
    {
        String* valueCopy = value.clone()->assertOwner();
        pair = new Pair(new String(name, true, true, false), valueCopy);
        _headersArray += pair;
        _headersHT += pair;
    }
    else
    {
        auto pairValue = utl::cast<String>(pair->second());
        pairValue->append(',');
        pairValue->append(value);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HttpRequest::body(BinaryData* body, const char* type)
{
    delete _body;
    _body = body;
    header("Content-Length", Uint(body->size()).toString());
    header("Content-Type", type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

HttpResponse*
HttpRequest::execute(Stream& stream) const
{
    // send the request
    stream << _request << " HTTP/1.1\r\n";
    for (auto hdr_ : _headersArray)
    {
        auto& hdr = utl::cast<Pair>(*hdr_);
        auto& name = utl::cast<String>(*hdr.first());
        auto& value = utl::cast<String>(*hdr.second());
        stream << name << ": " << value << "\r\n";
    }
    stream << "\r\n";

    // if there's a body, write it
    if (_body != nullptr)
    {
        stream.write((byte_t*)_body->get(), _body->size());
    }

    // flush the stream (in case it's buffered)
    stream.flush();

    // read the status line
    String line;
    stream.readLine(line);
    line.trim();
    size_t beginIdx = line.find(' ');
    if (beginIdx++ == size_t_max)
        throw StreamErrorEx();
    size_t endIdx = line.find(' ', beginIdx);
    String statusCodeStr = line.subString(beginIdx, endIdx - beginIdx);
    uint16_t statusCode = (uint16_t)Uint(statusCodeStr).get();
    String statusText = line.subString(endIdx + 1);

    HttpResponse* response = new HttpResponse(statusCode, statusText);
    AutoPtr<> responsePtr = response;

    // response headers
    for (;;)
    {
        stream.readLine(line);
        line.trim();
        if (line.empty())
            break;
        size_t colonIdx = line.find(':');
        if (colonIdx == size_t_max)
            throw StreamErrorEx();
        String* name = new String(line.subString(0, colonIdx));
        name->trim();
        String* value = new String(line.subString(colonIdx + 2));
        value->trim();
        response->header(name, value);
    }

    // read the body (if there is one)
    const String& contentLengthStr = response->header("Content-Length");
    if (!contentLengthStr.empty())
    {
        size_t bodyLength = Uint(contentLengthStr).get();
        auto body = new BinaryData(bodyLength);
        SCOPE_FAIL
        {
            delete body;
        };
        stream.read(body->get(), bodyLength);
        response->body(body);
    }
    else if (response->header("Transfer-Encoding") == "chunked")
    {
        // read chunks
        Uint chunkLength;
        String chunkLengthStr;
        auto body = new BinaryData((size_t)0, size_t_max);
        SCOPE_FAIL
        {
            delete body;
        };
        size_t size = 0;
        for (;;)
        {
            // read chunk length (zero-length chunk means we're done)
            stream.readLine(chunkLengthStr);
            chunkLengthStr.trim();
            chunkLength.setHex(chunkLengthStr);
            if (chunkLength == 0)
                break;

            // read the chunk into our buffer
            body->grow(size + chunkLength);
            stream.read(body->get() + size, chunkLength);
            char crlf[2];
            stream.read((byte_t*)crlf, 2);
            size += chunkLength;
        }

        // additional headers may be sent after the terminating zero-length chunk
        // .. (in the full spirit of not forcing the server to fully determine the response
        // ..  before starting to send it)
        for (;;)
        {
            stream.readLine(line);
            line.trim();
            if (line.empty())
                break;
            size_t colonIdx = line.find(':');
            if (colonIdx == size_t_max)
                throw StreamErrorEx();
            String* name = new String(line.subString(0, colonIdx));
            name->trim();
            String* value = new String(line.subString(colonIdx + 2));
            value->trim();
            response->header(name, value);
        }

        response->body(body);
    }

    responsePtr.release();
    return response;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// HttpResponse ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
HttpResponse::compare(const Object& rhs) const
{
    auto& resp = utl::cast<HttpResponse>(rhs);
    int res;
    res = utl::compare(_statusCode, resp._statusCode);
    if (res != 0)
        return res;
    res = _statusText.compare(resp._statusText);
    if (res != 0)
        return res;
    res = _headers.compare(resp._headers);
    if (res != 0)
        return res;
    res = utl::compareNullable(_body, resp._body);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HttpResponse::copy(const Object& rhs)
{
    auto& resp = utl::cast<HttpResponse>(rhs);
    _statusCode = resp._statusCode;
    _statusText = resp._statusText;
    _headers = resp._headers;
    delete _body;
    _body = utl::clone(resp._body);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const String&
HttpResponse::header(const String& name) const
{
    auto pair = utl::cast<Pair>(_headers.find(name));
    if (pair == nullptr)
        return emptyString;
    return utl::cast<String>(*pair->second());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HttpResponse::header(String* name, String* value)
{
    auto pair = utl::cast<Pair>(_headers.find(*name));
    if (pair == nullptr)
    {
        name->setCaseSensitive(false);
        pair = new Pair(name, value);
        _headers += pair;
    }
    else
    {
        auto pairValue = utl::cast<String>(pair->second());
        pairValue->append(',');
        pairValue->append(*value);
        delete name;
        delete value;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HttpResponse::printHeaders(Stream& os) const
{
    for (auto pair_ : _headers)
    {
        auto& pair = utl::cast<Pair>(*pair_);
        os << *pair.first() << ": " << *pair.second() << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
