#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Bi-directional List iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ListIt : public BidIt
{
    UTL_CLASS_DECL(ListIt, BidIt);

public:
    /**
       Constructor.
       \param list associated List
       \param node list node
    */
    ListIt(const List* list, ListNode* node)
        : _list(const_cast<List*>(list))
        , _node(node)
    {
        IFDEBUG(FwdIt::setOwner(list));
    }

    /** Compare with another ListIt. */
    virtual int compare(const Object& rhs) const;

    /** Copy another ListIt. */
    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual Object* get() const;

    /** Get the associated List. */
    List*
    getList() const
    {
        return _list;
    }

    /** Get the list node. */
    ListNode*
    getNode() const
    {
        return _node;
    }

    virtual void reverse(size_t dist = 1);

    virtual void set(const Object* object);

    /**
       Set the list and node.
       \param list associated List
       \param node associated node
    */
    void
    set(const List* list, ListNode* node)
    {
        _list = const_cast<List*>(list);
        _node = node;
        IFDEBUG(FwdIt::setOwner(list));
    }

    /** Set the list node. */
    void
    setNode(ListNode* node)
    {
        _node = node;
    }

    ListIt&
    operator++()
    {
        forward();
        return *this;
    }

    ListIt
    operator++(int)
    {
        ListIt res = *this;
        forward();
        return res;
    }

    ListIt&
    operator--()
    {
        reverse();
        return *this;
    }

    ListIt
    operator--(int)
    {
        ListIt res = *this;
        reverse();
        return res;
    }

private:
    void
    init()
    {
        _list = nullptr;
        _node = nullptr;
    }
    void
    deInit()
    {
    }

private:
    List* _list;
    ListNode* _node;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::begin() const
{
    iterator it(this, _front);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::begin()
{
    return iterator(this, _front);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
List::beginNew() const
{
    BidIt* it = new iterator(this, _front);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
List::beginNew()
{
    return new iterator(this, _front);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::end() const
{
    iterator it(this, _back);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::end()
{
    return iterator(this, _back);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
List::endNew() const
{
    BidIt* it = new iterator(this, _back);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
List::endNew()
{
    return new iterator(this, _back);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
