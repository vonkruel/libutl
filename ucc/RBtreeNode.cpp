#include <libutl/libutl.h>
#include <libutl/RBtreeNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtreeNode::addFixup()
{
    RBtreeNode* x = this;
    // while x is not the root and x's parent is red
    while (!x->isRoot() && (x->parent()->getColor() == RED))
    {
        // the red/black rules have been violated
        if (x->parent()->isLeftChild())
        {
            // y is x's uncle
            RBtreeNode* y = x->parent()->sibling();
            // if x's uncle is red
            if (y->getColor() == RED)
            {
                x->parent()->setColor(BLACK);
                y->setColor(BLACK);
                x->parent()->parent()->setColor(RED);
                x = x->parent()->parent();
            }
            // x's uncle is black
            else
            {
                // If x is a right child
                if (x->isRightChild())
                {
                    // make x a left child
                    x = x->parent();
                    x->rotateLeft();
                }
                // re-color and rotate
                x->parent()->setColor(BLACK);
                x->parent()->parent()->setColor(RED);
                x->parent()->parent()->rotateRight();
            }
        }
        // symmetric
        else
        {
            RBtreeNode* y = x->parent()->sibling();
            if (y->getColor() == RED)
            {
                x->parent()->setColor(BLACK);
                y->setColor(BLACK);
                x->parent()->parent()->setColor(RED);
                x = x->parent()->parent();
            }
            else
            {
                if (x->isLeftChild())
                {
                    x = x->parent();
                    x->rotateRight();
                }
                x->parent()->setColor(BLACK);
                x->parent()->parent()->setColor(RED);
                x->parent()->parent()->rotateLeft();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtreeNode::removeFixup(const BinTreeNode* removedNode)
{
    // no need to do anything unless removed node was black
    if (static_cast<const RBtreeNode*>(removedNode)->getColor() != BLACK)
    {
        return;
    }

    RBtreeNode* x = this;

    // while x is not root and is black
    while (!x->isRoot() && (x->getColor() == BLACK))
    {
        RBtreeNode* xParent = x->parent();
        // if x is a left child
        if (x->isLeftChild())
        {
            // w is x's sibling
            RBtreeNode* w = xParent->right();
            // if w is red
            if (w->getColor() == RED)
            {
                // color w black and rotate around x's parent
                w->setColor(BLACK);
                xParent->setColor(RED);
                xParent->rotateLeft();
                w = xParent->right();
            }
            // if w's children are black
            if ((w->left()->getColor() == BLACK) && (w->right()->getColor() == BLACK))
            {
                // color w red, move x up one level
                w->setColor(RED);
                x = xParent;
            }
            // else w has at least one red child
            else
            {
                if (w->right()->getColor() == BLACK)
                {
                    w->left()->setColor(BLACK);
                    w->setColor(RED);
                    w->rotateRight();
                    w = xParent->right();
                }
                w->setColor(xParent->getColor());
                xParent->setColor(BLACK);
                w->right()->setColor(BLACK);
                xParent->rotateLeft();
                return;
            }
        }
        // symmetric
        else
        {
            RBtreeNode* w = xParent->left();
            if (w->getColor() == RED)
            {
                w->setColor(BLACK);
                xParent->setColor(RED);
                xParent->rotateRight();
                w = xParent->left();
            }
            if ((w->left()->getColor() == BLACK) && (w->right()->getColor() == BLACK))
            {
                w->setColor(RED);
                x = xParent;
            }
            else
            {
                if (w->left()->getColor() == BLACK)
                {
                    w->right()->setColor(BLACK);
                    w->setColor(RED);
                    w->rotateLeft();
                    w = xParent->left();
                }
                w->setColor(xParent->getColor());
                xParent->setColor(BLACK);
                w->left()->setColor(BLACK);
                xParent->rotateRight();
                return;
            }
        }
    }
    x->setColor(BLACK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtreeNode::swapWith(BinTreeNode* p_rhs)
{
    ASSERTD(dynamic_cast<RBtreeNode*>(p_rhs) != nullptr);
    RBtreeNode* lhs = this;
    RBtreeNode* rhs = static_cast<RBtreeNode*>(p_rhs);
    BinTreeNode::swapWith(rhs);
    byte_t tmp;
    tmp = lhs->_color;
    lhs->_color = rhs->_color;
    rhs->_color = tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtreeNode::init(uint_t color)
{
    setColor(color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
