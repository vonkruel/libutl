#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Singly-linked list node.

   <b>Attributes</b>

   \arg \b next : Next node (= nullptr if tail node).

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class SlistNode
{
public:
    /**
       Constructor.
    */
    SlistNode()
    {
        init();
    }

    /**
       Constructor.
       \param object contained object
    */
    SlistNode(const Object* object)
    {
        init(object);
    }

    /** Add self after the given node. */
    void addAfter(SlistNode* node);

    /** Add self before the given node. */
    void addBefore(SlistNode* node);

    /** Get the contained object. */
    Object*
    get() const
    {
        return _object;
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
    SlistNode*
    next() const
    {
        return _next;
    }

    /** Set the next node. */
    void
    setNext(SlistNode* node)
    {
        _next = node;
    }

    /**
       Remove self from the list.
       \return next node in chain
       \param prev previous node
    */
    SlistNode* remove(SlistNode* prev = nullptr);

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
        _next = nullptr;
        _object = const_cast<Object*>(object);
    }

private:
    SlistNode* _next;
    Object* _object;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
