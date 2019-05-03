#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FwdIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Bi-directional iterator abstraction.

   BidIt inherits from FwdIt, and adds the ability to move backwards through the sequence of
   objects.

   \author Adam McKee
   \ingroup iterator
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BidIt : public FwdIt
{
    UTL_CLASS_DECL_ABC(BidIt, FwdIt);
    UTL_CLASS_DEFID;

public:
    /**
       Move backward the given number of objects.  If the beginning of the sequence is reached,
       backward movement will stop without error.
       \param dist distance to move backward
    */
    virtual void reverse(size_t dist = 1) = 0;

    /**
       Pre-decrement operator.  Move backward one object.
       \return self
    */
    BidIt&
    operator--()
    {
        reverse();
        return *this;
    }

    /**
       Post-decrement operator.  Move backward one object.
       \return copy of self before moving backward
    */
    void
    operator--(int)
    {
        reverse();
    }

    /**
       Decrement operator.  Move backward the given number of objects.
       \return self
       \param dist distance to move backward
    */
    BidIt&
    operator-=(size_t dist)
    {
        reverse(dist);
        return *this;
    }

public:
    // for STL
    typedef Object* value_type;
    typedef Object*& reference;
    typedef Object** pointer;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TBidIt.h>
