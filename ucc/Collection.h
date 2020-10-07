#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BidIt.h>
#include <libutl/Predicate.h>
#include <libutl/algorithms.h>
#ifdef DEBUG
#include <libutl/Mutex.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class ListNode;
class SlistNode;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A collection of objects.

   Collection is an abstraction for container classes.  As such, it provides a standard interface
   for functionality that is useful for all kinds of collection classes:

   \arg copying
   \arg serialization
   \arg iterating over contained objects
   \arg adding, updating, removing, and finding objects
   \arg writing a human-readable representation of the contained objects

   <b>Attributes</b>

   \arg <b><i>owner</i> flag</b> : If a collection owns its objects, it is responsible for deleting
   them when appropriate (e.g. upon its destruction).  This means that copies of objects must also
   be made when appropriate.  For example, if you copy objects into a collection that owns its
   objects, copies will be made of those objects (via Object::copy()) instead of just copying the
   pointers.

   \arg <b><i>multiSet</i> flag</b> : If a collection is a multi-set, it may contain two or more
   objects that are equivalent.

   \arg <b>ordering</b> : The utl::Ordering specified for a collection will be called upon to
   perform comparisons between objects.  If no ordering object is provided, objects will be
   compared using utl::Object::compare().  Whenever contained objects are compared to search
   keys (e.g. when invoking find()), the left-hand-side object is the contained object and
   the right-hand-side object is the search key.  This consistent behavior is meant to simplify
   the implementation of utl::Object::compare() overrides and utl::Ordering specializations,
   which can assume the class (or at least the base class) of the left-hand-side object involved in
   the comparison.  The right-hand-side object in a comparison may either be another contained
   object (possibly from another collection) or a search key.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Collection
    : public Object
    , protected FlagsMI
{
    UTL_CLASS_DECL_ABC(Collection, Object);

public:
    typedef Object type;
    typedef Object* value_type;
    typedef TBidIt<> iterator;

public:
    /**
       Assert ownership of contained objects.  Make copies of all contained objects, and set the
       \b owner flag to true.
    */
    void assertOwner();

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    virtual void vclone(const Object& rhs);

    virtual void dump(Stream& os, uint_t level = uint_t_max) const;

    virtual void
    serialize(Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        serialize(nullptr, stream, io, mode);
    }

    /**
       Serialize to/from the given stream.  If a RunTimeClass is given, it is assumed that all
       objects in the collection are of that same type.
       \see Object::serialize
       \see utl::serialize
    */
    void serialize(const RunTimeClass* rtc, Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual size_t innerAllocatedSize() const;

    /**
       Update the given object.
       \return true if object successfully found and updated, false otherwise
       \param object object to be updated
    */
    virtual bool update(const Object* object);

    /// \name Accessors
    //@{
    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Set the \b owner flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /** Get the ordering. */
    const Ordering*
    ordering() const
    {
        return _ordering;
    }

    /** Get the ordering. */
    Ordering*
    ordering()
    {
        return _ordering;
    }

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

    /** Determine whether the collection is empty. */
    bool
    empty() const
    {
        return (items() == 0);
    }

    /** Get the \b multiSet flag. */
    bool
    isMultiSet() const
    {
        return getFlag(flg_multiSet);
    }

    /** Set the \b multiSet flag. */
    void
    setMultiSet(bool multiSet)
    {
        setFlag(flg_multiSet, multiSet);
    }

    /** Get \b marked flag. */
    bool
    isMarked() const
    {
        return getFlag(flg_marked);
    }

    /** Set \b marked flag. */
    void
    setMarked(bool marked = true)
    {
        setFlag(flg_marked, marked);
    }

    /** Get the number of contained objects. */
    size_t
    items() const
    {
        return _items;
    }

    /** Get the number of contained objects. */
    size_t
    size() const
    {
        return _items;
    }
    //@}

    /// \name Adding Objects
    //@{
    /**
       Add an object to the collection.  If isOwner(), a copy of the object will be made.
       \return true if successful, false otherwise
       \param object object to add
    */
    bool
    add(const Object& object)
    {
        if (isOwner())
            return add(object.clone());
        else
            return add(&object);
    }

    /**
       Add an object to the collection.
       \return true if successful, false otherwise
       \param object object to add
    */
    virtual bool add(const Object* object) = 0;

    /**
       Add another collection's objects.  If isOwner(), copies will be made of all objects added.
       \param collection collection to copy objects from
    */
    void
    add(const Collection& collection)
    {
        copyItems(collection);
    }

    /**
       Add an object to the collection.
       \return iterator pointing to added object (end() if add failed).
       \param object object to add
    */
    virtual BidIt* addIt(const Object* object);

    /**
       Add the given object, or find a matching object already contained.
       \return found object (or provided object, if it was added)
    */
    Object*
    addOrFind(const Object& object)
    {
        if (isOwner())
            return addOrFind(object.clone());
        return addOrFind(&object);
    }

    /**
       Add the given object, or find a matching object already contained.
       \return found object (or provided object, if it was added)
    */
    virtual Object* addOrFind(const Object* object);

    /**
       Add or update the given object.  If the object already exists, it will be updated, otherwise
       it will be added.  If isOwner(), a copy of the object will be made.
       \return true if object was added, false if existing object was replaced
       \param object object to add or update
    */
    bool
    addOrUpdate(const Object& object)
    {
        if (isOwner())
            return addOrUpdate(object.clone());
        return addOrUpdate(&object);
    }

    /**
       Add or update the given object.
       If the object already exists, it will be updated, otherwise it will be added.
       \return true if object was replaced, false if it was newly added
       \param object object to add or update
    */
    virtual bool addOrUpdate(const Object* object);

    /**
       Copy objects from another collection.
       \see utl::copy
       \return self
       \param src source collection
       \param pred (optional) predicate : require (pred(object) == predVal)
       \param predVal (optional : true) predicate value
    */
    Collection&
    copyItems(const Collection* src, const Predicate* pred = nullptr, bool predVal = true);

    /**
       Copy objects from a double-linked list.
       \return self
       \param src head of source list
       \param pred (optional) predicate : require (pred(object) == predVal)
       \param predVal (optional : true) predicate value
    */
    Collection&
    copyItems(const ListNode* src, const Predicate* pred = nullptr, bool predVal = true);

    /**
       Copy objects from a single-linked list.
       \return self
       \param src head of source list
       \param pred (optional) predicate : require (pred(object) == predVal)
       \param predVal (optional : true) predicate value
    */
    Collection&
    copyItems(const SlistNode* src, const Predicate* pred = nullptr, bool predVal = true);

    /** "Steal" items from another collection. */
    Collection& stealItems(Collection* src);

    /**
       Add an object to the collection.
       If isOwner(), make a copy of the object.
       \return self
       \param rhs object to add
    */
    Collection&
    operator+=(const Object& rhs)
    {
        add(rhs);
        return *this;
    }

    /**
       Add an object to the collection.
       \return self
       \param rhs object to add
    */
    Collection&
    operator+=(const Object* rhs)
    {
        add(rhs);
        return *this;
    }

    /**
       Add another collection's objects.  If isOwner(), make copies of all objects added.
       \return self
       \param rhs collection to copy objects from
    */
    Collection&
    operator+=(const Collection& rhs)
    {
        add(rhs);
        return *this;
    }

    /**
       Remove an object from the collection.
       \return self
       \param rhs key to remove
    */
    Collection&
    operator-=(const Object& rhs)
    {
        remove(rhs);
        return *this;
    }
    //@}

    /// \name Human-Readable Representation
    //@{
    /**
       Dump contained objects to a stream.
       \param os output stream
       \param level level of 'verbosity' for the dump
       \param key dump object keys?
       \param printClassName print each object's class name?
       \param indent indent each object?
       \param separator dumped between each object
    */
    void dump(Stream& os,
              uint_t level,
              bool key,
              bool printClassName,
              uint_t indent,
              const char* separator) const;

    /**
       Obtain a string representation by invoking Object::toString() on all contained objects.
    */
    virtual String toString() const;

    /**
       Obtain a string representation by invoking Object::toString() on all contained objects
       (or their keys).
       \see Object::getKey
       \see utl::toString
       \return string representation of collection
       \param key invoke Object::toString() on object keys?
    */
    String toString(bool key) const;

    /**
       Obtain a string represenation by invoking Object::toString() on all contained objects
       (or their keys).
       \see Object::getKey
       \see utl::toString
       \return string representation of collection
       \param sep separator, such as ", "
       \param key invoke Object::toString() on object keys?
    */
    String
    toString(const char* sep, bool key = false) const
    {
        return toString(String(sep, false), key);
    }

    /**
       Obtain a string represenation of the collection by invoking Object::toString() on all
       contained objects (or their keys).
       \see Object::getKey
       \see utl::toString
       \return string representation of collection
       \param sep separator, such as ", "
       \param key invoke Object::toString() on object keys?
    */
    String toString(const String& sep, bool key = false) const;
    //@}

    /// \name Iterators
    //@{
    /** Return a const iterator pointing to the first object in the collection. */
    iterator begin() const;

    /** Return an iterator pointing to the first object in the collection. */
    iterator begin();

    /** Return a const iterator pointing to the beginning of the collection. */
    virtual BidIt* beginNew() const = 0;

    /** Return an iterator pointing to the beginning of the collection. */
    virtual BidIt* beginNew() = 0;

    /** Create a const iterator. */
    virtual BidIt* createIt() const;

    /** Create an iterator. */
    virtual BidIt* createIt();

    /** Return a const iterator pointing to the end of the collection. */
    iterator end() const;

    /** Return an iterator pointing to the end of the collection. */
    iterator end();

    /** Return a const iterator pointing to the end of the collection. */
    virtual BidIt* endNew() const = 0;

    /** Return an iterator pointing to the end of the collection. */
    virtual BidIt* endNew() = 0;
    //@}

    /// \name Querying and Searching
    //@{
    /**
       Determine whether the collection contains an object matching the given key.
       \return true if matching object exists, false otherwise
       \param key search key
    */
    bool
    contains(const Object* key) const
    {
        ASSERTD(key != nullptr);
        return contains(*key);
    }

    /**
       Determine whether the collection contains an object matching the given key.
       \return true if matching object exists, false otherwise
       \param key search key
    */
    bool contains(const Object& key) const;

    /**
       Count contained objects.
       \param pred (optional) predicate : require (pred(object) == predVal)
       \param predVal (optional : true) predicate value
    */
    size_t count(const Predicate* pred = nullptr, bool predVal = true) const;

    /**
       Find an object matching a given key.
       \return found object (nullptr if none)
       \param key search key
    */
    Object*
    find(const Object* key) const
    {
        ASSERTD(key != nullptr);
        return find(*key);
    }

    /**
       Find an object matching a given key.
       \return found object (nullptr if none)
       \param key search key
    */
    virtual Object* find(const Object& key) const;

    /**
       Find an object matching a given key.
       \param key search key
       \param it const result iterator (= end() if no object found)
    */
    inline void
    findIt(const Object& key, BidIt& it) const
    {
        const_cast_this->findIt(key, it);
        IFDEBUG(it.setConst(true));
    }

    /**
       Find an object matching a given key.
       \param key search key
       \param it result iterator (= end() if no object found)
    */
    virtual void findIt(const Object& key, BidIt& it);

    /** Return the first object (nullptr if empty). */
    Object* first() const;

    /** See contains(). */
    bool
    has(const Object* key) const
    {
        ASSERTD(key != nullptr);
        return contains(*key);
    }

    /** See contains(). */
    bool
    has(const Object& key) const
    {
        return contains(key);
    }

    /** Get the last object (nullptr if empty). */
    Object* last() const;
    //@}

    /// \name Removing Objects
    //@{
    /** Clear the collection by removing all objects. */
    virtual void clear() = 0;

    /**
       Remove the object matching the given key.
       \return true if object found and removed, false otherwise
       \param key search key
    */
    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key);
    }

    /**
       Remove the object matching the given key.
       \return true if object found and removed, false otherwise
       \param key search key
    */
    virtual bool remove(const Object& key);

    /**
       Remove the object the given iterator points to.  The iterator will be updated so that it
       points to the removed object's successor.
       \param it iterator specifying object to be removed
    */
    virtual void removeIt(BidIt& it);

    /**
       Remove the object the given iterator points to, but do not delete the object even if
       the collection owns its objects.
       \return removed object
       \param it iterator specifying object to be removed
    */
    Object* take(BidIt& it);
    //@}

    /// \name Operators
    //@{
    /** add-or-find access operator (returns Object*). */
    Object* operator[](const Object* object)
    {
        return addOrFind(object);
    }

    /** add-or-find access operator (returns Object&). */
    Object&
    operator()(const Object* object)
    {
        return *addOrFind(object);
    }
    //@}

#ifdef DEBUG
    virtual void sanityCheck() const;

    virtual void addOwnedIt(const FwdIt* it) const;

    virtual bool hasOwnedIt(const FwdIt* it) const;

    virtual void removeOwnedIt(const FwdIt* it) const;

    void exciseOwnedIts() const;
#endif

protected:
    enum flg_t
    {
        flg_owner,
        flg_multiSet,
        flg_marked
    };

protected:
    inline int
    compareObjects(const Object* lhs, const Object* rhs) const
    {
        ASSERTD(lhs != nullptr);
        ASSERTD(rhs != nullptr);
        return (_ordering == nullptr) ? lhs->compare(*rhs) : _ordering->cmp(lhs, rhs);
    }

protected:
    size_t _items;

private:
    void init();
    void deInit();

private:
    Ordering* _ordering;
#ifdef DEBUG
    mutable class RBtree* _ownedIts;
    mutable class Mutex _ownedItsLock;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
