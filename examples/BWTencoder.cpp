#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BWTencoder.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/CmdLineArgs.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int argc, char** argv)
{
    CmdLineArgs args(argc, argv);
    if (args.isSet("help"))
    {
        cout << "Usage: " << args(0) << " [-d]" << endl;
        return 0;
    }
    bool compress = !args.isSet("d");

    // determine block size
    size_t blockSize = KB(256);
    if (args.isSet("1"))
        blockSize = KB(64);
    else if (args.isSet("2"))
        blockSize = KB(128);
    else if (args.isSet("3"))
        blockSize = KB(256);
    else if (args.isSet("4"))
        blockSize = KB(384);
    else if (args.isSet("5"))
        blockSize = KB(512);
    else if (args.isSet("6"))
        blockSize = KB(640);
    else if (args.isSet("7"))
        blockSize = KB(768);
    else if (args.isSet("8"))
        blockSize = KB(896);
    else if (args.isSet("9"))
        blockSize = KB(1024);

    // ensure no bad arguments given
    if (args.printErrors(cerr))
        return 1;

    BWTencoder bwt(compress ? io_wr : io_rd, compress ? cout : cin, false, blockSize);
    bwt.setCRC(true);

    // compress or decompress, and do CRC-32 checking
    if (compress)
    {
        bwt.copyData(cin);
        // complete compression
        bwt.close();
        // write any remaining bits on cout
        cout.putBits();
        // write CRC-32
        uint32_t crc = bwt.getCRC();
        ::serialize(crc, cout, io_wr, ser_compact);
    }
    else
    {
        // decompress data
        cout.copyData(bwt);
        // read original CRC-32
        uint32_t crc;
        ::serialize(crc, cin, io_rd, ser_compact);
        // verify it against our computed CRC-32
        if (crc != bwt.getCRC())
        {
            cerr << "CRC error" << endl;
            return 1;
        }
    }

    return 0;
}
