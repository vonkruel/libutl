#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/SHA256.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    SHA256 sha256;

    static byte_t data[KB(64)];
    Stream* stream = cin.getStream();
    while (!stream->eof())
    {
        size_t num = stream->read(data, KB(64), 0);
        sha256.process(data, num);
    }

    SHA256sum hash;
    sha256.finalize(hash);
    cout << hash.toString() << endl;

    return 0;
}
