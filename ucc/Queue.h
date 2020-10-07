#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Deque.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   FIFO (first-in, first-out) data structure.

   \author Adam McKee
   \ingroup collection
   \see List
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object>
class Queue : public TDeque<T>
{
    UTL_CLASS_DECL_TPL(Queue, T, TDeque<T>);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param owner \b owner flag
    */
    Queue(bool owner)
        : TDeque<T>(owner)
    {
    }

    /** Return the object at the head of the queue. */
    T*
    deQ()
    {
        return utl::cast<T>(Deque::popFront());
    }

    /**
       Add the given object to the end of the queue.
       If isOwner(), a copy of the object will be made.
       \return true if object successfully added, false otherwise
       \param object object to add
    */
    bool
    enQ(const T& object)
    {
        return Deque::pushBack(const_cast<T&>(object));
    }

    /**
       Add the given object to the end of the queue.
       \return true if object successfully added, false otherwise
    */
    bool
    enQ(const T* object)
    {
        return Deque::pushBack(const_cast<T*>(object));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::Queue, T);
