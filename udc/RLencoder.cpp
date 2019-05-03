#include <libutl/libutl.h>
#include <libutl/RLencoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::RLencoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
RLencoder::decode(byte_t* block, size_t num)
{
    bool eob = getEOB();
    uint_t i, oBufPos = 0;
    int c, c2 = int_t_min;
    while (oBufPos < num)
    {
        // stop if we can't accommodate a full run
        if ((num - oBufPos) < _maxRunLen)
        {
            if (c2 != int_t_min)
            {
                _saveC2 = c2;
            }
            break;
        }

        if (_saveC2 != int_t_min)
        {
            c2 = _saveC2;
            _saveC2 = int_t_min;
        }

        if (c2 == int_t_min)
        {
            c = _stream->get();
        }
        else
        {
            c = c2;
            c2 = int_t_min;
        }
        uint_t runLen = 1;
        for (;;)
        {
            c2 = _stream->get();
            if (c2 == c)
            {
                if (++runLen == _minRunLen)
                {
                    c2 = int_t_min;
                    break;
                }
            }
            else
            {
                break;
            }
        }

        if (runLen == _minRunLen)
        {
            runLen += _stream->getBits(_runLenBits);
            if (eob && (c == 255) && (runLen == _maxRunLen))
            {
                setEOF(true);
                return oBufPos;
            }
        }
        for (i = 0; i < runLen; i++)
        {
            block[oBufPos++] = c;
        }
    }

    return oBufPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
RLencoder::encode(const byte_t* block, size_t num)
{
    bool eob = getEOB();
    uint_t i, iBufPos = 0;
    uint_t numLeft = num;
    while (iBufPos < num)
    {
        if ((numLeft < _maxRunLen) && !isLastBlock())
        {
            break;
        }
        byte_t c = block[iBufPos++];
        numLeft--;
        uint_t runLen = 1;
        uint_t maxRunLen = min(_maxRunLen, numLeft);
        if (eob && (c == 255) && (maxRunLen == _maxRunLen))
        {
            maxRunLen--;
        }
        while ((block[iBufPos] == c) && (numLeft > 0))
        {
            iBufPos++;
            numLeft--;
            if (++runLen == maxRunLen)
            {
                break;
            }
        }
        uint_t lim = min(runLen, _minRunLen);
        for (i = 0; i < lim; i++)
        {
            _stream->put(c);
        }
        if (i == _minRunLen)
        {
            _stream->putBits(runLen - _minRunLen, _runLenBits);
        }
    }
    return iBufPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RLencoder::start(
    uint_t mode, Stream* stream, bool owner, uint_t minRunLen, uint_t runLenBits, bool eob)
{
    clear();
    set(mode, stream, owner);
    setError(false);
    _minRunLen = minRunLen;
    _runLenBits = runLenBits;
    _maxRunLen = (1 << _runLenBits) - 1 + _minRunLen;
    setEOB(eob);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RLencoder::clear()
{
    super::clear();
    clearSelf();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RLencoder::finishEncoding()
{
    setLastBlock(true);
    encode(_oBuf, _oBufPos);
    if (getEOB())
    {
        for (uint_t i = 0; i < _minRunLen; i++)
        {
            _stream->put((byte_t)255);
        }
        _stream->putBits(_maxRunLen - _minRunLen, _runLenBits);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RLencoder::init()
{
    clearSelf();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RLencoder::clearSelf()
{
    _minRunLen = 0;
    _maxRunLen = 0;
    _runLenBits = 0;
    _saveC2 = int_t_min;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
