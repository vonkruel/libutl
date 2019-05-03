#include <libutl/libutl.h>
#include <libutl/Encoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::Encoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Encoder::close()
{
    if (_stream == nullptr)
        return;

    if (isOutput())
    {
        // encode remaining data
        flush();

        // finish encoding
        finishEncoding();

        // if connected stream is an encoder, close it also
        if (_stream->isA(Encoder))
        {
            _stream->close();
        }
    }
    _lastCRC = _crc.get();
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Encoder::set(uint_t mode, Stream* stream, bool owner, size_t bufSize)
{
    if (stream == nullptr)
    {
        setStream(nullptr, false);
        ASSERTD(error());
    }
    else
    {
        stream->setMode(mode);
        setStream(stream, owner, (mode & io_rd) ? bufSize : 0, (mode & io_wr) ? bufSize : 0);
        ASSERTD(!error());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Encoder::clear()
{
    if (_stream == nullptr)
    {
        super::clear();
        setLastBlock(false);
        setCRC(false);
        _crc.clear();
    }
    else
    {
        setStream(nullptr);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Encoder::overflow()
{
    if (_oBufPos == 0)
        return;

    size_t num = encode(_oBuf, _oBufPos);

    if (isCRC())
    {
        const byte_t* block = _oBuf.get();
        for (size_t i = 0; i < num; i++)
        {
            _crc.add(block[i]);
        }
    }

    memmove(_oBuf.get(), _oBuf.get() + num, _oBufPos - num);
    _oBufPos -= num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Encoder::underflow()
{
    _iBufPos = 0;
    _iBufLim = 0;

    // try to decode
    try
    {
        _iBufLim = decode(_iBuf, _iBuf.size());
    }
    catch (StreamEOFex&)
    {
        // don't re-throw unless finishDecoding() comes up empty
        ASSERTD(_iBufLim == 0);
        _iBufLim = finishDecoding();
        if (_iBufLim == 0)
            throw;
    }

    if (isCRC())
    {
        const byte_t* block = _iBuf.get();
        for (size_t i = 0; i < _iBufLim; i++)
        {
            _crc.add(block[i]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Encoder::finishEncoding()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Encoder::finishDecoding()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
