#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BinTree.h>
#include <libutl/RBtreeNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Red/black tree.

   A red/black tree is a balanced binary tree.  It is not kept perfectly balanced like an AVL
   tree, but it is kept \b mostly balanced -- searches into the tree are still guaranteed to be
   O(log n).  In practice, a red/black tree is often more efficient than an AVL tree, since less
   time is spent doing re-balancing operations.

   <b>Advantages</b>

   \arg add(), find(), remove() are O(log n)
   \arg contained objects are always sorted

   <b>Disadvantages</b>

   \arg lots of cache misses

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RBtree : public BinTree
{
    UTL_CLASS_DECL(RBtree, BinTree);

public:
    /**
       Constructor.
       \param owner owner flag
       \param multiSet (optional : false) multiSet flag
       \param ordering (optional) ordering
    */
    RBtree(bool owner, bool multiSet = false, Ordering* ordering = nullptr);

    virtual void steal(Object& rhs);

    virtual size_t innerAllocatedSize() const;

    /** Remove all objects from the tree. */
    virtual void clear();

protected:
    virtual BinTreeNode* createNode(const Object* object);
    virtual void resetRoot();
    RBtreeNode _leaf;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TRBtree.h>
