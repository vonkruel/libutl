#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/THeapIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of Heap.

   THeap's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class THeap : public Heap
{
    UTL_CLASS_DECL_TPL(THeap, T, Heap);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param size heap size
       \param increment (optional : uint_t_max) growth increment
       \param owner (optional : true) \b owner flag
       \param multiSet (optional : true) \b multiSet flag
       \param ordering (optional) ordering
    */
    THeap(size_t size,
          size_t increment = uint_t_max,
          bool owner = true,
          bool multiSet = true,
          Ordering* ordering = nullptr);

    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : true) \b multiSet flag
    */
    THeap(bool owner, bool multiSet = true);

    bool
    add(const T& object)
    {
        return super::add(const_cast<T&>(object));
    }

    bool
    add(const T* object)
    {
        return super::add(const_cast<T*>(object));
    }

    void
    add(const Collection& collection)
    {
        super::add(collection);
    }

    THeapIt<T>
    begin() const
    {
        THeapIt<T> res;
        res.copy(Heap::begin());
        return res;
    }

    THeapIt<T>
    end() const
    {
        THeapIt<T> res;
        res.copy(Heap::end());
        return res;
    }

    THeapIt<T>
    begin()
    {
        THeapIt<T> res;
        res.copy(Heap::begin());
        return res;
    }

    THeapIt<T>
    end()
    {
        THeapIt<T> res;
        res.copy(Heap::end());
        return res;
    }

    /** See find(). */
    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(Heap::find(key));
    }

    T*
    pop()
    {
        return utl::cast<T>(Heap::pop());
    }

public:
    typedef T type;
    typedef THeapIt<T> iterator;
    typedef T* value_type; // for STL
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
THeap<T>::THeap(size_t size, size_t increment, bool owner, bool multiSet, Ordering* ordering)
    : Heap(size, increment, owner, multiSet, ordering)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
THeap<T>::THeap(bool owner, bool multiSet)
    : Heap(owner, multiSet)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::THeap, T);
