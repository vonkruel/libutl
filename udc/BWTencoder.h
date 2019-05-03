#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/ArithmeticEncoder.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

/**
   BWT-based coder.

   BWTencoder is a block-sorting, lossless data compressor based on the Burrows-Wheeler Transform
   (BWT).  \b bzip2 by <b>Julian Seward</b> is currently the most popular compressor based on the
   BWT.  I have stolen some ideas (and even code!) from bzip2, and I've made it clear (with code
   comments) where I've done so.  In fact, I have to admit that this implementation contains no
   original ideas or insights of my own.  My goal here isn't research.  I've tried to provide a
   solid implementation that is:

   \arg fast enough for many purposes
   \arg easy to read and maintain
   \arg easy to use within the UTL++ framework

   I think I've achieved these goals.  I'll leave it to you to decide if this implementation is a
   good choice for your application.  The best I can do is be honest about the strengths and
   weaknesses.

   <b>Attributes</b>

   \arg \b blockSize : Setting the block size is the sole method of compression tuning.
   Compression benefits from larger block size, but the law of diminishing returns applies here.
   256 KB tends to be a nice balance between compression ratio and memory/CPU usage.  I don't
   recommend a block size smaller than 64 KB or larger than 1 MB though.  Space required
   is approx. (9 * \b blockSize) for compression and (5 * \b blockSize) for decompression.

   <b>Advantages</b>

   \arg provides excellent compression
   \arg code is easy to read and maintain
   \arg fast enough for many purposes
   \arg easy to use within the UTL++ framework
   \arg you can study the code to learn about the BWT

   <b>Disadvantages</b>

   \arg it's a pig:
   \arg -- requires more space than bzip2
   \arg -- requires more time than bzip2 (a \b lot more in the worst case)
   \arg possible patent issues due to the use of arith-coding
   \arg not thoroughly tested -- use at your own risk!

   <b>Worked Example</b>

   I thought it would be useful to provide a worked example, with a bit of explanation.
   We'll transform a small piece of text, and then undo the transformation.

   Suppose the message to be transformed is "BANANA".  The first thing we'll do is "stripe"
   the original input to make 6 strings, as follows:

   \code
   0. BANANA
   1. ANANAB
   2. NANABA
   3. ANABAN
   4. NABANA
   5. ABANAN
   \endcode

   Next, we'll sort those strings:

   \code
   0. ABANAN
   1. ANABAN
   2. ANANAB
   3. BANANA
   4. NABANA
   5. NANABA
   \endcode

   To reverse the transformation & recover the original message, we only need the last column
   in the sort ("NNBAAA"), together with the starting point (2).  How can we do this?  We will
   do this by iterating over the characters in the last column of the sorted block, creating
   a map that tells us the index of the left-shifted version of each string, and then using
   that to traverse from the starting point ("B") to the 5 remaining characters in the message.

   How do we make this traversal map?  First, look at the strings in the sorted block that end in
   "N", and then find their right-shifted versions (their predecessors).

   \code
   NABANA --precedes--> ABANAN
   NANABA --precedes--> ANABAN
   \endcode

   The key observation here is that the predecessors of the strings ending in "N" will be found
   in the exact same order as their successors, and the reason for this is the block sort that
   was performed.  "ABANAN" is before "ANABAN" in the sorted block, and likewise "NABANA" is
   before "NANABA" in the sorted block.  If it was not so, that would mean the block had not
   been sorted.

   Armed with this understanding, it's a simple matter to reconstruct the original message.
   First, we create two arrays:

   \code
       cum[]   - for storing the index of the first string starting with a given symbol
       count[] - for storing the number of times we've encountered each symbol
       next[]  - for storing the index of the successor to each index
   \endcode

   Initially, cum[] and count[] are zeroed, and we initialize cum[] as follows:

   \code
       cum['A'] = 0
       cum['B'] = 3
       cum['N'] = 4
   \endcode

   Then we proceed down the last column in the sorted block, identifying its predecessor
   & storing a mapping from the predecessor to it.

   \code
   The predecessor of string 0 is calculated as cum['N'] + count['N'] which is 4.
   next[4] = 0 ; ++count['N']

   The predecessor of string 1 is calculated as cum['N'] + count['N'] which is 5.
   next[5] = 1 ; ++count['N']

   The predecessor of string 2 is calculated as cum['B'] + count['B'] which is 3.
   next[3] = 2 ; ++count['B']

   The predecessor of string 3 is calculated as cum['A'] + count['A'] which is 0.
   next[0] = 3 ; ++count['A']

   The predecessor of string 4 is calculated as cum['A'] + count['A'] which is 1.
   next[1] = 4 ; ++count['A']

   The predecessor of string 5 is calculated as cum['A'] + count['A'] which is 2.
   next[2] = 5 ; ++count['A']
   \endcode

   Putting all this information together, we have:

   \code
               next
   0. ABANAN    3
   1. ANABAN    4
   2. ANANAB    5 <-- starting point
   3. BANANA    2
   4. NABANA    0
   5. NANABA    1
   \endcode

   As you can see, undoing the transformation is very simple & efficient.

   The BWT uses the characters <em>following</em> each symbol as context.  We will tend to find
   the same characters preceding repeated sequences in the message, so the last column in the
   sorted block can be very efficiently encoded with a move-to-front (MTF) encoder.

   <b>Research Papers</b>

   \code
   M. Burrows and D. J. Wheeler
   "A block-sorting lossless data compression algorithm"
   SRC Research Report 124
   \endcode

   <b>Links</b>

   \arg http://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform
   \arg http://marknelson.us/1996/09/01/bwt/
   \arg http://www.bzip.org/

   \author Adam McKee
   \ingroup compression
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BWTencoder : public Encoder
{
    UTL_CLASS_DECL(BWTencoder, Encoder);

public:
    /**
       Constructor.
       \param mode \b io_rd to encode, \b io_wr to decode (see utl::io_t)
       \param stream (optional) associated stream
       \param owner (optional : true) \b owner flag for stream
       \param blockSize (optional : 256 KB) block size
    */
    BWTencoder(uint_t mode, Stream* stream, bool owner = true, uint_t blockSize = KB(256))
    {
        init();
        start(mode, stream, owner, blockSize);
    }

    virtual size_t decode(byte_t* block, size_t num);

    virtual size_t encode(const byte_t* block, size_t num);

    /**
       Initialize for encoding or decoding.
       \param mode \b io_rd to encode, \b io_wr to decode (see utl::io_t)
       \param stream (optional) associated stream
       \param owner (optional : true) \b owner flag for stream
       \param blockSize (optional : 256 KB) block size
    */
    void start(uint_t mode, Stream* stream, bool owner = true, uint_t blockSize = KB(256));

protected:
    virtual void clear();
    virtual void finishEncoding();

private:
    void init();
    void
    deInit()
    {
        close();
    }
    void clearSelf();
    // transform
    void doTransform();
    void undoTransform(byte_t* block);
    // sort
    inline int compare(uint_t i1, uint_t i2, uint_t depth);
    void insertionSort(uint_t begin, uint_t end);
    uint_t medianOfThree(uint_t l, uint_t m, uint_t h, uint_t depth);
    void multiKeyInsertionSort(uint_t begin, uint_t end, uint_t depth);
    void multiKeyQuickSort(uint_t begin, uint_t end, uint_t depth);
    void quickSort(uint_t begin, uint_t end, uint_t depth);
    void stripe();
    inline void swap(uint_t i, uint_t j);
    inline void swap(uint_t i, uint_t j, uint_t n);
    // mtf encoder
    void mtfEncodeBlock();
    void mtfDecodeBlock();
    void mtfEncodeSymbol(uint_t symbol);
    uint_t mtfDecodeSymbol();
    void mtfEncodeRandomWord(uint_t w);
    uint_t mtfDecodeRandomWord();
    void mtfEncodeRandom(uint_t symbol);
    uint_t mtfDecodeRandom();
    void mtfEncodeZeroes();
// debug
#ifdef DEBUG
    void printBlock(uint_t begin = 0, uint_t end = uint_t_max, uint_t numBytes = 32);
#endif
    // sort
    uint_t* _ptr;
    uint_t* _freq;
    uint32_t* _stripe;
    // mtf
    ArithmeticEncoder _A;
    ArithContext* _H[9];
    uint_t _numZeroes;
    uint_t _pos[256];
    // general
    byte_t* _block;
    uint_t _blockSize;
    uint_t _origin;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::swap(uint_t i, uint_t j)
{
    uint_t tmp = _ptr[i];
    _ptr[i] = _ptr[j];
    _ptr[j] = tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BWTencoder::swap(uint_t i, uint_t j, uint_t n)
{
    uint_t* pi = _ptr + i;
    uint_t* pj = _ptr + j;
    uint_t* piLim = pi + n;
    uint_t tmp;
    while (pi < piLim)
    {
        tmp = *pi;
        *pi = *pj;
        *pj = tmp;
        pi++;
        pj++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
