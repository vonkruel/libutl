#include <libutl/libutl.h>
#include <libutl/SkipList.h>
#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::SkipListIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
SkipListIt::compare(const Object& rhs) const
{
    auto& li = utl::cast<SkipListIt>(rhs);
    ASSERTD(hasSameOwner(li));
    return utl::compare(_node, li._node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipListIt::copy(const Object& rhs)
{
    auto& li = utl::cast<SkipListIt>(rhs);
    super::copy(li);
    _skipList = li._skipList;
    _node = li._node;
    delete[] _update;
    _update = li._update;
    if (_update != nullptr)
    {
        size_t count = _skipList->_maxLevel + 1;
        _update = new SkipListNode*[count];
        memcpy(_update, li._update, count * sizeof(SkipListNode*));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipListIt::forward(size_t dist)
{
    ASSERTD(isValid(_skipList));
    ASSERTD(_node != nullptr);

    while (dist-- > 0)
    {
        if (_node->next() == nullptr)
        {
            break;
        }
        else
        {
            _node = _node->next();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
SkipListIt::get() const
{
    Object* res = _node->get();
    if (res == &maxObject)
        res = nullptr;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipListIt::reverse(size_t dist)
{
    ASSERTD(isValid(_skipList));
    ASSERTD(_node != nullptr);

    while (dist-- > 0)
    {
        if (_node->prev() == nullptr)
        {
            _node = _node->next();
            break;
        }
        else
        {
            _node = _node->prev();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipListIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_skipList));

    if (_node->get() == (MaxObject*)&maxObject)
    {
        ASSERTD(object != nullptr);
        _skipList->add(object);
    }
    else
    {
        if (object == nullptr)
        {
            _skipList->removeIt((SkipListIt&)*this);
        }
        else
        {
            Object* listObject = _node->get();
            if (object == listObject)
            {
                return;
            }
            if (_skipList->isOwner())
            {
                delete listObject;
            }
            _node->set(object);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
