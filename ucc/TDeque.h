#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/TDequeIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of Deque.

   TDeque's main purpose is to minimize the need for typecasts.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TDeque : public Deque
{
    UTL_CLASS_DECL_TPL(TDeque, T, Deque);
    UTL_CLASS_DEFID;

public:
    TDeque(bool owner, Ordering* ordering = nullptr)
        : Deque(owner, ordering)
    {
    }

    T*
    front() const
    {
        return utl::cast<T>(Deque::front());
    }

    T*
    back() const
    {
        return utl::cast<T>(Deque::back());
    }

    T*
    get(size_t idx) const
    {
        return utl::cast<T>(Deque::get(idx));
    }

    /** See find(). */
    T*
    findT(const Object& key) const
    {
        return utl::cast<T>(Deque::find(key));
    }

    TDequeIt<T>
    findIt(const Object& key) const
    {
        TDequeIt<T> res;
        res.copy(Deque::findIt(key));
        return res;
    }

    TDequeIt<T>
    findIt(const Object& key)
    {
        TDequeIt<T> res;
        res.copy(Deque::findIt(key));
        return res;
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

    TDequeIt<T>
    begin() const
    {
        TDequeIt<T> res;
        res.copy(Deque::begin());
        return res;
    }

    TDequeIt<T>
    end() const
    {
        TDequeIt<T> res;
        res.copy(Deque::end());
        return res;
    }

    TDequeIt<T>
    begin()
    {
        TDequeIt<T> res;
        res.copy(Deque::begin());
        return res;
    }

    TDequeIt<T>
    end()
    {
        TDequeIt<T> res;
        res.copy(Deque::end());
        return res;
    }

public:
    typedef T type;
    typedef TDequeIt<T> iterator;
    typedef T* value_type; // for STL
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TDeque, T);
