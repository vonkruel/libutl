#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Random-access iterator abstraction.

   RandIt is a random-access iterator abstraction.  RandIt inherits from BidIt, and adds the
   ability to randomly access the sequence of objects.

   \author Adam McKee
   \ingroup iterator
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RandIt : public BidIt
{
    UTL_CLASS_DECL_ABC(RandIt, BidIt);
    UTL_CLASS_DEFID;

public:
    virtual Object* get() const = 0;

    /**
       Get the object at the given offset.
       \return found object (nullptr if offset is >= end)
       \param offset position of object within sequence
    */
    virtual Object* get(size_t offset) const = 0;

    /** Get the current offset. */
    virtual size_t offset() const = 0;

    /** Seek to the given offset. */
    virtual void seek(size_t offset) = 0;

    virtual void set(const Object* object) = 0;

    /**
       Set the object at the given offset.  This method may not be called when the "const" flag
       is true.  If there is already an object at offset, it will be replaced by the given object
       (or removed if object == nullptr).  If offset refers to the end of the sequence, the given
       object will be added to the sequence.
       \param offset position within sequence
       \param object object to put at offset
    */
    virtual void set(size_t offset, const Object* object) = 0;

    /** Return the current size of the sequence. */
    virtual size_t size() const = 0;

    /**
       Determine the distance between self and another random-access iterator for the same
       sequence.  This is calculated simply as the difference between the current offsets of the
       two iterators.
       \return self.offset() - it.offset()
       \param it iterator to compare with
    */
    virtual size_t
    subtract(const RandIt& it) const
    {
        return (offset() - it.offset());
    }

    /** Provides a user-friendly interface to subtract(). */
    size_t
    operator-(const RandIt& it) const
    {
        return subtract(it);
    }

    /** Provides a user-friendly interface to get(). */
    Object* operator[](size_t offset) const
    {
        return get(offset);
    }

public:
    // for STL
    typedef Object* value_type;
    typedef Object*& reference;
    typedef Object** pointer;
    typedef std::random_access_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TRandIt.h>
