#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class BitArrayElem;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Array of n-bit values.

   BitArray provides a space-efficient way to represent an array of n-bit numbers.  For example,
   you could choose n = 1 if you'd like to store an array of boolean values without wasting memory.
   If you'd like to store 4-bit numbers (in the range [0,15]), you'd choose n = 4.

   If n = 8, 16, 32, or 64, there's no real point in using BitArray since it won't save you any
   memory, but it will cost you CPU cycles.  In those cases, just use the appropriate built-in type.

   Thanks to BitArrayElem, you can use a BitArray much like you would a regular C-style array.
   Have a look at the BitArray example program.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BitArray : public Object
{
    UTL_CLASS_DECL(BitArray, Object);

public:
    /**
       Constructor.
       \param size initial size (number of values)
       \param numBits bits per valuBe
    */
    BitArray(uint64_t size, uint_t numBits = 1)
    {
        init();
        initialize(size, numBits);
    }

    virtual size_t innerAllocatedSize() const;

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    virtual void dump(Stream& os, uint_t level = uint_t_max) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    void clear();

    /** Initialize the array. */
    void initialize(uint64_t size, uint_t numBits = 1);

    /** Get the array size. */
    uint64_t
    size() const
    {
        return _size;
    }

    /** Grow to the given size. */
    void
    grow(uint64_t size)
    {
        if (size > this->size())
            setSize(size);
    }

    /** Change the size. */
    void setSize(uint64_t size);

    /** Get the value at the given position. */
    uint64_t get(uint64_t pos) const;

    /** Set the value at the given index. */
    void set(uint64_t idx, uint64_t val);

    /** Set all values the same. */
    void setAll(uint64_t val);

    /** Return the value at the given index. */
    uint64_t operator[](uint_t idx) const
    {
        return get((uint64_t)idx);
    }

    /** Return the value at the given index. */
    uint64_t operator[](int idx) const
    {
        return get((uint64_t)idx);
    }

    /** Return the value at the given index. */
    uint64_t operator[](uint64_t idx) const
    {
        return get((uint64_t)idx);
    }

    /** Return the value at the given index. */
    uint64_t operator[](int64_t idx) const
    {
        return get((uint64_t)idx);
    }

    /** Return a BitArrayElem for the given index. */
    BitArrayElem operator[](uint_t idx);

    /** Return a BitArrayElem for the given index. */
    BitArrayElem operator[](int idx);

    /** Return a BitArrayElem for the given index. */
    BitArrayElem operator[](uint64_t idx);

    /** Return a BitArrayElem for the given index. */
    BitArrayElem operator[](int64_t idx);

private:
    void init();
    void deInit();

private:
    uint_t _numBits;
    uint64_t _size;
    size_t _arraySize;
    uint64_t* _array;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BitArrayElem.h>
