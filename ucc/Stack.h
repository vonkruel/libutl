#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   LIFO (last-in, first-out) data structure.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object> class Stack : public TArray<T>
{
    UTL_CLASS_DECL_TPL(Stack, T, TArray<T>);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param owner \b owner flag
    */
    Stack(bool owner)
        : TArray<T>(owner)
    {
    }

    /**
       Determine what object is on top of the stack.
    */
    T* top() const;

    /**
       Pop an object off the stack.
       \return object (nullptr if empty())
    */
    T* pop();

    /**
       Push an object onto the stack.  If isOwner(), a copy of the object will be made.
       \return true if object pushed onto stack, false otherwise
       \param object object to push onto stack
    */
    bool
    push(const T& object)
    {
        return Array::add(object);
    }

    /**
       Push an object onto the stack.
       \return true if object pushed onto stack, false otherwise
       \param object object to push onto stack
    */
    void
    push(const T* object)
    {
        Array::add(object);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T*
Stack<T>::top() const
{
    if (this->_items == 0)
        return nullptr;
    auto res = utl::cast<T>(this->last());
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T*
Stack<T>::pop()
{
    if (this->_items == 0)
        return nullptr;
    auto res = utl::cast<T>(this->last());
    FlagGuard fgo(this, Collection::flg_owner, false);
    super::remove(this->_items - 1);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::Stack, T);
