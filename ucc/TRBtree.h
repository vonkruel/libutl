#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TRBtreeIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of RBtree.

   TRBtree's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TRBtree : public RBtree
{
    UTL_CLASS_DECL_TPL(TRBtree, T, RBtree);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param owner owner flag
       \param multiSet (optional : false) multiSet flag
       \param ordering (optional) ordering
    */
    TRBtree(bool owner, bool multiSet = false, Ordering* ordering = nullptr);

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

    TRBtreeIt<T>
    begin() const
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::begin());
        return res;
    }

    TRBtreeIt<T>
    end() const
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::end());
        return res;
    }

    TRBtreeIt<T>
    begin()
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::begin());
        return res;
    }

    TRBtreeIt<T>
    end()
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::end());
        return res;
    }

    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(RBtree::find(key));
    }

    TRBtreeIt<T>
    findIt(const Object& key) const
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::findIt(key));
        return res;
    }

    TRBtreeIt<T>
    findIt(const Object& key)
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::findIt(key));
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

    TRBtreeIt<T>
    findFirstIt(const Object& key, bool insert = false) const
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::findFirstIt(key, insert));
        return res;
    }

    TRBtreeIt<T>
    findFirstIt(const Object& key, bool insert = false)
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::findFirstIt(key, insert));
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

    TRBtreeIt<T>
    findLastIt(const Object& key) const
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::findLastIt(key));
        return res;
    }

    TRBtreeIt<T>
    findLastIt(const Object& key)
    {
        TRBtreeIt<T> res;
        res.copy(RBtree::findLastIt(key));
        return res;
    }

    bool
    remove(const Object& key)
    {
        return super::remove(key);
    }

public:
    typedef T type;
    typedef TRBtreeIt<T> iterator;
    typedef T* value_type; // for STL
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
TRBtree<T>::TRBtree(bool owner, bool multiSet, Ordering* ordering)
    : RBtree(owner, multiSet, ordering)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TRBtree, T);
