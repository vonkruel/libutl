#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Binary tree node.

   You shouldn't need to use this class directly.

   <b>Attributes</b>

   \arg \b parent : Parent Node (= nullptr if self is root node).
   \arg \b left : Left child.
   \arg \b right : Right Child.
   \arg \b object : Contained object.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinTreeNode
{
public:
    /** Constructor. */
    BinTreeNode()
    {
        init();
    }

    /** Destructor. */
    virtual ~BinTreeNode()
    {
    }

    /**
       Constructor.
       \param object contained object
    */
    BinTreeNode(const Object* object)
    {
        init(object);
    }

    void dump(Stream& os, uint_t level);

    /** Get the contained object. */
    Object*
    get() const
    {
        return _object;
    }

    /** Determine whether self is a leaf node. */
    bool
    isLeaf() const
    {
        return (_object == nullptr);
    }

    /** Determine whether self is a left child. */
    bool
    isLeftChild() const
    {
        return ((_parent != nullptr) && (_parent->_left == this));
    }

    /** Determine whether self is a right child. */
    bool
    isRightChild() const
    {
        return ((_parent != nullptr) && (_parent->_right == this));
    }

    /** Determine whether self is the root node. */
    bool
    isRoot() const
    {
        return (_parent == nullptr);
    }

    /** Return the left child. */
    BinTreeNode*
    left() const
    {
        return _left;
    }

    /** Return the left-most child (smallest node reachable from self). */
    const BinTreeNode*
    leftMost() const
    {
        return const_cast_this->leftMost();
    }

    /** Return the left-most child (smallest node reachable from self). */
    BinTreeNode* leftMost();

    /** Return the in-order successor. */
    BinTreeNode* next() const;

    /** Return the parent node (nullptr if root). */
    BinTreeNode*
    parent() const
    {
        return _parent;
    }

    /** Return the in-order predecessor. */
    BinTreeNode* prev() const;

    /**
       Remove the node from the tree.
       \param x y's only child
       \param y node that should be removed
    */
    void remove(BinTreeNode*& x, BinTreeNode*& y);

    /** Return the right child. */
    BinTreeNode*
    right() const
    {
        return _right;
    }

    /** Return the right-most child (largest node reachable from self). */
    const BinTreeNode*
    rightMost() const
    {
        return const_cast_this->rightMost();
    }

    /** Return the right-most child (largest node reachable from self). */
    BinTreeNode* rightMost();

    /** Perform necessary re-balancing following the addition of a node. */
    virtual void addFixup() = 0;

    /** Perform re-balancing following the removal of a node. */
    virtual void removeFixup(const BinTreeNode* removedNode) = 0;

    /** Trade places with the given node. */
    virtual void swapWith(BinTreeNode* rhs);

    /** Perform a left rotation around self. */
    void rotateLeft();

    /** Perform a right rotation around self. */
    void rotateRight();

    /** Set the contained object. */
    void
    set(const Object* object)
    {
        _object = const_cast<Object*>(object);
    }

    /** Set the left child. */
    void
    setLeft(BinTreeNode* node)
    {
        _left = node;
        _left->_parent = this;
    }

    /** Set the right child. */
    void
    setRight(BinTreeNode* node)
    {
        _right = node;
        _right->_parent = this;
    }

    /** Return the sibling node (nullptr if self is parent). */
    BinTreeNode* sibling() const;

protected:
    BinTreeNode *_parent, *_left, *_right;
    Object* _object;

private:
    void init(const Object* object = nullptr);
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
