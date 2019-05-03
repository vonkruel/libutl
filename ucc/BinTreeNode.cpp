#include <libutl/libutl.h>
#include <libutl/BinTreeNode.h>
#include <libutl/Queue.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeNode::dump(Stream& os, uint_t level)
{
    // terminate on a leaf node
    if (isLeaf())
        return;

    // write our object
    for (uint_t i = 0; i < 2 * level; i++)
    {
        os << ' ';
    }
    os << *_object << endl;

    // dump children
    _left->dump(os, level + 1);
    _right->dump(os, level + 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTreeNode::leftMost()
{
    // Just walk down the left side of the tree until we reach a leaf
    BinTreeNode* res = this;
    while (!res->_left->isLeaf())
        res = res->_left;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTreeNode::next() const
{
    BinTreeNode* res;

    // if the right child is a leaf
    if (_right->isLeaf())
    {
        // The right child is a leaf, so the successor is found by walking
        // up the tree until we find a node that is a left child.  That
        // node's parent is the successor.  If no such node is found, there
        // is no successor.
        res = const_cast_this;

        // walk up the tree until we find a node that isn't a right child
        while (res->isRightChild())
            res = res->_parent;

        // if the node is a left child, we've found the successor
        if (res->isLeftChild())
        {
            res = res->_parent;
        }
        // else there is no successor
        else
        {
            res = nullptr;
        }
    }
    // else the successor is the smallest node in the right subtree
    else
    {
        res = _right->leftMost();
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTreeNode::prev() const
{
    BinTreeNode* res;

    // if the left child is a leaf
    if (_left->isLeaf())
    {
        // The left child is a leaf, so the predecessor is found by walking
        // up the tree until we find a node that is a right child.  That
        // node's parent is the predecessor.  If no such node is found, there
        // is no predecessor.
        res = const_cast_this;

        // walk up the tree until we find a node that isn't a left child
        while (res->isLeftChild())
            res = res->_parent;

        // if the node is a right child, we've found the predecessor
        if (res->isRightChild())
        {
            res = res->_parent;
        }
        // else there is no predecessor
        else
        {
            res = nullptr;
        }
    }
    // else the predecessor is the largest node in the left subtree
    else
    {
        res = _left->rightMost();
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeNode::remove(BinTreeNode*& p_x, BinTreeNode*& p_y)
{
    BinTreeNode* z = this;
    BinTreeNode* x;
    BinTreeNode* y;

    // y is the node that will actually be removed
    if ((z->left()->isLeaf()) || (z->right()->isLeaf()))
    {
        // simple case -- z has a NIL child
        y = z;
    }
    else
    {
        // y is z's successor
        y = z->next();

        // swap y's contents with z's
        y->swapWith(z);

        // swap y,z pointers
        BinTreeNode* tmp;
        tmp = y;
        y = z;
        z = tmp;
    }

    // x is y's only child
    if (y->left()->isLeaf())
    {
        x = y->right();
    }
    else
    {
        x = y->left();
    }

    // Remove y from the parent chain
    x->_parent = y->_parent;
    if (!y->isRoot())
    {
        if (y->isLeftChild())
        {
            y->parent()->_left = x;
        }
        else
        {
            y->parent()->_right = x;
        }
    }

    p_x = x;
    p_y = y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTreeNode::rightMost()
{
    BinTreeNode* res = this;
    while (!res->_right->isLeaf())
    {
        res = res->_right;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// note: this only handles the cases required by remove()
void
BinTreeNode::swapWith(BinTreeNode* rhs)
{
    BinTreeNode* lhs = this;
    BinTreeNode* tmp;
    bool lhsIsLC = lhs->isLeftChild();
    bool rhsIsLC = rhs->isLeftChild();

    // lhs is child of rhs?
    if (lhs->_parent == rhs)
    {
        lhs->_parent = rhs->_parent;
        rhs->_parent = lhs;

        // lhs must be right child of rhs
        ASSERTD(!lhsIsLC);
        tmp = lhs->_left;
        lhs->_left = rhs->_left;
        rhs->_left = tmp;
        tmp = lhs->_right;
        lhs->_right = rhs;
        rhs->_right = tmp;

        // re-set child links in parent of rhs
        if (lhs->_parent != nullptr)
        {
            if (rhsIsLC)
                lhs->_parent->_left = lhs;
            else
                lhs->_parent->_right = lhs;
        }
    }
    else // simple case: no relationship between lhs, rhs
    {
        ASSERTD(rhs->_parent != lhs);

        // swap parent links
        tmp = lhs->_parent;
        lhs->_parent = rhs->_parent;
        rhs->_parent = tmp;

        // swap child links
        tmp = lhs->_left;
        lhs->_left = rhs->_left;
        rhs->_left = tmp;
        tmp = lhs->_right;
        lhs->_right = rhs->_right;
        rhs->_right = tmp;

        // re-set child links in parents
        if (lhs->_parent != nullptr)
        {
            if (rhsIsLC)
                lhs->_parent->_left = lhs;
            else
                lhs->_parent->_right = lhs;
        }
        if (rhs->_parent != nullptr)
        {
            if (lhsIsLC)
                rhs->_parent->_left = rhs;
            else
                rhs->_parent->_right = rhs;
        }
    }

    // re-set parent links in children
    lhs->_left->_parent = lhs;
    lhs->_right->_parent = lhs;
    rhs->_left->_parent = rhs;
    rhs->_right->_parent = rhs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeNode::rotateLeft()
{
    BinTreeNode* x = this;
    BinTreeNode* y = x->_right;

    // y's left becomes x's right
    x->_right = y->_left;
    y->_left->_parent = x;

    // x's parent becomes y's parent
    y->_parent = x->_parent;
    if (x->_parent != nullptr)
    {
        if (x == x->_parent->_left)
        {
            x->_parent->_left = y;
        }
        else
        {
            x->_parent->_right = y;
        }
    }

    // x is now y's left
    y->_left = x;
    x->_parent = y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeNode::rotateRight()
{
    BinTreeNode* x = this;
    BinTreeNode* y = x->_left;

    // y's right becomes x's left
    x->_left = y->_right;
    y->_right->_parent = x;

    // x's parent becomes y's parent
    y->_parent = x->_parent;
    if (x->_parent != nullptr)
    {
        if (x == x->_parent->_right)
        {
            x->_parent->_right = y;
        }
        else
        {
            x->_parent->_left = y;
        }
    }

    // x is now y's right
    y->_right = x;
    x->_parent = y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTreeNode::sibling() const
{
    if (_parent == nullptr)
    {
        return nullptr;
    }
    else
    {
        return (this == _parent->_left) ? _parent->_right : _parent->_left;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeNode::init(const Object* object)
{
    _parent = _left = _right = nullptr;
    _object = const_cast<Object*>(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
