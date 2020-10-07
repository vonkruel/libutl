#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of Array.

   TArray's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TArray : public Array
{
    UTL_CLASS_DECL_TPL(TArray, T, Array);
    UTL_CLASS_DEFID;

public:
    typedef T type;
    typedef T* value_type; // for STL
    typedef T* const* iterator;

public:
    /**
       Constructor.
       \param size initial size
       \param increment (optional : uint_t_max) growth increment
       \param owner (optional : true) "owner" flag
       \param multiSet (optional : true) "multiSet" flag
       \param keepSorted (optional : false) "keepSorted" flag
       \param ordering (optional) ordering
    */
    TArray(size_t size,
           size_t increment = uint_t_max,
           bool owner = true,
           bool multiSet = true,
           bool keepSorted = false,
           Ordering* ordering = nullptr)
        : Array(size, increment, owner, multiSet, keepSorted, ordering)
    {
    }

    /**
       Constructor.
       \param owner (optional : true) "owner" flag
       \param multiSet (optional : true) "multiSet" flag
       \param keepSorted (optional : false) "keepSorted" flag
       \param ordering (optional) ordering
    */
    TArray(bool owner, bool multiSet = true, bool keepSorted = false, Ordering* ordering = nullptr)
        : Array(owner, multiSet, keepSorted, ordering)
    {
    }

    /// \name Misc. Read-Only
    //@{
    T*
    get(size_t idx) const
    {
        return utl::cast<T>(Array::get(idx));
    }
    //@}

    /// \name Searching
    //@{
    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(Array::find(key));
    }

    iterator
    findIt(const Object& key) const
    {
        return (iterator)Array::findIt(key);
    }

    void
    findIt(const Object& key, BidIt& it) const
    {
        super::findIt(key, it);
    }

    void
    findIt(const Object& key, BidIt& it)
    {
        super::findIt(key, it);
    }

    iterator
    findFirstIt(const Object& key, bool insert = false)
    {
        return (iterator)Array::findFirstIt(key, insert);
    }

    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false) const
    {
        super::findFirstIt(key, it, insert);
    }

    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false)
    {
        super::findFirstIt(key, it, insert);
    }

    iterator
    findLastIt(const Object& key)
    {
        return (iterator)Array::findLastIt(key);
    }

    void
    findLastIt(const Object& key, BidIt& it) const
    {
        super::findLastIt(key, it);
    }

    void
    findLastIt(const Object& key, BidIt& it)
    {
        super::findLastIt(key, it);
    }
    //@}

    /// \name Adding Objects
    //@{
    bool
    add(const T& object)
    {
        return super::add(object);
    }

    bool
    add(const T* object)
    {
        return super::add(object);
    }

    bool
    add(const Object* object, bool keepSorted)
    {
        return super::add(object, keepSorted);
    }

    void
    add(const Collection& collection)
    {
        super::add(collection);
    }

    void
    add(size_t idx, size_t num = 1)
    {
        super::add(idx, num);
    }

    void
    add(size_t idx, const Object& object)
    {
        super::add(idx, object);
    }

    void
    add(size_t idx, const Object* object)
    {
        super::add(idx, object);
    }

    void
    insert(const Object& object, iterator it)
    {
        super::insert(object, (Array::iterator)it);
    }

    void
    insert(const Object* object, iterator it)
    {
        super::insert(object, (Array::iterator)it);
    }

    void
    insert(const Object& object, const BidIt& it)
    {
        super::insert(object, it);
    }

    void
    insert(const Object* object, const BidIt& it)
    {
        super::insert(object, it);
    }
    //@}

    /// \name Removing Objects
    //@{
    void
    removeIt(iterator it)
    {
        super::removeIt((Array::iterator)it);
    }

    void
    removeIt(iterator begin, iterator end)
    {
        super::removeIt((Array::iterator)begin, (Array::iterator)end);
    }

    void
    removeIt(BidIt& it)
    {
        super::removeIt(it);
    }

    void
    removeIt(BidIt& begin, BidIt& end)
    {
        super::removeIt(begin, end);
    }
    //@}

    /// \name Replacing Objects
    //@{
    void
    replace(iterator it, const Object* newObject)
    {
        super::replace((Array::iterator)it, newObject);
    }

    void
    replace(Array::base_iterator& it, const Object* newObject)
    {
        super::replace(it, newObject);
    }

    void
    replace(const Object* oldObject, const Object* newObject)
    {
        super::replace(oldObject, newObject);
    }

    void
    replace(size_t idx, const Object* newObject)
    {
        super::replace(idx, newObject);
    }
    //@}

    /// \name Relocating Objects
    //@{
    void
    relocate(iterator destIt, iterator srcIt)
    {
        super::relocate((Array::iterator)destIt, (Array::iterator)srcIt);
    }

    void
    relocate(size_t destIdx, size_t srcIdx)
    {
        super::relocate(destIdx, srcIdx);
    }
    //@}

    /// \name Iterators
    //@{
    iterator
    begin() const
    {
        return (iterator)Array::begin();
    }

    iterator
    end() const
    {
        return (iterator)Array::end();
    }
    //@}

    /// \name Operators
    //@{
    T* operator[](size_t idx) const
    {
        return utl::cast<T>(Array::operator[](idx));
    }

    T* operator[](int idx) const
    {
        return operator[]((size_t)idx);
    }

    T* operator[](size_t idx)
    {
        return utl::cast<T>(Array::operator[](idx));
    }

    T* operator[](int idx)
    {
        return operator[]((size_t)idx);
    }

    T* operator[](uint_t idx) const
    {
        return operator[]((size_t)idx);
    }

    T* operator[](uint_t idx)
    {
        return operator[]((size_t)idx);
    }

    T&
    operator()(size_t idx) const
    {
        auto ptr = utl::cast<T>(Array::operator[](idx));
        ASSERTD(ptr != nullptr);
        return *ptr;
    }

    T* operator[](const Object* object)
    {
        return utl::cast<T>(addOrFind(object));
    }

    T&
    operator()(int idx) const
    {
        return operator()((size_t)idx);
    }

    T&
    operator()(uint_t idx) const
    {
        return operator()((size_t)idx);
    }
    //@}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TArray, T);
