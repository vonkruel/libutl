#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   List node.

   <b>Attributes</b>

   \arg \b prev : Previous node (= nullptr if head node).
   \arg \b next : Next node (= self if tail node).
   \arg \b object : Contained object (= nullptr if tail node).

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ListNode
{
public:
    /**
       Constructor.
    */
    ListNode()
    {
        init();
    }

    /**
       Constructor.
       \param object contained object
    */
    ListNode(const Object* object)
    {
        init(object);
    }

    /** Add self after the given node. */
    void addAfter(ListNode* node);

    /** Add self before the given node. */
    void addBefore(ListNode* node);

    /** Get the contained object. */
    Object*
    get() const
    {
        return _object;
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
        return (_next == nullptr);
    }

    /** Determine whether self is a "sentinal" tail node. */
    bool
    isSentinelTail() const
    {
        return (_object == &maxObject);
    }

    /** Get the next node (= self if self is tail node). */
    ListNode*
    next() const
    {
        return _next;
    }

    /** Set the next node. */
    void
    setNext(ListNode* node)
    {
        _next = node;
    }

    /** Get the previous node (= nullptr if self is head node). */
    ListNode*
    prev() const
    {
        return _prev;
    }

    /** Set the previous node. */
    void
    setPrev(ListNode* prev)
    {
        _prev = prev;
    }

    /** Remove self from the list. */
    void remove();

    /** Set the contained object. */
    void
    set(const Object* object)
    {
        _object = const_cast<Object*>(object);
    }

private:
    void
    init(const Object* object = nullptr)
    {
        _prev = _next = nullptr;
        _object = const_cast<Object*>(object);
    }

private:
    ListNode *_prev, *_next;
    Object* _object;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
