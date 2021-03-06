#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/DequeIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of DequeIt.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TDequeIt : public DequeIt
{
    UTL_CLASS_DECL_TPL(TDequeIt, T, DequeIt);
    UTL_CLASS_DEFID;

public:
    T*
    get() const
    {
        return utl::cast<T>(DequeIt::get());
    }

    T* operator*() const
    {
        return get();
    }

public:
    // for STL
    typedef T* value_type;
    typedef T*& reference;
    typedef T** pointer;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TDequeIt, T);
