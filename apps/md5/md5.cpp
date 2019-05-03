#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/FileStream.h>
#include <libutl/MD5.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(MD5app);
UTL_MAIN_RL(MD5app);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
MD5app::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    if (args.printErrors(cerr))
        return 1;

    byte_t* buf = new byte_t[4096];
    size_t i;
    for (i = 1; i < args.items(); i++)
    {
        String path = args(i);
        FileStream file;
        try
        {
            file.open(path, io_rd);
        }
        catch (Exception& ex)
        {
            cerr << "md5: " << path << ": ";
            ex.dump(cerr);
            continue;
        }
        MD5 md5;
        while (!file.eof())
        {
            size_t numRead = file.read(buf, 4096, 0);
            md5.add(buf, numRead);
        }
        cout << md5.get() << "  " << path << endl;
    }
    delete[] buf;

    return 0;
}
