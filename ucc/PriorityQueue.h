#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Heap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Priority queue.

   \author Adam McKee
   \ingroup collection
   \see Heap
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object>
class PriorityQueue : public THeap<T>
{
    UTL_CLASS_DECL_TPL(PriorityQueue, T, THeap<T>);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param size heap size
       \param increment (optional : uint_t_max) growth increment
       \param owner (optional : true) \b owner flag
       \param multiSet (optional : true) \b multiSet flag
       \param ordering (optional) ordering
    */
    PriorityQueue(uint_t size,
                  uint_t increment = uint_t_max,
                  bool owner = true,
                  bool multiSet = true,
                  Ordering* ordering = nullptr);

    /**
       Constructor.
       \param owner \b owner flag
       \param multiSet (optional : true) \b multiSet flag
    */
    PriorityQueue(bool owner, bool multiSet = true);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
PriorityQueue<T>::PriorityQueue(
    uint_t size, uint_t increment, bool owner, bool multiSet, Ordering* ordering)
    : THeap<T>(size, increment, owner, multiSet, ordering)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
PriorityQueue<T>::PriorityQueue(bool owner, bool multiSet)
    : THeap<T>(owner, multiSet)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::PriorityQueue, T);
