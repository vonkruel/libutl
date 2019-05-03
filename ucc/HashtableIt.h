#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Bi-directional Hashtable iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HashtableIt : public BidIt
{
    UTL_CLASS_DECL(HashtableIt, BidIt);

public:
    /**
       Constructor.
       \param ht associated Hashtable
       \param idx index of hash chain
       \param node hash chain node
    */
    HashtableIt(const Hashtable* ht, size_t idx, ListNode* node)
        : _ht(const_cast<Hashtable*>(ht))
        , _idx(idx)
        , _node(node)
    {
        IFDEBUG(FwdIt::setOwner(_ht));
    }

    /** Compare with another HashtableIt. */
    virtual int compare(const Object& rhs) const;

    /** Copy another HashtableIt. */
    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual Object* get() const;

    /** Get the associated Hashtable. */
    Hashtable*
    getHashtable() const
    {
        return _ht;
    }

    /** Get the index of the hash chain. */
    size_t
    getIdx() const
    {
        return _idx;
    }

    /** Get the hash chain node. */
    const ListNode*
    getNode() const
    {
        return _node;
    }

    virtual void reverse(size_t dist = 1);

    virtual void set(const Object* object);

    HashtableIt&
    operator++()
    {
        forward();
        return *this;
    }

    HashtableIt
    operator++(int)
    {
        HashtableIt res = *this;
        forward();
        return res;
    }

    HashtableIt&
    operator--()
    {
        reverse();
        return *this;
    }

    HashtableIt
    operator--(int)
    {
        HashtableIt res = *this;
        reverse();
        return res;
    }

private:
    void
    init()
    {
        _ht = nullptr;
        _idx = size_t_max;
        _node = nullptr;
    }
    void
    deInit()
    {
    }

private:
    Hashtable* _ht;
    size_t _idx;
    ListNode* _node;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

HashtableIt
Hashtable::end() const
{
    HashtableIt it(this, size_t_max, nullptr);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

HashtableIt
Hashtable::end()
{
    return HashtableIt(this, size_t_max, nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
