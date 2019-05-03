#include <libutl/libutl.h>
#include <libutl/HuffmanEncoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::HuffmanEncoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
HuffmanEncoder::decode(byte_t* block, size_t num)
{
    uint_t numDecoded = 0;
    while (numDecoded < num)
    {
        uint_t symbol = decode();
        if (symbol == _eob)
        {
            setEOF(true);
            break;
        }
        block[numDecoded++] = symbol;
    }
    return numDecoded;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
HuffmanEncoder::encode(const byte_t* block, size_t num)
{
    for (uint_t i = 0; i < num; i++)
    {
        encode(block[i]);
    }
    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::start(
    uint_t mode, Stream* stream, bool owner, uint_t numSymbols, uint_t incLimit, bool eob)
{
    clear();
    set(mode, stream, owner);
    setError(false);
    _numSymbols = numSymbols;
    _incLimit = incLimit;
    _eob = eob ? (numSymbols - 1) : uint_t_max;
    _left = new uint_t[numSymbols];
    _right = new uint_t[numSymbols];
    _up = new uint_t[2 * numSymbols];
    _freq = new uint_t[2 * numSymbols];
    uint_t i, lim;
    for (lim = 2 * _numSymbols, i = 2; i < lim; i++)
    {
        _up[i] = i / 2;
        _freq[i] = 1;
    }
    for (lim = _numSymbols, i = 1; i < lim; i++)
    {
        _left[i] = 2 * i;
        _right[i] = 2 * i + 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::clear()
{
    super::clear();
    clearSelf();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::finishEncoding()
{
    if (_eob != uint_t_max)
    {
        encode(_eob);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::init()
{
    _numSymbols = 0;
    _incLimit = 0;
    _eob = uint_t_max;
    _up = _freq = _left = _right = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::clearSelf()
{
    _numSymbols = 0;
    _incLimit = 0;
    _eob = uint_t_max;
    delete _up;
    _up = nullptr;
    delete _freq;
    _freq = nullptr;
    delete _left;
    _left = nullptr;
    delete _right;
    _right = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::updateFreq(uint_t a, uint_t b)
{
    do
    {
        _freq[_up[a]] = _freq[a] + _freq[b];
        a = _up[a];
        if (a != 1)
        {
            if (_left[_up[a]] == a)
                b = _right[_up[a]];
            else
                b = _left[_up[a]];
        }
    } while (a != 1);
    while (_freq[1] > _incLimit)
    {
        uint_t limit = 2 * _numSymbols;
        for (a = 1; a < limit; a++)
        {
            _freq[a] += 1;
            _freq[a] >>= 1;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HuffmanEncoder::updateModel(uint_t symbol)
{
    uint_t a, b, c, ua, uua;
    a = _numSymbols + symbol;
    _freq[a]++;
    if (_up[a] != 1)
    {
        ua = _up[a];
        if (_left[ua] == a)
            updateFreq(a, _right[ua]);
        else
            updateFreq(a, _left[ua]);
        do
        {
            uua = _up[ua];
            if (_left[uua] == ua)
                b = _right[uua];
            else
                b = _left[uua];
            if (_freq[a] > _freq[b])
            {
                if (_left[uua] == ua)
                    _right[uua] = a;
                else
                    _left[uua] = a;
                if (_left[ua] == a)
                {
                    _left[ua] = b;
                    c = _right[ua];
                }
                else
                {
                    _right[ua] = b;
                    c = _left[ua];
                }
                _up[b] = ua;
                _up[a] = uua;
                updateFreq(b, c);
                a = b;
            }
            a = _up[a];
            ua = _up[a];
        } while (ua != 1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
