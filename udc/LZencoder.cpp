#include <libutl/libutl.h>
#include <libutl/LZencoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
//#define DEBUG_LZ
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_LZ
#include <libutl/FDstream.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::LZencoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define LZ_HASH_SIZE KB(64)
#define LZ_HASH_MASK (LZ_HASH_SIZE - 1)
#define LZ_HASH_UNUSED uint_t_max
#define LZ_LOOK_SIZE 256U
#define LZ_LOOK_MASK (LZ_LOOK_SIZE - 1)
#define LZ_MIN_MATCH 3U
#define LZ_MAX_MATCH LZ_LOOK_SIZE
#define LZ_WIND_SIZE KB(32)
#define LZ_WIND_MASK (LZ_WIND_SIZE - 1)
#define LZ_UNROLL_SIZE 16

////////////////////////////////////////////////////////////////////////////////////////////////////

#define LZ_DIST_CODE(dist) ((dist) < 256 ? _distCode[dist] : _distCode[256 + ((dist) >> 7)])
#define LZ_MOD_LOOK(a) ((a)&LZ_LOOK_MASK)
#define LZ_MOD_WIND(a) ((a)&LZ_WIND_MASK)
#define LZ_LOOK_HASH(a) (_look[a] ^ (_look[a + 1] << 4) ^ ((_look[a + 2] & 0x7f) << 9))
#define LZ_WIND_HASH(a) (_wind[a] ^ (_wind[a + 1] << 4) ^ ((_wind[a + 2] & 0x7f) << 9))

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
LZencoder::decode(byte_t* block, size_t num)
{
    uint_t code, extraBits;
    uint_t i, c, oBufPos = 0;
    uint_t matchLen, matchPos, matchDist;
    while (oBufPos < num)
    {
        // stop if we can't accommodate a full-length match
        if ((num - oBufPos) < LZ_MAX_MATCH)
        {
            break;
        }

        c = _lEnc.decode();

        // end-of-file?
        if (c == eob)
        {
            setEOF(true);
            break;
        }

        if (c <= 255)
        {
            // literal
            block[oBufPos++] = c;
            _wind[LZ_MOD_WIND(_pos)] = c;
            _pos++;
        }
        else
        {
            // string match

            // decode match length
            code = c - 256;
            matchLen = _baseLen[code];
            extraBits = lenBits[code];
            if (extraBits > 0)
            {
                matchLen += _stream->getBits(extraBits);
            }
            matchLen += LZ_MIN_MATCH;

            // decode match distance
            code = _dEnc.decode();
            matchDist = _baseDist[code];
            extraBits = distBits[code];
            if (extraBits > 0)
            {
                matchDist += _stream->getBits(extraBits);
            }
            matchDist += matchLen;

            // determine matchPos
            matchPos = _pos;
            if (matchPos < matchDist)
            {
                matchPos += LZ_WIND_SIZE;
            }
            matchPos -= matchDist;

            // copy the matching characters
            for (i = 0; i < matchLen; i++, matchPos++, _pos++)
            {
                c = _wind[LZ_MOD_WIND(matchPos)];
                block[oBufPos++] = c;
                _wind[LZ_MOD_WIND(_pos)] = c;
            }
        }
    }

    return oBufPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
LZencoder::encode(const byte_t* block, size_t num)
{
    // This code would be painfully simple, but it's complicated a little
    // bit by two things:
    //     o lazy matching
    //     o retaining state between successive calls to encode()
    // Even with these two complications, it's not that bad.

    uint_t pos, code, extraBits;

    // fill look-ahead buffer -- it stays full after we fill it until
    // we are encoding the last block
    uint_t iBufPos = 0;
    while ((_lab < LZ_LOOK_SIZE) && (iBufPos < num))
    {
        _look[_lab] = block[iBufPos];
        if (_lab < LZ_UNROLL_SIZE)
        {
            _look[LZ_LOOK_SIZE + _lab] = block[iBufPos];
        }
        _lab++;
        iBufPos++;
    }

    // can't start encoding with non-full look-ahead unless
    // we're on the last block
    if ((_lab < LZ_LOOK_SIZE) && !isLastBlock())
    {
        return iBufPos;
    }

    while (_lab > 0)
    {
        while (_repc > 0)
        {
            if ((iBufPos == num) && !isLastBlock())
            {
                return iBufPos;
            }
            removeString(_pos);
            addString(_pos);
            pos = LZ_MOD_WIND(_pos);
            _wind[pos] = _look[LZ_MOD_LOOK(_pos)];
            if (pos < LZ_UNROLL_SIZE)
            {
                _wind[LZ_WIND_SIZE + pos] = _wind[pos];
            }
            if (iBufPos == num)
            {
                _lab--;
            }
            else
            {
                pos = LZ_MOD_LOOK(_pos);
                _look[pos] = block[iBufPos++];
                if (pos < LZ_UNROLL_SIZE)
                {
                    _look[LZ_LOOK_SIZE + pos] = _look[pos];
                }
            }
            _pos++;
            _repc--;
        }

        _prevLiteral = _literal;
        _prevMatchLen = _matchLen;
        _prevMatchDist = _matchDist;

        // find a match
        _literal = _look[LZ_MOD_LOOK(_pos)];
        if (_prevMatchLen <= _maxLazyLen)
        {
            _matchLen = matchString(_pos);
        }
        else
        {
            _matchLen = 0;
        }
        // a short match must not be too distant
        if ((_matchLen == LZ_MIN_MATCH) && (_matchDist > 400))
        {
            _matchLen = 0;
        }

        // previous match was better so output it
        if ((_prevMatchLen >= LZ_MIN_MATCH) && (_matchLen <= _prevMatchLen))
        {
            // encode the match length
            _repc = _prevMatchLen - 1;
            _prevMatchLen -= LZ_MIN_MATCH;
            code = _lenCode[_prevMatchLen];
            _lEnc.encode(256 + code);
            extraBits = lenBits[code];
            if (extraBits > 0)
            {
                _prevMatchLen -= _baseLen[code];
                _stream->putBits(_prevMatchLen, extraBits);
            }

            // encode the match distance
            code = LZ_DIST_CODE(_prevMatchDist);
            _dEnc.encode(code);
            extraBits = distBits[code];
            if (extraBits > 0)
            {
                _prevMatchDist -= _baseDist[code];
                _stream->putBits(_prevMatchDist, extraBits);
            }

            _matchLen = 0;
            _matchAvailable = false;
        }
        else if (_matchAvailable)
        {
            // match at current position is better -- encode literal
            _lEnc.encode(_prevLiteral);
            _repc = 1;
        }
        else
        {
            // no previous match to compare with
            _matchAvailable = true;
            _repc = 1;
        }
    }
    return iBufPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LZencoder::start(uint_t mode, Stream* stream, bool owner, uint_t level)
{
    clear();
    set(mode, stream, owner);
    setError(false);
    _baseLen = new uint_t[LZ_LEN_CODES];
    _lenCode = new uint_t[256];
    uint_t n, code, len = 0;
    for (code = 0; code < LZ_LEN_CODES; code++)
    {
        _baseLen[code] = len;
        for (n = 0; n < (1U << lenBits[code]); n++)
        {
            _lenCode[len++] = code;
        }
    }
    ASSERTD(len == LZ_MAX_MATCH);
    _baseDist = new uint_t[LZ_DIST_CODES];
    _distCode = new uint_t[512];
    uint_t dist = 0;
    for (code = 0; code < 16; code++)
    {
        _baseDist[code] = dist;
        for (n = 0; n < (1U << distBits[code]); n++)
        {
            _distCode[dist++] = code;
        }
    }
    ASSERTD(dist == 256);
    dist >>= 7;
    for (; code < LZ_DIST_CODES; code++)
    {
        _baseDist[code] = dist << 7;
        for (n = 0; n < (1U << (distBits[code] - 7)); n++)
        {
            _distCode[256 + dist++] = code;
        }
    }
    ASSERTD((dist * 128) == LZ_WIND_SIZE);

    // init for encoding or decoding
    _wind = new byte_t[LZ_WIND_SIZE + LZ_UNROLL_SIZE];
    memset(_wind, 0, LZ_WIND_SIZE + LZ_UNROLL_SIZE);

    // do encoder-specific initialization
    if (isOutput())
    {
        _look = new byte_t[LZ_LOOK_SIZE + LZ_UNROLL_SIZE];
        _head = new uint_t[LZ_HASH_SIZE];
        _tail = new uint_t[LZ_HASH_SIZE];
        _pred = new uint_t[LZ_WIND_SIZE];
        _succ = new uint_t[LZ_WIND_SIZE];
        memset(_head, LZ_HASH_UNUSED, LZ_HASH_SIZE * sizeof(uint_t));
        memset(_tail, LZ_HASH_UNUSED, LZ_HASH_SIZE * sizeof(uint_t));

        // set compress configuration
        switch (level)
        {
        case 0:
        case 1:
            _goodLen = 4;
            _maxLazyLen = 4;
            _niceLen = 8;
            _maxChain = 4;
            break;
        case 2:
            _goodLen = 8;
            _maxLazyLen = 16;
            _niceLen = 16;
            _maxChain = 16;
            break;
        case 3:
            _goodLen = 8;
            _maxLazyLen = 32;
            _niceLen = 32;
            _maxChain = 32;
            break;
        case 4:
            _goodLen = 16;
            _maxLazyLen = 32;
            _niceLen = 64;
            _maxChain = 64;
            break;
        case 5:
            _goodLen = 16;
            _maxLazyLen = 64;
            _niceLen = 64;
            _maxChain = 128;
            break;
        case 6:
            _goodLen = 24;
            _maxLazyLen = 64;
            _niceLen = 128;
            _maxChain = 128;
            break;
        case 7:
            _goodLen = 24;
            _maxLazyLen = 128;
            _niceLen = 128;
            _maxChain = 256;
            break;
        case 8:
            _goodLen = 32;
            _maxLazyLen = 128;
            _niceLen = 256;
            _maxChain = KB(1);
            break;
        default:
            _goodLen = 32;
            _maxLazyLen = 256;
            _niceLen = 256;
            _maxChain = KB(4);
            break;
        }
    }
    _lEnc.start(mode, _stream, false, 256 + LZ_LEN_CODES + 1, 1000);
    _dEnc.start(mode, _stream, false, LZ_DIST_CODES, 1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LZencoder::clear()
{
    super::clear();
    setLastBlock(false);
    _lab = 0;
    _repc = 0;
    _pos = 0;
    // sliding window, look-ahead buffer
    delete[] _wind;
    _wind = nullptr;
    delete[] _look;
    _look = nullptr;
    // match information
    _prevMatchLen = _prevMatchDist = _prevLiteral = 0;
    _matchLen = _matchDist = _literal = 0;
    _matchAvailable = false;
    // compress configuration;
    _goodLen = _maxLazyLen = _niceLen = _maxChain = 0;
    // coding distances, lengths
    delete[] _baseLen;
    _baseLen = nullptr;
    delete[] _lenCode;
    _lenCode = nullptr;
    delete[] _baseDist;
    _baseDist = nullptr;
    delete[] _distCode;
    _distCode = nullptr;
    // hashing
    delete[] _head;
    _head = nullptr;
    delete[] _tail;
    _tail = nullptr;
    delete[] _pred;
    _pred = nullptr;
    delete[] _succ;
    _succ = nullptr;
    // H-coders
    _lEnc.close();
    _dEnc.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LZencoder::finishEncoding()
{
    setLastBlock(true);
    encode(_oBuf, _oBufPos);
    _lEnc.encode(eob);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LZencoder::init()
{
    _wind = _look = nullptr;
    _baseLen = _lenCode = _baseDist = _distCode = nullptr;
    _head = _tail = _pred = _succ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LZencoder::addString(uint_t pos)
{
    uint_t lpos = LZ_MOD_LOOK(pos);
    uint_t key = LZ_LOOK_HASH(lpos);
    uint_t wpos = LZ_MOD_WIND(pos);
    if (_head[key] == LZ_HASH_UNUSED)
    {
        _tail[key] = wpos;
        _succ[wpos] = LZ_HASH_UNUSED;
    }
    else
    {
        _succ[wpos] = _head[key];
        _pred[_head[key]] = wpos;
    }
    _head[key] = wpos;
    _pred[wpos] = LZ_HASH_UNUSED;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
LZencoder::matchString(uint_t pos)
{
    uint_t lpos = LZ_MOD_LOOK(pos);
    uint_t key = LZ_LOOK_HASH(lpos);
    uint_t index = _head[key];
    if (index == LZ_HASH_UNUSED)
        return 0;
    uint_t best = 0;
    pos = LZ_MOD_WIND(pos);
    uint_t maxChain = _maxChain;
    if (_prevMatchLen > _goodLen)
    {
        maxChain >>= 1;
    }

#ifdef DEBUG_LZ
    uint_t i;
    cerr << "searching for: ";
    for (i = 0; i < 16; i++)
    {
        char c = _look[LZ_MOD_LOOK(pos + i)];
        if (isgraph(c))
        {
            cerr << c;
        }
        else
        {
            cerr << "<" << (int)c << ">";
        }
    }
    cerr << endl;
    cerr << "hash = " << key << endl;
#endif

    while ((index != LZ_HASH_UNUSED) && (maxChain-- > 0))
    {
#ifdef DEBUG_LZ
        cerr << "found: ";
        for (i = 0; i < 16; i++)
        {
            char c = _wind[LZ_MOD_WIND(index + i)];
            if (isgraph(c))
            {
                cerr << c;
            }
            else
            {
                cerr << "<" << (int)c << ">";
            }
        }
        cerr << endl;
#endif
        if (_wind[LZ_MOD_WIND(index + best)] == _look[LZ_MOD_LOOK(pos + best)])
        {
            uint_t len = 0;
            byte_t* windPtr = _wind + index;
            byte_t* lookPtr = _look + LZ_MOD_LOOK(pos);
            byte_t* windLim = _wind + LZ_WIND_SIZE;
            byte_t* lookLim = _look + LZ_LOOK_SIZE;
            uint_t posDist;
            if (index <= pos)
            {
                posDist = pos - index;
            }
            else
            {
                posDist = (LZ_WIND_SIZE - index) + pos;
            }
            uint_t maxLen = (posDist < _lab) ? posDist : _lab;
            while (len < maxLen)
            {
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (*windPtr != *lookPtr)
                    break;
                windPtr++;
                lookPtr++;
                len++;
                if (windPtr >= windLim)
                {
                    windPtr -= LZ_WIND_SIZE;
                }
                if (lookPtr >= lookLim)
                {
                    lookPtr -= LZ_LOOK_SIZE;
                }
            }
            // can't have a match longer than the maximum
            if (len > maxLen)
                len = maxLen;

            if (len > best)
            {
                best = len;
                // compute distance
                uint_t dist = pos;
                uint_t tmp = index + len;
                if (dist < tmp)
                {
                    dist += LZ_WIND_SIZE;
                }
                dist -= tmp;
                // update result
                _matchDist = dist;
                ASSERTD(_matchDist < LZ_WIND_SIZE);
                // good enough
                if (len >= _niceLen)
                {
                    break;
                }
            }
        }
        index = _succ[index];
    }
    return best;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LZencoder::removeString(uint_t pos)
{
    if (pos < LZ_WIND_SIZE)
    {
        return;
    }
    uint_t wpos = LZ_MOD_WIND(pos);
    uint_t key = LZ_WIND_HASH(wpos);
    if (_head[key] == _tail[key])
    {
        _head[key] = LZ_HASH_UNUSED;
    }
    else
    {
        _succ[_pred[_tail[key]]] = LZ_HASH_UNUSED;
        _tail[key] = _pred[_tail[key]];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint_t LZencoder::eob = 256 + LZ_LEN_CODES;

const uint_t LZencoder::lenBits[LZ_LEN_CODES] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, // 0 - 9
    1, 1, 2, 2, 2, 2, 3, 3, 3, 3, // 10 - 19
    4, 4, 4, 4, 5, 5, 5, 5        // 20 - 27
};

const uint_t LZencoder::distBits[LZ_DIST_CODES] = {
    0, 0, 0,  0,  1,  1,  2,  2,  3,  3, // 0 - 9
    4, 4, 5,  5,  6,  6,  7,  7,  8,  8, // 10 - 19
    9, 9, 10, 10, 11, 11, 12, 12, 13, 13 // 20 - 29
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
