#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// FlagsMI /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Mix-in to provide 64-bits for space-efficient storage of up to 64 boolean flags.  You can also
   use some of the bits to store multi-bit patterns if you wish.

   \author Adam McKee
   \ingroup abstract
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class FlagsMI
{
public:
    /** Constructor. */
    FlagsMI()
    {
        _flags = 0;
    }

    /** Destructor. */
    virtual ~FlagsMI()
    {
    }

    /** Copy the given flags. */
    void
    copyFlags(const FlagsMI& rhs)
    {
        _flags = rhs._flags;
    }

    /**
       Copy (some of) the given flags.
       \param rhs right-hand-side flags
       \param lsb least significant bit (0-63)
       \param msb most significant bit (0-63)
    */
    void
    copyFlags(const FlagsMI& rhs, uint_t lsb, uint_t msb)
    {
        copyFlags(rhs._flags, lsb, msb);
    }

    /**
       Copy (some of) the given flags.
       \param flags right-hand-side flags
       \param lsb least significant bit (0-63)
       \param msb most significant bit (0-63)
    */
    void
    copyFlags(uint64_t flags, uint_t lsb, uint_t msb)
    {
        ASSERTD(lsb <= msb);
        ASSERTD(msb <= 63);
        uint64_t mask;
        if (msb == 63)
            mask = uint64_t_max;
        else
            mask = ((uint64_t)1 << (msb + 1)) - 1;
        if (lsb > 0)
            mask &= ~(((uint64_t)1 << lsb) - 1);
        _flags = (_flags & ~mask) | (flags & mask);
    }

    void
    serializeFlags(Stream& stream, uint_t io, uint_t mode)
    {
        utl::serialize(_flags, stream, io, mode);
    }

    /**
       Get a user-defined flag.
       \return requested flag (bool)
       \param flagNum flag # (0 <= flagNum <= 63)
    */
    bool
    getFlag(uint_t flagNum) const
    {
        ASSERTD(flagNum < 64);
        uint64_t mask = ((uint64_t)1 << flagNum);
        return ((_flags & mask) != 0);
    }

    /**
       Set a user-defined flag.
       \param flagNum flag # (0 <= flagNum <= 63)
       \param val new flag value
    */
    void
    setFlag(uint_t flagNum, bool val)
    {
        ASSERTD(flagNum < 64);
        uint64_t mask = ((uint64_t)1 << flagNum);
        if (val)
            _flags |= mask;
        else
            _flags &= ~mask;
    }

    /**
       Get a multi-bit value in the flags data (which is stored as one 64-bit integer).
       Return the bitwise AND of the flags with the given 64-bit integer.
       \param mask bitmask (e.g. 0x3 for a two-bit number using the two least-significant bits)
       \param shift the least-significant bit in the number (e.g. 0 if using the two lowest bits)
    */
    uint64_t
    getFlagsNumber(uint64_t mask, uint64_t shift = 0)
    {
        return (_flags & mask) >> shift;
    }

    /**
       Set a multi-bit value in the flags data (which is stored as one 64-bit integer).
       \param mask bitmask (e.g. 0x3 for a two-bit number using the two least-significant bits)
       \param shift the least-significant bit in the number (e.g. 0 if using the two lowest bits)
       \param num the number to set in the identified portion of the flags data
    */
    void
    setFlagsNumber(uint64_t mask, uint64_t shift, uint64_t num)
    {
        _flags &= ~mask;
        _flags |= (num << shift);
    }

    /** Get the flags. */
    uint64_t
    getFlags() const
    {
        return _flags;
    }

    /** Set the flags. */
    void
    setFlags(uint64_t flags)
    {
        _flags = flags;
    }

protected:
    uint64_t _flags;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// FlagGuard ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Flag guard.

   Safely toggle a flag stored in a FlagMI-enabled object.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class FlagGuard
{
public:
    /**
       Constructor.
       \param object object whose flag is to be toggled
       \param flagNum which flag to toggle
       \param val desired (temporary) value of flag
    */
    FlagGuard(FlagsMI* object, uint_t flagNum, bool val)
    {
        // save the original value and toggle the flag
        _object = object;
        _flagNum = flagNum;
        _value = object->getFlag(flagNum);
        object->setFlag(flagNum, val);
    }

    /** Restore the previous value of the flag. */
    ~FlagGuard()
    {
        ASSERTD(_object != nullptr);
        _object->setFlag(_flagNum, _value);
    }

private:
    FlagsMI* _object;
    uint_t _flagNum;
    bool _value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
