#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Stream.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class BufferedStream;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BufferedStream& (*bsmanip0)(BufferedStream& stream);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Buffered stream.

   BufferedStream provides input and/or output buffering for a stream.

   <b>Attributes</b>

   \arg <b><i>lineBuffered</i> flag</b> : For a line-buffered stream, the utl::endl manipulator
   will flush the stream's output.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BufferedStream : public Stream
{
    UTL_CLASS_DECL(BufferedStream, Stream);

public:
    /**
       Constructor.
       \param stream buffered stream
       \param streamOwner (optional : true) stream is owned?
       \param iBufSize (optional : 16 kb) input buffer size
       \param oBufSize (optional : 16 kb) output buffer size
    */
    BufferedStream(Stream* stream,
                   bool streamOwner = true,
                   size_t iBufSize = KB(16),
                   size_t oBufSize = KB(16))
    {
        init();
        setStream(stream, streamOwner, iBufSize, oBufSize);
    }

    virtual void copy(const Object& rhs);

    virtual void close();

    /// \name Stream Status Overrides
    //@{
    virtual bool isInput() const;
    virtual void setInput(bool input);
    virtual bool isOutput() const;
    virtual void setOutput(bool output);
    virtual bool eof() const;
    virtual void setEOF(bool eof);
    virtual bool error() const;
    virtual void setError(bool error);
    //@}

    /** Get the stream. */
    const Stream*
    getStream() const
    {
        return _stream;
    }

    /** Get the stream. */
    Stream*
    getStream()
    {
        return _stream;
    }

    /**
       Set the attached stream.
       \param stream buffered stream
       \param streamOwner (optional : true) stream is owned?
       \param iBufSize (optional : 16 kb) input buffer size
       \param oBufSize (optional : 16 kb) output buffer size
    */
    void setStream(Stream* stream,
                   bool streamOwner = true,
                   size_t iBufSize = KB(16),
                   size_t oBufSize = KB(16));

    /** Steal the stream. */
    Stream* takeStream();

    /**
       Determine whether an EOF condition exists.  If an EOF condition doesn't exist, attempt to
       read data from the stream.  If no data can be read into the buffer, return true, otherwise
       return false.
    */
    bool eofBlocking();

    /**
       Set the mode.
       \param mode I/O mode (see utl::io_t)
       \param p_setBufs (optional : true) set default buffer sizes?
    */
    void
    setMode(uint_t mode, bool p_setBufs = true)
    {
        setInput((mode & io_rd) != 0);
        setOutput((mode & io_wr) != 0);
        if (p_setBufs)
            setBufs();
    }

    /// \name Input
    //@{
    void
    get(byte_t& b)
    {
        ASSERTD(isInput());
        _inCount++;
        if (_iBufPos == _iBufLim)
            underflow();
        b = _iBuf[_iBufPos++];
    }

    void
    get(char& c)
    {
        get((byte_t&)c);
    }

    byte_t
    get()
    {
        byte_t b;
        get(b);
        return b;
    }

    /** Peek at the next byte to be read from the stream. */
    byte_t peek();

    virtual Stream& readLine(String& str);

    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max);

    /** Un-get a byte.  You may not un-get more than one byte in a row. */
    void
    unget(byte_t b)
    {
        ASSERTD(_iBufPos > 0);
        _inCount--;
        _iBuf[--_iBufPos] = b;
    }

    /** Unget a char.  You may not un-get more than one char in a row. */
    void
    unget(char c)
    {
        unget((byte_t)c);
    }
    //@}

    /// \name Output
    //@{
    /** Write a newline. */
    virtual BufferedStream&
    newline()
    {
        return newline(false);
    }

    /** Write a newline. */
    BufferedStream&
    newline(bool forceFlush)
    {
        put((byte_t)'\n');
        setBOL(true);
        if (forceFlush || isLineBuffered())
            flush();
        return self;
    }

    /** Write the given byte. */
    BufferedStream&
    put(byte_t b)
    {
        ASSERTD(isOutput());
        _outCount++;
        _oBuf[_oBufPos++] = b;
        if (_oBufPos == _oBuf.size())
            overflow();
        return self;
    }

    /** Write the given character. */
    BufferedStream&
    put(char c)
    {
        if (c == '\n')
            return newline();
        else
        {
            indentIfBOL();
            return put((byte_t)c);
        }
    }

    /** Write the given character. */
    BufferedStream&
    put(int c)
    {
        return put((byte_t)c);
    }

    virtual void write(const byte_t* array, size_t num);
    //@}

    /// \name Buffering
    //@{
    /** Has unread input? */
    bool
    hasInput() const
    {
        return (_iBufPos < _iBufLim);
    }

    virtual BufferedStream& flush(uint_t mode = io_wr);

    /** Determine whether the stream is line-buffered. */
    bool
    isLineBuffered() const
    {
        return getFlag(io_lineBuffered);
    }

    /** Set the line-buffered flag. */
    void
    setLineBuffered(bool lineBuffered)
    {
        setFlag(io_lineBuffered, lineBuffered);
    }

    /**
       Set the buffers for input and/or output.
       \param iBufSize (optional : 16 kb) input buffer size
       \param oBufSize (optional : 16 kb) output buffer size
    */
    void setBufs(size_t iBufSize = KB(16), size_t oBufSize = KB(16));

    /**
       Set the input buffer size.
       \param size (optional : 16 kilobytes) buffer size
    */
    void setInputBuf(size_t size = KB(16));

    /**
       Set the output buffer size.
       \param size (optional : 16 kilobytes) buffer size
    */
    void setOutputBuf(size_t size = KB(16));
    //@}

    /// \name I/O Operators
    //@{
    BufferedStream&
    operator<<(bsmanip0 manip)
    {
        return manip(self);
    }
    BufferedStream&
    operator<<(void* ptr)
    {
        static_cast<Stream&>(self) << ptr;
        return self;
    }
    BufferedStream&
    operator<<(const char* str)
    {
        static_cast<Stream&>(self) << str;
        return self;
    }
    BufferedStream&
    operator<<(char c)
    {
        static_cast<Stream&>(self) << c;
        return self;
    }
    BufferedStream&
    operator>>(char& c)
    {
        static_cast<Stream&>(self) >> c;
        return self;
    }
    BufferedStream&
    operator<<(byte_t b)
    {
        static_cast<Stream&>(self) << b;
        return self;
    }
    BufferedStream&
    operator>>(byte_t& b)
    {
        static_cast<Stream&>(self) >> b;
        return self;
    }
    BufferedStream&
    operator<<(short n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(short& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    BufferedStream&
    operator<<(ushort_t n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(ushort_t& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    BufferedStream&
    operator<<(int n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(int& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    BufferedStream&
    operator<<(uint_t n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(uint_t& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    BufferedStream&
    operator<<(long n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(long& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    BufferedStream&
    operator<<(ulong_t n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(ulong_t& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
#if UTL_SIZEOF_LONG == 4
    BufferedStream&
    operator<<(int64_t n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(int64_t& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    BufferedStream&
    operator<<(uint64_t n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(uint64_t& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
#endif
    BufferedStream&
    operator<<(double n)
    {
        static_cast<Stream&>(self) << n;
        return self;
    }
    BufferedStream&
    operator>>(double& n)
    {
        static_cast<Stream&>(self) >> n;
        return self;
    }
    //@}
protected:
    virtual void clear();

    /// \name Input
    //@{
    /**
       Read from the underlying stream into the input buffer.
       Upon return, _iBufPos = 0 and _iBufLim = number of bytes in the input buffer.
    */
    virtual void underflow();
    //@}

    /// \name Output
    //@{
    /**
       Write the contents of the output buffer to the underlying stream.
       Upon return, _oBufPos = 0.
    */
    virtual void overflow();
    //@}
protected:
    // input buffer
    size_t _iBufPos, _iBufLim;
    Vector<byte_t> _iBuf;

    // output buffer
    size_t _oBufPos;
    Vector<byte_t> _oBuf;

    // buffered stream
    Stream* _stream;

private:
    void init();
    void deInit();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Output a newline to the stream.
   \ingroup io
*/
inline utl::BufferedStream&
endl(utl::BufferedStream& stream)
{
    return stream.newline(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Output a newline to the stream and flush its output.
   \ingroup io
*/
inline utl::BufferedStream&
endlf(utl::BufferedStream& stream)
{
    return stream.newline(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Flush the stream's output.
   \ingroup io
*/
inline utl::BufferedStream&
flush(utl::BufferedStream& stream)
{
    return stream.flush();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Output an object's string representation to a stream
   (with Object::toString()).
   \ingroup io
   \return output stream
   \param lhs stream to output to
   \param rhs object to output
*/
utl::BufferedStream& operator<<(utl::BufferedStream& lhs, const utl::Object& rhs);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Read a string from the given input stream.
   \ingroup io
   \return reference to input stream
   \param lhs input stream
   \param rhs output string
*/
utl::BufferedStream& operator>>(utl::BufferedStream& lhs, utl::String& rhs);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Write a string to the given output stream.
   \ingroup io
   \return reference to output stream.
   \param lhs output stream
   \param rhs input string
*/
utl::BufferedStream& operator<<(utl::BufferedStream& lhs, const utl::String& rhs);
