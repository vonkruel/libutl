#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   In-order bi-directional BinTree iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinTreeIt : public BidIt
{
    UTL_CLASS_DECL(BinTreeIt, BidIt);

public:
    /**
       Constructor.
       \param tree associated BinTree
       \param node tree node
    */
    BinTreeIt(const BinTree* tree, BinTreeNode* node)
        : _tree(const_cast<BinTree*>(tree))
        , _node(node)
    {
        IFDEBUG(FwdIt::setOwner(_tree));
    }

#ifdef DEBUG
    BinTreeIt(const BinTree* tree, BinTreeNode* node, bool notifyOwner)
    {
        _tree = const_cast<BinTree*>(tree);
        _node = node;
        FwdIt::setOwner(_tree, notifyOwner);
    }
#endif

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual Object* get() const;

    /** Get the current node. */
    BinTreeNode*
    node() const
    {
        return _node;
    }

    /** Get the associated BinTree. */
    BinTree*
    tree() const
    {
        return _tree;
    }

    virtual void reverse(size_t dist = 1);

    virtual void set(const Object* object);

    /**
       Set the tree and node.
       \param tree associated BinTree
       \param node tree node
    */
    void
    set(const BinTree* tree, BinTreeNode* node = nullptr)
    {
        _tree = const_cast<BinTree*>(tree);
        _node = node;
        IFDEBUG(FwdIt::setOwner(_tree));
    }

    /** Set the node. */
    void
    setNode(BinTreeNode* node)
    {
        _node = node;
    }

    BinTreeIt&
    operator++()
    {
        forward();
        return *this;
    }

    BinTreeIt
    operator++(int)
    {
        BinTreeIt res = *this;
        forward();
        return res;
    }

    BinTreeIt&
    operator--()
    {
        reverse();
        return *this;
    }

    BinTreeIt
    operator--(int)
    {
        BinTreeIt res = *this;
        reverse();
        return res;
    }

private:
    void
    init()
    {
        _tree = nullptr;
        _node = nullptr;
    }
    void
    deInit()
    {
    }

private:
    BinTree* _tree;
    BinTreeNode* _node;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::begin() const
{
    iterator it(this, _root->leftMost());
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::begin()
{
    return iterator(this, _root->leftMost());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
BinTree::beginNew() const
{
    BidIt* it = new iterator(this, _root->leftMost());
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
BinTree::beginNew()
{
    return new iterator(this, _root->leftMost());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::end() const
{
    iterator it(this, _root->rightMost());
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::end()
{
    return iterator(this, _root->rightMost());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
BinTree::endNew() const
{
    BidIt* it = new iterator(this, _root->rightMost());
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
BinTree::endNew()
{
    return new iterator(this, _root->rightMost());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::root() const
{
    iterator it(this, _root);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::root()
{
    return iterator(this, _root);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
inline BinTree::iterator
BinTree::begin(bool notifyOwner)
{
    return iterator(this, _root->leftMost(), notifyOwner);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
