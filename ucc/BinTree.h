#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BinTreeNode.h>
#include <libutl/MaxObject.h>
#include <libutl/SortedCollection.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinTreeBfsIt;
class BinTreeIt;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Binary tree abstraction.

   BinTree provides an absraction for a SortedCollection with a binary tree representation.
   It provides a basic binary tree implementation, which can be subclassed to implement different
   kinds of binary trees, such as red/black (RBtree).

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinTree : public SortedCollection
{
    UTL_CLASS_DECL_ABC(BinTree, SortedCollection);

public:
    typedef BinTreeIt iterator;

public:
    /**
       Constructor.
       \param owner owner flag
       \param multiSet (optional : false) multiSet flag
       \param ordering (optional) ordering
    */
    BinTree(bool owner, bool multiSet = false, Ordering* ordering = nullptr);

    virtual void steal(Object& rhs);

    /// \name Misc. Read-Only
    //@{
    /** Dump the tree to the given stream (for debugging). */
    void
    dumpTree(Stream& os) const
    {
        _root->dump(os, 0);
    }
    //@}

    /// \name Misc. Modification
    //@{
    /**
       Serialize to/from the given stream.
       \see Collection::serialize
    */
    void
    serializeFast(Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        serializeFast(nullptr, stream, io, mode);
    }

    /**
       Serialize to/from the given stream.
       If a utl::RunTimeClass is given, it is assumed that all objects are of that same type.
       \see Collection::serialize
    */
    void
    serializeFast(const RunTimeClass* rtc, Stream& stream, uint_t io, uint_t mode = ser_default);
    //@}

    /// \name Searching
    //@{
    virtual Object* find(const Object& key) const;

    /**
       Find the object matching the given key.
       \return const iterator for found object (= end if none found)
       \param key search key
    */
    iterator findIt(const Object& key) const;

    /**
       Find the object matching the given key.
       \return const iterator for found object (= end if none found)
       \param key search key
    */
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

    virtual void
    removeIt(BidIt& begin, BidIt& end)
    {
        super::removeIt(begin, end);
    }
    //@}

    /// \name Iterators
    //@{
    /** Return a const in-order iterator for the first object. */
    inline iterator begin() const;

    /** Return an in-order iterator for the first object. */
    inline iterator begin();

    /** Return a const breadth-first iterator for the root object. */
    inline BinTreeBfsIt beginBFS() const;

    /** Return a breadth-first iterator for the root object. */
    inline BinTreeBfsIt beginBFS();

    inline virtual BidIt* beginNew() const;

    inline virtual BidIt* beginNew();

    /** Return a const breadth-first iterator for the root object. */
    inline BinTreeBfsIt* beginBFSNew() const;

    /** Return a breadth-first iterator for the root object. */
    inline BinTreeBfsIt* beginBFSNew();

    /** Return a const in-order iterator for the end. */
    inline iterator end() const;

    /** Return an in-order iterator for the end. */
    inline iterator end();

    /**
       Return a const breadth-first iterator for the
       end of the tree.
    */
    inline BinTreeBfsIt endBFS() const;

    /** Return a breadth-first iterator for the end of the tree. */
    inline BinTreeBfsIt endBFS();

    inline virtual BidIt* endNew() const;

    inline virtual BidIt* endNew();

    /**
       Return a const breadth-first iterator for the end
       of the tree.
    */
    inline BinTreeBfsIt* endBFSNew() const;

    /** Return a breadth-first iterator for the end of the tree. */
    inline BinTreeBfsIt* endBFSNew();

    /** Return a const iterator for the root object. */
    inline iterator root() const;

    /** Return an iterator for the root object. */
    inline iterator root();
    //@}

#ifdef DEBUG
    iterator begin(bool notifyOwner);

    virtual void sanityCheck() const;
#endif
protected:
    inline int
    compareObjects(const Object* lhs, const Object* rhs) const
    {
        return (lhs == &maxObject) ? 1 : super::compareObjects(lhs, rhs);
    }
    void clear(BinTreeNode* node);
    virtual BinTreeNode* createNode(const Object* object) = 0;
    BinTreeNode* findFirstMatch(const Object& key, BinTreeNode* node) const;
    BinTreeNode* findInsertionPoint(const Object& key, bool* insertLeft = nullptr) const;
    iterator findIt(const Object& key, uint_t findType) const;
    iterator findIt(const Object& key, uint_t findType);
    BinTreeNode* findLastMatch(const Object& key, BinTreeNode* node) const;
    BinTreeNode* findNode(const Object& key, uint_t findType) const;
    BinTreeNode* removeNode(BinTreeNode* node);
    virtual void resetRoot();

protected:
    BinTreeNode* _root;

private:
    void init(bool owner = true, bool multiSet = false, Ordering* ordering = nullptr);
    void deInit();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BinTreeIt.h>
#include <libutl/BinTreeBfsIt.h>
