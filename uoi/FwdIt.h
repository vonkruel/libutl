#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FlagsMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Forward iterator abstraction.

   A forward iterator allows forward movement through a sequence of objects.  FwdIt is the most
   basic kind of iterator.  More powerful iterators such as bi-directional (BidIt) and
   random-access (RandIt) are derived from it.

   \author Adam McKee
   \ingroup iterator
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class FwdIt : public Object, protected FlagsMI
{
    UTL_CLASS_DECL_ABC(FwdIt, Object);

public:
    /**
       Compare with another iterator.  The default implementation performs the comparison by
       comparing the objects referred to by each iterator (as determined by calling get()).
    */
    virtual int compare(const Object& rhs) const;

    /**
       Copy another iterator.
    */
    virtual void copy(const Object& rhs);

    /**
       Clone another iterator.
    */
    virtual void vclone(const Object& rhs);

    /**
       Move forward the given number of objects.  If the end of the sequence is reached, forward
       movement will stop without error.
       \param dist distance to move forward
    */
    virtual void forward(size_t dist = 1) = 0;

    /**
       Get the current object.
       \return current object (nullptr if at end of sequence)
    */
    virtual Object* get() const = 0;

    /**
       Get the \b const flag.  The \b const flag indicates that the sequence the iterator refers
       to cannot be modified by adding, changing, or removing objects.  Specifically, the set()
       method may not be called with the \b const flag is true.
    */
    bool
    isConst() const
    {
        return getFlag(flg_const);
    }

    /** Set the \b const flag. */
    void
    setConst(bool p_const)
    {
        setFlag(flg_const, p_const);
    }

    /**
       Determine whether the iterator points to the end of the sequence.
       \return true if get() = nullptr, false otherwise
    */
    virtual bool
    isEnd() const
    {
        return (get() == nullptr);
    }

    /**
       Set the current object.  This method may not be called when the \b const flag is true.
       If self points to an object, it will be replaced by the given object (or removed if
       object == nullptr).  If self points to the end of the sequence, the given object will be
       added to the sequence.
       \param object object to put at current position
    */
    virtual void set(const Object* object) = 0;

    /**
       Pointer dereference operator.  An iterator is analagous to a pointer, so it makes
       intuitive sense to use this operator.
       \see get
    */
    Object* operator*() const
    {
        return get();
    }

    /**
       Pre-increment operator.  Move forward one object.
       \return self
    */
    FwdIt&
    operator++()
    {
        forward();
        return *this;
    }

    /**
       Post-increment operator.  Move forward one object.
       \return copy of self before moving forward
    */
    void
    operator++(int)
    {
        forward();
    }

    /**
       Increment operator.  Move forward the given number of objects.
       \return self
       \param dist distance to move forward
    */
    FwdIt&
    operator+=(size_t dist)
    {
        forward(dist);
        return *this;
    }

#ifdef DEBUG
    /** Assert that the iterator is valid. */
    bool isValid(const utl::Object* owner = nullptr) const;

    /** Invalidate the iterator. */
    void
    invalidate() const
    {
        _owner = nullptr;
    }

    /** Query if self has the same owner as the given iterator. */
    bool
    hasSameOwner(const FwdIt* it) const
    {
        return (_owner == it->_owner);
    }

    /** Get the owner. */
    const Object*
    owner() const
    {
        return _owner;
    }

    /** Set the owner. */
    void setOwner(const Object* owner, bool notifyOwner = true) const;
#endif
public:
    // for STL
    typedef Object* value_type;
    typedef Object*& reference;
    typedef Object** pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

private:
    enum flg_t
    {
        flg_const
    };

private:
#ifdef DEBUG
    void init();
    void deInit();
#else
    void
    init()
    {
    }
    void
    deInit()
    {
    }
#endif
#ifdef DEBUG
    mutable const Object* _owner;
    mutable bool _notifyOwner;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TFwdIt.h>
