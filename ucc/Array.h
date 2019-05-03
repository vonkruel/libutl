#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/RandUtils.h>
#include <libutl/SortedCollection.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class ArrayIt;

/**
   SortedCollection that stores objects in an array.

   Due to its storage representation, Array is able to provide random-access to its contained
   objects.

   <b>Attributes</b>

   \arg \b size : The number of objects that can be contained before it will be necessary to grow
   the array.

   \arg \b increment : When the array is to be grown, the growth increment determines the amount of
   growth.  For example, if increment = 4, the array will always be grown by a multiple of 4.  As a
   special case, if increment = size_t_max, the array will be doubled in size until it is >= the
   required size.

   \arg <b><i>keepSorted</i> flag</b> : If \b keepSorted = true, then sortedness will be
   maintained by methods that add or remove objects.

   \arg <b><i>sorted</i> flag</b> : If \b sorted = true, the array is assumed to be sorted.
   When the array is known to be sorted, it allows searches to be done much more efficiently
   (using a O(log n) binary search).

   Note that when you set \b keepSorted = true, you also implicitly set \b sorted = true.
   Likewise, when you set \b sorted = false, you implicitly set \b keepSorted = false.

   <b>Performance Considerations</b>

   Remember that setting \b keepSorted = true imposes a performance penalty.  If you don't
   require the array to be sorted, make sure it's set to false.  Also, even if you \b do need
   the array to be sorted, you may be able to set \b keepSorted = false.  Consider this code:

   \code
   RBtree myTree;
   myTree += objectA;
   myTree += objectB;
   myTree += objectC;

   Array myArray(false);        // keepSorted = false by default
   myArray = myTree;            // copy myTree's contents

   ASSERT(myTree == myArray);   // array's objects are sorted
   \endcode

   Since the tree's objects are sorted, and the array will have the same ordering, we know the
   array is sorted.  Having \b keepSorted = true would have been pointless, unless we wanted our
   array to have an ordering different from our tree's.  Even in that case, it would likely be
   better to leave \b keepSorted = false, and just sort() the array after copying the tree's
   objects.

   <b>"Holes" and Removal Operations</b>

   A "hole" in an array consists of one or more null entries followed by a non-null entry.
   It's generally best to avoid creating such holes in an array, but it's up to you to decide
   what's appropriate.  When you remove objects, you can specify a \b relocate flag.  The meaning
   of this flag depends on the \b keepSorted flag.  Consider the following array of integers.

   <b>0 1 2 3 4</b>

   Now suppose we do array.remove(2, false).  Since we specified relocate = false, no objects
   are relocated as a result of removing 2:

   <b>0 1 \<null\> 3 4</b>

   Thus, we have created a hole in the array.  Suppose we instead did array.remove(2, true, false)
   (relocate = true, keepSorted = false).
   Here's what we'd end up with:

   <b>0 1 4 3</b>

   We didn't create a hole, and yet we didn't preserve the sortedness of the array.  Finally,
   suppose we did array.remove(2, true, true) (relocate = true, keepSorted = true).  Here's what
   we'd end up with:

   <b>0 1 3 4</b>

   This is the most expensive way (it is O(n)), but it does preserve the sortedness of the array
   without creating a hole.

   <b>Advantages</b>

   \arg random access
   \arg add() for unsorted array is O(1)
   \arg find() for sorted array is O(log n)
   \arg remove() for unsorted array is O(1)
   \arg low memory usage (as little as one pointer per contained object)

   <b>Disadvantages</b>

   \arg add() for sorted array is O(n)
   \arg find() for unsorted array is O(n)
   \arg remove() for sorted array is O(n)

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Array : public SortedCollection
{
    UTL_CLASS_DECL(Array, SortedCollection);
    friend class ArrayIt;

public:
    typedef Object* const* iterator; // native iterator
    typedef ArrayIt base_iterator;   // iterator for compatibility with base

public:
    /**
       Constructor.
       \param size initial size
       \param increment (optional : size_max) growth increment
       \param owner (optional : true) owner flag
       \param multiSet (optional : true) multiSet flag
       \param keepSorted (optional : false) keepSorted flag
       \param ordering (optional) ordering
    */
    Array(size_t size,
          size_t increment = size_t_max,
          bool owner = true,
          bool multiSet = true,
          bool keepSorted = false,
          Ordering* ordering = nullptr);

    /**
       Constructor.
       \param owner (optional : true) owner flag
       \param multiSet (optional : true) multiSet flag
       \param keepSorted (optional : false) keepSorted flag
       \param ordering (optional) ordering
    */
    Array(bool owner, bool multiSet = true, bool keepSorted = false, Ordering* ordering = nullptr);

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    /// \name Misc. Read-Only
    //@{
    virtual size_t innerAllocatedSize() const;

    /** Return the index of the first null object in the array. */
    size_t
    firstNull() const
    {
        return _firstNull;
    }

    /**
       Get the object at the given index.
       \return object at given index (nullptr if none)
       \param idx array index
    */
    Object*
    get(size_t idx) const
    {
        return _array[idx];
    }

    /** Get the size of the array (which is only as large as needed). */
    size_t
    totalSize() const
    {
        return _array.size();
    }

    /**  Determine whether there are one or more "holes" in the array. */
    bool
    hasHole() const
    {
        return (_firstNull != _array.size());
    }

    /** Return the \b keepSorted flag. */
    bool
    isKeptSorted() const
    {
        return getFlag(flg_keepSorted);
    }

    /** Return the \b sorted flag. */
    bool
    isSorted() const
    {
        return getFlag(flg_sorted);
    }
    //@}

    /// \name Misc. Modification
    //@{
    /**
       Remove "holes" within [low,high).
       \param low (optional : 0) low index
       \param high (optional : size_t_max) high index
    */
    void removeHoles(size_t low = 0, size_t high = size_t_max);

    /** Make the allocation exactly large enough to contain the last non-null Object*. */
    void
    economize()
    {
        _array.economize();
    }

    /**
       Same as clear(), but in addition the Object*[] array is deleted.  Call this method instead
       of clear() when you want to minimize memory usage as much as possible.
    */
    void
    excise()
    {
        clear();
        _array.clear();
    }

    /**
       Reserve allocation space.
       \param minSize required minimum size
    */
    void
    reserve(size_t minSize)
    {
        _array.reserve(minSize);
    }

    /**
       Set the object at the given index.  If isOwner(), a copy of the object will be made.
       \param idx array index
       \param object object
       \param firstNull (optional) new index of first null (if known by caller)
    */
    void
    set(size_t idx, const Object& object, size_t firstNull = size_t_max)
    {
        set(idx, isOwner() ? object.clone() : &object, firstNull);
    }

    /**
       Set the object at the given index.
       \param idx array index
       \param object object
       \param firstNull (optional) new index of first null (if known by caller)
    */
    void set(size_t idx, const Object* object, size_t firstNull = size_t_max);

    /** Set the growth increment. */
    void
    setIncrement(size_t increment)
    {
        _array.setIncrement(increment);
    }

    /** Set the \b keepSorted flag.  \b keepSorted = true implies \b sorted = true. */
    void
    setKeepSorted(bool keepSorted)
    {
        setFlag(flg_keepSorted, keepSorted);
        if (keepSorted)
            setFlag(flg_sorted, true);
    }

    /** Set the \b sorted flag.  \b sorted = false implies \b keepSorted = false. */
    void
    setSorted(bool sorted)
    {
        setFlag(flg_sorted, sorted);
        if (!sorted)
            setFlag(flg_keepSorted, false);
    }

    /**
       Randomly shuffle objects within [low,high).
       \param low low index
       \param high high index
       \param rng random number generator
    */
    void
    shuffle(size_t low = 0, size_t high = size_t_max, randutils::mt19937_64_rng* rng = nullptr);

    void
    sort()
    {
        sort(sort_quickSort);
    }

    virtual void sort(uint_t algorithm);
    //@}

    /// \name Searching
    //@{
    virtual Object* find(const Object& key) const;

    /**
       Search [low,high) for an object matching the given key.  This method may also be used to
       find an insertion point for a new object.
       \return index of found object (size_t_max if none)
       \param key search key
       \param findType (optional) search type (see utl::find_t)
       \param low (optional : 0) low index for search
       \param high (optional : size_t_max) high index for search.
    */
    size_t findIndex(const Object& key,
                     uint_t findType = uint_t_max,
                     size_t low = 0,
                     size_t high = size_t_max) const;

    /**
       Find the object matching the given key.
       \return iterator pointing to found object (= end if none)
       \param key search key
    */
    iterator findIt(const Object& key) const;

    void
    findIt(const Object& key, BidIt& it) const
    {
        super::findIt(key, it);
    }

    virtual void findIt(const Object& key, BidIt& it);

    /**
       Find the first object matching the given key.
       \return const iterator for found object (= end if none found)
       \param key search key
       \param insert (optional : false) find insertion point?
    */
    iterator findFirstIt(const Object& key, bool insert = false) const;

    void
    findFirstIt(const Object& key, BidIt& it, bool insert) const
    {
        const_cast_this->findFirstIt(key, it, insert);
        IFDEBUG(it.setConst(true));
    }

    virtual void findFirstIt(const Object& key, BidIt& it, bool insert = false);

    /**
       Find the last object matching the given key.
       \return const iterator for found object (= end if none found)
       \param key search key
    */
    iterator findLastIt(const Object& key) const;

    void
    findLastIt(const Object& key, BidIt& it) const
    {
        const_cast_this->findLastIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    virtual void findLastIt(const Object& key, BidIt& it);
    //@}

    /// \name Adding Objects
    //@{
    bool
    add(const Object& object)
    {
        return super::add(object);
    }

    virtual bool
    add(const Object* object)
    {
        return add(object, isKeptSorted());
    }

    /**
       Add an object, specify the \b keepSorted flag for the addition.
       \return true if object was added, false otherwise
       \param object object to be added
       \param keepSorted should addition maintain sortedness?
    */
    bool add(const Object* object, bool keepSorted);

    void
    add(const Collection& collection)
    {
        super::add(collection);
    }

    /**
       Add null entries, shifting other objects out of the way to make room.
       \param idx index where null entries are to be added
       \param num number of null entries to add
    */
    void add(size_t idx, size_t num = 1);

    /**
       Add an object at a given index.  Other objects will be shifted over to make room if
       necessary.  If isOwner(), a copy of the object will be made.
       \param idx index where object should be added
       \param object object to add
    */
    void
    add(size_t idx, const Object& object)
    {
        add(idx, isOwner() ? object.clone() : &object);
    }

    /**
       Add an object at a given index.  Other objects will be shifted over to make room if
       necessary.
       \param idx index where object should be added
       \param object object to add
    */
    void add(size_t idx, const Object* object);

    void
    insert(const Object& object, iterator it)
    {
        insert(isOwner() ? object.clone() : &object, it);
    }

    void insert(const Object* object, iterator it);

    void
    insert(const Object& object, const BidIt& it)
    {
        return super::insert(object, it);
    }

    virtual void insert(const Object* object, const BidIt& it);
    //@}

    /// \name Removing Objects
    //@{
    /** Remove all objects from the array. */
    virtual void clear();

    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key, true, true);
    }

    virtual bool
    remove(const Object& key)
    {
        return remove(key, true, true);
    }

    /**
       Remove the object matching the given key.
       \return true if object found and removed
       \param key search key
       \param relocate relocate objects to avoid creating a hole?
       \param preserveOrder (optional : true) preserve existing order of objects?
    */
    bool remove(const Object& key, bool relocate, bool preserveOrder = true);

    /**
       Remove the object at the given index.
       \param idx index of object to be removed
       \param relocate (optional : true) relocate objects to avoid creating a hole?
       \param preserveOrder (optional : true) preserve existing order of objects?
    */
    void remove(size_t idx, bool relocate = true, bool preserveOrder = true);

    /**
       Remove the object referred to by the given iterator.
       \param it iterator pointing to the object that will be removed
       \param relocate (optional : true) relocate objects to avoid creating a hole?
       \param preserveOrder (optional : true) preserve existing order of objects?
    */
    void removeIt(iterator it, bool relocate = true, bool preserveOrder = true);

    /**
       Remove objects in the range [begin,end).
       \param begin the beginning of the subsequence to be removed
       \param end end end of the subsequence to be removed
       \param relocate (optional : true) relocate objects to avoid creating a hole?
       \param preserveOrder (optional : true) preserve existing order of objects?
    */
    void removeIt(iterator begin, iterator end, bool relocate = true, bool preserveOrder = true);

    virtual void removeIt(BidIt& it);

    virtual void removeIt(BidIt& begin, BidIt& end);

    /**
       Remove one or more objects starting at the given index.
       \param idx of first object to be removed
       \param num number of objects to be removed
       \param relocate (optional : true) relocate objects to avoid creating a hole?
       \param preserveOrder (optional : true) preserve existing order of objects?
    */
    void removeItems(size_t idx, size_t num, bool relocate = true, bool preserveOrder = true);
    //@}

    //@{
    /// \name Replacing Objects
    /**
       Replace the object at \c it with \c newObject.
       \param it iterator pointing to object to be replaced
       \param newObject replacement object
       \param keepSorted (optional : true) maintain sortedness?
    */
    void replace(iterator it, const Object* newObject, bool keepSorted = true);

    /**
       Replace the object at \c it with \c newObject.
       \param it iterator pointing to object to be replaced
       \param newObject replacement object
    */
    void replace(base_iterator& it, const Object* newObject);

    /**
       Replace object at \c idx with \c newObject.
       \param idx index of object to be replaced
       \param newObject replacement object
    */
    void
    replace(size_t idx, const Object* newObject)
    {
        replace(this->begin() + idx, newObject);
    }

    /**
       Replace oldObject with newObject.
       \return true iff replacement was performed
       \param oldObject object to be replaced
       \param newObject replacement object
    */
    bool
    replace(const Object& oldObject, const Object* newObject)
    {
        return replace(&oldObject, newObject);
    }

    /**
       Replace oldObject with newObject.
       \return true iff replacement was performed
       \param oldObject object to be replaced
       \param newObject replacement object
    */
    bool replace(const Object* oldObject, const Object* newObject);
    //@}

    /// \name Relocating Objects
    //@{
    /**
       Relocate an object to a new position.
       \param destIt iterator pointing to object that will immediately follow the relocated object
       \param srcIt iterator pointing to object that will be relocated
    */
    void
    relocate(iterator destIt, iterator srcIt)
    {
        _array.relocate(indexOf(destIt), indexOf(srcIt));
    }

    /**
       Relocate an object to a new position.
       \param destIdx index of object that will immediately follow the relocated object
       \param srcIdx index of object to be relocated
    */
    void
    relocate(size_t destIdx, size_t srcIdx)
    {
        _array.relocate(destIdx, srcIdx);
    }
    //@}

    /// \name Iterators
    //@{
    inline iterator
    begin() const
    {
        return _array.begin();
    }

    inline iterator
    end() const
    {
        return _array.end();
    }

    inline virtual RandIt* beginNew() const;

    inline virtual RandIt* beginNew();

    inline virtual RandIt* endNew() const;

    inline virtual RandIt* endNew();

    size_t
    indexOf(iterator it) const
    {
        ASSERTD(isValid(it));
        return it - this->begin();
    }

#ifdef DEBUG
    bool
    isValid(iterator it) const
    {
        return (it >= this->begin()) && (it <= this->end());
    }
#endif
    //@}

    /// \name Operators
    //@{
    /**
       Array access operator.
       \see Array::get
       \return object at given index (nullptr if none)
       \param idx index of requested object
    */
    Object* operator[](size_t idx) const
    {
        return get(idx);
    }

    Object* operator[](int idx) const
    {
        return operator[]((size_t)idx);
    }

    Object* operator[](size_t idx)
    {
        return get(idx);
    }

    Object* operator[](int idx)
    {
        return operator[]((size_t)idx);
    }

    Object* operator[](uint_t idx) const
    {
        return operator[]((size_t)idx);
    }

    Object* operator[](uint_t idx)
    {
        return operator[]((size_t)idx);
    }

    /**
       Array access operator (returning reference instead of pointer).
       When you use this method make sure there's an object at the given index.
       \return reference to object at given index
       \param idx array index
    */
    Object&
    operator()(size_t idx) const
    {
        ASSERTD(idx < _array.size());
        ASSERTD(_array[idx] != nullptr);
        return *(_array[idx]);
    }

    Object&
    operator()(size_t idx)
    {
        return const_self(idx);
    }
    Object&
    operator()(int idx) const
    {
        return operator()((size_t)idx);
    }
    Object&
    operator()(int idx)
    {
        return operator()((size_t)idx);
    }
    Object&
    operator()(uint_t idx) const
    {
        return operator()((size_t)idx);
    }
    Object&
    operator()(uint_t idx)
    {
        return operator()((size_t)idx);
    }

    /** Add-or-find access operator (returns Object*). */
    Object* operator[](const Object* object)
    {
        return addOrFind(object);
    }

    /** Add-or-find access operator (returns Object&). */
    Object&
    operator()(const Object* object)
    {
        return *addOrFind(object);
    }

#ifdef DEBUG
    virtual void sanityCheck() const;
#endif
    //@}
private:
    void init(size_t size = 4,
              size_t increment = size_t_max,
              bool owner = true,
              bool multiSet = true,
              bool keepSorted = false,
              Ordering* ordering = nullptr);
    void deInit();

    iterator findIt(const Object& key, uint_t findType) const;

    void _remove(size_t idx);

    void setFirstNull(size_t idx);

    void
    fixArraySize()
    {
        fixArraySize(_array.size());
    }

    void fixArraySize(size_t size);

private:
    enum flg_t
    {
        flg_keepSorted = 4,
        flg_sorted = 5
    };

private:
    size_t _firstNull;
    Vector<Object*> _array;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/ArrayIt.h>
#include <libutl/TArray.h>
