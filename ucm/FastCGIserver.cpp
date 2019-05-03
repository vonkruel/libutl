#include <libutl/libutl.h>
#include <libutl/FastCGIserver.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define FCGI_BEGIN_REQUEST 1
#define FCGI_ABORT_REQUEST 2
#define FCGI_END_REQUEST 3
#define FCGI_PARAMS 4
#define FCGI_STDIN 5
#define FCGI_STDOUT 6
#define FCGI_STDERR 7
#define FCGI_DATA 8
#define FCGI_GET_VALUES 9
#define FCGI_GET_VALUES_RESULT 10
#define FCGI_UNKNOWN_TYPE 11
#define FCGI_MAXTYPE 11

#define FCGI_RESPONDER 1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER 3

#define FCGI_KEEP_CONN 1

#define FCGI_REQUEST_COMPLETE 0
#define FCGI_CANT_MPX_CONN 1
#define FCGI_OVERLOADED 2
#define FCGI_UNKNOWN_ROLE 3

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    unsigned char version;
    unsigned char type;
    unsigned char requestIdB1;
    unsigned char requestIdB0;
    unsigned char contentLengthB1;
    unsigned char contentLengthB0;
    unsigned char paddingLength;
    unsigned char reserved;
} FCGI_Header;

typedef struct
{
    unsigned char roleB1;
    unsigned char roleB0;
    unsigned char flags;
    unsigned char reserved[5];
} FCGI_BeginRequestBody;

typedef struct
{
    unsigned char appStatusB3;
    unsigned char appStatusB2;
    unsigned char appStatusB1;
    unsigned char appStatusB0;
    unsigned char protocolStatus;
    unsigned char reserved[3];
} FCGI_EndRequestBody;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FCGI_record ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class FCGI_record : public Object
{
    UTL_CLASS_DECL(FCGI_record, Object);

public:
    FCGI_record(int)
    {
        contentLength = 0;
        contentData = nullptr;
    }

    virtual void serialize(Stream& stream, uint_t io, uint_t);

public:
    byte_t version;
    byte_t type;
    uint16_t requestId;
    uint16_t contentLength;
    byte_t* contentData;

private:
    void
    init()
    {
        contentLength = 0;
        contentData = new byte_t[65536];
    }
    void
    deInit()
    {
        delete[] contentData;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FCGI_record::serialize(Stream& stream, uint_t io, uint_t)
{
    if (io == io_rd)
    {
        FCGI_Header header;
        stream.read((byte_t*)&header, sizeof(header));
        version = header.version;
        if (version != 1)
            throw StreamSerializeEx();
        type = header.type;
        requestId = ((uint16_t)header.requestIdB1 << 8) | header.requestIdB0;
        contentLength = ((uint16_t)header.contentLengthB1 << 8) | header.contentLengthB0;
        stream.read(contentData, contentLength);
        if (header.paddingLength > 0)
        {
            ASSERTD(header.paddingLength <= 512);
            byte_t padding[512];
            stream.read(padding, header.paddingLength);
        }
    }
    else
    {
        FCGI_Header header;
        header.version = 1;
        header.type = type;
        header.requestIdB1 = (requestId >> 8);
        header.requestIdB0 = (requestId & 0xff);
        header.contentLengthB1 = (contentLength >> 8);
        header.contentLengthB0 = (contentLength & 0xff);
        header.paddingLength = 0;
        header.reserved = 0;
        stream.write((byte_t*)&header, sizeof(header));
        if (contentLength > 0)
            stream.write(contentData, contentLength);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::FCGI_record);
UTL_CLASS_IMPL(utl::FastCGIstreamWriter);
UTL_CLASS_IMPL_ABC(utl::FastCGIserver);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FastCGIserver //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIserver::onClientConnect(NetServerClient* client)
{
    client->setSocket(new FastCGIstreamWriter(client->socket()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIserver::clientReadMsg(NetServerClient* client)
{
    SCOPE_FAIL
    {
        clientDisconnect(client);
    };
    FCGI_record rec;
    Stream& socket = client->unbufferedSocket();
    rec.serializeIn(socket);
    if (rec.type != FCGI_BEGIN_REQUEST)
        return;
    if (rec.contentLength != sizeof(FCGI_BeginRequestBody))
        return;
    FCGI_BeginRequestBody* brb = (FCGI_BeginRequestBody*)rec.contentData;
    if (brb->roleB1 != 0)
        return;
    if (brb->roleB0 != FCGI_RESPONDER)
        return;
    bool keepConn = (brb->flags & FCGI_KEEP_CONN) != 0;
    respond(client, rec.requestId, keepConn);
    if (!keepConn)
        clientDisconnect(client);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIserver::respond(NetServerClient* client, uint16_t requestId, bool keepConn)
{
    FCGI_record rec;
    Stream& socket = client->unbufferedSocket();
    MemStream paramsData, input;
    StringVars params;
    for (;;)
    {
        rec.serializeIn(socket);
        if (rec.requestId != requestId)
            continue;
        if (rec.type == FCGI_PARAMS)
        {
            if (rec.contentLength > 0)
            {
                paramsData.write(rec.contentData, rec.contentLength);
            }
        }
        else if (rec.type == FCGI_STDIN)
        {
            if (rec.contentLength == 0)
            {
                if (paramsData.tellp() > 0)
                {
                    readParams(params, paramsData);
                }

                // set requestId in FastCGIstreamWriter
                auto& fcgiWriter = utl::cast<FastCGIstreamWriter>(client->socket());
                fcgiWriter.setRequestId(rec.requestId);

                // let the derived class respond to the request
                vrespond(client, params, input);

                // flush out fcgi-writer's buffer
                client->socket().flush();

                // terminate stdout
                /*rec.type = FCGI_STDOUT;
                rec.contentLength = 0;
                rec.serializeOut(socket);*/

                // signal that the response is complete
                rec.type = FCGI_END_REQUEST;
                rec.contentLength = sizeof(FCGI_EndRequestBody);
                FCGI_EndRequestBody* erb = (FCGI_EndRequestBody*)rec.contentData;
                erb->appStatusB3 = 0;
                erb->appStatusB2 = 0;
                erb->appStatusB1 = 0;
                erb->appStatusB0 = 0;
                erb->protocolStatus = FCGI_REQUEST_COMPLETE;
                erb->reserved[0] = erb->reserved[1] = erb->reserved[2] = 0;
                rec.serializeOut(socket);

                break;
            }
            else
            {
                input.write(rec.contentData, rec.contentLength);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIserver::readParams(StringVars& params, MemStream& paramsData)
{
    const byte_t* p = paramsData.get();
    const byte_t* lim = p + paramsData.tellp();
    while (p < lim)
    {
        size_t nameLen = *p++;
        if (nameLen >= 128)
        {
            nameLen &= 0x7f;
            nameLen <<= 8;
            nameLen |= *p++;
            nameLen <<= 8;
            nameLen |= *p++;
            nameLen <<= 8;
            nameLen |= *p++;
        }
        size_t valueLen = *p++;
        if (valueLen >= 128)
        {
            valueLen &= 0x7f;
            valueLen <<= 8;
            valueLen |= *p++;
            valueLen <<= 8;
            valueLen |= *p++;
            valueLen <<= 8;
            valueLen |= *p++;
        }

        // pick out nameStr, valueStr, adjust p
        char* nameStr = new char[nameLen + 1];
        memcpy(nameStr, p, nameLen);
        nameStr[nameLen] = '\0';
        p += nameLen;
        char* valueStr = new char[valueLen + 1];
        memcpy(valueStr, p, valueLen);
        valueStr[valueLen] = '\0';
        p += valueLen;

        // empty value -> skip it
        if (valueLen == 0)
        {
            delete[] nameStr;
            delete[] valueStr;
            continue;
        }

        // HTTP_COOKIE?
        if ((nameLen == 11) && (nameStr[0] == 'H') && (nameStr[4] == '_') && (nameStr[5] == 'C') &&
            !strcmp(nameStr, "HTTP_COOKIE"))
        {
            delete[] nameStr;
            String value(valueStr, true, false);
            for (size_t i = 0; i < valueLen;)
            {
                String cookie = value.nextToken(i, ';');
                size_t equalsPos = cookie.find('=');
                String name = cookie.subString(0, equalsPos);
                String value = cookie.subString(equalsPos + 1);
                name.trim();
                value.trim();
                if (name.empty() || value.empty())
                    continue;
                params.setValue("Cookie-" + name, value);
            }
        }
        else
        {
            // add variable
            String* name = new String(nameStr, true, false);
            String* value = new String(valueStr, true, false);
            params.setValue(name, value);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// FastCGIstreamWriter ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

FastCGIstreamWriter::FastCGIstreamWriter(Stream* stream, bool streamOwner)
    : BufferedStream(stream, streamOwner, 0, KB(64) - 1)
{
    init();
    _rec->version = 1;
    _rec->type = FCGI_STDOUT;
    _rec->requestId = 0;
    _rec->contentLength = 0;
    _rec->contentData = _oBuf.get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIstreamWriter::setRequestId(uint16_t requestId)
{
    _rec->requestId = requestId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIstreamWriter::underflow()
{
    ABORT();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIstreamWriter::overflow()
{
    if (_oBufPos == 0)
        return;
    ASSERTD(_stream != nullptr);

    // write a record
    ASSERTD(_oBufPos < KB(64));
    _rec->contentLength = _oBufPos;
    _rec->serializeOut(*_stream);
    _oBufPos = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIstreamWriter::init()
{
    _rec = new FCGI_record(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FastCGIstreamWriter::deInit()
{
    _rec->contentData = nullptr;
    delete _rec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
