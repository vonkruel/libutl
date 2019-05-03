#include <libutl/libutl.h>
#include <libutl/SkipListNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipListNode::set(const Object* object, uint_t level, SkipListNode** update)
{
    _object = const_cast<Object*>(object);
    _prev = update[0];
    update[0]->_next[0]->_prev = this;
    uint_t i;
    for (i = 0; i <= level; i++)
    {
        _next[i] = update[i]->_next[i];
        update[i]->_next[i] = this;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipListNode::remove(uint_t level, SkipListNode** update)
{
    _object = nullptr;
    _next[0]->_prev = _prev;
    uint_t i;
    for (i = 0; i <= level; i++)
    {
        if (update[i]->_next[i] != this)
        {
            break;
        }
        update[i]->_next[i] = _next[i];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
