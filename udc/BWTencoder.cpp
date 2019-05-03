#include <libutl/libutl.h>
#include <libutl/BWTencoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

//#define BWT_DEBUG

#ifdef BWT_DEBUG
#include <libutl/FDstream.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BWTencoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

/// sort ///////////////////////////////////////////////////////////////////////////////////////////

#define BWT_CH_IDX(x) ((_ptr[x] + depth) % _blockSize)
#define BWT_CH(x) _block[BWT_CH_IDX(x)]
#define BWT_BIGFREQ(x) (_freq[((x) + 1) << 8] - _freq[(x) << 8])
#define BWT_ISSORTED(x) (((_freq[x]) & BWT_MASK_SORTED) != 0)
#define BWT_MASK_SORTED 0x80000000U
#define BWT_MASK_CLEAR 0x7fffffffU

/// mtf ////////////////////////////////////////////////////////////////////////////////////////////

#define BWT_ZRUN0 256
#define BWT_ZRUN1 257
#define BWT_EOB 258
#define BWT_ONE 259
#define BWT_M2 260
#define BWT_M4 261
#define BWT_M8 262
#define BWT_M16 263
#define BWT_M32 264
#define BWT_M64 265
#define BWT_M128 266

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
BWTencoder::decode(byte_t* block, size_t num)
{
    // don't decode unless buffer is large enough
    if (num < _iBuf.size())
        return 0;

    // read the origin
    _origin = mtfDecodeRandomWord();

    // end-of-stream?
    if (_origin >= _iBuf.size())
    {
        setEOF(true);
        return 0;
    }

    // decode
    mtfDecodeBlock();

    // reverse the transformation
    undoTransform(block);

    return _blockSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
BWTencoder::encode(const byte_t* block, size_t num)
{
    // do the transform
    _block = (byte_t*)block;
    _blockSize = num;
    doTransform();

    // mtf-encode the block
    mtfEncodeRandomWord(_origin);
    mtfEncodeBlock();

    // so we don't delete _block in clear()...
    _block = nullptr;

    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::start(uint_t mode, Stream* stream, bool owner, uint_t blockSize)
{
    // if we did compression or decompression already, do clear()
    if ((_freq != nullptr) || (_block != nullptr))
    {
        clear();
    }

    set(mode, stream, owner, blockSize);
    setError(false);

    // sort
    _ptr = new uint_t[blockSize + 1];
    if (isOutput())
    {
        _freq = new uint_t[KB(64) + 1];
        _stripe = new uint32_t[blockSize + 64];
    }

    // mtf
    _A.start(mode, stream, false);
    _H[0] = new ArithContext(11, 9, 1000);
    _H[1] = new ArithContext(2, 5, 1000);
    _H[2] = new ArithContext(4, 2, 1000);
    _H[3] = new ArithContext(8, 1, 1000);
    _H[4] = new ArithContext(16, 1, 1000);
    _H[5] = new ArithContext(32, 1, 1000);
    _H[6] = new ArithContext(64, 1, 1000);
    _H[7] = new ArithContext(128, 1, 1000);
    _H[8] = new ArithContext(256, 0, 1000);
    _numZeroes = 0;
    for (uint_t i = 0; i < 256; i++)
    {
        _pos[i] = i;
    }

    // general
    if (isInput())
    {
        _block = new byte_t[blockSize];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::clear()
{
    super::clear();
    clearSelf();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::finishEncoding()
{
    // encode end-of-stream
    mtfEncodeRandomWord(uint_t_max);
    _A.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::init()
{
    // sort
    _ptr = nullptr;
    _freq = nullptr;
    _stripe = nullptr;
    // mtf
    for (uint_t i = 0; i < 9; i++)
    {
        _H[i] = nullptr;
    }
    _numZeroes = 0;
    // general
    _block = nullptr;
    _blockSize = 0;
    _origin = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::clearSelf()
{
    uint_t i;

    // sort
    delete[] _ptr;
    _ptr = nullptr;
    delete[] _freq;
    _freq = nullptr;
    delete[] _stripe;
    _stripe = nullptr;

    // mtf
    _A.close();
    for (i = 0; i < 9; i++)
    {
        delete _H[i];
        _H[i] = nullptr;
    }
    _numZeroes = 0;

    // general
    delete[] _block;
    _block = nullptr;
    _blockSize = 0;
    _origin = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   A lot of the code for doTransform() was shamelessly stolen from Julian Seward's excellent bzip2.
   This method contains the most important high-level block-sorting logic.  The optimizations that I
   saw in bzip2 impressed me so much, that I really just *had* to steal them.  I was especially
   blown away by the handling of the [ss,ss] case (see below), which makes a front-end
   run-length-encoder unnecessary.
*/
void
BWTencoder::doTransform()
{
    uint_t i, j, k;
    bool bigDone[256];
    uint_t bigOrder[256];

    // small block is a special case because we unrolled the loop
    if (_blockSize < 64)
    {
        for (i = 0; i < _blockSize; i++)
        {
            _ptr[i] = i;
        }
        insertionSort(0, _blockSize);
    }
    else
    {
        stripe();

        // radix-sort on first two bytes
        memset(_freq, 0, (KB(64) + 1) * sizeof(uint_t));
        for (i = 0; i < _blockSize; i++)
        {
            _freq[_stripe[i] >> 16]++;
        }
        for (i = 1; i <= KB(64); i++)
        {
            _freq[i] += _freq[i - 1];
        }
        for (i = 0; i < _blockSize; i++)
        {
            j = _stripe[i] >> 16;
            _ptr[--_freq[j]] = i;
        }

        // calculate bigOrder[]
        for (i = 0; i < 256; i++)
        {
            bigDone[i] = false;
            bigOrder[i] = i;
        }
        uint_t h = 1;
        do
            h = 3 * h + 1;
        while (h <= 256);
        do
        {
            h /= 3;
            for (i = h; i < 256; i++)
            {
                uint_t vv = bigOrder[i];
                j = i;
                while (BWT_BIGFREQ(bigOrder[j - h]) > BWT_BIGFREQ(vv))
                {
                    bigOrder[j] = bigOrder[j - h];
                    j -= h;
                    if (j <= (h - 1))
                        break;
                }
                bigOrder[j] = vv;
            }
        } while (h != 1);

        for (i = 0; i < 256; i++)
        {
            uint_t ss = bigOrder[i];

#ifdef BWT_DEBUG
            cerr << "i = " << i << ", ss = " << ss << endl;
#endif

            for (j = 0; j < 256; j++)
            {
                if (j == ss)
                    continue;

                // order [ss,j]
                uint_t sb = (ss << 8) + j;
                if (!BWT_ISSORTED(sb))
                {
                    uint_t lo = (_freq[sb] & BWT_MASK_CLEAR);
                    uint_t hi = (_freq[sb + 1] & BWT_MASK_CLEAR) - 1;
                    if (hi > lo)
                    {
#ifdef BWT_DEBUG
                        cerr << "    j = " << j << ", "
                             << "sort(" << lo << "," << hi + 1 << ")" << endl;
                        if ((i == 253) && (ss == 1) && (j == 0))
                        {
                            printBlock(lo, hi + 1, 32);
                        }
#endif
                        multiKeyQuickSort(lo, hi + 1, 2);
                    }
                    _freq[sb] |= BWT_MASK_SORTED;
                }
            }

            // order [ss,ss] (this is pretty neat)
            uint_t get0, put0, get1, put1;
            uint_t sbn = (ss << 8) + ss;
            uint_t lo = (_freq[sbn] & BWT_MASK_CLEAR);
            uint_t hi = (_freq[sbn + 1] & BWT_MASK_CLEAR) - 1;
            byte_t ssc = (byte_t)ss;
            get0 = _freq[ss << 8] & BWT_MASK_CLEAR;
            put0 = lo;
            while (get0 < put0)
            {
                j = _ptr[get0] - 1;
                if (j == uint_t_max)
                    j = _blockSize - 1;
                if (_block[j] == ssc)
                {
                    _ptr[put0++] = j;
                }
                get0++;
            }
            get1 = (_freq[(ss + 1) << 8] & BWT_MASK_CLEAR) - 1;
            put1 = hi;
            while (get1 > put1)
            {
                j = _ptr[get1] - 1;
                if (j == uint_t_max)
                    j = _blockSize - 1;
                if (_block[j] == ssc)
                {
                    _ptr[put1--] = j;
                }
                get1--;
            }
            _freq[sbn] |= BWT_MASK_SORTED;

            // [ss,*] are now ordered
            bigDone[ss] = true;

            // mark blocks for faster comparisons later on
            // (no point in doing this if we're on the last big block)
            if (i < 255)
            {
                uint_t lo = _freq[ss << 8] & BWT_MASK_CLEAR;
                uint_t size = (_freq[(ss + 1) << 8] & BWT_MASK_CLEAR) - lo;
                // only 16 bits available for tag
                uint_t shifts = 0;
                while ((size >> shifts) >= KB(64))
                    shifts++;
                for (j = 0; j < size; j++)
                {
                    uint_t val = j >> shifts;
                    k = _ptr[lo + j];
                    _stripe[k] &= 0xffff0000;
                    _stripe[k] |= val;
                    if (k < 64)
                    {
                        _stripe[_blockSize + k] = _stripe[k];
                    }
                }
            }

            // order [j, ss]
            uint_t ssPut[256];
            for (j = 0; j < 256; j++)
            {
                ssPut[j] = (_freq[(j << 8) + ss] & BWT_MASK_CLEAR);
            }
            lo = (_freq[ss << 8] & BWT_MASK_CLEAR);
            hi = (_freq[(ss + 1) << 8] & BWT_MASK_CLEAR);
            for (j = lo; j < hi; j++)
            {
                k = _ptr[j] - 1;
                if (k == uint_t_max)
                    k = _blockSize - 1;
                byte_t c1 = _block[k];
                if (!bigDone[c1])
                {
                    _ptr[ssPut[c1]++] = k;
                }
            }
            for (j = 0; j < 256; j++)
            {
                _freq[(j << 8) + ss] |= BWT_MASK_SORTED;
            }
        }
    }

    // find the origin
    for (i = 0; i < _blockSize; i++)
    {
        if (_ptr[i] == 1)
        {
            _origin = i;
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::undoTransform(byte_t* block)
{
    uint_t i, j, c, sum;

    // determine count[].  count[i] = #/occurences of character i.
    uint_t count[256];
    memset(count, 0, 256 * sizeof(uint_t));
    for (i = 0; i < _blockSize; i++)
    {
        count[_block[i]]++;
    }
    // cum[i] = count[0] + count[1] + ... + count[i - 1]
    uint_t cum[256];
    for (sum = 0, i = 0; i < 256; i++)
    {
        cum[i] = sum;
        sum += count[i];
        count[i] = 0;
    }
    // ptr[i] = pointer to i's successor string
    for (i = 0; i < _blockSize; i++)
    {
        c = _block[i];
        _ptr[cum[c] + count[c]] = i;
        count[c]++;
    }
    // now we can re-create the original block
    j = _origin;
    for (i = 0; i < _blockSize; i++)
    {
        block[i] = _block[j];
        j = _ptr[j];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
BWTencoder::compare(uint_t i1, uint_t i2, uint_t depth)
{
    if (i1 == i2)
    {
        return 0;
    }

    uint_t nComps;
    uint32_t w1, w2, *wp1, *wp2, *wpLimit;

    nComps = 0;
    wp1 = _stripe + BWT_CH_IDX(i1);
    wp2 = _stripe + BWT_CH_IDX(i2);
    wpLimit = _stripe + _blockSize;
    for (;;)
    {
        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        w1 = *wp1;
        w2 = *wp2;
        if (w1 < w2)
            return -1;
        else if (w1 > w2)
            return 1;
        wp1 += 4;
        wp2 += 4;

        nComps += 64;
        if (nComps >= _blockSize)
        {
            return 0;
        }
        if (wp1 >= wpLimit)
        {
            wp1 -= _blockSize;
        }
        if (wp2 >= wpLimit)
        {
            wp2 -= _blockSize;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::insertionSort(uint_t begin, uint_t end)
{
    uint_t pi, pj;
    const byte_t* s;
    const byte_t* t;
    const byte_t* eob = _block + _blockSize;
    const byte_t* sLim;
    for (pi = begin + 1; pi < end; pi++)
    {
        for (pj = pi; pj > begin; pj--)
        {
            s = _block + ((_ptr[pj - 1]) % _blockSize);
            t = _block + (_ptr[pj] % _blockSize);
            sLim = s;
            while (*s == *t)
            {
                s++;
                if (s == eob)
                    s = _block;
                t++;
                if (t == eob)
                    t = _block;
                if (s == sLim)
                    break;
            }
            if (*s <= *t)
            {
                break;
            }
            swap(pj - 1, pj);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
BWTencoder::medianOfThree(uint_t l, uint_t m, uint_t h, uint_t depth)
{
    byte_t vl, vm, vh;
    if ((vl = BWT_CH(l)) == (vm = BWT_CH(m)))
    {
        return l;
    }
    if (((vh = BWT_CH(h)) == vl) || (vh == vm))
    {
        return h;
    }
    if (vl < vm)
    {
        return (vm < vh) ? m : (vl < vh) ? h : l;
    }
    else
    {
        return (vm > vh) ? m : (vl < vh) ? l : h;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::multiKeyInsertionSort(uint_t begin, uint_t end, uint_t depth)
{
    uint_t i, j;
    for (i = begin + 1; i < end; i++)
    {
        for (j = i; j > begin; j--)
        {
            if (compare(j - 1, j, depth) <= 0)
            {
                break;
            }
            swap(j - 1, j);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::multiKeyQuickSort(uint_t begin, uint_t end, uint_t depth)
{
    int r;
    uint_t i, l, m, h, a, b, c, d, n;
    uint_t stack[96];
    uint_t* sp = stack;
    uint_t bed[4][4];

    for (;;)
    {
        n = end - begin;
        while ((n <= 20) || (depth >= 16))
        {
            if (n > 1)
            {
                quickSort(begin, end, depth);
            }

            // pop a segment off the stack -- we're done if stack is empty
            if (sp == stack)
            {
                return;
            }
            depth = *(--sp);
            end = *(--sp);
            begin = *(--sp);
            n = end - begin;
        }

        // median-of-three (or pseudo-median of 9) partitioning
        l = begin;
        m = (begin + end) / 2;
        h = end - 1;
        if (n > 30)
        {
            uint_t n8 = n / 8;
            l = medianOfThree(l, l + n8, l + 2 * n8, depth);
            m = medianOfThree(m - n8, m, m + n8, depth);
            h = medianOfThree(h - 2 * n8, h - n8, h, depth);
        }
        m = medianOfThree(l, m, h, depth);
        swap(begin, m);
        uint_t v = BWT_CH(begin);

        a = b = begin + 1;
        c = d = end - 1;

        for (;;)
        {
            while (b <= c)
            {
                r = BWT_CH(b) - v;
                if (r > 0)
                {
                    break;
                }
                if (r == 0)
                {
                    swap(a, b);
                    a++;
                }
                b++;
            }
            // [begin,a) =  v
            // [a, b)    <  v
            // [begin,b) <= v
            // (b > c) || ([b] > v)

            while (b <= c)
            {
                r = BWT_CH(c) - v;
                if (r < 0)
                {
                    break;
                }
                if (r == 0)
                {
                    swap(c, d);
                    d--;
                }
                c--;
            }
            // [c + 1,d + 1)   >  v
            // [d + 1,end)     =  v
            // [c + 1,end)     >= v
            // (b > c) || ([c] < v)

            if (b > c)
            {
                break;
            }
            // ([b] > v) && ([c] < v)
            swap(b, c);
            // ([b] < v) && ([c] > v)
            b++;
            c--;
        }

        r = min(a - begin, b - a);
        swap(begin, b - r, r);
        r = min(d - c, end - d - 1);
        swap(b, end - r, r);

        // identify segments to be sorted
        r = b - a;
        bed[0][0] = begin;
        bed[0][1] = begin + r;
        bed[0][2] = depth;
        bed[0][3] = r;
        bed[1][0] = begin + r;
        bed[1][1] = end - (d - c);
        bed[1][2] = depth + 1;
        bed[1][3] = bed[1][1] - bed[1][0];
        r = d - c;
        bed[2][0] = end - r;
        bed[2][1] = end;
        bed[2][2] = depth;
        bed[2][3] = r;

        // bubble sort in order of increasing size
        if (bed[2][3] < bed[1][3])
        {
            memmove(bed[3], bed[1], 4 * sizeof(uint_t));
            memmove(bed[1], bed[2], 4 * sizeof(uint_t));
            memmove(bed[2], bed[3], 4 * sizeof(uint_t));
        }
        if (bed[1][3] < bed[0][3])
        {
            memmove(bed[3], bed[0], 4 * sizeof(uint_t));
            memmove(bed[0], bed[1], 4 * sizeof(uint_t));
            memmove(bed[1], bed[3], 4 * sizeof(uint_t));
        }
        if (bed[2][3] < bed[1][3])
        {
            memmove(bed[3], bed[1], 4 * sizeof(uint_t));
            memmove(bed[1], bed[2], 4 * sizeof(uint_t));
            memmove(bed[2], bed[3], 4 * sizeof(uint_t));
        }

        // push larger segments onto the stack
        for (i = 2; i >= 1; i--)
        {
            if (bed[i][3] > 1)
            {
                ASSERTD(sp != (stack + 96));
                *sp++ = bed[i][0];
                *sp++ = bed[i][1];
                *sp++ = bed[i][2];
            }
        }

        // prepare to sort the smallest segment
        begin = bed[0][0];
        end = bed[0][1];
        depth = bed[0][2];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::quickSort(uint_t begin, uint_t end, uint_t depth)
{
    uint_t l = begin, h = end;
    uint_t i, j, k, mid, p;
    uint_t stack[64];
    uint_t* sp = stack;

    if ((h-- - l) <= 1)
    {
        return;
    }

    for (;;)
    {
        if ((h - l) >= 9)
        {
            mid = (l + h) >> 1;
            if (compare(l, mid, depth) > 0)
            {
                swap(l, mid);
            }
            if (compare(mid, h, depth) > 0)
            {
                swap(mid, h);
            }
            if (compare(l, mid, depth) > 0)
            {
                swap(l, mid);
            }
            swap(mid, h - 1);
            p = h - 1;
            i = l;
            j = h - 1;
            for (;;)
            {
                for (;;)
                {
                    i++;
                    if (compare(i, p, depth) >= 0)
                    {
                        break;
                    }
                }
                for (;;)
                {
                    j--;
                    if (compare(j, p, depth) <= 0)
                    {
                        break;
                    }
                }
                if (i >= j)
                {
                    break;
                }
                swap(i, j);
            }
            swap(i, h - 1);
            if ((i - l) > (h - i))
            {
                *sp++ = l;
                *sp++ = i - 1;
                l = i + 1;
            }
            else
            {
                *sp++ = i + 1;
                *sp++ = h;
                h = i - 1;
            }
        }
        else
        {
            i = l;
            while (++i <= h)
            {
                _ptr[_blockSize] = _ptr[i];
                k = i;
                for (;;)
                {
                    if (compare(_blockSize, k - 1, depth) >= 0)
                    {
                        break;
                    }
                    _ptr[k] = _ptr[k - 1];
                    if (--k == l)
                    {
                        break;
                    }
                }
                _ptr[k] = _ptr[_blockSize];
            }
            if (sp == stack)
            {
                return;
            }
            h = *(--sp);
            l = *(--sp);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::stripe()
{
    uint_t i;
    int i1, i2, i3;
    byte_t c;
    memset(_stripe, 0, _blockSize * sizeof(uint32_t));
    for (i = 0; i < _blockSize; i++)
    {
        c = _block[i];
        i1 = i - 1;
        if (i1 < 0)
            i1 += _blockSize;
        i2 = i - 2;
        if (i2 < 0)
            i2 += _blockSize;
        i3 = i - 3;
        if (i3 < 0)
            i3 += _blockSize;
        _stripe[i] |= (c << 24);
        _stripe[i1] |= (c << 16);
        _stripe[i2] |= (c << 8);
        _stripe[i3] |= c;
    }
    for (i = _blockSize; i < _blockSize + 64; i++)
    {
        _stripe[i] = _stripe[i - _blockSize];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// MTF encoder ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::mtfEncodeBlock()
{
    uint_t i, c, j, k, t, t2;
    uint_t* myPos;

    for (i = 0; i < _blockSize; i++)
    {
        // c is the current char
        j = _ptr[i];
        if (j == 0)
            j = _blockSize - 1;
        else
            j--;
        c = _block[j];

        // find the position (k) of c
        k = 0;
        myPos = _pos;
        t = *myPos;
        while (t != c)
        {
            t2 = t;
            t = *++myPos;
            k++;
            *myPos = t2;
        }

        // move c to the front
        _pos[0] = t;

        // encode c's position (k)
        if (k > 0)
        {
            if (_numZeroes > 0)
            {
                mtfEncodeZeroes();
            }
            mtfEncodeSymbol(k);
        }
        else
        {
            _numZeroes++;
        }
    }
    if (_numZeroes > 0)
    {
        mtfEncodeZeroes();
    }
    mtfEncodeSymbol(BWT_EOB);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::mtfDecodeBlock()
{
    uint_t c;
    byte_t* p = _block;
    byte_t* pLim = p + _iBuf.size();
    for (;;)
    {
        c = mtfDecodeSymbol();
        if ((c == BWT_ZRUN0) || (c == BWT_ZRUN1))
        {
            uint_t numZeroes = 0;
            do
            {
                numZeroes <<= 1;
                if (c == BWT_ZRUN1)
                {
                    numZeroes |= 1;
                }
                numZeroes++;
                c = mtfDecodeSymbol();
            } while ((c == BWT_ZRUN0) || (c == BWT_ZRUN1));
            while ((numZeroes-- > 0) && (p < pLim))
            {
                *p++ = _pos[0];
            }
        }
        if (c == BWT_EOB)
        {
            _blockSize = p - _block;
            return;
        }
        if (p == pLim)
            return;
        *p++ = _pos[c];
        uint_t t = _pos[c];
        for (uint_t i = c; i > 0; i--)
        {
            _pos[i] = _pos[i - 1];
        }
        _pos[0] = t;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::mtfEncodeSymbol(uint_t symbol)
{
    switch (symbol)
    {
    case BWT_ZRUN0:
        _A.encode(BWT_ZRUN0 - 256, _H[0]);
        break;
    case BWT_ZRUN1:
        _A.encode(BWT_ZRUN1 - 256, _H[0]);
        break;
    case BWT_EOB:
        _A.encode(BWT_EOB - 256, _H[0]);
        break;
    case 1:
        _A.encode(BWT_ONE - 256, _H[0]);
        break;
    case 2:
    case 3:
        _A.encode(BWT_M2 - 256, _H[0]);
        _A.encode(symbol - 2, _H[1]);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        _A.encode(BWT_M4 - 256, _H[0]);
        _A.encode(symbol - 4, _H[2]);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        _A.encode(BWT_M8 - 256, _H[0]);
        _A.encode(symbol - 8, _H[3]);
        break;
    default:
        if ((symbol >= 16) && (symbol <= 31))
        {
            _A.encode(BWT_M16 - 256, _H[0]);
            _A.encode(symbol - 16, _H[4]);
        }
        else if ((symbol >= 32) && (symbol <= 63))
        {
            _A.encode(BWT_M32 - 256, _H[0]);
            _A.encode(symbol - 32, _H[5]);
        }
        else if ((symbol >= 64) && (symbol <= 127))
        {
            _A.encode(BWT_M64 - 256, _H[0]);
            _A.encode(symbol - 64, _H[6]);
        }
        else
        {
            _A.encode(BWT_M128 - 256, _H[0]);
            _A.encode(symbol - 128, _H[7]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
BWTencoder::mtfDecodeSymbol()
{
    switch (_A.decode(_H[0]))
    {
    case BWT_ZRUN0 - 256:
        return BWT_ZRUN0;
    case BWT_ZRUN1 - 256:
        return BWT_ZRUN1;
    case BWT_EOB - 256:
        return BWT_EOB;
    case BWT_ONE - 256:
        return 1;
    case BWT_M2 - 256:
        return _A.decode(_H[1]) + 2;
    case BWT_M4 - 256:
        return _A.decode(_H[2]) + 4;
    case BWT_M8 - 256:
        return _A.decode(_H[3]) + 8;
    case BWT_M16 - 256:
        return _A.decode(_H[4]) + 16;
    case BWT_M32 - 256:
        return _A.decode(_H[5]) + 32;
    case BWT_M64 - 256:
        return _A.decode(_H[6]) + 64;
    default:
        return _A.decode(_H[7]) + 128;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::mtfEncodeRandomWord(uint_t w)
{
    mtfEncodeRandom((w & 0xff000000) >> 24);
    mtfEncodeRandom((w & 0x00ff0000) >> 16);
    mtfEncodeRandom((w & 0x0000ff00) >> 8);
    mtfEncodeRandom((w & 0x000000ff));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
BWTencoder::mtfDecodeRandomWord()
{
    uint_t i, b[4];
    for (i = 0; i < 4; i++)
    {
        b[i] = mtfDecodeRandom();
    }
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::mtfEncodeRandom(uint_t symbol)
{
    _A.encode(symbol, _H[8]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
BWTencoder::mtfDecodeRandom()
{
    return _A.decode(_H[8]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::mtfEncodeZeroes()
{
    uint_t numBits = 0;
    uint_t bits = 0;
    while (_numZeroes > 0)
    {
        bits <<= 1;
        _numZeroes--;
        if ((_numZeroes & 1) != 0)
        {
            bits |= 1;
        }
        _numZeroes >>= 1;
        numBits++;
    }
    while (numBits-- > 0)
    {
        if (bits & 1)
        {
            mtfEncodeSymbol(BWT_ZRUN1);
        }
        else
        {
            mtfEncodeSymbol(BWT_ZRUN0);
        }
        bits >>= 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef BWT_DEBUG
void
BWTencoder::printBlock(uint_t begin, uint_t end, uint_t numBytes)
{
    if (end == uint_t_max)
    {
        end = _blockSize;
    }

    uint_t i;
    for (i = begin; i < end; i++)
    {
        uint_t j;
        fprintf(stderr, "%08u  ", 2097152 + _ptr[i]);
        for (j = 0; j < numBytes; j++)
        {
            uint_t idx = (_ptr[i] + j) % _blockSize;
            uint_t ch = _block[idx];
            fprintf(stderr, "%02x ", ch);
        }
        fprintf(stderr, "\n");
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
