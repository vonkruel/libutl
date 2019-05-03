#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   ReCaptcha response verifier.

   \author Adam McKee
   \ingroup communication
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ReCaptcha : public utl::Object
{
    UTL_CLASS_DECL(ReCaptcha, utl::Object);
    UTL_CLASS_DEFID;

public:
    /**
       Query the ReCaptcha server to check the user's answer.  If clientIP matches 10.0.0.0/8 or
       192.168.0.0/16, the request to the ReCaptcha verification server will provide serverIP as
       the value of "remoteid" (instead of specifying remoteid=clientIP).
       \param secret the servers private key for ReCaptcha
       \param response the token for the user's correct response
       \param clientIP (optional) the client's IP address
       \param serverIP (optional) the server's IP address
    */
    static bool checkAnswer(const String& secret,
                            const String& response,
                            const String* clientIP = nullptr,
                            const String* serverIP = nullptr);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
