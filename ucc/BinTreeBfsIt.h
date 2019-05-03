#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FwdIt.h>
#include <libutl/Queue.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef new
#include <queue>
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinTree;
class BinTreeNode;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Breadth-first BinTree iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinTreeBfsIt : public FwdIt
{
    UTL_CLASS_DECL(BinTreeBfsIt, FwdIt);

public:
    /**
       Constructor.
       \param tree associated BinTree
       \param node tree node
    */
    BinTreeBfsIt(const BinTree* tree, BinTreeNode* node)
    {
        init(tree, node);
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void forward(size_t dist = 1);

    virtual Object* get() const;

    /** Get the current node. */
    BinTreeNode*
    getNode() const
    {
        return _node;
    }

    /** Get the associated BinTree. */
    BinTree*
    getTree() const
    {
        return _tree;
    }

    virtual void set(const Object* object);

    /** Set the node. */
    void
    setNode(BinTreeNode* node)
    {
        _node = node;
    }

    BinTreeBfsIt&
    operator++()
    {
        forward();
        return *this;
    }

    BinTreeBfsIt
    operator++(int)
    {
        BinTreeBfsIt res = *this;
        forward();
        return res;
    }

private:
    typedef std::queue<BinTreeNode*> nodeq_t;

private:
    void init(const BinTree* tree = nullptr, BinTreeNode* node = nullptr);
    void
    deInit()
    {
    }

private:
    BinTree* _tree;
    BinTreeNode* _node;
    nodeq_t _queue;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt
BinTree::beginBFS() const
{
    BinTreeBfsIt it(this, _root);
    if (it.get() == nullptr)
        it.forward();
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt
BinTree::beginBFS()
{
    BinTreeBfsIt it(this, _root);
    if (it.get() == nullptr)
        it.forward();
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt*
BinTree::beginBFSNew() const
{
    BinTreeBfsIt* it = new BinTreeBfsIt(this, _root);
    if (it->get() == nullptr)
        it->forward();
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt*
BinTree::beginBFSNew()
{
    BinTreeBfsIt* it = new BinTreeBfsIt(this, _root);
    if (it->get() == nullptr)
        it->forward();
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt
BinTree::endBFS() const
{
    BinTreeBfsIt it(this, _root->rightMost());
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt
BinTree::endBFS()
{
    return BinTreeBfsIt(this, _root->rightMost());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt*
BinTree::endBFSNew() const
{
    BinTreeBfsIt* it = new BinTreeBfsIt(this, _root->rightMost());
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeBfsIt*
BinTree::endBFSNew()
{
    return new BinTreeBfsIt(this, _root->rightMost());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
