#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   SkipList node.

   You shouldn't need to use this class directly.

   <b>Attributes</b>

   \arg \b prev : Previous node (= nullptr if head node).
   \arg \b next : Next node (= self if tail node).
   \arg \b object : Contained object (= nullptr if tail node).

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SkipListNode
{
    friend class SkipList;

public:
    /** Get the contained object. */
    Object*
    get() const
    {
        return _object;
    }

    /** Set the contained object, update link pointers. */
    void set(const Object* object, uint_t level, SkipListNode** update);

    /** Set the contained object. */
    void
    set(const Object* object)
    {
        _object = const_cast<Object*>(object);
    }

    /** Determine whether self is the head node. */
    bool
    isHead() const
    {
        return (_prev == nullptr);
    }

    /** Determine whether self is the tail node. */
    bool
    isTail() const
    {
        return (_next[0] == nullptr);
    }

    /** Get the next node. */
    SkipListNode*
    next(uint_t idx = 0) const
    {
        return _next[idx];
    }

    /** Get the previous node. */
    SkipListNode*
    prev() const
    {
        return _prev;
    }

    /** Set the next node. */
    void
    setNext(uint_t i, SkipListNode* node)
    {
        _next[i] = node;
    }

    /** Set the previous node. */
    void
    setPrev(SkipListNode* node)
    {
        _prev = node;
    }

    /** Remove self from the list. */
    void remove(uint_t level, SkipListNode** update);

private:
    Object* _object;
    SkipListNode* _prev;
    SkipListNode* _next[1];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
