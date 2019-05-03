#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Exception.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class String;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Stream flags.
   \ingroup io
*/
enum ios_flg_t
{
    io_eof = 3,         /**< end-of-file */
    io_error = 4,       /**< I/O error */
    io_bol = 5,         /**< begin-of-line */
    io_lineBuffered = 6 /**< line-buffered */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Stream& (*smanip0)(Stream& stream);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Stream I/O abstraction.

   A stream is anything you want to read data from or write data to.  For example, a disk file is
   a stream and so is a TCP socket.

   <b>Attributes</b>

   \arg <b><i>begin-of-line</i> flag</b> : If true, indicates that the last character written to
   the stream was a newline.

   \arg <b><i>lineBuffered</i> flag</b> : For a line-buffered stream, the utl::endl manipulator
   will flush the stream's output.

   \arg \b base : When an integer is written to the stream, it will be shown in the given \b base
   (10 by default).

   \arg \b name : A stream's name is sometimes useful to know.  For example, for a FileStream,
   the name should be the path to the open file.  For a Socket, the name should be the FQDN/IP
   address of the other end.  This allows exceptions on the stream to be more informative.

   \arg \b inCount : Count of input bytes.

   \arg \b outCount : Count of output bytes.

   \arg \b indent : Indent each output line by \b indent spaces.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Stream : public Object, protected FlagsMI
{
    UTL_CLASS_DECL_ABC(Stream, Object);

public:
    virtual void copy(const Object& rhs);

    /** Close the stream. */
    virtual void close() = 0;

    /// \name Accessors
    //@{
    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Set the \b owner flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /** Get the count of input bytes. */
    size_t
    getInputCount() const
    {
        return _inCount;
    }

    /** Get the count of output bytes. */
    size_t
    getOutputCount() const
    {
        return _outCount;
    }

    /** Get the current mode. */
    uint_t
    getMode() const
    {
        uint_t res = 0;
        if (isInput())
            res |= io_rd;
        if (isOutput())
            res |= io_wr;
        return res;
    }

    /** Get the stream's name. */
    const String& getName() const;

    /** Get the stream's name. */
    const String* getNamePtr() const;

    /** Set the name. */
    void setName(const String& name);

    /** Set the name. */
    void setName(String* name);

    /** Get the begin-of-line flag. */
    virtual bool isBOL() const;

    /** Set the BOL flag. */
    virtual void setBOL(bool p_bol);

    /** Determine whether the stream is an input stream. */
    virtual bool isInput() const;

    /** Set the input mode flag. */
    virtual void setInput(bool input);

    /** Determine whether the stream is an output stream. */
    virtual bool isOutput() const;

    /** Set the output mode flag. */
    virtual void setOutput(bool output);

    /** Determine whether the stream is an input/output stream. */
    bool
    isRDWR() const
    {
        return (isInput() && isOutput());
    }

    /** Set the base, for writing integer values. */
    void
    setBase(uint_t base)
    {
        _base = base;
    }

    /**
       Set the mode.
       \param mode I/O mode (see utl::io_t)
    */
    void
    setMode(uint_t mode)
    {
        setInput((mode & io_rd) != 0);
        setOutput((mode & io_wr) != 0);
    }
    //@}

    /// \name Auto-Indentation
    //@{
    /** Get the indentation level. */
    uint_t
    getIndent() const
    {
        return _indent;
    }

    /** Set the indentation level. */
    void
    setIndent(uint_t indent)
    {
        _indent = indent;
    }

    /** Increase indentation by the given number of spaces. */
    void
    indent(uint_t num = 4)
    {
        _indent += num;
    }

    /** Decrease indentation by the given number of spaces. */
    void
    unindent(uint_t num = 4)
    {
        if (num > _indent)
            _indent = 0;
        else
            _indent -= num;
    }
    //@}

    /// \name Error Conditions
    //@{
    /**
       If there is an error condition (error()), throw StreamErrorEx.
       If there is an EOF condition (eof()), throw StreamEOFex.
    */
    void checkOK();

    /** Clear the EOF condition. */
    void clearEOF();

    /** Clear the error condition. */
    void clearError();

    /** Determine whether an EOF condition exists. */
    virtual bool eof() const;

    /** Set the EOF flag. */
    virtual void setEOF(bool p_eof);

    /** Determine whether an error condition exists. */
    virtual bool error() const;

    /** Set the error flag. */
    virtual void setError(bool p_error);

    /** Determine whether an error condition exists. */
    bool ok() const;
    //@}

    /// \name Input
    //@{
    /**
       Copy data from another stream.
       \return number of bytes successfully copied
       \param in input stream
       \param numBytes number of bytes to copy
       \param bufSize buffer size
    */
    size_t copyData(Stream& in, size_t numBytes = size_t_max, size_t bufSize = KB(4));

    /**
       Get a single byte.
       \param val byte reference
    */
    void
    get(byte_t& val)
    {
        read(&val, 1);
    }

    /**
       Get a single char.
       \param val char reference
    */
    void
    get(char& val)
    {
        get((byte_t&)val);
    }

    /**
       Get a single byte.
       \return byte
    */
    byte_t
    get()
    {
        byte_t b;
        get(b);
        return b;
    }

    /**
       Get a single bit.
       \return true if bit == 1, false if bit == 0
    */
    bool
    getBit()
    {
        if (_bitMask == 0x80)
            get(_bitByte);
        bool res = ((_bitByte & _bitMask) != 0);
        _bitMask >>= 1;
        if (_bitMask == 0)
            _bitMask = 0x80;
        return res;
    }

    /**
       Get multiple bits.
       \return 32-bit word (bit 0 = last bit)
       \param numBits number of bits to read
    */
    uint32_t
    getBits(uint_t numBits)
    {
        ASSERTD(numBits <= 32);
        uint32_t res = 0, mask = 1 << (numBits - 1);
        while (mask != 0)
        {
            if (getBit())
                res |= mask;
            mask >>= 1;
        }
        return res;
    }

    /** Read a line from stream into the given String object. */
    virtual Stream& readLine(String& str);

    /**
       Read data into a given buffer.
       \return number of bytes read
       \param array buffer to read into
       \param maxBytes maximum number of bytes to read
       \param minBytes (optional : maxBytes) minimum number of bytes to read
    */
    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max) = 0;
    //@}

    /// \name Output
    //@{
    /** Write the given character. */
    Stream&
    put(byte_t b)
    {
        ASSERTD(isOutput());
        write(&b, 1);
        return self;
    }

    /** Write the given character. */
    Stream&
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
    Stream&
    put(int c)
    {
        return put((byte_t)c);
    }

    /** Write the given string. */
    Stream& put(const char* str);

    /** Write the given string. */
    Stream& put(const char* str, size_t len);

    /** Write the given bit. */
    Stream&
    putBit(bool b)
    {
        if (b)
            _bitByte |= _bitMask;
        _bitMask >>= 1;
        if (!_bitMask)
        {
            put(_bitByte);
            _bitMask = 0x80;
            _bitByte = 0;
        }
        return self;
    }

    /** Write all outstanding bits. */
    Stream&
    putBits()
    {
        while (_bitMask != 0x80)
            putBit(false);
        return self;
    }

    /**
       Write multiple bits.
       \param n bits to write (bit 0 is last bit)
       \param numBits number of bits
    */
    Stream&
    putBits(uint32_t n, uint_t numBits)
    {
        uint32_t mask = 1 << (numBits - 1);
        while (mask != 0)
        {
            putBit((n & mask) != 0);
            mask >>= 1;
        }
        return self;
    }

    /** Write a newline. */
    virtual Stream&
    newline()
    {
        put((byte_t)'\n');
        setBOL(true);
        return self;
    }

    /** Write the given string, followed by a newline. */
    Stream&
    putLine(const char* str)
    {
        put(str);
        return newline();
    }

    /** Write spaces. */
    void writeSpaces(size_t num);

    /**
       Write a sequence of bytes.
       \param array data to write
       \param num number of bytes to write
    */
    virtual void write(const byte_t* array, size_t num) = 0;
    //@}

    /**
       Flush the stream (if it is buffered).
       \param mode see utl::io_t
    */
    virtual Stream&
    flush(uint_t mode = io_wr)
    {
        return self;
    }

    /// \name I/O Operators
    //@{
    /** Execute a manipulator (e.g. endl, endlf, flush). */
    Stream&
    operator<<(smanip0 manip)
    {
        return manip(self);
    }

    /** Write a void pointer. */
    Stream& operator<<(void* ptr);

    /** Write a string. */
    Stream& operator<<(const char* str);

    // char
    Stream&
    operator<<(char c)
    {
        put(c);
        return self;
    }
    Stream&
    operator>>(char& c)
    {
        get(c);
        return self;
    }

    // byte
    Stream&
    operator<<(byte_t b)
    {
        put(b);
        return self;
    }
    Stream&
    operator>>(byte_t& b)
    {
        get(b);
        return self;
    }

    // int16_t
    Stream& operator<<(int16_t n);
    Stream& operator>>(int16_t& n);

    // uint16_t
    Stream& operator<<(uint16_t n);
    Stream& operator>>(uint16_t& n);

    // int32_t
    Stream& operator<<(int32_t n);
    Stream& operator>>(int32_t& n);

    // uint32_t
    Stream& operator<<(uint32_t n);
    Stream& operator>>(uint32_t& n);

    // long
    Stream& operator<<(long n);
    Stream& operator>>(long& n);

    // ulong_t
    Stream& operator<<(ulong_t n);
    Stream& operator>>(ulong_t& n);

#if UTL_SIZEOF_LONG == 4
    // int64_t
    Stream& operator<<(int64_t n);
    Stream& operator>>(int64_t& n);

    // uint64_t
    Stream& operator<<(uint64_t n);
    Stream& operator>>(uint64_t& n);
#endif

    // double
    Stream& operator<<(double n);
    Stream& operator>>(double& n);
    //@}

    static void utl_init();

protected:
    virtual void clear();

    /// \name Auto-Indentation
    //@{
    /** If \b begin-of-line flag is true, clear the flag and indent. */
    inline void
    indentIfBOL()
    {
        if (!isBOL())
            return;
        setBOL(false);
        _indentIfBOL();
    }

    /** Write _indent spaces at beginning of line. */
    void
    _indentIfBOL()
    {
        uint_t indent = _indent;
        while (indent > 256)
        {
            write(spaces, 256);
            indent -= 256;
        }
        write(spaces, indent);
    }
    //@}

    /// \name Error Conditions
    //@{
    /** Throw a StreamEOFex exception. */
    void throwStreamEOFex();

    /** Throw a StreamErrorEx exception. */
    void throwStreamErrorEx();
    //@}

    /// \name Input
    //@{
    /**
       Read a token from the stream.
       \param buf token buffer
       \param size size of buffer
    */
    void readToken(char* buf, size_t size);

    /**
       Read until a whitespace character is encountered.
       The whitespace character will be put back into the input buffer.
       \param buf buffer to read into
       \param size size of buffer
    */
    void readUntilWS(char* buf, size_t size);

    /** Skip past whitespace. */
    byte_t skipWS();
    //@}

    // track # input/output bytes
    size_t _inCount;
    size_t _outCount;
    // bit i/o
    byte_t _bitMask;
    byte_t _bitByte;

private:
    static byte_t spaces[256];
    enum flg_t
    {
        flg_owner
    };

private:
    void init();
    void deInit();

private:
    String* _name;
    uint_t _base;
    uint_t _indent;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Output a newline to the stream.
   \ingroup io
*/
inline utl::Stream&
endl(utl::Stream& stream)
{
    return stream.put('\n');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Output a newline to the stream and flush its output.
   \ingroup io
*/
inline utl::Stream&
endlf(utl::Stream& stream)
{
    stream.newline();
    return stream.flush();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Flush the stream's output.
   \ingroup io
*/
inline utl::Stream&
flush(utl::Stream& stream)
{
    return stream.flush();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Output an object's string representation to a stream (with Object::toString()).
   \ingroup io
   \return output stream
   \param lhs stream to output to
   \param rhs object to output
*/
utl::Stream& operator<<(utl::Stream& lhs, const utl::Object& rhs);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Read a string from the given input stream.
   \ingroup io
   \return reference to input stream
   \param lhs input stream
   \param rhs output string
*/
utl::Stream& operator>>(utl::Stream& lhs, utl::String& rhs);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Write a string to the given output stream.
   \ingroup io
   \return reference to output stream.
   \param lhs output stream
   \param rhs input string
*/
utl::Stream& operator<<(utl::Stream& lhs, const utl::String& rhs);
