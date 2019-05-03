#include <libutl/libutl.h>
#include <libutl/String.h>
#include <libutl/SHA256.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::SHA256);
UTL_CLASS_IMPL(utl::SHA256sum);

////////////////////////////////////////////////////////////////////////////////////////////////////

// bit rotation
#define ROR(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

// various operations for SHA-2
#define s0(w) (ROR(w, 7) ^ ROR(w, 18) ^ ((w) >> 3))
#define s1(w) (ROR(w, 17) ^ ROR(w, 19) ^ ((w) >> 10))
#define S0(a) (ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22))
#define S1(e) (ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25))

// CH ("choose"): e selects input f or g.  e bit = 1 chooses f.
#define CH(e, f, g) ((g) ^ ((e) & ((f) ^ (g))))

// MAJ ("majority"): has a 1-bit at positions where at least two of (a,b,c) have 1-bit
#define MAJ(a, b, c) (((a) & (b)) | ((c) & ((a) | (b))))

// this is how CH and MAJ are written in the wikipedia page on SHA-2
//#define CH(e,f,g)       (((e) & (f)) ^ (~(e) & (g)))
//#define MAJ(a,b,c)      (((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))

// for rounds 0-15
#define R1(a, b, c, d, e, f, g, h, i)                                                              \
    t1 = h + S1(e) + CH(e, f, g) + k[i] + w[i];                                                    \
    t2 = S0(a) + MAJ(a, b, c);                                                                     \
    d += t1;                                                                                       \
    h = t1 + t2;

// for rounds 16-63
#define R2(a, b, c, d, e, f, g, h, i)                                                              \
    tw = w[i & 15] + w[(i - 7) & 15] + s0(w[(i - 15) & 15]) + s1(w[(i - 2) & 15]);                 \
    w[i & 15] = tw;                                                                                \
    t1 = h + S1(e) + CH(e, f, g) + k[i] + tw;                                                      \
    t2 = S0(a) + MAJ(a, b, c);                                                                     \
    d += t1;                                                                                       \
    h = t1 + t2;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SHA256 /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256::clear()
{
    _dataPtr = _data;
    _dataLim = _data + 64;
    _bitLen = 0;
    _h[0] = 0x6a09e667;
    _h[1] = 0xbb67ae85;
    _h[2] = 0x3c6ef372;
    _h[3] = 0xa54ff53a;
    _h[4] = 0x510e527f;
    _h[5] = 0x9b05688c;
    _h[6] = 0x1f83d9ab;
    _h[7] = 0x5be0cd19;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256::process(const byte_t* data, size_t dataLen)
{
    const byte_t* dataPtr = data;
    size_t dataRem = dataLen;
    size_t _dataRem = _dataLim - _dataPtr;
    if (dataRem >= _dataRem)
    {
        memcpy(_dataPtr, dataPtr, _dataRem);
        transform(_data);
        _bitLen += 512;
        _dataPtr = _data;
        dataPtr += _dataRem;
        dataRem -= _dataRem;
    }
    size_t numBlocks = dataRem / 64;
    if (numBlocks > 0)
    {
        transform(dataPtr, numBlocks);
        _bitLen += (numBlocks * 512);
        // note: _dataPtr = _data
        dataPtr += (numBlocks * 64);
        dataRem &= 63;
    }
    memcpy(_dataPtr, dataPtr, dataRem);
    _dataPtr += dataRem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256::finalize(SHA256sum& sum)
{
    _bitLen += (_dataPtr - _data) * 8;
    byte_t* dataLim56 = _dataLim - 8;
    *_dataPtr++ = 0x80;
    if (_dataPtr <= dataLim56)
    {
        while (_dataPtr < dataLim56)
            *_dataPtr++ = 0;
    }
    else
    {
        while (_dataPtr < _dataLim)
            *_dataPtr++ = 0;
        transform(_data);
        memset(_data, 0, 56);
    }

    // append the bit-length (as big-endian) to the message
    uint64_t bitLen = _bitLen;
#ifdef UTL_ARCH_LITTLE_ENDIAN
    bitLen = utl::reverseBytes(bitLen);
#endif
    uint64_t* bits448_511 = (uint64_t*)(_data + 56);
    *bits448_511 = bitLen;

    // do the final transform
    transform(_data);

    // copy the sum
    sum.set(_h);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256::transform(const byte_t* data, size_t numBlocks)
{
    const uint32_t* dataW;
    uint32_t* wp;
    uint32_t a, b, c, d, e, f, g, h, t1, t2, tw, w[16];

    a = _h[0];
    b = _h[1];
    c = _h[2];
    d = _h[3];
    e = _h[4];
    f = _h[5];
    g = _h[6];
    h = _h[7];

    for (size_t block = 0; block < numBlocks; ++block, data += 64)
    {
        dataW = (uint32_t*)data;

#ifdef UTL_ARCH_LITTLE_ENDIAN
        wp = w;
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
        *wp++ = utl::reverseBytes(*dataW++);
#else
        memcpy(w, dataW, 64);
#endif

        // for (; i < 64; ++i)
        //{
        //    w[i] = w[i-7] + w[i-16] + s0(w[i-15]) + s1(w[i-2]);
        //}

        R1(a, b, c, d, e, f, g, h, 0);
        R1(h, a, b, c, d, e, f, g, 1);
        R1(g, h, a, b, c, d, e, f, 2);
        R1(f, g, h, a, b, c, d, e, 3);
        R1(e, f, g, h, a, b, c, d, 4);
        R1(d, e, f, g, h, a, b, c, 5);
        R1(c, d, e, f, g, h, a, b, 6);
        R1(b, c, d, e, f, g, h, a, 7);
        R1(a, b, c, d, e, f, g, h, 8);
        R1(h, a, b, c, d, e, f, g, 9);
        R1(g, h, a, b, c, d, e, f, 10);
        R1(f, g, h, a, b, c, d, e, 11);
        R1(e, f, g, h, a, b, c, d, 12);
        R1(d, e, f, g, h, a, b, c, 13);
        R1(c, d, e, f, g, h, a, b, 14);
        R1(b, c, d, e, f, g, h, a, 15);

        R2(a, b, c, d, e, f, g, h, 16);
        R2(h, a, b, c, d, e, f, g, 17);
        R2(g, h, a, b, c, d, e, f, 18);
        R2(f, g, h, a, b, c, d, e, 19);
        R2(e, f, g, h, a, b, c, d, 20);
        R2(d, e, f, g, h, a, b, c, 21);
        R2(c, d, e, f, g, h, a, b, 22);
        R2(b, c, d, e, f, g, h, a, 23);
        R2(a, b, c, d, e, f, g, h, 24);
        R2(h, a, b, c, d, e, f, g, 25);
        R2(g, h, a, b, c, d, e, f, 26);
        R2(f, g, h, a, b, c, d, e, 27);
        R2(e, f, g, h, a, b, c, d, 28);
        R2(d, e, f, g, h, a, b, c, 29);
        R2(c, d, e, f, g, h, a, b, 30);
        R2(b, c, d, e, f, g, h, a, 31);
        R2(a, b, c, d, e, f, g, h, 32);
        R2(h, a, b, c, d, e, f, g, 33);
        R2(g, h, a, b, c, d, e, f, 34);
        R2(f, g, h, a, b, c, d, e, 35);
        R2(e, f, g, h, a, b, c, d, 36);
        R2(d, e, f, g, h, a, b, c, 37);
        R2(c, d, e, f, g, h, a, b, 38);
        R2(b, c, d, e, f, g, h, a, 39);
        R2(a, b, c, d, e, f, g, h, 40);
        R2(h, a, b, c, d, e, f, g, 41);
        R2(g, h, a, b, c, d, e, f, 42);
        R2(f, g, h, a, b, c, d, e, 43);
        R2(e, f, g, h, a, b, c, d, 44);
        R2(d, e, f, g, h, a, b, c, 45);
        R2(c, d, e, f, g, h, a, b, 46);
        R2(b, c, d, e, f, g, h, a, 47);
        R2(a, b, c, d, e, f, g, h, 48);
        R2(h, a, b, c, d, e, f, g, 49);
        R2(g, h, a, b, c, d, e, f, 50);
        R2(f, g, h, a, b, c, d, e, 51);
        R2(e, f, g, h, a, b, c, d, 52);
        R2(d, e, f, g, h, a, b, c, 53);
        R2(c, d, e, f, g, h, a, b, 54);
        R2(b, c, d, e, f, g, h, a, 55);
        R2(a, b, c, d, e, f, g, h, 56);
        R2(h, a, b, c, d, e, f, g, 57);
        R2(g, h, a, b, c, d, e, f, 58);
        R2(f, g, h, a, b, c, d, e, 59);
        R2(e, f, g, h, a, b, c, d, 60);
        R2(d, e, f, g, h, a, b, c, 61);
        R2(c, d, e, f, g, h, a, b, 62);
        R2(b, c, d, e, f, g, h, a, 63);

        a = _h[0] += a;
        b = _h[1] += b;
        c = _h[2] += c;
        d = _h[3] += d;
        e = _h[4] += e;
        f = _h[5] += f;
        g = _h[6] += g;
        h = _h[7] += h;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t SHA256::k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// SHA256sum //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
SHA256sum::compare(const Object& rhs) const
{
    auto& sum = utl::cast<SHA256sum>(rhs);
    return utl::compare(_h, sum._h, 32);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256sum::copy(const Object& rhs)
{
    auto& sum = utl::cast<SHA256sum>(rhs);
    memcpy(_h, sum._h, 32);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
SHA256sum::hash(size_t size) const
{
    const byte_t* s = (byte_t*)_h;
    const byte_t* sLim = s + 32;
    size_t h;
    for (h = 0; s != sLim; s++)
    {
        h = ((64 * h) + (size_t)*s) % size;
    }
    return h;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256sum::serialize(Stream& stream, uint_t io, uint_t mode)
{
    utl::serialize(_h, 8, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256sum::set(const uint32_t* h)
{
    memmove(_h, h, 32);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SHA256sum::set(const String& str)
{
    ASSERTD(str.length() == 64);

    uint_t i;
    for (i = 0; i < 8; i++)
    {
        uint32_t v = Uint("0x" + str.subString(i * 8, 8)).get();
        _h[i] = v;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
SHA256sum::toString() const
{
    String res;
    res.reserve(65, 1);
    const uint32_t* ptr = _h;
    const uint32_t* lim = ptr + 8;
    for (; ptr < lim; ++ptr)
    {
        uint32_t v = *ptr;
        res += Uint(v).toHex(8);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
