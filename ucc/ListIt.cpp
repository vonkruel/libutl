#include <libutl/libutl.h>
#include <libutl/List.h>
#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ListIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
ListIt::compare(const Object& rhs) const
{
    auto& li = utl::cast<ListIt>(rhs);
    ASSERTD(hasSameOwner(li));
    return utl::compare(_node, li._node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ListIt::copy(const Object& rhs)
{
    auto& li = utl::cast<ListIt>(rhs);
    super::copy(li);
    _list = li._list;
    _node = li._node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ListIt::forward(size_t dist)
{
    ASSERTD(isValid(_list));
    ASSERTD(_node != nullptr);

    while (dist-- > 0)
    {
        if (_node->next() == nullptr)
        {
            break;
        }
        else
        {
            const_cast_this->_node = _node->next();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
ListIt::get() const
{
    ASSERTD(isValid(_list));

    Object* res = _node->get();
    if (res == (MaxObject*)&maxObject)
    {
        res = nullptr;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ListIt::reverse(size_t dist)
{
    ASSERTD(isValid(_list));
    ASSERTD(_node != nullptr);

    while (dist-- > 0)
    {
        if (_node->prev() == nullptr)
        {
            break;
        }
        else
        {
            const_cast_this->_node = _node->prev();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ListIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_list));

    if (_node->get() == (MaxObject*)&maxObject)
    {
        ASSERTD(object != nullptr);
        _list->add(object);
    }
    else
    {
        if (object == nullptr)
        {
            _list->removeIt((ListIt&)*this);
        }
        else
        {
            Object* listObject = _node->get();
            if (object == listObject)
            {
                return;
            }
            if (_list->isOwner())
            {
                delete listObject;
            }
            _node->set(object);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
