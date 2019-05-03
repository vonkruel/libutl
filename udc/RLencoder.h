#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Encoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Run-length coder.

   A run-length encoder attempts to compress data by recognizing where a symbol has been repeated
   in the data stream, and encoding this \b run as <b><symbol,runLength></b>.

   <b>Attributes</b>

   \arg \b minRunLength : Do not encode a \b run that is less than \b minRunLength symbols long.

   \arg \b runLengthBits : Number of bits to represent the run-length.
   The maximum run-length that can be encoded is \b minRunLength + \b 2^runLengthBits - 1.

   \arg <b><i>eob</i> flag</b> : If \b true, encode end-of-block when encoding is finished,
   so that the end of the block can be recognized when decoding.

   \author Adam McKee
   \ingroup compression
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RLencoder : public Encoder
{
    UTL_CLASS_DECL(RLencoder, Encoder);

public:
    /**
       Constructor.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream associated stream
       \param owner (optional : true) \b owner flag for stream
       \param minRunLength (optional : 3) minimum run-length
       \param runLengthBits (optional : 8) bits to represent run-length
       \param eob (optional : false) encode end-of-block?
    */
    RLencoder(uint_t mode,
              Stream* stream,
              bool owner = true,
              uint_t minRunLength = 3,
              uint_t runLengthBits = 8,
              bool eob = true)
    {
        init();
        start(mode, stream, owner, minRunLength, runLengthBits, eob);
    }

    virtual size_t decode(byte_t* block, size_t num);

    virtual size_t encode(const byte_t* block, size_t num);

    /**
       Initialize for encoding or decoding.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream associated stream
       \param owner (optional : true) \b owner flag for stream
       \param minRunLength (optional : 3) minimum run-length
       \param runLengthBits (optional : 8) bits to represent run-length
       \param eob (optional : false) encode end-of-block?
    */
    void start(uint_t mode,
               Stream* stream,
               bool owner = true,
               uint_t minRunLength = 3,
               uint_t runLengthBits = 8,
               bool eob = true);

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
    bool
    getEOB() const
    {
        return getFlag(flg_eob);
    }
    void
    setEOB(bool eob)
    {
        setFlag(flg_eob, eob);
    }

private:
    uint_t _minRunLen;
    uint_t _maxRunLen;
    uint_t _runLenBits;
    int _saveC2;
    enum flg_t
    {
        flg_eob = 9
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
