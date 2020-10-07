#pragma once

#include <cmath>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Pointer-like traits.

   Define how many low-order bits are available for use in a pointer-like type.
   Define a method for extracting the pointer value.

   \author Adam McKee
   \ingroup utility
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename PtrT>
struct PointerLikeTraits
{
    typedef typename std::remove_pointer<PtrT>::type T;
    static constexpr int FreeBits = (alignof(T) >= 8) ? 3 : (alignof(T) == 4) ? 2 : 1;
    static auto
    getPointer(uintptr_t value)
    {
        return reinterpret_cast<PtrT>(value);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Non-template base class for PointerIntPair.

   \author Adam McKee
   \ingroup utility
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class PointerIntPairBase
{
public:
    PointerIntPairBase(uintptr_t value)
        : _value(value)
    {
    }

    PointerIntPairBase(const PointerIntPairBase&) = default;

    /** Get raw value. */
    auto
    get() const
    {
        return _value;
    }

    /** Don't try to serialize. */
    void
    serialize(Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        ABORT();
        utl::serialize(_value, stream, io, mode);
    }

    /// \name Comparison Operators
    //@{
    inline bool
    operator<(const PointerIntPairBase& rhs) const
    {
        return _value < rhs._value;
    }
    inline bool
    operator<=(const PointerIntPairBase& rhs) const
    {
        return _value <= rhs._value;
    }
    inline bool
    operator==(const PointerIntPairBase& rhs) const
    {
        return _value == rhs._value;
    }
    inline bool
    operator>(const PointerIntPairBase& rhs) const
    {
        return _value > rhs._value;
    }
    inline bool
    operator>=(const PointerIntPairBase& rhs) const
    {
        return _value >= rhs._value;
    }
    //@}

protected:
    uintptr_t _value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   (Pointer,Int) pair.

   Use available low-order bits in pointer-like values to store small integer values.
   The alignment of the pointed-to object determines how many bits are available for use.

   \author Adam McKee
   \ingroup utility
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename PtrT,
          int IntBits,
          typename IntT = utl::uint_t,
          typename PtrTraits = PointerLikeTraits<PtrT>>
class PointerIntPair : public PointerIntPairBase
{
public:
    PointerIntPair()
        : PointerIntPairBase(0)
    {
    }

    PointerIntPair(uintptr_t value)
        : PointerIntPairBase(value)
    {
    }

    PointerIntPair(const PointerIntPair&) = default;

    /** Set pointer & value. */
    void
    set(PtrT ptr, IntT val)
    {
        ASSERTD(((uintptr_t)ptr & PtrMask) == (uintptr_t)ptr);
        ASSERTD(((uintptr_t)val & IntMask) == (uintptr_t)(val));
        _value = (uintptr_t)ptr | (uintptr_t)(((intptr_t)val << IntShift));
    }

    /// \name Pointer Value Accessors
    //@{
    auto
    getPointer() const
    {
        return PtrTraits::getPointer(_value & PtrMask);
    }

    void
    setPointer(PtrT ptr)
    {
        ASSERTD(((uintptr_t)ptr & PtrMask) == (uintptr_t)ptr);
        _value = (_value & IntMask) | (uintptr_t)ptr;
    }
    //}

    /// \name Integer Value Accessors
    //@{
    auto
    getInt() const
    {
        return (IntT)((_value >> IntShift) & IntMask);
    }

    void
    setInt(IntT val)
    {
        ASSERTD(((uintptr_t)val & IntMask) == (uintptr_t)(val));
        _value = (_value & PtrMask) | (uintptr_t)(((intptr_t)val << IntShift));
    }
    //@}

private:
    static constexpr int IntShift = PtrTraits::FreeBits - IntBits;
    static constexpr uintptr_t IntMask = (uintptr_t)(((intptr_t)1 << IntBits) - 1);
    static constexpr uintptr_t PtrMask = ~(uintptr_t)(((intptr_t)1 << PtrTraits::FreeBits) - 1);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename PtrT, int IntBits, typename IntT, typename PtrTraits>
inline bool
operator==(const utl::PointerIntPair<PtrT, IntBits, IntT, PtrTraits>& lhs, std::nullptr_t rhs)
{
    return (lhs.getPointer() == nullptr);
}

template <typename PtrT, int IntBits, typename IntT, typename PtrTraits>
inline bool
operator==(std::nullptr_t lhs, const utl::PointerIntPair<PtrT, IntBits, IntT, PtrTraits>& rhs)
{
    return (rhs.getPointer() == nullptr);
}

template <typename PtrT, int IntBits, typename IntT, typename PtrTraits>
inline bool
operator!=(const utl::PointerIntPair<PtrT, IntBits, IntT, PtrTraits>& lhs, std::nullptr_t rhs)
{
    return (lhs.getPointer() != nullptr);
}

template <typename PtrT, int IntBits, typename IntT, typename PtrTraits>
inline bool
operator!=(std::nullptr_t lhs, const utl::PointerIntPair<PtrT, IntBits, IntT, PtrTraits>& rhs)
{
    return (rhs.getPointer() != nullptr);
}
