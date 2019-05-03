#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Pathname.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE
UTL_APP(Test)
UTL_MAIN_RL(Test)

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    Pathname path("foo/bar/foobar.txt");
    cout << "path = " << path << endl;
    cout << "path.getDirectory() = " << path.directory() << endl;
    cout << "path.getFilename() = " << path.filename() << endl;
    cout << "path.getBasename() = " << path.basename() << endl;
    cout << "path.getSuffix() = " << path.suffix() << endl;
    cout << "path.getFirstComponent() = " << path.firstComponent() << endl;
    cout << "path.getLastComponent() = " << path.lastComponent() << endl;

    return 0;
}
