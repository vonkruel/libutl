#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Collection.h>
#include <libutl/AutoPtr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Templated proxy for Collection.

   Inherit from TCollection when you want your class to provide
   a Collection interface without exposing the actual concrete collection
   type to the client.  This allows you to change to a new collection class
   (even at run-time) without breaking any clients.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object>
class TCollection : public Object
{
    UTL_CLASS_DECL_TPL(TCollection, T, Object);

public:
    /** Constructor. */
    TCollection(Collection* col)
    {
        _col = nullptr;
        setCollection(col);
    }

    virtual int
    compare(const Object& rhs) const
    {
        auto rhsCol = findRHS(rhs);
        if (_col == rhsCol)
            return 0;
        if (rhsCol == nullptr)
            return 1;
        if (_col == nullptr)
            return -1;
        return _col->compare(*rhsCol);
    }

    virtual void
    copy(const Object& rhs)
    {
        auto rhsCol = findRHS(rhs);
        if (_col == rhsCol)
            return;
        setCollection(nullptr);
        if (rhsCol == nullptr)
            return;
        _col = rhsCol->clone();
    }

    void
    assertOwner()
    {
        col()->assertOwner();
    }

    virtual void
    dump(Stream& os, uint_t level = uint_t_max) const
    {
        col()->dump(os, level);
    }

    virtual void
    serialize(Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        col()->serialize(stream, io, mode);
    }

    void
    serialize(const RunTimeClass* rtc, Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        col()->serialize(rtc, stream, io, mode);
    }

    bool
    update(const Object* object)
    {
        return col()->update(object);
    }

    bool
    isOwner() const
    {
        return col()->isOwner();
    }

    void
    setOwner(bool owner)
    {
        col()->setOwner(owner);
    }

    const Ordering*
    ordering() const
    {
        return col()->ordering();
    }

    Ordering*
    ordering()
    {
        return col()->ordering();
    }

    void
    setOrdering(const Ordering& ordering, uint_t algorithm = sort_quickSort)
    {
        col()->setOrdering(ordering, algorithm);
    }

    void
    setOrdering(Ordering* ordering, uint_t algorithm = sort_quickSort)
    {
        col()->setOrdering(ordering, algorithm);
    }

    bool
    empty() const
    {
        return col()->empty();
    }

    bool
    isMultiSet() const
    {
        return col()->isMultiSet();
    }

    void
    setMultiSet(bool multiSet)
    {
        col()->setMultiSet(multiSet);
    }

    size_t
    items() const
    {
        return col()->items();
    }

    size_t
    size() const
    {
        return col()->size();
    }

    bool
    add(const Object& object)
    {
        return col()->add(object);
    }

    bool
    add(const Object* object)
    {
        return col()->add(object);
    }

    void
    add(const Collection& collection)
    {
        col()->add(collection);
    }

    TBidIt<T>
    addIt(const Object* object)
    {
        return TBidIt<T>(col()->addIt(object));
    }

    void
    addOrUpdate(const Object& object)
    {
        col()->addOrUpdate(object);
    }

    void
    addOrUpdate(const Object* object)
    {
        col()->addOrUpdate(object);
    }

    TCollection<T>&
    copyItems(const Collection* src, const Predicate* pred = nullptr, bool predVal = true)
    {
        col()->copyItems(src, pred, predVal);
        return self;
    }

    TCollection<T>&
    copyItems(const ListNode* src, const Predicate* pred = nullptr, bool predVal = true)
    {
        col()->copyItems(src, pred, predVal);
        return self;
    }

    TCollection<T>&
    copyItems(const SlistNode* src, const Predicate* pred = nullptr, bool predVal = true)
    {
        col()->copyItems(src, pred, predVal);
        return self;
    }

    TCollection<T>&
    operator+=(const Object& rhs)
    {
        col()->operator+=(rhs);
        return self;
    }

    TCollection<T>&
    operator+=(const Object* rhs)
    {
        col()->operator+=(rhs);
        return self;
    }

    TCollection<T>&
    operator+=(const Collection& rhs)
    {
        col()->operator+=(rhs);
        return self;
    }

    TCollection<T>&
    operator-=(const Object& rhs)
    {
        col()->operator-=(rhs);
        return self;
    }

    void
    dump(Stream& os,
         bool key,
         bool printClassName,
         uint_t indent,
         const char* separator,
         uint_t level) const
    {
        col()->dump(os, level, key, printClassName, indent, separator);
    }

    virtual String
    toString() const
    {
        return col()->toString();
    }

    String
    toString(bool key) const
    {
        return col()->toString(key);
    }

    String
    toString(const char* sep, bool key = false) const
    {
        return col()->toString(sep, key);
    }

    String
    toString(const String& sep, bool key = false) const
    {
        return col()->toString(sep, key);
    }

    TBidIt<T>
    begin() const
    {
        return TBidIt<T>(col()->beginNew());
    }

    TBidIt<T>
    begin()
    {
        return TBidIt<T>(col()->beginNew());
    }

    BidIt*
    beginNew() const
    {
        return col()->beginNew();
    }

    BidIt*
    beginNew()
    {
        return col()->beginNew();
    }

    BidIt*
    createIt() const
    {
        return col()->createIt();
    }

    BidIt*
    createIt()
    {
        return col()->createIt();
    }

    TBidIt<T>
    end() const
    {
        return TBidIt<T>(col()->endNew());
    }

    TBidIt<T>
    end()
    {
        return TBidIt<T>(col()->endNew());
    }

    BidIt*
    endNew() const
    {
        return col()->endNew();
    }

    BidIt*
    endNew()
    {
        return col()->endNew();
    }

    bool
    contains(const Object* key) const
    {
        return col()->contains(key);
    }

    bool
    contains(const Object& key) const
    {
        return col()->contains(key);
    }

    size_t
    count(const Predicate* pred = nullptr, bool predVal = true) const
    {
        return col()->count(pred, predVal);
    }

    T*
    find(const Object* key) const
    {
        return utl::cast<T>(col()->find(key));
    }

    T*
    find(const Object& key) const
    {
        return utl::cast<T>(col()->find(key));
    }

    void
    findIt(const Object& key, BidIt& it) const
    {
        return col()->findIt(key, it);
    }

    void
    findIt(const Object& key, BidIt& it)
    {
        return col()->findIt(key, it);
    }

    T*
    first() const
    {
        return utl::cast<T>(col()->first());
    }

    bool
    has(const Object* key) const
    {
        return col()->has(key);
    }

    bool
    has(const Object& key) const
    {
        return col()->has(key);
    }

    T*
    last() const
    {
        return utl::cast<T>(col()->last());
    }

    void
    clear()
    {
        col()->clear();
    }

    bool
    remove(const Object* key)
    {
        ASSERTD(key != nullptr);
        return remove(*key);
    }

    virtual bool
    remove(const Object& key)
    {
        return col()->remove(key);
    }

    virtual void
    removeIt(BidIt& it)
    {
        col()->removeIt(it);
    }

    T*
    take(BidIt& it)
    {
        return (T*)(col()->take(it));
    }

    operator Collection&()
    {
        return *col();
    }

    operator const Collection&() const
    {
        return *col();
    }

    operator Collection*()
    {
        return _col;
    }

    operator const Collection*() const
    {
        return _col;
    }

public:
    typedef T type;
    typedef TBidIt<T> iterator;
    typedef T* value_type; // for STL
protected:
    const Collection*
    col() const
    {
        ASSERTD(_col != nullptr);
        return _col;
    }

    Collection*
    col()
    {
        ASSERTD(_col != nullptr);
        return _col;
    }

    void
    setCollection(Collection* col)
    {
        if (col == _col)
            return;
        delete _col;
        _col = col;
    }

    static const Collection*
    findRHS(const Object& rhs)
    {
        if (rhs.isA(TCollection<T>))
        {
            const TCollection<T>& rhsTCol = (const TCollection<T>&)rhs;
            return rhsTCol._col;
        }
        else
        {
            return utl::cast<Collection>(&rhs);
        }
    }

private:
    void
    init()
    {
        _col = nullptr;
    }
    void
    deInit()
    {
        delete _col;
    }

private:
    Collection* _col;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TCollection, T);
