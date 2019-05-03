#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Collection.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class HeapIt;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Heap data structure.

   To learn about heaps, please consult your favourite book on algorithms and data structures.

   Two common applications of Heap are the heap-sort (utl::heapSort()) and PriorityQueue.

   <b>Attributes</b>

   \arg \b size : Number of objects that can be contained before it will be necessary to grow the
   heap.

   \arg \b increment : When the heap is to be grown, the growth increment determines the amount of
   growth.  For example, if increment = 16, the heap will always be grown by a multiple of 16.  As
   a special case, if increment = size_t_max, the heap will be doubled in size until it is
   >= the required size.

   \arg \b ordering : You may specify \b ordering = nullptr to use the objects' \b natural ordering
   (via Object::compare()), or you may provide an Ordering for a customized comparison method.

   <b>Advantages</b>

   \arg add() is O(log n)
   \arg pop() O(log n) (removing largest object)
   \arg getMax() is O(1)

   <b>Disadvantages</b>

   \arg find() is O(n)
   \arg remove() is O(n) (removing via search key)

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Heap : public Collection
{
    UTL_CLASS_DECL(Heap, Collection);
    friend class HeapIt;

public:
    typedef HeapIt iterator;

public:
    /**
       Constructor.
       \param size heap size
       \param increment (optional : size_t_max) growth increment
       \param owner (optional : true) \b owner flag
       \param multiSet (optional : true) \b multiSet flag
       \param ordering (optional) ordering
    */
    Heap(size_t size,
         size_t increment = size_t_max,
         bool owner = true,
         bool multiSet = true,
         Ordering* ordering = nullptr)
    {
        init(size, increment, owner, multiSet, ordering);
    }

    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : true) \b multiSet flag
    */
    Heap(bool owner, bool multiSet = true)
    {
        init(16, size_t_max, owner, multiSet);
    }

    virtual void steal(Object& rhs);

    virtual size_t innerAllocatedSize() const;

    /// \name Misc. Modification
    //@{
    /**
       Grow to the given size (at least).
       \param newSize required size
    */
    void
    reserve(size_t newSize)
    {
        _array.grow(newSize);
    }
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

    /** Return the largest contained object (nullptr if empty()). */
    Object*
    getMax() const
    {
        if (_items == 0)
            return nullptr;
        else
            return _array[0];
    }
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
    //@}

    /// \name Removing Objects
    //@{
    /** Remove all objects from the heap. */
    virtual void clear();

    /** Remove the largest object. */
    Object* pop();

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
    inline iterator begin() const;

    inline iterator begin();

    inline virtual BidIt* beginNew() const;

    inline virtual BidIt* beginNew();

    inline iterator end() const;

    inline iterator end();

    inline virtual BidIt* endNew() const;

    inline virtual BidIt* endNew();
    //@}

private:
    void init(size_t size = 16,
              size_t increment = size_t_max,
              bool owner = true,
              bool multiSet = true,
              Ordering* ordering = nullptr);

    void deInit();

    inline int
    compareObjects(const Object* lhs, const Object* rhs) const
    {
        return super::compareObjects(lhs, rhs);
    }

    void find(const Object& key, size_t& idx) const;

    void removeIdx(size_t idx);

    void swapUp(size_t idx);

    void swapDown(size_t idx);

private:
    Vector<Object*> _array;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/THeap.h>
