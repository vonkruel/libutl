#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Deque iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DequeIt : public BidIt
{
    UTL_CLASS_DECL(DequeIt, BidIt);

public:
    /**
       Constructor.
       \param deque associated Deque
       \param blockPtr block pointer
       \param blockPos block position
    */
    DequeIt(const Deque* deque, Object*** blockPtr, uint32_t blockPos)
        : _deque(const_cast<Deque*>(deque))
        , _blockPtr(blockPtr)
        , _blockPos(blockPos)
    {
        IFDEBUG(FwdIt::setOwner(deque));
    }

    /** Compare with another DequeIt. */
    virtual int compare(const Object& rhs) const;

    /** Copy another DequeIt. */
    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual void reverse(size_t dist = 1);

    virtual Object* get() const;

    /** Get the associated Deque. */
    Deque*
    deque() const
    {
        return _deque;
    }

    virtual void set(const Object* object);

    /**
       Set the deque, block pointer, and block position.
       \param deque associated Deque
       \param blockPtr block pointer
       \param blockPos block position
    */
    void
    set(const Deque* deque, Object*** blockPtr, uint32_t blockPos)
    {
        _deque = const_cast<Deque*>(deque);
        _blockPtr = blockPtr;
        _blockPos = blockPos;
        IFDEBUG(FwdIt::setOwner(deque));
    }

    DequeIt&
    operator++()
    {
        forward();
        return *this;
    }

    DequeIt
    operator++(int)
    {
        DequeIt res = *this;
        forward();
        return res;
    }

    DequeIt&
    operator--()
    {
        reverse();
        return *this;
    }

    DequeIt
    operator--(int)
    {
        DequeIt res = *this;
        reverse();
        return res;
    }

#ifdef DEBUG
    bool isValid(const utl::Object* owner = nullptr) const;
#endif

private:
    void
    init()
    {
        _deque = nullptr;
        _blockPtr = nullptr;
        _blockPos = uint32_t_max;
    }
    void
    deInit()
    {
    }

private:
    Deque* _deque;
    Object*** _blockPtr;
    uint32_t _blockPos;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::iterator
Deque::begin() const
{
    DequeIt it(this, _beginBlock, _beginPos);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::iterator
Deque::begin()
{
    return DequeIt(this, _beginBlock, _beginPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Deque::beginNew() const
{
    BidIt* it = new DequeIt(this, _beginBlock, _beginPos);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Deque::beginNew()
{
    return new DequeIt(this, _beginBlock, _beginPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::iterator
Deque::end() const
{
    DequeIt it(this, _endBlock, _endPos);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::iterator
Deque::end()
{
    return DequeIt(this, _endBlock, _endPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Deque::endNew() const
{
    BidIt* it = new DequeIt(this, _endBlock, _endPos);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Deque::endNew()
{
    return new DequeIt(this, _endBlock, _endPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
