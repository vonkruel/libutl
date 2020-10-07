#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TListIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of List.

   TList's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TList : public List
{
    UTL_CLASS_DECL_TPL(TList, T, List);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : true) \b multiSet flag
       \param keepSorted (optional : false) \b keepSorted flag
       \param ordering (optional) ordering
    */
    TList(bool owner, bool multiSet = true, bool keepSorted = false, Ordering* ordering = nullptr);

    T*
    front() const
    {
        return utl::cast<T>(List::front());
    }

    T*
    back() const
    {
        return utl::cast<T>(List::back());
    }

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

    /** See find(). */
    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(List::find(key));
    }

    TListIt<T>
    findIt(const Object& key) const
    {
        TListIt<T> res;
        res.copy(List::findIt(key));
        return res;
    }

    TListIt<T>
    findIt(const Object& key)
    {
        TListIt<T> res;
        res.copy(List::findIt(key));
        return res;
    }

    void
    findFirstIt(const Object& key, BidIt& it, bool insert = false) const
    {
        super::findFirstIt(key, it, insert);
    }

    virtual void
    findFirstIt(const Object& key, BidIt& it, bool insert = false)
    {
        super::findFirstIt(key, it, insert);
    }

    TListIt<T>
    findFirstIt(const Object& key, bool insert = false) const
    {
        TListIt<T> res;
        res.copy(List::findFirstIt(key, insert));
        return res;
    }

    TListIt<T>
    findFirstIt(const Object& key, bool insert = false)
    {
        TListIt<T> res;
        res.copy(List::findFirstIt(key, insert));
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

    TListIt<T>
    findLastIt(const Object& key) const
    {
        TListIt<T> res;
        res.copy(List::findLastIt(key));
        return res;
    }

    TListIt<T>
    findLastIt(const Object& key)
    {
        TListIt<T> res;
        res.copy(List::findLastIt(key));
        return res;
    }

    T*
    popFront()
    {
        return utl::cast<T>(List::popFront());
    }

    T*
    popBack()
    {
        return utl::cast<T>(List::popBack());
    }

    TListIt<T>
    begin() const
    {
        TListIt<T> res;
        res.copy(List::begin());
        return res;
    }

    TListIt<T>
    end() const
    {
        TListIt<T> res;
        res.copy(List::end());
        return res;
    }

    TListIt<T>
    begin()
    {
        TListIt<T> res;
        res.copy(List::begin());
        return res;
    }

    TListIt<T>
    end()
    {
        TListIt<T> res;
        res.copy(List::end());
        return res;
    }

public:
    typedef T type;
    typedef TListIt<T> iterator;
    typedef T* value_type; // for STL
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
TList<T>::TList(bool owner, bool multiSet, bool keepSorted, Ordering* ordering)
    : List(owner, multiSet, keepSorted, ordering)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TList, T);
