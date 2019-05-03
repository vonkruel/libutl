#include <libutl/libutl.h>
#include <libutl/Heap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::HeapIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
HeapIt::compare(const Object& rhs) const
{
    auto& hit = utl::cast<HeapIt>(rhs);
    ASSERTD(hasSameOwner(hit));
    return utl::compare(_idx, hit._idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HeapIt::copy(const Object& rhs)
{
    auto& hit = utl::cast<HeapIt>(rhs);
    super::copy(hit);
    _heap = hit._heap;
    _idx = hit._idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HeapIt::forward(size_t dist)
{
    ASSERTD(isValid(_heap));
    size_t items = _heap->_items;
    while ((dist-- > 0) && (_idx < items))
        ++_idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
HeapIt::get() const
{
    ASSERTD(isValid(_heap));
    if (_idx >= _heap->_items)
        return nullptr;
    return _heap->_array[_idx];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HeapIt::reverse(size_t dist)
{
    ASSERTD(isValid(_heap));
    while ((dist-- > 0) && (_idx > 0))
        --_idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HeapIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_heap));

    // adding the object
    if (get() == nullptr)
    {
        ASSERTD(object != nullptr);
        _heap->add(object);
        return;
    }

    // removing the object
    if (object == nullptr)
    {
        _heap->removeIt((HeapIt&)*this);
        return;
    }

    // replacing an object
    Vector<Object*>& array = _heap->_array;
    Object* heapObject = array[_idx];
    if (object == heapObject)
        return;
    if (_heap->isOwner())
        delete heapObject;
    array[_idx] = const_cast<Object*>(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
