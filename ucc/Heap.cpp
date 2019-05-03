#include <libutl/libutl.h>
#include <libutl/Heap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Heap);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::steal(Object& rhs_)
{
    auto& rhs = utl::cast<Heap>(rhs_);
    super::steal(rhs);
    _array.steal(rhs._array);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Heap::innerAllocatedSize() const
{
    size_t sz = super::innerAllocatedSize();
    sz += _array.innerAllocatedSize();
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Heap::add(const Object* object)
{
    // disallow a duplicate if this is not a multi-set
    if (!isMultiSet() && (find(*object) != nullptr))
        return false;

    // grow if necessary
    if (_items == _array.size())
        _array.grow(_items + 1);

    size_t idx = _items;
    _array[idx] = const_cast<Object*>(object);
    _items++;
    swapUp(idx);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::clear()
{
    size_t i;
    for (i = 0; i < _items; i++)
    {
        if (isOwner())
        {
            delete _array[i];
        }
        _array[i] = nullptr;
    }
    _items = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Heap::find(const Object& key) const
{
    size_t idx;
    find(key, idx);
    if (idx == size_t_max)
    {
        return nullptr;
    }
    return _array[idx];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Heap::iterator
Heap::findIt(const Object& key) const
{
    iterator it = const_cast_this->findIt(key);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Heap::iterator
Heap::findIt(const Object& key)
{
    size_t idx;
    find(key, idx);
    if (idx == size_t_max)
    {
        return end();
    }
    return iterator(this, idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::findIt(const Object& key, BidIt& it)
{
    it = findIt(key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Heap::pop()
{
    if (_items == 0)
        return nullptr;
    Object* res = _array[0];
    bool saveOwner = isOwner();
    setOwner(false);
    removeIdx(0);
    setOwner(saveOwner);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Heap::remove(const Object& key)
{
    size_t idx;
    find(key, idx);
    if (idx == size_t_max)
        return false;
    removeIdx(idx);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::removeIt(BidIt& it)
{
    ASSERTD(*it != nullptr);
    auto& hit = utl::cast<iterator>(it.getProxiedObject());
    ASSERTD(hit.isValid(this));
    size_t idx = hit.getIdx();
    hit++;
    removeIdx(idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::init(size_t size, size_t increment, bool owner, bool multiSet, Ordering* ordering)
{
    setOwner(owner);
    setMultiSet(multiSet);
    if (ordering != nullptr)
        setOrdering(ordering);

    // Set up the Object* array
    _array.setAutoInit(true);
    _array.setIncrement(increment);
    _array.grow(size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::deInit()
{
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::find(const Object& key, size_t& idx) const
{
    for (idx = 0; idx < _items; idx++)
    {
        if (compareObjects(_array[idx], key) == 0)
        {
            return;
        }
    }
    idx = size_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::removeIdx(size_t idx)
{
    // remove the object
    if (isOwner())
        delete _array[idx];

    if (idx == (_items - 1))
    {
        _array[idx] = nullptr;
        _items--;
        return;
    }

    // restore heap property
    _items--;
    _array[idx] = _array[_items];
    _array[_items] = nullptr;

    // swap upward?
    if ((idx > 0) && (compareObjects(_array[idx], _array[(idx - 1) / 2]) > 0))
    {
        swapUp(idx);
    }
    else
    {
        swapDown(idx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::swapUp(size_t idx)
{
    // swap upward until the object is <= its parent
    Object* object = _array[idx];
    while (idx > 0)
    {
        size_t j = (idx - 1) / 2;
        if (compareObjects(_array[j], object) >= 0)
            break;
        _array[idx] = _array[j];
        idx = j;
    }
    _array[idx] = object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Heap::swapDown(size_t idx)
{
    // swap downward until the object is >= either child
    Object* object = _array[idx];
    for (;;)
    {
        size_t j = (idx * 2) + 1;
        if (j >= _items)
            break;
        if (((j + 1) < _items) && (compareObjects(_array[j + 1], _array[j]) > 0))
        {
            j++;
        }
        if (compareObjects(object, _array[j]) >= 0)
            break;
        _array[idx] = _array[j];
        idx = j;
    }
    _array[idx] = object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
