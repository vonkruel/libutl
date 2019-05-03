#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/TCPserverSocket.h>
#include <libutl/TCPsocket.h>
#include <libutl/Time.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    // parse command-line arguments
    CmdLineArgs args(argc, argv);
    String portStr;
    utl::uint16_t port = 1025;
    if (args.isSet("p", portStr))
    {
        port = Uint(portStr);
    }

    // ensure no bad arguments given
    if (args.printErrors(cerr))
        return 1;

    // create server socket
    InetHostAddress addr = InetHostname::localInetHostname().address();
    TCPserverSocket serverTCPsocket(addr, port);
    cout << "listening on port " << port << "..." << endl;
    cout << "to connect: \"nc " << InetHostname::localInetHostname() << " " << port << "\"" << endl;

    // accept connection
    TCPsocket socket;
    serverTCPsocket.accept(socket);

    // write a simple message on the socket
    socket << endl;
    socket << "====================================" << endl;
    socket << "current time is: " << Time::current() << endl;
    socket << "====================================" << endl;
    socket << endl;

    return 0;
}
