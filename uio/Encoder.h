#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/CRC32.h>
#include <libutl/BufferedStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Encoder/decoder abstraction.

   An Encoder-derived class is capable of doing and/or undoing a transformation on a data stream.

   <b>Attributes</b>

   \arg mode : An encoder may be in read (\b io_rd) or write (\b io_wr) mode.  In read mode,
   underflow() fills the read buffer by decoding data from the associated stream.  In write mode,
   overflow() encodes the data in the write buffer to the associated stream.

   \arg \b stream : Associated stream.

   \arg <b><i>owner</i> flag</b> : Ownership flag for the associated stream.

   \arg <b><i>lastBlock</i> flag</b> : Encoding the last block?

   \arg <b><i>crc</i> flag</b> : Calculate CRC-32 for encoded or decoded data?

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Encoder : public BufferedStream
{
    UTL_CLASS_DECL_ABC(Encoder, BufferedStream);
    UTL_CLASS_NO_COPY;

public:
    /**
       Constructor.
       \param mode mode (see utl::io_t)
       \param stream associated stream
       \param owner (optional : true) stream owner flag
    */
    Encoder(uint_t mode, Stream* stream, bool owner = true)
    {
        set(mode, stream, owner);
    }

    /// \name Stream Public Method Overrides
    //@{
    virtual void close();
    //@}

    /// \name Accessors
    //@{
    /** Get the CRC code. */
    uint32_t
    getCRC() const
    {
        return isCRC() ? _crc.get() : _lastCRC;
    }

    /** Get the last CRC code. */
    uint32_t
    getLastCRC() const
    {
        return _lastCRC;
    }

    /** Get the \b crc flag. */
    bool
    isCRC() const
    {
        return getFlag(flg_crc);
    }

    /** Set the \b crc flag. */
    void
    setCRC(bool crc)
    {
        setFlag(flg_crc, crc);
    }
    //@}

    /**
       Decode data from the associated stream.
       \return number of bytes decoded
       \param block block to decode into
       \param num size of block
    */
    virtual size_t decode(byte_t* block, size_t num) = 0;

    /**
       Encode data to the associated stream.
       \return number of bytes encoded
       \param block block to encode
       \param num size of block
    */
    virtual size_t encode(const byte_t* block, size_t num) = 0;

    /**
       Set a new configuration.
       \param mode mode (see utl::io_t)
       \param stream associated stream
       \param owner (optional : true) stream owner flag
       \param bufSize (optional : 16K) buffer size
    */
    void set(uint_t mode, Stream* stream, bool owner = true, size_t bufSize = KB(16));

protected:
    virtual void clear();

    /// \name Accessors
    //@{
    /** Get the \b lastBlock flag. */
    bool
    isLastBlock() const
    {
        return getFlag(flg_lastBlock);
    }

    /** Set the \b lastBlock flag. */
    void
    setLastBlock(bool lastBlock)
    {
        setFlag(flg_lastBlock, lastBlock);
    }
    //@}

    /** Encode the write buffer to the stream (by calling encode()). */
    virtual void overflow();

    /** Decode from the stream into the read buffer (by calling decode()). */
    virtual void underflow();

    /** Perform any necessary work when encoding is finished. */
    virtual void finishEncoding();

    /** Perform any necessary work when decoding is finished. */
    virtual size_t finishDecoding();

private:
    void
    init()
    {
        _lastCRC = 0;
    }
    void
    deInit()
    {
    }

private:
    enum flg_t
    {
        flg_lastBlock = 8,
        flg_crc
    };

private:
    CRC32 _crc;
    uint32_t _lastCRC;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
