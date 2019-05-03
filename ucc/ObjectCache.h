#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Hashtable.h>
#include <libutl/List.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Cache write policy.
   \ingroup collection
*/
enum cache_t
{
    cache_write_thru, /**< write-thru (safest) */
    cache_write_back  /**< write-back (best performance) */
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// CachedObject ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Cached object.

   When you iterate over objects held in an ObjectCache, you are actually iterating over
   CachedObject instances.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class CachedObject : public Object
{
    UTL_CLASS_DECL(CachedObject, Object);

public:
    void
    set(const Object* object, const ListNode* node)
    {
        _object = const_cast<Object*>(object);
        _node = const_cast<ListNode*>(node);
        _dirty = false;
    }

    virtual const Object&
    getKey() const
    {
        ASSERTD(_object != nullptr);
        return _object->getKey();
    }

    ListNode*
    getNode() const
    {
        return _node;
    }

    Object*
    getObject() const
    {
        return _object;
    }

    bool
    isDirty() const
    {
        return _dirty;
    }

    void
    setDirty(bool dirty)
    {
        _dirty = dirty;
    }

private:
    void
    init()
    {
    }
    void
    deInit()
    {
        delete _object;
    }

private:
    Object* _object;
    ListNode* _node;
    bool _dirty;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ObjectCache /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   LRU object cache.

   ObjectCache implements an LRU object cache.  ObjectCache is designed to assist you in
   implementing a cache for externally stored objects (such as ExtCache).

   <b>Concepts</b>

   \arg <b>LRU object</b> : The least-recently-used object.  When the cache is full, the LRU
   object may be removed to make room to add() a new object.

   \arg <b>MRU object</b> : The most-recently-used object.  When an object in the cache is
   accessed (see access()), it becomes the MRU object.

   <b>Attributes</b>

   \arg \b size : The number of objects that can be held in the cache.  When the cache is full,
   adding a new object to the cache will cause the LRU object to be removed.

   \arg <b><i>frozen</i> flag</b> : When frozen, the cache will be unaffected by accesses to
   cached objects.  In particular, accessing an object will not make it the MRU object.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ObjectCache : public Collection
{
    UTL_CLASS_DECL(ObjectCache, Collection);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

public:
    /**
       Constructor.
       \param size cache size
    */
    ObjectCache(size_t size)
    {
        init(size);
    }

    /** Access the given object, so that it becomes the MRU object. */
    void access(const Object* object);

    /**
       Add the given object to the cache and make it the MRU object.
       \return true if object successfully added, false otherwise
       \param object object to be added
    */
    virtual bool
    add(const Object* object)
    {
        return addCache(object);
    }

    ListIt
    begin() const
    {
        return _list.begin();
    }

    virtual BidIt*
    beginNew() const
    {
        return _list.beginNew();
    }

    virtual BidIt*
    beginNew()
    {
        return _list.beginNew();
    }

    /** Clear the cache. */
    virtual void
    clear()
    {
        _ht.clear();
        _list.clear();
        _items = 0;
    }

    ListIt
    end() const
    {
        return _list.end();
    }

    virtual BidIt*
    endNew() const
    {
        return _list.endNew();
    }

    virtual BidIt*
    endNew()
    {
        return _list.endNew();
    }

    virtual Object*
    find(const Object& key) const
    {
        return find(key, isFrozen());
    }

    /**
       Find the object matching the given key.
       \return found object (nullptr if none)
       \param key search key
       \param frozen frozen flag
    */
    CachedObject* find(const Object& key, bool frozen) const;

    /**
       Find the object matching the given key.
       \return iterator of found object (end() if not found)
       \param key key to search for
    */
    ListIt findIt(const Object& key) const;

    void
    findIt(const Object& key, BidIt& it) const
    {
        const_cast_this->findIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    virtual void findIt(const Object& key, BidIt& it);

    /** Get the LRU object. */
    Object*
    getLRU() const
    {
        return _list.last();
    }

    /** Get the MRU object. */
    Object*
    getMRU() const
    {
        return _list.first();
    }

    /** Get the cache size. */
    size_t
    getSize() const
    {
        return _size;
    }

    /** Get the \b frozen flag. */
    bool
    isFrozen() const
    {
        return getFlag(flg_frozen);
    }

    /** Determine whether the cache is full. */
    bool
    isFull() const
    {
        return (_list.items() == _size);
    }

    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key);
    }

    virtual bool
    remove(const Object& key)
    {
        return removeCache(key);
    }

    virtual void removeIt(BidIt& it);

    /** Clear the cache and set the cache size. */
    void set(size_t size);

    /** Set the \b frozen flag. */
    void
    setFrozen(bool frozen)
    {
        setFlag(flg_frozen, frozen);
    }

public:
    typedef ListIt iterator;

private:
    void
    init(size_t size = 1024)
    {
        _size = size;
        _list.setOwner(false);
    }
    void
    deInit()
    {
        clear();
    }
    void access(ListNode* node) const;
    bool addCache(const Object* object);
    bool removeCache(const Object* object);

private:
    size_t _size;
    Hashtable _ht;
    mutable List _list;
    enum flg_t
    {
        flg_frozen = 4
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
