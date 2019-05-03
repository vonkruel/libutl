#include <libutl/libutl.h>
#include <libutl/SlistNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SlistNode::addAfter(SlistNode* node)
{
    ASSERTD(!node->isSentinelTail());
    setNext(node->next());
    node->setNext(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SlistNode::addBefore(SlistNode* node)
{
    const Object* tmpObject = node->get();
    SlistNode* tmpNext = node->next();
    node->set(_object);
    node->setNext(this);
    set(tmpObject);
    setNext(tmpNext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SlistNode*
SlistNode::remove(SlistNode* prev)
{
    if (prev == nullptr)
    {
        // removing only node in list?
        if (_next == nullptr)
        {
            return this;
        }
        else
        {
            _object = _next->_object;
            _next = _next->_next;
            return _next;
        }
    }
    else
    {
        prev->setNext(_next);
        return this;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
