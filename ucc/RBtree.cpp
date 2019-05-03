#include <libutl/libutl.h>
#include <libutl/MaxObject.h>
#include <libutl/RBtree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::RBtree);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

RBtree::RBtree(bool owner, bool multiSet, Ordering* ordering)
    : BinTree(owner, multiSet, ordering)
{
    init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtree::steal(Object& rhs_)
{
    auto& rhs = utl::cast<RBtree>(rhs_);
    deInit();
    super::steal(rhs);
    _root = rhs._root;
    // fix pointers to _leaf
    auto rhsLeaf = &rhs._leaf;
    iterator it = this->begin();
    for (;;)
    {
        auto node = it.node();
        if (node->left() == rhsLeaf)
            node->setLeft(&_leaf);
        if (node->right() == rhsLeaf)
            node->setRight(&_leaf);
        if (node->get() == (MaxObject*)&maxObject)
        {
            break;
        }
        ++it;
    }
    rhs.init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
RBtree::innerAllocatedSize() const
{
    size_t sz = super::innerAllocatedSize();
    sz += (this->size() + 1) * sizeof(RBtreeNode);
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtree::clear()
{
    super::clear();
    init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
RBtree::createNode(const Object* object)
{
    BinTreeNode* node = new RBtreeNode(object);
    node->setLeft(&_leaf);
    node->setRight(&_leaf);
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtree::resetRoot()
{
    // while root is not root...
    while (_root->parent() != nullptr)
    {
        _root = _root->parent();
    }
    // root is always colored black
    static_cast<RBtreeNode*>(_root)->setColor(RBtreeNode::BLACK);
    // leaves are always colored black
    _leaf.setColor(RBtreeNode::BLACK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RBtree::init()
{
    _leaf.setLeft(&_leaf);
    _leaf.setRight(&_leaf);
    _leaf.setColor(RBtreeNode::BLACK);
    _root = new RBtreeNode(&maxObject, RBtreeNode::BLACK);
    _root->setLeft(&_leaf);
    _root->setRight(&_leaf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
