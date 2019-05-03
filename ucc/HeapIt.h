#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Bi-directional Heap iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HeapIt : public BidIt
{
    UTL_CLASS_DECL(HeapIt, BidIt);

public:
    /**
       Constructor.
       \param heap associated heap
       \param idx heap index
    */
    HeapIt(const Heap* heap, size_t idx)
        : _heap(const_cast<Heap*>(heap))
        , _idx(idx)
    {
        IFDEBUG(FwdIt::setOwner(_heap));
    }

    /** Compare with another HeapIt. */
    virtual int compare(const Object& rhs) const;

    /** Copy another HeapIt. */
    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual Object* get() const;

    /** Get the associated Heap. */
    Heap*
    getHeap() const
    {
        return _heap;
    }

    /** Get the heap index. */
    size_t
    getIdx() const
    {
        return _idx;
    }

    virtual void reverse(size_t dist = 1);

    virtual void set(const Object* object);

    HeapIt&
    operator++()
    {
        forward();
        return *this;
    }

    HeapIt
    operator++(int)
    {
        HeapIt res = *this;
        forward();
        return res;
    }

    HeapIt&
    operator--()
    {
        reverse();
        return *this;
    }

    HeapIt
    operator--(int)
    {
        HeapIt res = *this;
        reverse();
        return res;
    }

private:
    void
    init()
    {
        _heap = nullptr;
        _idx = size_t_max;
    }
    void
    deInit()
    {
    }

private:
    Heap* _heap;
    size_t _idx;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

Heap::iterator
Heap::begin() const
{
    iterator it(this, 0);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Heap::iterator
Heap::begin()
{
    return iterator(this, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Heap::beginNew() const
{
    BidIt* it = new iterator(this, 0);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Heap::beginNew()
{
    return new iterator(this, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Heap::iterator
Heap::end() const
{
    iterator it(this, _items);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Heap::iterator
Heap::end()
{
    return iterator(this, _items);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Heap::endNew() const
{
    BidIt* it = new iterator(this, _items);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Heap::endNew()
{
    return new iterator(this, _items);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
