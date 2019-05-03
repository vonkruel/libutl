#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Encoder.h>
#include <libutl/HuffmanEncoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define LZ_LEN_CODES 28
#define LZ_DIST_CODES 30

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   LZ77-based compressor.

   LZencoder implements the same LZ77-based compression technique used in such popular compressors
   as \b gzip and \b WinZip.  It is considerably slower than gzip, which I think is largely due to
   the use of adaptive instead of static Huffman coding.

   <b>Attributes</b>

   \arg \b level : The compression level (0-9) determines how aggressively the compressor will
   search for string matches in previously seen text.  Higher values generally mean that
   compression will take longer, but be improved.  Lower values trade away some compression in
   favor of performance.  Decompression speed is unaffected by the compression level.

   <b>Advantages</b>

   \arg less resource-intensive than BWTencoder
   \arg code is easy to read and maintain
   \arg fast enough for many purposes
   \arg delivers good compression
   \arg easy to use within the UTL++ framework

   <b>Disadvantages</b>

   \arg considerably slower than gzip
   \arg doesn't compress \b quite as well as gzip
   \arg not as thoroughly tested as gzip -- use at your own risk!

   \author Adam McKee
   \ingroup compression
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LZencoder : public Encoder
{
    UTL_CLASS_DECL(LZencoder, Encoder);

public:
    /**
       Constructor.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream (optional) associated stream
       \param owner (optional : true) \b owner flag for stream
       \param level (optional : 9) compression level (0-9)
    */
    LZencoder(uint_t mode, Stream* stream, bool owner = true, uint_t level = 9)
    {
        init();
        start(mode, stream, owner, level);
    }

    virtual size_t decode(byte_t* block, size_t num);

    virtual size_t encode(const byte_t* block, size_t num);

    /**
       Initialize for encoding or decoding.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream (optional) associated stream
       \param owner (optional : true) \b owner flag for stream
       \param level (optional : 9) compression level (0-9)
    */
    void start(uint_t mode, Stream* stream, bool owner = true, uint_t level = 9);

protected:
    virtual void clear();
    virtual void finishEncoding();

private:
    void init();
    void
    deInit()
    {
        close();
    }
    void addString(uint_t pos);
    uint_t matchString(uint_t pos);
    void removeString(uint_t pos);

private:
    uint_t _lab, _repc, _pos;
    // sliding window, look-ahead buffer
    byte_t* _wind;
    byte_t* _look;
    // match information
    uint_t _prevMatchLen, _prevMatchDist, _prevLiteral;
    uint_t _matchLen, _matchDist, _literal;
    bool _matchAvailable;
    // compress configuration
    uint_t _goodLen;    // reduce lazy search above this length
    uint_t _maxLazyLen; // do not perform lazy search above this match length
    uint_t _niceLen;    // quit search above this match length
    uint_t _maxChain;   // follow at most this many hash chains
    // coding distances, lengths
    uint_t* _baseLen;
    uint_t* _lenCode;
    uint_t* _baseDist;
    uint_t* _distCode;
    static const uint_t eob;
    static const uint_t lenBits[LZ_LEN_CODES];
    static const uint_t distBits[LZ_DIST_CODES];
    // hashing
    uint_t* _head;
    uint_t* _tail;
    uint_t* _pred;
    uint_t* _succ;
    // H-coders
    HuffmanEncoder _lEnc;
    HuffmanEncoder _dEnc;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
