#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/THashtableIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of Hashtable.

   THashtable's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class THashtable : public Hashtable
{
    UTL_CLASS_DECL_TPL(THashtable, T, Hashtable);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param size size of hash-table
       \param maxPct (optional : 90) maximum percentage full
       \param owner (optional : true) owner flag
       \param multiSet (optional : true) multiSet flag
    */
    THashtable(size_t size, size_t maxPct = 90, bool owner = true, bool multiSet = true);

    /**
       Constructor.
       \param owner owner flag
       \param multiSet multiSet flag
    */
    THashtable(bool owner, bool multiSet = true);

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

    void
    add(const Collection& collection)
    {
        super::add(collection);
    }

    THashtableIt<T>
    begin() const
    {
        THashtableIt<T> res;
        res.copy(Hashtable::begin());
        return res;
    }

    THashtableIt<T>
    end() const
    {
        THashtableIt<T> res;
        res.copy(Hashtable::end());
        return res;
    }

    THashtableIt<T>
    begin()
    {
        THashtableIt<T> res;
        res.copy(Hashtable::begin());
        return res;
    }

    THashtableIt<T>
    end()
    {
        THashtableIt<T> res;
        res.copy(Hashtable::end());
        return res;
    }

    /** See find(). */
    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(Hashtable::find(key));
    }

public:
    typedef T type;
    typedef THashtableIt<T> iterator;
    typedef T* value_type; // for STL
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
THashtable<T>::THashtable(size_t size, size_t maxPct, bool owner, bool multiSet)
    : Hashtable(size, maxPct, owner, multiSet)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
THashtable<T>::THashtable(bool owner, bool multiSet)
    : Hashtable(owner, multiSet)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::THashtable, T);
