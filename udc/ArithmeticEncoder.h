#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Encoder.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// ArithContext ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Statistical context for ArithmeticEncoder.

   freq(), interval() and update() are all O(log n), thanks to the use of a Fenwick tree.  Thanks
   go to Alistair Moffat who clued me into this elegant technique.

   See Alistair's home page at: http://people.eng.unimelb.edu.au/ammoffat/

   \author Adam McKee
   \ingroup compression
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ArithContext : public Object
{
    UTL_CLASS_DECL(ArithContext, Object);

public:
    /**
       Constructor.
       \param numSymbols alphabet size
       \param inc symbol frequency increment
       \param incLimit halve frequency counts when totFreq()
              exceeds \b incLimit
    */
    ArithContext(uint_t numSymbols, uint_t inc, uint_t incLimit)
    {
        _F = nullptr;
        set(numSymbols, inc, incLimit);
    }

    /** Copy another ArithContext. */
    virtual void copy(const Object& rhs);

    /** Get the frequency count for the given symbol. */
    uint_t
    freq(uint_t symbol) const
    {
        uint_t low, high;
        interval(&low, &high, symbol);
        return high - low;
    }

    /**
       Return the total frequency of all symbols \b before the
       given symbol.
    */
    void interval(uint_t* low, uint_t* high, uint_t symbol) const;

    /** Return the number of symbols. */
    uint_t
    numSymbols() const
    {
        return _numSymbols;
    }

    /**
       Set a new configuration.
       \param numSymbols number of distinct symbols
       \param inc symbol frequency increment
       \param incLimit halve frequency counts when totFreq()
              exceeds \b incLimit
    */
    void set(uint_t numSymbols, uint_t inc, uint_t incLimit);

    /**
       Return the symbol whose cumulative frequency count is
       the least that exceeds the given target.
    */
    uint_t symbol(uint_t target);

    /** Return the total frequency of all symbols. */
    uint_t
    totFreq() const
    {
        return _totFreq;
    }

    /**
       Add \b inc to the frequency count for the given symbol.  If
       totFreq() exceeds \b incLimit, all frequency counts are halved.
    */
    void update(uint_t symbol);

private:
    void
    init()
    {
        _F = nullptr;
        set(0, 0, 0);
    }
    void
    deInit()
    {
        delete[] _F;
    }
    void halve();

private:
    uint_t _numSymbols;
    uint_t _size;
    uint_t _mid;
    uint_t* _F;
    uint_t _inc;
    uint_t _incLimit;
    uint_t _totFreq;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef _L
#undef _R

////////////////////////////////////////////////////////////////////////////////////////////////////
// ArithmeticEncoder ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   DCC95 arithmetic coder.

   This implementation is really due to Alistair Moffat.  All I have really done here is
   "UTL-ify" it.

   See Alistair's home page at: http://www.cs.mu.oz.au/~alistair/

   <b>Attributes</b>

   \arg \b ctx : Default context.  This context is used by encoding/decoding methods that don't
   take an explicit context as a parameter.

   \arg <b><i>eob</i> flag</b> : If \b true, encode end-of-block when encoding is finished, so
   that the end of the block can be recognized when decoding.

   \author Alistair Moffat, Adam McKee
   \ingroup compression
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ArithmeticEncoder : public Encoder
{
    UTL_CLASS_DECL(ArithmeticEncoder, Encoder);

public:
    /**
       Constructor.
       \param mode \b io_rd to decode, \b io_wr to encode (see utl::io_t)
       \param stream (optional) associated stream
       \param owner (optional : true) \b owner flag for stream
       \param ctx (optional) default context
       \param eob (optional : false) encode end-of-block?
    */
    ArithmeticEncoder(uint_t mode,
                      Stream* stream = nullptr,
                      bool owner = true,
                      ArithContext* ctx = nullptr,
                      bool eob = false)
    {
        _ctx = nullptr;
        start(mode, stream, owner, ctx, eob);
    }

    virtual size_t decode(byte_t* block, size_t num);

    /**
       Decode a symbol with the given context.
       \return symbol
       \param ctx context
    */
    inline uint_t decode(ArithContext* ctx);

    virtual size_t encode(const byte_t* block, size_t num);

    /**
       Encode a symbol with the given context.
       \param symbol symbol to encode
       \param ctx context
    */
    inline void encode(uint_t symbol, ArithContext* ctx);

    /** Set the default context. */
    void
    setContext(ArithContext* ctx)
    {
        delete _ctx;
        _ctx = ctx;
    }

    /**
       Initialize for encoding or decoding.
       \param mode \b io_rd to encode, \b io_wr to decode (see utl::io_t)
       \param stream (optional) associated stream
       \param owner (optional : true) \b owner flag for stream
       \param ctx (optional) default context
       \param eob (optional : false) encode end-of-block?
    */
    void start(uint_t mode,
               Stream* stream,
               bool owner = true,
               ArithContext* ctx = nullptr,
               bool eob = false);

protected:
    virtual void clear();
    virtual void finishEncoding();

private:
    void
    init()
    {
        _ctx = nullptr;
        clearSelf();
    }
    void
    deInit()
    {
        close();
    }
    void clearSelf();
    inline void putBit(bool b);

private:
    uint_t _half, _qtr;
    uint_t _L, _R;
    uint_t _followBits;
    ArithContext* _ctx;
    uint_t _eob;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithmeticEncoder::putBit(bool b)
{
    _stream->putBit(b);
    while (_followBits > 0)
    {
        _stream->putBit(!b);
        _followBits--;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArithmeticEncoder::encode(uint_t symbol, ArithContext* ctx)
{
    uint_t l, h, t, r, rl;

    ctx->interval(&l, &h, symbol);
    t = ctx->totFreq();
    r = _R / t;
    rl = r * l;

    // adjust _L, _R
    _L += rl;
    if (h < t)
    {
        _R = r * h - rl;
    }
    else
    {
        _R -= rl;
    }

    // write bits as necessary
    while (_R <= _qtr)
    {
        if (_L + _R <= _half)
        {
            putBit(0);
        }
        else if (_L >= _half)
        {
            putBit(1);
            _L -= _half;
        }
        else
        {
            _followBits++;
            _L -= _qtr;
        }
        _L <<= 1;
        _R <<= 1;
    }

    ctx->update(symbol);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
ArithmeticEncoder::decode(ArithContext* ctx)
{
    uint_t l, h, t, r, rl, target, symbol;
    bool bit;

    t = ctx->totFreq();
    r = _R / t;

    // find l, h, symbol
    target = min(t - 1, _L / r);
    symbol = ctx->symbol(target);
    ctx->interval(&l, &h, symbol);
    rl = r * l;

    // adjust _L, _R
    _L -= rl;
    if (h < t)
    {
        _R = r * h - rl;
    }
    else
    {
        _R -= rl;
    }

    // read bits as necessary
    while (_R <= _qtr)
    {
        _R <<= 1;
        bit = _stream->getBit();
        _L = (_L << 1) | (bit ? 1 : 0);
    }

    ctx->update(symbol);
    return symbol;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
