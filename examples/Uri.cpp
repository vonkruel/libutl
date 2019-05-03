#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/HostOS.h>
#include <libutl/Uint.h>
#include <libutl/URI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

void
dissectURI(const String& p_uri)
{
    URI uri(p_uri);

    // port
    String port = Uint(uri.port()).toString();
    if (port == "0")
        port.clear();

    // segments
    String segments;
    for (size_t i = 1; i < size_t_max; ++i)
    {
        String seg = uri.segment(i);
        if (seg.empty())
            break;
        if (i > 1)
            segments += ",";
        segments += seg;
    }

    // print uri's parts
    cout << "URI(\"" << uri << "\")" << endl
         << "     isRelative   : " << Bool(uri.isRelative()) << endl
         << "     scheme       : " << uri.scheme() << endl
         << "     username     : " << uri.username() << endl
         << "     password     : " << uri.password() << endl
         << "     hostname     : " << uri.hostname() << endl
         << "     port         : " << port << endl
         << "     fullPath     : " << uri.fullPath() << endl
         << "     fullFilename : " << uri.fullFilename() << endl
         << "     path         : " << uri.path() << endl
         << "     numSegments  : " << uri.numSegments() << endl
         << "     segments     : " << segments << endl
         << "     filename     : " << uri.filename() << endl
         << "     extension    : " << uri.extension() << endl
         << "     query        : " << uri.query() << endl
         << "     fragment     : " << uri.fragment() << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    dissectURI("http://guest:pass@somesite.com:80/main.php?var1=val1;var2=val2#main");
    dissectURI("http://www.google.com/");
    dissectURI("../auto/nissan/maxima.html#intro");
    return 0;
}
