#include <libutl/libutl.h>
#include <libutl/String.h>
#include <libutl/Uint.h>
#include <libutl/MD5.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::MD5);
UTL_CLASS_IMPL(utl::MD5sum);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// MD5sum /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
MD5sum::compare(const Object& rhs) const
{
    auto& md5sum = utl::cast<MD5sum>(rhs);
    return utl::compare(_sum, md5sum._sum, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5sum::copy(const Object& rhs)
{
    auto& md5sum = utl::cast<MD5sum>(rhs);
    memmove(_sum, md5sum._sum, 16);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
MD5sum::hash(size_t size) const
{
    const byte_t* s = (byte_t*)_sum;
    const byte_t* sLim = s + 16;
    size_t h;
    for (h = 0; s != sLim; s++)
    {
        h = ((64 * h) + (size_t)*s) % size;
    }
    return h;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5sum::serialize(Stream& stream, uint_t io, uint_t mode)
{
    utl::serialize(_sum, 4, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5sum::set(const String& str)
{
    ASSERTD(str.length() == 32);

    uint_t i;
    for (i = 0; i < 4; i++)
    {
        String uint32 = String("0x") + str.subString(i * 8, 8);
        _sum[i] = Uint(uint32).get();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
MD5sum::toString() const
{
    String res;
    const uint32_t* ptr = _sum;
    for (uint_t i = 0; i < 4; i++)
    {
        res += Uint(ptr[i]).toHex(8);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// MD5 ////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5::copy(const Object& rhs)
{
    auto& md5 = utl::cast<MD5>(rhs);
    _A = md5._A;
    _B = md5._B;
    _C = md5._C;
    _D = md5._D;
    _len[0] = md5._len[0];
    _len[1] = md5._len[1];
    _bufLen = md5._bufLen;
    memmove(_buf, md5._buf, _bufLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5::add(byte_t ch)
{
    add(&ch, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5::add(const byte_t* buf, size_t len)
{
    // if there's buffered data, append to buffer first
    if (_bufLen > 0)
    {
        uint32_t bufLeftOver = _bufLen;
        uint32_t bufAdd = (64 - bufLeftOver) > len ? len : 64 - bufLeftOver;
        memmove(_buf + bufLeftOver, buf, bufAdd);
        _bufLen += bufAdd;

        // process a complete block if possible
        if (_bufLen == 64)
        {
            doBlock(_buf, 64);
            _bufLen = 0;
        }

        buf += bufAdd;
        len -= bufAdd;
    }

    // process complete blocks
    if (len >= 64)
    {
        uint32_t blockSize = len & ~63;
        uint32_t bufRemaining = len & 63;
        doBlock(buf, blockSize);
        buf += blockSize;
        len = bufRemaining;
    }

    // remaining bytes are buffered
    if (len > 0)
    {
        memmove(_buf, buf, len);
        _bufLen = len;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5::clear()
{
    _A = 0x67452301;
    _B = 0xefcdab89;
    _C = 0x98badcfe;
    _D = 0x10325476;
    _len[0] = _len[1] = 0;
    _bufLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MD5sum
MD5::get() const
{
    // finish encoding if necessary
    if (!((_bufLen == 0) && (_len[0] == 0) && (_len[1] == 0)))
    {
        ((MD5*)this)->finish();
    }

    uint32_t sum[4];
    sum[0] = revLE(_A);
    sum[1] = revLE(_B);
    sum[2] = revLE(_C);
    sum[3] = revLE(_D);
    return MD5sum(sum);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MD5sum
MD5::compute(Stream& is)
{
    MD5 md5;
    Vector<byte_t> bufVector(4096);
    byte_t* buf = bufVector;

    for (;;)
    {
        size_t numRead;
        try
        {
            numRead = is.read(buf, 4096);
            md5.add(buf, numRead);
        }
        catch (StreamEOFex&)
        {
            break;
        }
    }
    return md5.get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5::finish()
{
    uint32_t numBytes = _bufLen;

    // count remaining bytes
    _len[0] += numBytes;
    if (_len[0] < numBytes)
    {
        _len[1]++;
    }

    // pad buffer
    uint32_t padLen = (numBytes < 56) ? (56 - numBytes) : (64 + 56 - numBytes);
    _buf[numBytes] = 0x80;
    uint32_t i;
    for (i = 1; i < padLen; i++)
    {
        _buf[numBytes + i] = 0;
    }

    // put 64-bit file length in bits at the end of the buffer
    auto ptr = (uint32_t*)(_buf + _bufLen + padLen);
    ptr[0] = revBE(_len[0] << 3);
    ptr[1] = revBE((_len[1] << 3) | (_len[0] >> 29));

    // process last block
    doBlock(_buf, numBytes + padLen + 8);

    // reset
    _bufLen = 0;
    _len[0] = _len[1] = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define FF(b, c, d) (d ^ (b & (c ^ d)))
#define FG(b, c, d) FF(d, b, c)
#define FH(b, c, d) (b ^ c ^ d)
#define FI(b, c, d) (c ^ (b | ~d))
#define ROTLEFT(w, s) (w = (w << s) | (w >> (32 - s)))

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MD5::doBlock(const byte_t* buf, size_t len)
{
    ASSERTD((len % 64) == 0);
    uint32_t cwords[16];
    const uint32_t* words = (const uint32_t*)buf;
    uint32_t numWords = len / sizeof(uint32_t);
    const uint32_t* wordsLim = words + numWords;
    uint32_t A = _A;
    uint32_t B = _B;
    uint32_t C = _C;
    uint32_t D = _D;

    _len[0] += len;
    if (_len[0] < len)
    {
        _len[1]++;
    }

    while (words < wordsLim)
    {
        uint32_t* cwp = cwords;
        uint32_t saveA = A;
        uint32_t saveB = B;
        uint32_t saveC = C;
        uint32_t saveD = D;

#define OP(a, b, c, d, s, T)                                                                       \
    a += FF(b, c, d) + (*cwp++ = revBE(*words)) + T;                                               \
    ++words;                                                                                       \
    ROTLEFT(a, s);                                                                                 \
    a += b;

        // round 1
        OP(A, B, C, D, 7, 0xd76aa478);
        OP(D, A, B, C, 12, 0xe8c7b756);
        OP(C, D, A, B, 17, 0x242070db);
        OP(B, C, D, A, 22, 0xc1bdceee);
        OP(A, B, C, D, 7, 0xf57c0faf);
        OP(D, A, B, C, 12, 0x4787c62a);
        OP(C, D, A, B, 17, 0xa8304613);
        OP(B, C, D, A, 22, 0xfd469501);
        OP(A, B, C, D, 7, 0x698098d8);
        OP(D, A, B, C, 12, 0x8b44f7af);
        OP(C, D, A, B, 17, 0xffff5bb1);
        OP(B, C, D, A, 22, 0x895cd7be);
        OP(A, B, C, D, 7, 0x6b901122);
        OP(D, A, B, C, 12, 0xfd987193);
        OP(C, D, A, B, 17, 0xa679438e);
        OP(B, C, D, A, 22, 0x49b40821);

#undef OP
#define OP(f, a, b, c, d, k, s, T)                                                                 \
    a += f(b, c, d) + cwords[k] + T;                                                               \
    ROTLEFT(a, s);                                                                                 \
    a += b;

        // round 2
        OP(FG, A, B, C, D, 1, 5, 0xf61e2562);
        OP(FG, D, A, B, C, 6, 9, 0xc040b340);
        OP(FG, C, D, A, B, 11, 14, 0x265e5a51);
        OP(FG, B, C, D, A, 0, 20, 0xe9b6c7aa);
        OP(FG, A, B, C, D, 5, 5, 0xd62f105d);
        OP(FG, D, A, B, C, 10, 9, 0x02441453);
        OP(FG, C, D, A, B, 15, 14, 0xd8a1e681);
        OP(FG, B, C, D, A, 4, 20, 0xe7d3fbc8);
        OP(FG, A, B, C, D, 9, 5, 0x21e1cde6);
        OP(FG, D, A, B, C, 14, 9, 0xc33707d6);
        OP(FG, C, D, A, B, 3, 14, 0xf4d50d87);
        OP(FG, B, C, D, A, 8, 20, 0x455a14ed);
        OP(FG, A, B, C, D, 13, 5, 0xa9e3e905);
        OP(FG, D, A, B, C, 2, 9, 0xfcefa3f8);
        OP(FG, C, D, A, B, 7, 14, 0x676f02d9);
        OP(FG, B, C, D, A, 12, 20, 0x8d2a4c8a);

        // round 3
        OP(FH, A, B, C, D, 5, 4, 0xfffa3942);
        OP(FH, D, A, B, C, 8, 11, 0x8771f681);
        OP(FH, C, D, A, B, 11, 16, 0x6d9d6122);
        OP(FH, B, C, D, A, 14, 23, 0xfde5380c);
        OP(FH, A, B, C, D, 1, 4, 0xa4beea44);
        OP(FH, D, A, B, C, 4, 11, 0x4bdecfa9);
        OP(FH, C, D, A, B, 7, 16, 0xf6bb4b60);
        OP(FH, B, C, D, A, 10, 23, 0xbebfbc70);
        OP(FH, A, B, C, D, 13, 4, 0x289b7ec6);
        OP(FH, D, A, B, C, 0, 11, 0xeaa127fa);
        OP(FH, C, D, A, B, 3, 16, 0xd4ef3085);
        OP(FH, B, C, D, A, 6, 23, 0x04881d05);
        OP(FH, A, B, C, D, 9, 4, 0xd9d4d039);
        OP(FH, D, A, B, C, 12, 11, 0xe6db99e5);
        OP(FH, C, D, A, B, 15, 16, 0x1fa27cf8);
        OP(FH, B, C, D, A, 2, 23, 0xc4ac5665);

        // round 4
        OP(FI, A, B, C, D, 0, 6, 0xf4292244);
        OP(FI, D, A, B, C, 7, 10, 0x432aff97);
        OP(FI, C, D, A, B, 14, 15, 0xab9423a7);
        OP(FI, B, C, D, A, 5, 21, 0xfc93a039);
        OP(FI, A, B, C, D, 12, 6, 0x655b59c3);
        OP(FI, D, A, B, C, 3, 10, 0x8f0ccc92);
        OP(FI, C, D, A, B, 10, 15, 0xffeff47d);
        OP(FI, B, C, D, A, 1, 21, 0x85845dd1);
        OP(FI, A, B, C, D, 8, 6, 0x6fa87e4f);
        OP(FI, D, A, B, C, 15, 10, 0xfe2ce6e0);
        OP(FI, C, D, A, B, 6, 15, 0xa3014314);
        OP(FI, B, C, D, A, 13, 21, 0x4e0811a1);
        OP(FI, A, B, C, D, 4, 6, 0xf7537e82);
        OP(FI, D, A, B, C, 11, 10, 0xbd3af235);
        OP(FI, C, D, A, B, 2, 15, 0x2ad7d2bb);
        OP(FI, B, C, D, A, 9, 21, 0xeb86d391);

        A += saveA;
        B += saveB;
        C += saveC;
        D += saveD;
    }

    _A = A;
    _B = B;
    _C = C;
    _D = D;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t
MD5::revBE(uint32_t w)
{
#ifdef UTL_ARCH_BIG_ENDIAN
    return reverseBytes(w);
#else
    return w;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t
MD5::revLE(uint32_t w)
{
#ifdef UTL_ARCH_LITTLE_ENDIAN
    return reverseBytes(w);
#else
    return w;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
