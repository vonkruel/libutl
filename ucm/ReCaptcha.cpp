#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/AutoPtr.h>
#include <libutl/BufferedSSLsocket.h>
#include <libutl/HttpRequest.h>
#include "ReCaptcha.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ReCaptcha);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
ReCaptcha::checkAnswer(const String& secret,
                       const String& response,
                       const String* clientIP,
                       const String* serverIP)
{
    // construct the HTTP POST request
    HttpRequest request("POST /recaptcha/api/siteverify");
    request.header("Host", "www.google.com");
    request.header("User-Agent", "reCAPTCHA/aCMS");
    auto reqBody = "secret=" + secret;
    reqBody += "&response=" + response;
    if (clientIP != nullptr)
    {
        reqBody += "&remoteip=";
        bool useServerIP;
        if (serverIP == nullptr)
        {
            useServerIP = false;
        }
        else
        {
            // client address is non-Internet-routable -> use server's external IP
            // .. (this is for testing, when the client is actually on the server's local network)
            useServerIP = ((clientIP->comparePrefix("10.") == 0) ||
                           (clientIP->comparePrefix("192.168.") == 0));
        }
        reqBody += useServerIP ? *serverIP : *clientIP;
    }
    request.body(new BinaryData((byte_t*)reqBody.get(), reqBody.length(), false),
                 "application/x-www-form-urlencoded;");

    // connect to the server, write the request, read the response
    try
    {
        auto addr = InetHostname("www.google.com");
        BufferedSSLsocket s(addr, 443);

        // send the request & get the response
        AutoPtr<HttpResponse> response = request.execute(s);

        // create a String from the response body
        auto bodyData = response->body();
        String body;
        body.set(reinterpret_cast<const char*>(bodyData->get()), true, true, bodyData->size());

        // first line of the body should be "true"
        auto successIdx = body.find("\"success\": ");
        return ((successIdx != size_t_max) && (body.subString(successIdx + 11, 4) == "true"));
    }
    catch (Exception&)
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
