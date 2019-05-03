#include <libutl/libutl.h>
#include <libutl/BufferedStream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BufferedStream);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::copy(const Object& rhs)
{
    auto& stream = utl::cast<BufferedStream>(rhs);
    super::copy(stream);
    _iBufPos = stream._iBufPos;
    _iBufLim = stream._iBufLim;
    _iBuf = stream._iBuf;
    _oBufPos = stream._oBufPos;
    _oBuf = stream._oBuf;
    _stream = stream._stream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::close()
{
    setStream(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BufferedStream::isInput() const
{
    if (_stream == nullptr)
        return super::isInput();
    return _stream->isInput();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setInput(bool input)
{
    if (_stream == nullptr)
        super::setInput(input);
    else
        _stream->setInput(input);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BufferedStream::isOutput() const
{
    if (_stream == nullptr)
        return super::isOutput();
    return _stream->isOutput();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setOutput(bool output)
{
    if (_stream == nullptr)
        super::setOutput(output);
    else
        _stream->setOutput(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BufferedStream::eof() const
{
    if (_iBufPos < _iBufLim)
        return false;
    return (Stream::eof() || ((_stream != nullptr) && _stream->eof()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setEOF(bool eof)
{
    super::setEOF(eof);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BufferedStream::error() const
{
    if (_stream == nullptr)
        return super::error();
    return _stream->error();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setError(bool error)
{
    if (_stream == nullptr)
        super::setError(error);
    else
        _stream->setError(error);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setStream(Stream* stream, bool streamOwner, size_t iBufSize, size_t oBufSize)
{
    // fix streamOwner flag
    if (stream == nullptr)
        streamOwner = false;

    // same stream -> do nothing
    if (stream == _stream)
    {
        ASSERTD(streamOwner == isOwner());
        return;
    }

    // nuke existing stream
    if (_stream != nullptr)
    {
        try
        {
            flush(io_rdwr);
        }
        catch (Exception&)
        {
        }
        if (isOwner())
            delete _stream;
        _stream = nullptr;
        clear();
    }

    // set new stream & buffers
    _stream = stream;
    setOwner(streamOwner);
    setBufs(iBufSize, oBufSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Stream*
BufferedStream::takeStream()
{
    if (_stream == nullptr)
        return nullptr;
    ASSERTD(isOwner());
    Stream* stream = _stream;
    flush();
    setOwner(false);
    clear();
    return stream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BufferedStream::eofBlocking()
{
    try
    {
        if (_iBufPos == _iBufLim)
        {
            checkOK();
            underflow();
        }
    }
    catch (StreamEOFex&)
    {
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

byte_t
BufferedStream::peek()
{
    byte_t c = get();
    unget(c);
    return c;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Stream&
BufferedStream::readLine(String& p_str)
{
    // initialize the string
    char* str = nullptr;
    char* strPtr = nullptr;
    char* strLim = nullptr;

    SCOPE_EXIT
    {
        delete[] str;
    };

    try
    {
        // keep reading until we get newline, nul, or eof
        bool done = false;
        do
        {
            // read more data if we ran out
            if (_iBufPos == _iBufLim)
                underflow();

            // define the range of characters to be consumed: [start,end)
            byte_t* start = _iBuf.get() + _iBufPos;
            byte_t* end = start;
            byte_t* lim = _iBuf.get() + _iBufLim;

            // search 8 bytes at a time
            for (byte_t* endPlus8 = end + 8; endPlus8 <= lim; endPlus8 += 8)
            {
                byte_t c;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
                c = *end;
                if ((c == 0) || (c == '\n'))
                    goto found;
                ++end;
            }

            // one char at a time for the remainder
            for (; end < lim; ++end)
            {
                if ((*end == '\0') || (*end == '\n'))
                    goto found;
            }

            // set done flag
            goto notfound;
        found:
            done = true;
            ++end;
        notfound:

            // consume the characters from the input buffer
            size_t copyLen = (end - start);
            _iBufPos += copyLen;

            if (done)
            {
                --copyLen;
                if (str == nullptr)
                {
                    p_str.set(reinterpret_cast<char*>(start), true, true, copyLen);
                    return self;
                }
            }

            // grow the character array to receive the data
            while ((size_t)(strLim - strPtr) < (copyLen + 1))
            {
                if (str == nullptr)
                {
                    size_t sz = nextPow2(copyLen);
                    str = new char[sz];
                    strPtr = str;
                    strLim = str + sz;
                    break;
                }
                arrayGrow(str, strPtr, strLim);
            }

            // copy the data into str[]
            memcpy(strPtr, start, copyLen);
            strPtr += copyLen;
        } while (!done);
    }
    catch (StreamEOFex&)
    {
        if (str == strPtr)
        {
            throw;
        }
    }

    p_str.set(str, true, true, strPtr - str);

    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
BufferedStream::read(byte_t* array, size_t maxBytes, size_t minBytes)
{
    ASSERTD(isInput());

    // check maxBytes, fix minBytes
    if (maxBytes == 0)
        return 0;
    if (minBytes > maxBytes)
        minBytes = maxBytes;

    byte_t* iBufPtr = _iBuf + _iBufPos;
    byte_t* iBufLim = _iBuf + _iBufLim;
    byte_t* arrayPtr = array;
    byte_t* arrayMin = array + minBytes;
    byte_t* arrayLim = array + maxBytes;
    do
    {
        // re-fill the read buffer?
        if (iBufPtr == iBufLim)
        {
            try
            {
                checkOK();
                underflow();
                iBufPtr = _iBuf.get();
                iBufLim = iBufPtr + _iBufLim;
            }
            catch (StreamEOFex&)
            {
                if (arrayPtr >= arrayMin)
                    break;
                throw;
            }
        }

        // copy as much data as we can into caller's buffer
        size_t curNum = min(iBufLim - iBufPtr, arrayLim - arrayPtr);
        memcpy(arrayPtr, iBufPtr, curNum);

        // book-keeping
        arrayPtr += curNum;
        iBufPtr += curNum;
        _inCount += curNum;
    } while (arrayPtr < arrayMin);

    _iBufPos = iBufPtr - _iBuf.get();
    return (arrayPtr - array);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::write(const byte_t* array, size_t num)
{
    ASSERTD(isOutput());

    // can't be an error
    checkOK();

    _outCount += num;
    byte_t* oBufPtr = _oBuf.get() + _oBufPos;
    byte_t* oBufLim = _oBuf.get() + _oBuf.size();
    const byte_t* arrayPtr = array;
    const byte_t* arrayLim = array + num;
    while (arrayPtr < arrayLim)
    {
        size_t curNum = min(oBufLim - oBufPtr, arrayLim - arrayPtr);
        memcpy(oBufPtr, arrayPtr, curNum);
        arrayPtr += curNum;
        oBufPtr += curNum;
        if (oBufPtr == oBufLim)
        {
            _oBufPos = _oBuf.size();
            overflow();
            oBufPtr = _oBuf.get();
        }
    }
    _oBufPos = oBufPtr - _oBuf.get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BufferedStream&
BufferedStream::flush(uint_t mode)
{
    if ((mode & io_rd) != 0)
    {
        _iBufPos = _iBufLim = 0;
    }
    if (isOutput() && ((mode & io_wr) != 0))
    {
        putBits();
        overflow();
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setBufs(size_t iBufSize, size_t oBufSize)
{
    setInputBuf(isInput() ? iBufSize : 0);
    setOutputBuf(isOutput() ? oBufSize : 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setInputBuf(size_t size)
{
    _iBufPos = _iBufLim = 0;
    if (_iBuf.size() != size)
    {
        _iBuf.excise();
        _iBuf.setSize(size);
    }
    _bitMask = 0x80;
    _bitByte = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::setOutputBuf(size_t size)
{
    _oBufPos = 0;
    if (_oBuf.size() != size)
    {
        _oBuf.excise();
        _oBuf.setSize(size);
    }
    _bitMask = 0x80;
    _bitByte = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::clear()
{
    try
    {
        flush(io_rdwr);
    }
    catch (Exception&)
    {
    }
    if (isOwner())
        delete _stream;
    _stream = nullptr;
    setOwner(false);
    super::clear();
    setBufs(0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::underflow()
{
    ASSERTD(_stream != nullptr);
    _iBufPos = 0;
    _iBufLim = 0;
    _iBufLim = _stream->read(_iBuf, _iBuf.size(), 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::overflow()
{
    if (_oBufPos > 0)
    {
        ASSERTD(_stream != nullptr);
        _stream->write(_oBuf, _oBufPos);
        _oBufPos = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::init()
{
    _stream = nullptr;
    setOwner(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BufferedStream::deInit()
{
    if (_stream == nullptr)
        return;
    try
    {
        flush(io_wr);
    }
    catch (Exception&)
    {
    }
    if (isOwner())
        delete _stream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

utl::BufferedStream&
operator<<(utl::BufferedStream& lhs, const utl::Object& rhs)
{
    return lhs << rhs.toString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

utl::BufferedStream&
operator>>(utl::BufferedStream& lhs, utl::String& rhs)
{
    static_cast<utl::Stream&>(lhs) >> rhs;
    return lhs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

utl::BufferedStream&
operator<<(utl::BufferedStream& lhs, const utl::String& rhs)
{
    static_cast<utl::Stream&>(lhs) << rhs;
    return lhs;
}
