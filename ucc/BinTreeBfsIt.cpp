#include <libutl/libutl.h>
#include <libutl/MaxObject.h>
#include <libutl/BinTree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BinTreeBfsIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
BinTreeBfsIt::compare(const Object& rhs) const
{
    auto& ti = utl::cast<BinTreeBfsIt>(rhs);
    ASSERTD(hasSameOwner(ti));
    return utl::compare(_node, ti._node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeBfsIt::copy(const Object& rhs)
{
    // Copy rhs's elements
    auto& ti = utl::cast<BinTreeBfsIt>(rhs);
    super::copy(ti);
    init(ti._tree, ti._node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeBfsIt::forward(size_t dist)
{
    ASSERTD(isValid(_tree));
    ASSERTD(_node != nullptr);

    // While there is distance left to travel
    while (dist-- > 0)
    {
        if (_queue.empty())
        {
            copy(_tree->endBFS());
            break;
        }
        _node = _queue.front();
        _queue.pop();

        // skip past MaxObject
        if (_node->get() == (MaxObject*)&maxObject)
        {
            ++dist;
            continue;
        }

        if (!_node->left()->isLeaf())
        {
            _queue.push(_node->left());
        }
        if (!_node->right()->isLeaf())
        {
            _queue.push(_node->right());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
BinTreeBfsIt::get() const
{
    ASSERTD(isValid(_tree));
    Object* res = _node->get();
    if (res == (MaxObject*)&maxObject)
    {
        res = nullptr;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeBfsIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_tree));

    // If we currently refer to the tree's sentinel (MaxObject), we
    // handle the situation by adding the object to the tree
    if (_node->get()->isA(MaxObject))
    {
        ASSERTD(object != nullptr);
        _tree->add(object);
    }
    // else we clobber the object currently referred to
    else
    {
        if (object == nullptr)
        {
            BinTreeIt it(_tree, _node);
            _tree->removeIt(it);
        }
        else
        {
            Object* btObject = _node->get();
            if (object == btObject)
            {
                return;
            }
            // If the tree has owner, we must delete node's object
            if (_tree->isOwner())
            {
                delete btObject;
            }
            _node->set(object);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeBfsIt::init(const BinTree* tree, BinTreeNode* node)
{
    nodeq_t emptyQ;
    std::swap(_queue, emptyQ);
    _tree = const_cast<BinTree*>(tree);
    _node = node;
    if (_node != nullptr)
    {
        if (!_node->left()->isLeaf())
        {
            _queue.push(_node->left());
        }
        if (!_node->right()->isLeaf())
        {
            _queue.push(_node->right());
        }
    }
    IFDEBUG(FwdIt::setOwner(tree));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
