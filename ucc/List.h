#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SortedCollection.h>
#include <libutl/ListNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class ListIt;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Doubly-linked list.

   List contains its own specialized merge-sort (see sort()), though the generic sort functions
   can still be used (see SortedCollection::sort()).

   <b>Attributes</b>

   \arg <b><i>keepSorted</i> flag</b> : If \b keepSorted = true, then sortedness will be maintained
   by methods that add or remove objects.

   \arg <b><i>sorted</i> flag</b> : If \b sorted = true, the list is assumed to be sorted.  When
   the list is known to be sorted, it allows searches to be done much more efficiently
   (unsuccessful searches terminate more quickly).

   Note that when you set \b keepSorted = true, you also implicitly set \b sorted = true.
   Likewise, when you set \b sorted = false, you implicitly set \b keepSorted = false.

   <b>Performance Considerations</b>

   Remember that setting \b keepSorted = true imposes a performance penalty.  If you don't require
   the list to be sorted, make sure it's set to false.  Also, even if you \b do need the list to
   be sorted, you may be able to set \b keepSorted = false.

   Consider this code:

   \code
   RBtree myTree;
   myTree += objectA;
   myTree += objectB;
   myTree += objectC;

   List myList(false);          // keepSorted = false by default
   myList = myTree;             // copy myTree's contents

   ASSERT(myTree == myList);    // list's objects are sorted
   \endcode

   Since the tree's objects are sorted, and the list will have the same ordering, we know the list
   is sorted.  Having \b keepSorted = true would have been pointless, unless we wanted our list to
   have an ordering different from our tree's.  Even in that case, it would likely be better to
   leave \b keepSorted = false, and just sort() the list after copying the tree's objects.
   Batch processing is usually more efficient.

   <b>Advantages</b>

   \arg add() is O(1) (adding at front, back, or via ListIt)
   \arg remove() is O(1) (removing front, back, or via ListIt)

   <b>Disadvantages</b>

   \arg lots of cache misses
   \arg add() for sorted list is O(n)
   \arg find() is O(n) even if list is sorted
   \arg remove() for sorted list is O(n)

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class List : public SortedCollection
{
    UTL_CLASS_DECL(List, SortedCollection);

public:
    typedef ListIt iterator;

public:
    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : true) \b multiSet flag
       \param keepSorted (optional : false) \b keepSorted flag
       \param ordering (optional) ordering
    */
    List(bool owner, bool multiSet = true, bool keepSorted = false, Ordering* ordering = nullptr);

    virtual void steal(Object& rhs);

    virtual size_t innerAllocatedSize() const;

    /// \name Misc. Read-Only
    //@{
    /** Return the object at the front of the list (nullptr if none). */
    Object*
    front() const
    {
        return first();
    }

    /** Return the object at the back of the list (nullptr if none). */
    Object*
    back() const
    {
        return last();
    }

    /** Return the front node. */
    const ListNode*
    frontNode() const
    {
        return _front;
    }

    /** Get the \b keepSorted flag. */
    bool
    isKeptSorted() const
    {
        return getFlag(flg_keepSorted);
    }

    /** Get the \b sorted flag. */
    bool
    isSorted() const
    {
        return getFlag(flg_sorted);
    }
    //@}

    /// \name Misc. Modification
    //@{
    /** Set the \b keepSorted flag. */
    void
    setKeepSorted(bool keepSorted)
    {
        setFlag(flg_keepSorted, keepSorted);
        if (keepSorted)
            setFlag(flg_sorted, true);
    }

    virtual void setOrdering(Ordering* ordering, uint_t algorithm = sort_mergeSort);

    void
    setOrdering(const Ordering& ordering, uint_t algorithm = sort_mergeSort)
    {
        setOrdering(ordering.clone(), algorithm);
    }

    /** Set the \b sorted flag. */
    void
    setSorted(bool sorted)
    {
        setFlag(flg_sorted, sorted);
        if (!sorted)
            setFlag(flg_keepSorted, false);
    }

    void
    sort()
    {
        sort(sort_mergeSort);
    }

    virtual void sort(uint_t algorithm);

    /**
       Move the lhs node so that it precedes the rhs node.
       \param lhs specifies lhs node
       \param rhs specifies rhs node
    */
    void moveBefore(const iterator& lhs, const iterator& rhs);
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

    void
    findEqualRange(const Object& key, BidIt& begin, BidIt& end) const
    {
        const_cast_this->findEqualRange(key, begin, end);
        IFDEBUG(begin.setConst(true));
        IFDEBUG(end.setConst(true));
    }

    virtual void findEqualRange(const Object& key, BidIt& begin, BidIt& end);

    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false) const
    {
        const_cast_this->findFirstIt(key, it, insert);
        IFDEBUG(it.setConst(true));
    }

    virtual void findFirstIt(const Object& key, BidIt& it, bool insert = false);

    /**
       Find the first object matching the given key.
       \return const iterator for found object (= end if none found)
       \param key search key
       \param insert (optional : false) find insertion point?
    */
    iterator findFirstIt(const Object& key, bool insert = false) const;

    /**
       Find the first object matching the given key.
       \return iterator for found object (= end if none found)
       \param key search key
       \param insert (optional : false) find insertion point?
    */
    iterator findFirstIt(const Object& key, bool insert = false);

    void
    findLastIt(const Object& key, BidIt& it) const
    {
        const_cast_this->findLastIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    virtual void findLastIt(const Object& key, BidIt& it);

    /**
       Find the last object matching the given key.
       \return const iterator for found object (= end if none found)
       \param key search key
    */
    iterator findLastIt(const Object& key) const;

    /**
       Find the last object matching the given key.
       \return iterator for found object (= end if none found)
       \param key search key
    */
    iterator findLastIt(const Object& key);
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

    /**
       Add the given object before the given node.
       \param object object to be added
       \param it specifies node to place new node before
    */
    void addBefore(const Object* object, const iterator& it);

    /** Add the given object at the front end. */
    void
    pushFront(const Object& object)
    {
        isOwner() ? pushFront(object.clone()) : pushFront(&object);
    }

    /** Add the given object at the back end. */
    void
    pushBack(const Object& object)
    {
        isOwner() ? pushBack(object.clone()) : pushBack(&object);
    }

    /** Add the given object at the front of the list. */
    void pushFront(const Object* object);

    /** Add the given object at the back of the list. */
    void pushBack(const Object* object);

    void
    insert(const Object& object, const BidIt& it)
    {
        return super::insert(object, it);
    }

    virtual void insert(const Object* object, const BidIt& it);
    //@}

    /// \name Removing Objects
    //@{
    /** Remove all objects from the list. */
    virtual void clear();

    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key);
    }

    virtual bool remove(const Object& key);

    virtual void removeIt(BidIt& it);

    /** Remove the front node. */
    bool removeFront();

    /** Remove the back node. */
    bool removeBack();

    /** Retrieve & remove the object at the front. */
    Object* popFront();

    /** Retrieve & remove the object at the back. */
    Object* popBack();
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
    void init(bool owner = true,
              bool multiSet = true,
              bool keepSorted = false,
              Ordering* ordering = nullptr);
    void deInit();
    ListNode* findInsertionPoint(const Object& key) const;
    iterator findIt(const Object& key, uint_t findType) const;
    iterator findIt(const Object& key, uint_t findType);
    ListNode* findNode(const Object& key) const;
    ListNode* insertionSort(ListNode* c);
    ListNode* merge(ListNode* a, ListNode* b);
    ListNode* mergeSort(ListNode* c);
    void removeNode(ListNode* node);

private:
    ListNode *_front, *_back;
    enum flg_t
    {
        flg_keepSorted = 4,
        flg_sorted = 5
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TList.h>
