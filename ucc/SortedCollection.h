#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Collection.h>
#include <libutl/Ordering.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for a Collection whose objects may be sorted.

   Several of SortedCollection's methods take a parameter called \b out which specifies the output
   collection to store the result.  For all such methods, if no output collection is given
   (\b out == nullptr), a new collection of the same class as self will be created to store the
   result.

   <b>Attributes</b>

   \arg \b ordering : You may specify (ordering = nullptr) to use the objects' \b natural ordering
   (via Object::compare()), or you may provide an Ordering for a customized comparison method.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SortedCollection : public Collection
{
    UTL_CLASS_DECL_ABC(SortedCollection, Collection);
    UTL_CLASS_DEFID;

public:
    /**
       Where self and rhs intersect, make self's object equal to the rhs version (i.e. "clobber"
       it with the rhs version).
       \see utl::clobber
       \param rhs collection to clobber with
    */
    void clobber(const SortedCollection* rhs);

    /**
       Compare with another SortedCollection by comparing contained objects.
       \see utl::compare
    */
    virtual int compare(const Object& rhs) const;

    bool
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

    /**
       Insert an object before the pointed-to location.
       \param object object being inserted
       \param it reference location
    */
    void
    insert(const Object& object, const BidIt& it)
    {
        if (isOwner())
            insert(object.clone(), it);
        else
            return insert(&object, it);
    }

    /**
       Insert an object before the pointed-to location.
       \param object object being inserted
       \param it reference location
    */
    virtual void insert(const Object* object, const BidIt& it);

    /**
       Reverse the order of the contained objects.
       \see utl::reverse
    */
    void reverse();

    /// \name Accessors
    //@{
    /**
       Set the ordering, and optionally sort the collection to reflect the new ordering.
       \param ordering new ordering (make a copy)
       \param algorithm (optional : sort_quickSort) sort algorithm
    */
    void
    setOrdering(const Ordering& ordering, uint_t algorithm = sort_quickSort)
    {
        setOrdering(ordering.clone(), algorithm);
    }

    /**
       Set the ordering, and optionally sort the collection to reflect the new ordering.
       \param ordering new ordering
       \param algorithm (optional : sort_quickSort) sort algorithm
    */
    virtual void setOrdering(Ordering* ordering, uint_t algorithm = sort_quickSort);
    //@}

    /// \name Querying and Searching
    //@{
    /**
       Find the range elements [begin,end) that match a given key.
       \param key search key
       \param begin beginning of located range (= end if no object found)
       \param end end of located range (= end if no object found)
    */
    void
    findEqualRange(const Object& key, BidIt& begin, BidIt& end) const
    {
        const_cast_this->findEqualRange(key, begin, end);
        IFDEBUG(begin.setConst(true));
        IFDEBUG(end.setConst(true));
    }

    /**
       Find the range of elements [begin,end) that match a given key.
       \param key search key
       \param begin beginning of located range (= end if no object found)
       \param end end of located range (= end if no object found)
    */
    virtual void findEqualRange(const Object& key, BidIt& begin, BidIt& end);

    /**
       Find the first object matching the given key.
       \param key search key
       \param it const result iterator (= end if no object found)
       \param insert (optional : false) find insertion point?
    */
    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false) const
    {
        const_cast_this->findFirstIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    /**
       Find the first object matching the given key.
       \param key search key
       \param it result iterator (= end if no object found)
       \param insert (optional : false) find insertion point?
    */
    virtual void findFirstIt(const Object& key, BidIt& it, bool insert = false);

    /**
       Find the last object matching the given key.
       \param key search key
       \param it const result iterator (= end if no object found)
    */
    void
    findLastIt(const Object& key, BidIt& it) const
    {
        const_cast_this->findLastIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    /**
       Find the last object matching the given key.
       \param key search key
       \param it result iterator (= end if no object found)
    */
    virtual void findLastIt(const Object& key, BidIt& it);

    /**
       Linear search for an object matching the given key.
       \see utl::linearSearch
       \return found object (nullptr if none)
       \param key search key
    */
    Object* findLinear(const Object& key) const;

    /**
       Linear search for an object matching the given key.  Since the collection is known to be
       sorted, an unsuccessful search can terminate more quickly.
       \return found object (nullptr if none)
       \param key search key
    */
    Object* findLinearSorted(const Object& key) const;
    //@}

    /// \name Removing Objects
    //@{
    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key);
    }

    virtual bool
    remove(const Object& key)
    {
        return super::remove(key);
    }

    /**
       Remove objects that have a match in rhs.
       \see utl::difference
       \param rhs sorted collection to compare with
    */
    void
    remove(const SortedCollection* rhs)
    {
        difference(rhs, this);
    }

    /**
       Remove the object the given iterator points to.
       \param it iterator
    */
    virtual void
    removeIt(BidIt& it)
    {
        super::removeIt(it);
    }

    /**
       Remove part of the collection.
       \param begin beginning of subsequence to be removed
       \param end end of subsequence to be removed
    */
    virtual void removeIt(BidIt& begin, const BidIt& end);

    /**
       Remove objects that have a match in rhs.
       \return self
       \param rhs sorted collection to compare with
    */
    SortedCollection&
    operator-=(const SortedCollection& rhs)
    {
        remove(rhs);
        return *this;
    }
    //@}

    /// \name Set Algorithms
    //@{
    /**
       Determine the difference between self and rhs.
       \see utl::difference
       \return result
       \param rhs sorted collection to compare with
       \param out (optional) output collection
    */
    Collection* difference(const SortedCollection* rhs, Collection* out = nullptr) const;

    /**
       Set self to the intersection of self and rhs.
       \see utl::intersect
       \param rhs sorted collection to intersect with
    */
    void
    intersect(const SortedCollection* rhs)
    {
        intersection(rhs, this);
    }

    /**
       Determine the intersection of self and rhs.
       \see utl::intersect
       \return result
       \param rhs sorted collection to intersect with
       \param out (optional) output collection
       \param multiSet (optional : false) can one of rhs's objects match more than one of
                       self's objects?
    */
    Collection* intersection(const SortedCollection* rhs,
                             Collection* out = nullptr,
                             bool multiSet = false) const;

    /**
       Determine the cardinality of the intersection of self and rhs.
       \see utl::intersectCard
       \return cardinality of intersection
       \param rhs sorted collection to intersect with
    */
    size_t intersectCard(const SortedCollection* rhs) const;

    /**
       Determine whether the intersection of self and rhs is non-empty.
       \return true if intersection is non-empty, false otherwise
       \param rhs sorted collection to intersect with
    */
    bool intersects(const SortedCollection* rhs) const;

    /**
       Determine whether self is a subset of rhs.
       \see utl::isSubSet
       \return true if self is a subset of rhs, false otherwise
       \param rhs sorted collection to compare with
    */
    bool isSubSet(const SortedCollection* rhs) const;

    /**
       Determine whether self is a superset of rhs.
       \see utl::isSuperSet
       \return true if self is a superset of rhs, false otherwise
       \param rhs sorted collection to compare with
    */
    bool
    isSuperSet(const SortedCollection* rhs) const
    {
        return rhs->isSubSet(this);
    }

    /**
       Merge with rhs to form a single sorted sequence.
       \see utl::merge
       \return result
       \param rhs sorted collection to merge with
       \param out (optional) output collection
    */
    Collection* merge(const SortedCollection* rhs, Collection* out = nullptr) const;

    /**
       Determine the symmetric difference of self and rhs.
       \see utl::symmetricDifference
       \return result
       \param rhs sorted collection to compare with
       \param out (optional) output collection
    */
    Collection* symmetricDifference(const SortedCollection* rhs, Collection* out = nullptr) const;

    /**
       Determine the symmetric difference of self and rhs.
       \see utl::symmetricDifference
       \param rhs sorted collection to compare with
       \param lhsOut output collection for self's objects
       \param rhsOut output collection for rhs's objects
    */
    void
    symmetricDifference(const SortedCollection* rhs, Collection* lhsOut, Collection* rhsOut) const;

    /**
       Remove duplicate objects.
       \see utl::unique
       \return result
       \param out (optional) output collection
    */
    Collection* unique(Collection* out = nullptr);

    /**
       Intersect with rhs.
       \see intersect
       \return self
       \param rhs sorted collection to intersect with
    */
    SortedCollection&
    operator&=(const SortedCollection* rhs)
    {
        intersect(rhs);
        return *this;
    }

    /**
       Determine whether self is a subset of rhs.
       \see isSubSet
       \return true if self a subset of rhs, false otherwise
       \param rhs sorted collection to compare with
    */
    bool
    operator<=(const SortedCollection* rhs)
    {
        return isSubSet(rhs);
    }

    /**
       Determine whether self is a superset of rhs.
       \see isSuperSet
       \return true if self a superset of rhs, false otherwise
       \param rhs sorted collection to compare with
    */
    bool
    operator>=(const SortedCollection* rhs)
    {
        return isSuperSet(rhs);
    }
    //@}

    /// \name Sorting
    /** Determine whether the collection is sorted (often useful for testing). */
    bool testSorted() const;

    //@{
    /**
       Multi-key quick-sort the collection.  Every object in the collection must either be a
       String (\b key = false) or have a String as its key (\b key = true).
       \param key (optional : true) objects' strings are their keys?
       \param reverse (optional : false) reverse sort?
       \see getKey
       \see utl::multiKeyQuickSort
    */
    void multiKeyQuickSort(bool key = true, bool reverse = false);

    /** Sort the collection using the quicksort algorithm. */
    void
    sort()
    {
        sort(sort_quickSort);
    }

    /**
       Sort the collection using the given algorithm.
       \see utl::sort
       \param algorithm sort algorithm (see utl::sort_t)
    */
    virtual void sort(uint_t algorithm);
    //@}

#ifdef DEBUG
    virtual void sanityCheck() const;
#endif
protected:
    inline int
    compareObjects(const Object* lhs, const Object* rhs) const
    {
        return super::compareObjects(lhs, rhs);
    }

#ifdef DEBUG
    void checkInsert(const Object* object, const BidIt& it, bool sorted) const;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
