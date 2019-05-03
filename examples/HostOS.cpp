#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/HostOS.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    cout << "hostOS->sleep(1)" << endl;
    hostOS->sleep(1);
    cout << "hostOS->usleep(500000)" << endl;
    hostOS->usleep(350000);
    cout << "hostOS->chdir(\"/\")" << endl;
    hostOS->chdir("/");
    TArray<String> args;
    args += String("/bin/ls");
    args += String("-l");
    cout << args.toString(" ") << endl;
    hostOS->run("/bin/ls", args);
    return 0;
}
