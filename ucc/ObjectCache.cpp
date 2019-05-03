#include <libutl/libutl.h>
#include <libutl/ObjectCache.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ObjectCache);
UTL_CLASS_IMPL(utl::CachedObject);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectCache::access(const Object* object)
{
    if (isFrozen())
        return;

    auto co = utl::cast<CachedObject>(_ht.find(object->getKey()));

    // not cached -> add to cache and we're done
    if (co == nullptr)
    {
        addCache(object);
        return;
    }

    // already cached -> move list node to front
    access(co->getNode());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CachedObject*
ObjectCache::find(const Object& key, bool frozen) const
{
    auto co = utl::cast<CachedObject>(_ht.find(key));
    if (co == nullptr)
        return nullptr;

    // move list node to front
    if (!frozen)
    {
        access(co->getNode());
    }

    return co;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListIt
ObjectCache::findIt(const Object& key) const
{
    auto co = utl::cast<CachedObject>(_ht.find(key));

    // not cached -> return end()
    if (co == nullptr)
        return _list.end();

    // cached -> move to front and return begin()
    access(co->getNode());
    return ListIt(_list, co->getNode());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectCache::findIt(const Object& key, BidIt& it)
{
    it = findIt(key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectCache::removeIt(BidIt& it)
{
    auto& lit = utl::cast<ListIt>(it);
    auto object = lit.get();
    ++it;
    removeCache(object->getKey());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectCache::set(size_t size)
{
    clear();
    _size = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectCache::access(ListNode* node) const
{
    if (isFrozen())
        return;
    if (node != _list.frontNode())
    {
        _list.moveBefore(ListIt(_list, node), _list.begin());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
ObjectCache::addCache(const Object* object)
{
    auto co = utl::cast<CachedObject>(_ht.find(object->getKey()));
    if (co != nullptr)
        return false;

    // cache maxed out -> remove LRU object
    if (_ht.items() == _size)
    {
        auto it = _list.end();
        --it;
        auto object = it.get();
        ASSERTFNP(_ht.remove(object->getKey()));
        _list.removeIt(it);
    }

    // add new CO
    co = new CachedObject();
    _list.pushFront(co);
    auto node = _list.frontNode();
    co->set(object, node);
    _ht += co;

    // fix _items
    _items = _ht.items();

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
ObjectCache::removeCache(const Object* object)
{
    auto hit = _ht.findIt(object->getKey());
    auto co = utl::cast<CachedObject>(*hit);
    if (co == nullptr)
    {
        return false;
    }

    // remove from list
    ListIt lit(_list, co->getNode());
    _list.removeIt(lit);

    // remove from hashtable
    _ht.removeIt(hit);

    // fix _items
    _items = _ht.items();

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
