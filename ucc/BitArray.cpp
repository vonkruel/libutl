#include <libutl/libutl.h>
#include <libutl/BitArray.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BitArray);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
BitArray::innerAllocatedSize() const
{
    return _arraySize * sizeof(uint64_t);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
BitArray::compare(const Object& rhs) const
{
    auto& ba = utl::cast<BitArray>(rhs);
    int res = utl::compare(_numBits, ba._numBits);
    if (res != 0)
        return res;
    res = utl::compare(_size, ba._size);
    if (res != 0)
        return res;
    res = memcmp(_array, ba._array, _arraySize * sizeof(uint64_t));
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::copy(const Object& rhs)
{
    auto& ba = utl::cast<BitArray>(rhs);
    _numBits = ba._numBits;
    _size = ba._size;
    _arraySize = ba._arraySize;
    delete[] _array;
    if (_arraySize == 0)
    {
        _array = nullptr;
    }
    else
    {
        _array = new uint64_t[_arraySize];
        memcpy(_array, ba._array, _arraySize * sizeof(uint64_t));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::steal(Object& rhs_)
{
    auto& rhs = utl::cast<BitArray>(rhs_);
    if (_array != nullptr)
        delete[] _array;
    _numBits = rhs._numBits;
    _size = rhs._size;
    _arraySize = rhs._arraySize;
    _array = rhs._array;
    rhs._numBits = 0;
    rhs._size = 0;
    rhs._arraySize = 0;
    rhs._array = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::dump(Stream& os, uint_t level) const
{
    uint64_t i, lim = _size;
    for (i = 0; i < lim; i++)
    {
        os << "[" << i << "] = " << get(i) << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::serialize(Stream& stream, uint_t io, uint_t mode)
{
    super::serialize(stream, io, mode);
    utl::serialize(_numBits, stream, io, mode);
    utl::serialize(_size, stream, io, mode);
    utl::serialize(_arraySize, stream, io, mode);
    if (io == io_rd)
    {
        delete[] _array;
        if (_arraySize == 0)
            _array = nullptr;
        else
            _array = new uint64_t[_arraySize];
    }
    utl::serialize(_array, _arraySize, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::clear()
{
    if (_array == nullptr)
        return;
    _numBits = 0;
    _size = 0;
    _arraySize = 0;
    delete[] _array;
    _array = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::initialize(uint64_t size, uint_t numBits)
{
    clear();
    if (size == 0)
        return;
    ASSERTD(numBits > 0);
    ASSERTD(numBits < 64);
    _numBits = numBits;
    _size = size;
    auto totalBits = utl::nextMultipleOfPow2((size_t)64, size * numBits);
    _arraySize = (totalBits / 64);
    _array = new uint64_t[_arraySize];
    memset(_array, 0, _arraySize * sizeof(uint64_t));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::setSize(uint64_t size)
{
    // remember old array and its size
    auto oldArray = _array;
    auto oldArraySize = _arraySize;

    // find size of new array
    _size = size;
    auto totalBits = utl::nextMultipleOfPow2((uint64_t)64, size * _numBits);
    _arraySize = (totalBits / 64);

    // do nothing else if array size is unchanged
    if (_arraySize == oldArraySize)
        return;

    // create the new array and zero it to start with
    _array = new uint64_t[_arraySize];
    memset(_array, 0, _arraySize * sizeof(uint64_t));

    // copy values into the new array
    auto copySize = utl::min(oldArraySize, _arraySize);
    memmove(_array, oldArray, copySize * sizeof(uint64_t));

    // forget the old array
    delete[] oldArray;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t
BitArray::get(uint64_t idx) const
{
    ASSERTD(idx < _size);
    const uint64_t one = 1;
    auto firstBit = (idx * _numBits);
    auto lastBit = ((idx + 1) * _numBits) - 1;
    auto firstBitIdx = firstBit / 64;
    auto lastBitIdx = lastBit / 64;
    auto& word0 = _array[firstBitIdx];
    auto& word1 = _array[lastBitIdx];
    uint_t firstBitPos = 63 - (firstBit % 64);
    uint_t lastBitPos = 63 - (lastBit % 64);

    // first bit and last bit belong to the same word?
    if (firstBitIdx == lastBitIdx)
    {
        auto mask = (firstBitPos == 63) ? uint64_t_max : ((one << (firstBitPos + 1)) - 1);
        if (lastBitPos > 0)
        {
            mask &= ~((one << lastBitPos) - 1);
        }
        auto res = ((word0 & mask) >> lastBitPos);
        return res;
    }
    else
    {
        // value spans two words
        ASSERTD(lastBitIdx == (firstBitIdx + 1));
        auto bits0 = firstBitPos + 1;
        auto bits1 = _numBits - bits0;
        auto mask0 = ((one << bits0) - 1);
        auto mask1 = ~((one << (64 - bits1)) - 1);
        auto res = (word0 & mask0) << bits1;
        res |= ((word1 & mask1) >> (64 - bits1));
        return res;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::set(uint64_t idx, uint64_t val)
{
    ASSERTD(idx < _size);
    ASSERTD(val < ((uint64_t)1 << _numBits));
    const uint64_t one = 1;
    auto firstBit = (idx * _numBits);
    auto lastBit = ((idx + 1) * _numBits) - 1;
    auto firstBitIdx = firstBit / 64;
    auto lastBitIdx = lastBit / 64;
    auto& word0 = _array[firstBitIdx];
    auto& word1 = _array[lastBitIdx];
    uint_t firstBitPos = 63 - (firstBit % 64);
    uint_t lastBitPos = 63 - (lastBit % 64);

    // first bit and last bit belong to the same word?
    if (firstBitIdx == lastBitIdx)
    {
        auto mask = (firstBitPos == 63) ? uint64_t_max : ((one << (firstBitPos + 1)) - 1);
        if (lastBitPos > 0)
        {
            mask &= ~((one << lastBitPos) - 1);
        }
        word0 &= ~mask;
        word0 |= (val << lastBitPos);
    }
    else
    {
        // value spans two words
        ASSERTD(lastBitIdx == (firstBitIdx + 1));
        auto bits0 = firstBitPos + 1;
        auto bits1 = _numBits - bits0;
        auto val0 = val >> bits1;
        auto val1 = val << (64 - bits1);
        auto mask0 = ((one << bits0) - 1);
        auto mask1 = ~((one << (64 - bits1)) - 1);
        word0 &= ~mask0;
        word1 &= ~mask1;
        word0 |= val0;
        word1 |= val1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::setAll(uint64_t val)
{
    // simple case #1: val == 0
    if (val == 0)
    {
        memset(_array, 0, _arraySize * sizeof(uint64_t));
        return;
    }

    // simple case #2: val = (2^numBits) - 1
    if (val == (((uint64_t)1 << _numBits) - 1))
    {
        memset(_array, 0xff, _arraySize * sizeof(uint64_t));
        return;
    }

    // the slow way
    for (uint64_t idx = 0; idx < _size; ++idx)
    {
        set(idx, val);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BitArrayElem BitArray::operator[](uint_t idx)
{
    return BitArrayElem(this, (uint64_t)idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BitArrayElem BitArray::operator[](int idx)
{
    return BitArrayElem(this, (uint64_t)idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BitArrayElem BitArray::operator[](uint64_t idx)
{
    return BitArrayElem(this, idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BitArrayElem BitArray::operator[](int64_t idx)
{
    return BitArrayElem(this, (uint64_t)idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::init()
{
    _numBits = 0;
    _size = 0;
    _arraySize = 0;
    _array = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BitArray::deInit()
{
    delete[] _array;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
