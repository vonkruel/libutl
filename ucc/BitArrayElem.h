#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BitArray.h>
#include <libutl/String.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reference to a value stored in a BitArray.

   You should not need to use BitArrayElem directly.  You will use it indirectly when you invoke
   <code>operator[]</code> on a non-const BitArray.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BitArrayElem : public Object
{
    UTL_CLASS_DECL(BitArrayElem, Object);
    UTL_CLASS_DEFID;
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_SERIALIZE;

public:
    /**
       Constructor.
       \param bitArray associated BitArray
       \param idx value index
    */
    BitArrayElem(BitArray* bitArray, uint64_t idx)
    {
        ASSERTD(bitArray != nullptr);
        _bitArray = bitArray;
        _idx = idx;
    }

    virtual void copy(const Object& rhs);

    virtual String toString() const;

    /// \name Accessors
    //@{
    /** Assignment from uint_t. */
    BitArrayElem&
    operator=(uint_t val)
    {
        _bitArray->set(_idx, val);
        return self;
    }

    /** Assignment from uint64_t. */
    BitArrayElem&
    operator=(uint64_t val)
    {
        _bitArray->set(_idx, val);
        return self;
    }

    /** Assignment from bool. */
    BitArrayElem&
    operator=(bool val)
    {
        _bitArray->set(_idx, val ? 1 : 0);
        return self;
    }

    /** Assignment from non-const BitArrayElem. */
    BitArrayElem&
    operator=(BitArrayElem& ref)
    {
        _bitArray->set(_idx, ref.get());
        return self;
    }

    /** Get value. */
    uint64_t
    get() const
    {
        return _bitArray->get(_idx);
    }

    /** Conversion to uint_t. */
    operator uint_t() const
    {
        return (uint_t)_bitArray->get(_idx);
    }

    /** Conversion to uint64_t. */
    operator uint64_t() const
    {
        return _bitArray->get(_idx);
    }

    /** Conversion to bool. */
    operator bool() const
    {
        return (_bitArray->get(_idx) != 0);
    }
    //@}

    /// \name Comparison Operators
    //@{
    /** Equal-to operator. */
    bool
    operator==(bool rhs) const
    {
        return (get() == (uint64_t)rhs);
    }
    /** Unequal-to operator. */
    bool
    operator!=(bool rhs) const
    {
        return (get() == (uint64_t)rhs);
    }

    /** Less-than operator. */
    bool
    operator<(uint64_t rhs) const
    {
        return (get() < rhs);
    }
    /** Less-than-or-equal-to operator. */
    bool
    operator<=(uint64_t rhs) const
    {
        return (get() <= rhs);
    }
    /** Greater-than operator. */
    bool
    operator>(uint64_t rhs) const
    {
        return (get() > rhs);
    }
    /** Greater-than-or-equal-to operator. */
    bool
    operator>=(uint64_t rhs) const
    {
        return (get() >= rhs);
    }
    /** Equal-to operator. */
    bool
    operator==(uint64_t rhs) const
    {
        return (get() == rhs);
    }
    /** Unequal-to operator. */
    bool
    operator!=(uint64_t rhs) const
    {
        return (get() != rhs);
    }

    /** Less-than operator. */
    bool
    operator<(int64_t rhs) const
    {
        return (get() < (uint64_t)rhs);
    }
    /** Less-than-or-equal-to operator. */
    bool
    operator<=(int64_t rhs) const
    {
        return (get() <= (uint64_t)rhs);
    }
    /** Greater-than operator. */
    bool
    operator>(int64_t rhs) const
    {
        return (get() > (uint64_t)rhs);
    }
    /** Greater-than-or-equal-to operator. */
    bool
    operator>=(int64_t rhs) const
    {
        return (get() >= (uint64_t)rhs);
    }
    /** Equal-to operator. */
    bool
    operator==(int64_t rhs) const
    {
        return (get() == (uint64_t)rhs);
    }
    /** Unequal-to operator. */
    bool
    operator!=(int64_t rhs) const
    {
        return (get() != (uint64_t)rhs);
    }

    /** Less-than operator. */
    bool
    operator<(const BitArrayElem& rhs) const
    {
        return (get() < rhs.get());
    }
    /** Less-than-or-equal-to operator. */
    bool
    operator<=(const BitArrayElem& rhs) const
    {
        return (get() <= rhs.get());
    }
    /** Greater-than operator. */
    bool
    operator>(const BitArrayElem& rhs) const
    {
        return (get() > rhs.get());
    }
    /** Greater-than-or-equal-to operator. */
    bool
    operator>=(const BitArrayElem& rhs) const
    {
        return (get() >= rhs.get());
    }
    /** Equal-to operator. */
    bool
    operator==(const BitArrayElem& rhs) const
    {
        return (get() == rhs.get());
    }
    /** Unequal-to operator. */
    bool
    operator!=(const BitArrayElem& rhs) const
    {
        return (get() != rhs.get());
    }
    //@}
private:
    BitArray* _bitArray;
    uint64_t _idx;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
