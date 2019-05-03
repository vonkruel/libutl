#include <libutl/libutl.h>
#include <libutl/ArithmeticEncoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define FEN_PREV(x) ((x) & ((x)-1))
#define FEN_NEXT(x) ((x) + ((x) & -(x)))

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ArithmeticEncoder);
UTL_CLASS_IMPL(utl::ArithContext);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
ArithmeticEncoder::decode(byte_t* block, size_t num)
{
    size_t numDecoded = 0;
    while (numDecoded < num)
    {
        uint_t symbol = decode(_ctx);
        if ((_eob != uint_t_max) && (symbol == _eob))
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
ArithmeticEncoder::encode(const byte_t* block, size_t num)
{
    for (size_t i = 0; i < num; i++)
    {
        encode(block[i], _ctx);
    }
    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithmeticEncoder::start(uint_t mode, Stream* stream, bool owner, ArithContext* ctx, bool eob)
{
    clear();
    set(mode, stream, owner);
    setError(false);
    _ctx = ctx;
    if (eob && (_ctx != nullptr))
    {
        _eob = _ctx->numSymbols() - 1;
    }
    else
    {
        _eob = uint_t_max;
    }
    _half = 1 << (31 - 1);
    _qtr = _half >> 1;
    if (isInput())
    {
        _L = 0;
        uint_t mask = _half;
        while (mask != 0)
        {
            SCOPE_FAIL
            {
                delete _ctx;
                _ctx = nullptr;
            };
            if (_stream->getBit())
            {
                _L |= mask;
            }
            mask >>= 1;
        }
        _R = _half;
    }
    else if (isOutput())
    {
        _L = 0;
        _R = _half;
        _followBits = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithmeticEncoder::clear()
{
    super::clear();
    clearSelf();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithmeticEncoder::finishEncoding()
{
    // send the EOB symbol if appropriate
    if (_eob != uint_t_max)
    {
        encode(_eob, _ctx);
    }
    // then finish encoding
    uint_t mask = _half;
    while (mask != 0)
    {
        putBit((_L & mask) != 0);
        mask >>= 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithmeticEncoder::clearSelf()
{
    _L = _R = 0;
    _followBits = 0;
    delete _ctx;
    _ctx = nullptr;
    _eob = uint_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ArithContext ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithContext::copy(const Object& rhs)
{
    auto& ctx = utl::cast<ArithContext>(rhs);
    _numSymbols = ctx._numSymbols;
    _size = ctx._size;
    _mid = ctx._mid;
    delete[] _F;
    _F = new uint_t[_size];
    memmove(_F, ctx._F, _size * sizeof(uint_t));
    _inc = ctx._inc;
    _incLimit = ctx._incLimit;
    _totFreq = ctx._totFreq;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithContext::interval(uint_t* p_low, uint_t* p_high, uint_t symbol) const
{
    uint_t low, high, shared, parent;

    // first symbol at position 1
    symbol++;

    // calculate first part of high path
    high = _F[symbol];
    parent = FEN_PREV(symbol);

    // calculate first part of low path
    symbol--;
    low = 0;
    while (symbol != parent)
    {
        low += _F[symbol];
        symbol = FEN_PREV(symbol);
    }

    // sum the shared part of the path back to root
    shared = 0;
    while (symbol > 0)
    {
        shared += _F[symbol];
        symbol = FEN_PREV(symbol);
    }

    *p_low = shared + low;
    *p_high = shared + high;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithContext::set(uint_t numSymbols, uint_t inc, uint_t incLimit)
{
    // set numSymbols, size, mid
    _numSymbols = numSymbols;
    _size = 1;
    while (_size <= _numSymbols)
    {
        _size <<= 1;
    }
    _mid = _size >> 1;

    // create _F[]
    delete[] _F;
    _F = new uint_t[_size];
    memset(_F, 0, _size * sizeof(uint_t));

    // set incLimit, totFreq
    _incLimit = incLimit;
    _totFreq = 0;

    // init _F[]
    uint_t i;
    _inc = 1;
    for (i = 0; i < _numSymbols; i++)
    {
        update(i);
    }
    _inc = inc;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
ArithContext::symbol(uint_t target)
{
    uint_t symbol = 0;
    uint_t mid = _mid;
    while (mid > 0)
    {
        if (_F[symbol + mid] <= target)
        {
            symbol += mid;
            target -= _F[symbol];
        }
        mid >>= 1;
    }
    return symbol;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithContext::update(uint_t symbol)
{
    // first symbol at position 1
    symbol++;

    _F[symbol] += _inc;
    symbol = FEN_NEXT(symbol);
    while (symbol < _size)
    {
        _F[symbol] += _inc;
        symbol = FEN_NEXT(symbol);
    }
    _totFreq += _inc;

    if (_totFreq > _incLimit)
    {
        halve();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithContext::halve()
{
    // first get frequency counts for all symbols
    uint_t i;
    uint_t* i_freq = new uint_t[_numSymbols];
    for (i = 0; i < _numSymbols; i++)
    {
        i_freq[i] = (freq(i) + 1) >> 1;
    }

    // update symbols
    memset(_F, 0, _size * sizeof(uint_t));
    _totFreq = 0;
    uint_t saveInc = _inc;
    for (i = 0; i < _numSymbols; i++)
    {
        _inc = i_freq[i];
        update(i);
    }
    _inc = saveInc;

    delete[] i_freq;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
