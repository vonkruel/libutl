#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BinTreeNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Red/black tree node.

   You shouldn't need to use this class directly.

   <b>Attributes</b>

   \arg \b color : All nodes in a red/black tree are colored \b red or
   \b black.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RBtreeNode : public BinTreeNode
{
public:
    RBtreeNode()
    {
        init();
    }

    RBtreeNode(const Object* object, uint_t color = RED)
        : BinTreeNode(object)
    {
        init(color);
    }

    RBtreeNode*
    left() const
    {
        return static_cast<RBtreeNode*>(_left);
    }
    RBtreeNode*
    next() const
    {
        return static_cast<RBtreeNode*>(BinTreeNode::next());
    }
    RBtreeNode*
    parent() const
    {
        return static_cast<RBtreeNode*>(_parent);
    }
    RBtreeNode*
    prev() const
    {
        return static_cast<RBtreeNode*>(BinTreeNode::prev());
    }
    RBtreeNode*
    right() const
    {
        return static_cast<RBtreeNode*>(_right);
    }
    virtual void addFixup();
    virtual void removeFixup(const BinTreeNode* removedNode);
    virtual void swapWith(BinTreeNode* rhs);

    /** Get the color (\b RED or \b BLACK). */
    uint_t
    getColor() const
    {
        return (uint_t)_color;
    }

    /** Set the color (\b RED or \b BLACK). */
    void
    setColor(uint_t color)
    {
        _color = color;
    }

    /** Return self's sibling (nullptr if none). */
    RBtreeNode*
    sibling() const
    {
        return (RBtreeNode*)(BinTreeNode::sibling());
    }

    /** A node in a red/black tree is colored \b red or \b black. */
    enum color_t
    {
        BLACK, /**< \b black node */
        RED    /**< \b red node */
    };

private:
    void init(uint_t color = RED);
    void
    deInit()
    {
    }

private:
    enum flg_t
    {
        flg_color
    };
    byte_t _color;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
