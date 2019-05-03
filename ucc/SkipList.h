#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SortedCollection.h>
#include <libutl/SkipListNode.h>

// comment out this #define to try the regular/deterministic method of setting node level
//#define UTL_SKIPLIST_RNG
#ifdef UTL_SKIPLIST_RNG
#include <libutl/RandUtils.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class SkipListIt;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Skip list.

   A skip list is simpler to implement and understand than a balanced binary tree.  In theory, the
   performance characteristics of a skip list are similar to those of a balanced binary tree.
   In my own performance tests, I found that for adding & removing objects, this implementation is
   slower than RBtree by a constant (but significant) factor.  On the other hand, the iteration
   performance of SkipList is markedly superior to that of RBtree (since there is no need to do
   inorder traversal of a tree structure).  I encourage you to look at the code to see how it can
   be optimized.  I'll happily accept any patches that help the performance.

   For practical use, I would recommend RBtree over SkipList.  I leave SkipList in the library for
   educational purposes.

   If you decide to use SkipList or experiment with it, I recommend you set \b maxLevel (in the
   constructor) based on your expectation of how many objects the data structure will contain.
   The default value of \b maxLevel is 19 which is appropriate for storing around 2^20 objects
   (just over one million).  If you expected to store up to about sixty thousand objects, you'd use
   a \b maxLevel of 15 (2^16 = 65,536).  If you aren't sure how many objects the structure will
   contain, you're probably better off to err on the side of a <i>larger</i> value for \b maxLevel.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SkipList : public SortedCollection
{
    UTL_CLASS_DECL(SkipList, SortedCollection);
    friend class SkipListIt;

public:
    typedef SkipListIt iterator;

public:
    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : false) \b multiSet flag
       \param ordering (optional) ordering
       \param maxLevel (optional : 19) number of levels minus one
    */
    SkipList(bool owner, bool multiSet = false, Ordering* ordering = nullptr, uint_t maxLevel = 19)
    {
        init(owner, multiSet, ordering, maxLevel);
    }

    virtual void steal(Object& rhs);

    virtual size_t innerAllocatedSize() const;

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

    void
    insert(const Object& object, const BidIt& it)
    {
        return super::insert(object, it);
    }

    virtual void insert(const Object* object, const BidIt& it);
    //@}

    /// \name Removing Objects
    //@{
    virtual void clear();

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

    static void utl_init();

    static void utl_deInit();

private:
    void init(bool owner = true,
              bool multiSet = false,
              Ordering* ordering = nullptr,
              uint_t maxLevel = 19);
    void deInit();
    iterator findIt(const Object& key, uint_t findType) const;
    iterator findIt(const Object& key, uint_t findType);
    SkipListNode* findNode(const Object& key,
                           uint_t findType = find_first,
                           SkipListNode** update = nullptr) const;
    void removeNode(SkipListNode* node, SkipListNode** update);

private:
    SkipListNode *_head, *_tail;
    uint_t _level;
    uint_t _maxLevel;
#ifdef UTL_SKIPLIST_RNG
    randutils::default_rng* _rng;
#else
    uint32_t _counter;
    uint32_t _counterLim;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TSkipList.h>
