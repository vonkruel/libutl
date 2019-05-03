#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SkipListIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of SkipListIt.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> class TSkipListIt : public SkipListIt
{
    UTL_CLASS_DECL_TPL(TSkipListIt, T, SkipListIt);
    UTL_CLASS_DEFID;

public:
    T*
    get() const
    {
        return utl::cast<T>(SkipListIt::get());
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

UTL_CLASS_IMPL_TPL(utl::TSkipListIt, T);
