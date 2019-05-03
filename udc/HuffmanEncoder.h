#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Encoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Adaptive Huffman coder.

   Huffman coding is due to the late, great <b>David A. Huffman</b>.  To learn about Huffman
   coding, consult a book or website that teaches lossless data compression.  This implementation
   of adaptive Huffman coding uses a splay tree.

   <b>Attributes</b>

   \arg \b numSymbols : Number of distinct symbols to be encoded/decoded.

   \arg \b incLimit : Increment limit.  Frequency counts are halved when \b incLimit symbols have
   been encoded/decoded.

   \arg <b><i>eob</i> flag</b> : If \b true, encode end-of-block when encoding is finished, so
   that the end of the block can be recognized when decoding.

   \author Adam McKee
   \ingroup compression
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HuffmanEncoder : public Encoder
{
    UTL_CLASS_DECL(HuffmanEncoder, Encoder);

public:
    /**
       Constructor.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream associated stream
       \param owner \b owner flag for stream
       \param numSymbols alphabet size
       \param incLimit increment limit
       \param eob (optional : false) encode end-of-block?
    */
    HuffmanEncoder(uint_t mode,
                   Stream* stream,
                   bool owner,
                   uint_t numSymbols,
                   uint_t incLimit,
                   bool eob = false)
    {
        init();
        start(mode, stream, owner, numSymbols, incLimit, eob);
    }

    virtual size_t decode(byte_t* block, size_t num);

    /** Decode a symbol. */
    inline uint_t decode();

    virtual size_t encode(const byte_t* block, size_t num);

    /** Encode a symbol. */
    inline void encode(uint_t symbol);

    /**
       Initialize for encoding or decoding.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream associated stream
       \param owner \b owner flag for stream
       \param numSymbols alphabet size
       \param incLimit halve frequence counts when \b incLimit symbols
              have been encoded
       \param eob (optional : false) encode end-of-block?
    */
    void start(uint_t mode,
               Stream* stream,
               bool owner,
               uint_t numSymbols,
               uint_t incLimit,
               bool eob = false);

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
    void clearSelf();
    void updateFreq(uint_t a, uint_t b);
    void updateModel(uint_t symbol);
    uint_t _numSymbols;
    uint_t _incLimit;
    uint_t _eob;
    uint_t* _up;
    uint_t* _freq;
    uint_t* _left;
    uint_t* _right;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::encode(uint_t symbol)
{
    uint_t a, sp = 0;
    bool stack[64];
    a = _numSymbols + symbol;
    do
    {
        stack[sp++] = (_right[_up[a]] == a);
        a = _up[a];
    } while (a != 1);
    do
    {
        _stream->putBit(stack[--sp]);
    } while (sp > 0);
    updateModel(symbol);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
HuffmanEncoder::decode()
{
    uint_t a = 1;
    do
    {
        if (_stream->getBit())
        {
            a = _right[a];
        }
        else
        {
            a = _left[a];
        }
    } while (a < _numSymbols);
    a -= _numSymbols;
    updateModel(a);
    return a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
