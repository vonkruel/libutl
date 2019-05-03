#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/AutoPtr.h>
#include <libutl/BinaryData.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/HttpRequest.h>
#include <libutl/InetHostAddress.h>
#include <libutl/InetHostname.h>
#include <libutl/SSLsocket.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    // make a request
    InetHostAddress addr = InetHostname("www.verisign.com").address();
    SSLsocket socket(addr, 443);
    cout << "certificate passed verification = " << Bool(socket.certificateOK()) << endl;
    HttpRequest request("GET /");
    request.header("Host", "www.verisign.com");
    AutoPtr<HttpResponse> response = request.execute(socket);

    // print the result
    cout << response->statusCode() << " " << response->statusText() << endl;
    response->printHeaders(cout);
    const BinaryData* body = response->body();
    if (body != nullptr)
    {
        cout << endl;
        cout.write(body->get(), body->size());
    }

    return 0;
}
