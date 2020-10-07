#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SortedCollection.h>
#include <libutl/TCollection.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Templated proxy for SortedCollection.

   Inherit from TSortedCollection when you want your class to provide
   a SortedCollection interface without exposing the actual concrete
   collection type to the client.  This allows you to change to a new
   collection class (even at run-time) without breaking any clients.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object>
class TSortedCollection : public TCollection<T>
{
    UTL_CLASS_DECL_TPL(TSortedCollection, T, TCollection<T>);

public:
    TSortedCollection(SortedCollection* col)
        : TCollection<T>(col)
    {
    }

    void
    clobber(const SortedCollection* rhs)
    {
        col()->clobber(rhs);
    }

    void
    reverse()
    {
        col()->reverse();
    }

    void
    findEqualRange(const Object& key, BidIt& first, BidIt& last) const
    {
        col()->findEqualRange(key, first, last);
    }

    void
    findEqualRange(const Object& key, BidIt& first, BidIt& last)
    {
        col()->findEqualRange(key, first, last);
    }

    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false) const
    {
        col()->findFirstIt(key, it, insert);
    }

    virtual void
    findFirstIt(const Object& key, BidIt& it, bool insert = false)
    {
        col()->findFirstIt(key, it, insert);
    }

    void
    findLastIt(const Object& key, BidIt& it) const
    {
        col()->findLastIt(key, it);
    }

    virtual void
    findLastIt(const Object& key, BidIt& it)
    {
        col()->findLastIt(key, it);
    }

    T*
    findLinear(const Object& key) const
    {
        return (T*)(col()->findLinear(key));
    }

    T*
    findLinearSorted(const Object& key) const
    {
        return (T*)(col()->findLinearSorted(key));
    }

    TSortedCollection<T>&
    operator+=(const SortedCollection& rhs)
    {
        col()->operator+=(rhs);
    }

    TSortedCollection<T>&
    operator-=(const SortedCollection& rhs)
    {
        col()->operator-=(rhs);
        return self;
    }

    Collection*
    difference(const SortedCollection* rhs, Collection* out = nullptr) const
    {
        return col()->difference(rhs, out);
    }

    void
    intersect(const SortedCollection* rhs)
    {
        col()->intersect(rhs);
    }

    Collection*
    intersection(const SortedCollection* rhs,
                 Collection* out = nullptr,
                 bool multiSet = false) const
    {
        return col()->intersection(rhs, out, multiSet);
    }

    size_t
    intersectCard(const SortedCollection* rhs) const
    {
        return col()->intersectCard(rhs);
    }

    bool
    intersects(const SortedCollection* rhs) const
    {
        return col()->intersects(rhs);
    }

    bool
    isSubSet(const SortedCollection* rhs) const
    {
        return col()->isSubSet(rhs);
    }

    bool
    isSuperSet(const SortedCollection* rhs) const
    {
        return col()->isSuperSet(rhs);
    }

    Collection*
    merge(const SortedCollection* rhs, Collection* out = nullptr) const
    {
        return col()->merge(rhs, out);
    }

    Collection*
    symmetricDifference(const SortedCollection* rhs, Collection* out = nullptr) const
    {
        return col()->symmetricDifference(rhs, out);
    }

    void
    symmetricDifference(const SortedCollection* rhs, Collection* lhsOut, Collection* rhsOut) const
    {
        col()->symmetricDifference(rhs, lhsOut, rhsOut);
    }

    Collection*
    unique(Collection* out = nullptr)
    {
        return col()->unique(out);
    }

    TSortedCollection<T>&
    operator&=(const SortedCollection* rhs)
    {
        col()->operator&=(rhs);
        return self;
    }

    bool
    operator<=(const SortedCollection* rhs)
    {
        return col()->operator<=(rhs);
    }

    bool
    operator>=(const SortedCollection* rhs)
    {
        return col()->operator>=(rhs);
    }

    void
    multiKeyQuickSort(bool key = true, bool reverse = false)
    {
        col()->multiKeyQuickSort(key, reverse);
    }

    void
    sort()
    {
        col()->sort();
    }

    void
    sort(uint_t algorithm)
    {
        col()->sort(algorithm);
    }

    operator SortedCollection&()
    {
        return *col();
    }

    operator const SortedCollection&() const
    {
        return *col();
    }

    operator SortedCollection*()
    {
        return col();
    }

    operator const SortedCollection*() const
    {
        return col();
    }

protected:
    const SortedCollection*
    col() const
    {
        ASSERTD(_col != nullptr);
        return utl::cast<SortedCollection>(_col);
    }

    SortedCollection*
    col()
    {
        ASSERTD(_col != nullptr);
        return utl::cast<SortedCollection>(_col);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TSortedCollection, T);
