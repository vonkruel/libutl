#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BitArray.h>
#include <libutl/Collection.h>
#include <libutl/PointerIntPair.h>
#include <libutl/Vector.h>
#include <libutl/dlist.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class HashFunction;
class HashtableIt;

////////////////////////////////////////////////////////////////////////////////////////////////////
// HashFunction ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Hash function.
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HashFunction
{
public:
    /** Virtual destructor. */
    virtual ~HashFunction()
    {
    }

    /** Create new instance. */
    virtual HashFunction* clone() const = 0;

    /** Compute the hash function for the given object. */
    virtual size_t
    hash(const Object* object, size_t size) const
    {
        return object->hash(size);
    }

    size_t
    operator()(const Object* object, size_t size) const
    {
        return hash(object, size);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Hashtable ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Chained hashing collection.

   Hashtable doesn't inherit from SortedCollection because objects in a hashtable aren't organized
   based on any ordering of search keys.  The hash function is supposed to bring us straight to the
   object we're looking for, or at least to a very small group of objects that have the same hash.
   If utl::Collection::isMultiSet() is false, then add(const utl::Object*) won't permit you to add
   two objects that compare as equal.  Comparison of objects is also done by utl::Hashtable::find().

   When multiple objects hash to the same location, they are put into a linked list (but only then).

   You can either provide a hash function (in the constructor or by calling setHashFunction()), or
   you can ensure Object::hash() is defined for the contained objects or their keys
   (see Object::getKey()).

   <b>Advantages</b>

   \arg add(const utl::Object*), find(), remove() are practically O(1) if:
   \arg -- the hash function gives a fairly uniform distribution
   \arg -- the hash table isn't overloaded (at least one table entry per contained object)
   \arg performance degrades in near-linear fashion as load factor is increased
   \arg low memory usage (probably ~1.5 pointers per contained object on average)

   <b>Disadvantages</b>

   \arg contained objects are not sorted

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Hashtable : public Collection
{
    UTL_CLASS_DECL(Hashtable, Collection);
    friend class HashtableIt;

public:
    typedef HashtableIt iterator;

public:
    /**
       Constructor.
       \param size initial size of hash-table
       \param maxLF (optional : 90) maximum load factor
       \param owner (optional : true) owner flag
       \param multiSet (optional : false) multiSet flag
       \param hfunc (optional) hash function
    */
    Hashtable(size_t size,
              uint_t maxLF = 90,
              bool owner = true,
              bool multiSet = false,
              const HashFunction* hfunc = nullptr)
    {
        init(size, maxLF, owner, multiSet, hfunc);
    }

    /**
       Constructor.
       \param owner owner flag
       \param multiSet multiSet flag
       \param hfunc (optional) hash function
    */
    Hashtable(bool owner, bool multiSet = false, const HashFunction* hfunc = nullptr)
    {
        init(0, 90, owner, multiSet, hfunc);
    }

    virtual void steal(Object& rhs);

    virtual void vclone(const Object& rhs);

    virtual size_t innerAllocatedSize() const;

    /// \name Misc. Modification
    //@{
    /** Set the hash function. */
    void
    setHashFunction(const HashFunction* hashfn)
    {
        delete _hashfn;
        _hashfn = hashfn;
    }

    /** Grow the hash-table to a size large enough to contain the given number of objects. */
    void reserve(size_t newSize);
    //@}

    /// \name Searching
    //@{
    virtual Object* find(const Object& key) const;

    iterator findIt(const Object& key) const;

    iterator findIt(const Object& key);

    void
    findIt(const Object& key, BidIt& it) const
    {
        const_cast_this->findIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    virtual void findIt(const Object& key, BidIt& it);
    //@}

    /// \name Adding Objects
    //@{
    bool
    add(const Object& object)
    {
        return super::add(object);
    }

    virtual bool add(const Object* object);

    void
    add(const Collection& collection)
    {
        super::add(collection);
    }

    Object*
    addOrFind(const Object& object)
    {
        return super::addOrFind(object);
    }

    virtual Object* addOrFind(const Object* object);

    bool
    addOrUpdate(const Object& object)
    {
        return super::addOrUpdate(object);
    }

    virtual bool addOrUpdate(const Object* object);
    //@}

    /// \name Removing Objects
    //@{
    virtual void clear();

    /**
       Same as clear(), but in addition the pointer array is deleted.  Call this method instead
       of clear() when you want to minimize memory usage as much as possible.
    */
    void excise();

    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key);
    }

    virtual bool remove(const Object& key);

    virtual void removeIt(BidIt& it);
    //@}

    /// \name Iterators
    //@{
    iterator begin() const;

    iterator begin();

    virtual BidIt* beginNew() const;

    virtual BidIt* beginNew();

    inline iterator end() const;

    inline iterator end();

    virtual BidIt* endNew() const;

    virtual BidIt* endNew();
    //@}

private:
    void init(size_t size = 0,
              uint_t maxLF = 90,
              bool owner = true,
              bool multiSet = false,
              const HashFunction* hashfn = nullptr);
    void deInit();

    void grow(size_t newSize);

    void find(const Object& key, size_t& idx, ListNode*& node) const;

    void remove(size_t idx, ListNode* node);

    size_t
    hash(const Object* object) const
    {
        ASSERTD(object != nullptr);
        if (_hashfn == nullptr)
            return object->hash(_array.size());
        return _hashfn->hash(object, _array.size());
    }

    bool
    isObject(size_t idx) const
    {
        return (_array[idx].getInt() == 0);
    }

    auto
    getObject(size_t idx) const
    {
        ASSERTD(isObject(idx));
        return _array[idx].getPointer();
    }

    auto
    setObject(size_t idx, Object* obj)
    {
        _array[idx].set(obj, 0);
        return obj;
    }

    bool
    isHead(size_t idx) const
    {
        return (_array[idx].getInt() == 1);
    }

    ListNode*
    getHead(size_t idx) const
    {
        ASSERTD(isHead(idx));
        return reinterpret_cast<ListNode*>(_array[idx].getPointer());
    }

    void
    setHead(size_t idx, ListNode* ln)
    {
        _array[idx].set(reinterpret_cast<Object*>(ln), 1);
    }

private:
    typedef PointerIntPair<Object*, 1> pip_t;

private:
    const HashFunction* _hashfn;
    size_t _limit;
    uint_t _maxLF;
    Vector<pip_t> _array;
    static size_t primes[];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/THashtable.h>
