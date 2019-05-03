#include <libutl/libutl.h>
#include <libutl/MaxObject.h>
#include <libutl/BinTree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BinTreeIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
BinTreeIt::compare(const Object& rhs) const
{
    auto& ti = utl::cast<BinTreeIt>(rhs);
    ASSERTD(hasSameOwner(ti));
    return utl::compare(_node, ti._node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeIt::copy(const Object& rhs)
{
    auto& ti = utl::cast<BinTreeIt>(rhs);
    super::copy(ti);
    _tree = ti._tree;
    _node = ti._node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeIt::forward(size_t dist)
{
    ASSERTD(isValid(_tree));
    ASSERTD(_node != nullptr);

    // While there is distance left to travel
    while (dist-- > 0)
    {
        BinTreeNode* nextNode = _node->next();
        // If node has no successor, we can't move forward
        if (nextNode == nullptr)
        {
            break;
        }
        // Else move forward (node = node's successor)
        else
        {
            _node = nextNode;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
BinTreeIt::get() const
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
BinTreeIt::reverse(size_t dist)
{
    ASSERTD(isValid(_tree));
    ASSERTD(_node != nullptr);

    // While there is distance left to travel
    while (dist-- > 0)
    {
        // If the node has no predecessor, we can't move backward
        BinTreeNode* prevNode = _node->prev();
        if (prevNode == nullptr)
        {
            break;
        }
        // Else move backwards (node = node's predecessor)
        else
        {
            _node = prevNode;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTreeIt::set(const Object* object)
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
    // Else we clobber the object currently referred to
    else
    {
        if (object == nullptr)
        {
            _tree->removeIt((BinTreeIt&)*this);
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

UTL_NS_END;
