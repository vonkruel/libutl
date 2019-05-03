#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TSkipListIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of SkipList.

   TSkipList's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class TSkipList : public SkipList
{
    UTL_CLASS_DECL_TPL(TSkipList, T, SkipList);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : true) \b multiSet flag
       \param ordering (optional) ordering
    */
    TSkipList(bool owner, bool multiSet = true, Ordering* ordering = nullptr);

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

    TSkipListIt<T>
    begin() const
    {
        TSkipListIt<T> res;
        res.copy(SkipList::begin());
        return res;
    }

    TSkipListIt<T>
    end() const
    {
        TSkipListIt<T> res;
        res.copy(SkipList::end());
        return res;
    }

    TSkipListIt<T>
    begin()
    {
        TSkipListIt<T> res;
        res.copy(SkipList::begin());
        return res;
    }

    TSkipListIt<T>
    end()
    {
        TSkipListIt<T> res;
        res.copy(SkipList::end());
        return res;
    }

    /** See find(). */
    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(SkipList::find(key));
    }

    TSkipListIt<T>
    findIt(const Object& key) const
    {
        TSkipListIt<T> res;
        res.copy(SkipList::findIt(key));
        return res;
    }

    TSkipListIt<T>
    findIt(const Object& key)
    {
        TSkipListIt<T> res;
        res.copy(SkipList::findIt(key));
        return res;
    }

    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false) const
    {
        super::findFirstIt(key, it);
    }

    virtual void
    findFirstIt(const Object& key, BidIt& it, bool insert = false)
    {
        super::findFirstIt(key, it, insert);
    }

    TSkipListIt<T>
    findFirstIt(const Object& key, bool insert = false) const
    {
        TSkipListIt<T> res;
        res.copy(SkipList::findFirstIt(key, insert));
        return res;
    }

    TSkipListIt<T>
    findFirstIt(const Object& key, bool insert = false)
    {
        TSkipListIt<T> res;
        res.copy(SkipList::findFirstIt(key, insert));
        return res;
    }

    void
    findLastIt(const Object& key, BidIt& it) const
    {
        super::findLastIt(key, it);
    }

    virtual void
    findLastIt(const Object& key, BidIt& it)
    {
        super::findLastIt(key, it);
    }

    TSkipListIt<T>
    findLastIt(const Object& key) const
    {
        TSkipListIt<T> res;
        res.copy(SkipList::findLastIt(key));
        return res;
    }

    TSkipListIt<T>
    findLastIt(const Object& key)
    {
        TSkipListIt<T> res;
        res.copy(SkipList::findLastIt(key));
        return res;
    }

    bool
    remove(const Object& key)
    {
        return super::remove(key);
    }

public:
    typedef T type;
    typedef TSkipListIt<T> iterator;
    typedef T* value_type; // for STL
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
TSkipList<T>::TSkipList(bool owner, bool multiSet, Ordering* ordering)
    : SkipList(owner, multiSet, ordering)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TSkipList, T);
