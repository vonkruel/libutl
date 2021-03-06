#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/ListIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of ListIt.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class TListIt : public ListIt
{
    UTL_CLASS_DECL_TPL(TListIt, T, ListIt);
    UTL_CLASS_DEFID;

public:
    T*
    get() const
    {
        return utl::cast<T>(ListIt::get());
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

UTL_CLASS_IMPL_TPL(utl::TListIt, T);
