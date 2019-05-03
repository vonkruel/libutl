#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SortedCollection.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class DequeIt;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A sequence of objects permitting efficient insertion and removal at either end.

   Currently this implementation doesn't permit insertion or removal except at the ends.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Deque : public SortedCollection
{
    friend class DequeIt;
    UTL_CLASS_DECL(Deque, SortedCollection);

public:
    typedef DequeIt iterator;

public:
    /**
       Constructor.
       \param owner \b owner flag
       \param ordering (optional) ordering
    */
    Deque(bool owner, Ordering* ordering = nullptr);

    virtual void steal(Object& rhs);

    virtual size_t innerAllocatedSize() const;

    /// \name Misc. Read-Only
    //@{
    /** Return the object at the front end (nullptr if none). */
    Object*
    front() const
    {
        return first();
    }

    /** Return the object at the back end (nullptr if none). */
    Object*
    back() const
    {
        return last();
    }

    Object* get(size_t idx) const;
    //@}

    /// \name Searching
    //@{
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

    /** Add the given object at the front end. */
    bool
    pushFront(const Object& object)
    {
        return isOwner() ? pushFront(object.clone()) : pushFront(&object);
    }

    /** Add the given object at the back end. */
    bool
    pushBack(const Object& object)
    {
        return isOwner() ? pushBack(object.clone()) : pushBack(&object);
    }

    /** Add the given object at the front end. */
    bool pushFront(const Object* object);

    /** Add the given object at the back end. */
    bool pushBack(const Object* object);
    //@}

    /// \name Removing Objects
    //@{
    /** Remove all objects from the sequence. */
    virtual void clear();

    /** Remove the object at the front. */
    bool removeFront();

    /** Remove the object at the back. */
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
    void init(bool owner = true, Ordering* ordering = nullptr);
    void deInit();

    void grow();

    void next(Object***& blockPtr, uint32_t& blockPos);
    void prev(Object***& blockPtr, uint32_t& blockPos);

private:
    Object*** _blocks;
    Object*** _blocksLim;
    Object*** _beginBlock;
    Object*** _endBlock;
    uint32_t _beginPos;
    uint32_t _endPos;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TDeque.h>
