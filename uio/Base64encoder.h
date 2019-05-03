#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Encoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Base64encode ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Encode binary data using Base64 (the MIME / RFC 2045 encoding).

   This encoder doesn't place a limit on line-length, so if you want to limit line-length, you
   need to feed line-sized chunks to the encoder and insert the newlines yourself.  You would
   feed data to the encoder in multiples of 3 bytes.  3 bytes of input map to 4 base-64 (6-bit)
   digits.  For example if you write a newline after every 60 bytes of input, you'll have
   80 base-64 digits per output line.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Base64encode : public Object
{
    UTL_CLASS_DECL(Base64encode, Object);
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_COPY;

public:
    /**
       Constructor.
       \param os output stream for encoded data
       \param owner (optional : false) ownership flag for output stream
    */
    Base64encode(utl::Stream* os, bool owner = false)
    {
        init();
        setOutputStream(os, owner);
    }

    /** Get the output stream. */
    Stream*
    stream() const
    {
        return _os;
    }

    /** Get the ownership flag for the output stream. */
    bool
    streamOwner() const
    {
        return _owner;
    }

    /**
       Set output stream.
       \param os output stream
       \param owner ownership flag for output stream
    */
    void
    setOutputStream(Stream* os, bool owner = false)
    {
        if (_owner)
            delete _os;
        _os = os;
        _owner = owner;
    }

    /** Process the provided data. */
    void process(const byte_t* data, size_t dataLen);

    /** Complete the encoding process. */
    void finalize();

private:
    void
    init()
    {
        _dataLen = 0;
        _owner = false;
        _os = nullptr;
    }
    void
    deInit()
    {
        try
        {
            finalize();
        }
        catch (...)
        {
        }
        setOutputStream(nullptr);
    }

private:
    byte_t _data[3];
    byte_t _dataLen;
    byte_t _owner;
    utl::Stream* _os;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Base64decode ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Decode binary data that was encoded using Base64 (the MIME / RFC 2045 encoding).

   This decoder doesn't handle newlines in the input data, so you have to handle them yourself
   if they are present.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Base64decode : public Object
{
    UTL_CLASS_DECL(Base64decode, Object);
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_COPY;

public:
    /**
       Constructor.
       \param os output stream for decoded data
       \param owner (optional : false) ownership flag for output stream
    */
    Base64decode(utl::Stream* os, bool owner = false)
    {
        init();
        setOutputStream(os, owner);
    }

    /** Get the output stream. */
    Stream*
    stream() const
    {
        return _os;
    }

    /** Get the ownership flag for the output stream. */
    bool
    streamOwner() const
    {
        return _owner;
    }

    /**
       Set output stream.
       \param os output stream
       \param owner ownership flag for output stream
    */
    void
    setOutputStream(Stream* os, bool owner = false)
    {
        if (_owner)
            delete _os;
        _os = os;
        _owner = owner;
    }

    /** Process the provided data. */
    void process(const byte_t* data, size_t dataLen);

    /** Complete the decoding process. */
    void finalize();

private:
    void init();
    void
    deInit()
    {
        try
        {
            finalize();
        }
        catch (...)
        {
        }
        setOutputStream(nullptr);
    }

private:
    uint32_t _data;
    uint32_t _dataShift;
    byte_t _owner;
    utl::Stream* _os;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Base64encoder ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Base64 encoder/decoder stream.

   Base64encoder is a utl::Encoder which employs utl::Base64encode or utl::Base64decode
   to perform base64 encoding or decoding of the data that passes through it.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Base64encoder : public Encoder
{
    UTL_CLASS_DECL(Base64encoder, Encoder);

public:
    /**
       Constructor.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream associated stream
       \param owner (optional : true) \b owner flag for stream
    */
    Base64encoder(uint_t mode, Stream* stream, bool owner = true)
    {
        init();
        start(mode, stream, owner);
    }

    virtual size_t encode(const byte_t* block, size_t num);

    virtual size_t decode(byte_t* block, size_t num);

    /**
       Initialize for encoding or decoding.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream associated stream
       \param owner (optional : true) \b owner flag for stream
    */
    void start(uint_t mode, Stream* stream, bool owner = true);

protected:
    virtual void clear();
    virtual void finishEncoding();
    virtual size_t finishDecoding();

private:
    void
    init()
    {
        _codec.object = nullptr;
    }
    void
    deInit()
    {
        close();
    }

private:
    union {
        Object* object;
        Base64encode* encode;
        Base64decode* decode;
    } _codec;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
