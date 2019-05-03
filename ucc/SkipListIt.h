#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Bi-directional SkipList iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SkipListIt : public BidIt
{
    UTL_CLASS_DECL(SkipListIt, BidIt);

public:
    /**
       Constructor.
       \param skipList associated SkipList
       \param node list node
       \param update array of updated nodes for insertion
    */
    SkipListIt(const SkipList* skipList, SkipListNode* node, SkipListNode** update = nullptr)
        : _skipList(const_cast<SkipList*>(skipList))
        , _node(node)
        , _update(update)
    {
        IFDEBUG(FwdIt::setOwner(_skipList));
    }

    /** Compare with another SkipListIt. */
    virtual int compare(const Object& rhs) const;

    /** Copy another SkipListIt. */
    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual Object* get() const;

    /** Get the associated SkipList. */
    SkipList*
    skipList() const
    {
        return _skipList;
    }

    /** Get the list node. */
    SkipListNode*
    node() const
    {
        return _node;
    }

    /** Get the update array. */
    SkipListNode**
    update() const
    {
        return _update;
    }

    virtual void reverse(size_t dist = 1);

    virtual void set(const Object* object);

    /**
       Set the skipList and node.
       \param skipList associated SkipList
       \param node associated node
    */
    void
    set(const SkipList* skipList, SkipListNode* node)
    {
        _skipList = const_cast<SkipList*>(skipList);
        _node = node;
        IFDEBUG(FwdIt::setOwner(_skipList));
    }

    /** Set the skip-list node. */
    void
    setNode(SkipListNode* node)
    {
        _node = node;
    }

    SkipListIt&
    operator++()
    {
        forward();
        return *this;
    }

    SkipListIt
    operator++(int)
    {
        SkipListIt res = *this;
        forward();
        return res;
    }

    SkipListIt&
    operator--()
    {
        reverse();
        return *this;
    }

    SkipListIt
    operator--(int)
    {
        SkipListIt res = *this;
        reverse();
        return res;
    }

private:
    void
    init()
    {
        _skipList = nullptr;
        _node = nullptr;
        _update = nullptr;
    }
    void
    deInit()
    {
        if (_update != nullptr)
            delete[] _update;
    }

private:
    SkipList* _skipList;
    SkipListNode* _node;
    SkipListNode** _update;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::begin() const
{
    iterator it(this, _head);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::begin()
{
    return iterator(this, _head->next());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
SkipList::beginNew() const
{
    BidIt* it = new iterator(this, _head->next());
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
SkipList::beginNew()
{
    return new iterator(this, _head->next());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::end() const
{
    iterator it(this, _tail);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::end()
{
    return iterator(this, _tail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
SkipList::endNew() const
{
    BidIt* it = new iterator(this, _tail);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
SkipList::endNew()
{
    return new iterator(this, _tail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
