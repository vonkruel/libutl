#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/RBtree.h>
#include <libutl/RWlock.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Object registry.

   ObjectRegistry uses reference-counting to keep a "registry" of unique objects, and avoid
   duplication of objects that are equivalent.

   If you don't supply an ordering, the objects' "native" ordering will be used via
   utl::Object::compare().  ObjectRegistry will automatically order objects of differing types
   based on their type names, so the provided ordering (or native object ordering) will only be
   invoked when the objects being compared are of the same type.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ObjectRegistry : public Object
{
    UTL_CLASS_DECL(ObjectRegistry, Object);
    UTL_CLASS_DEFID;
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

public:
    typedef RBtree::iterator iterator;

public:
    /** Constructor. */
    ObjectRegistry(Ordering* ordering)
    {
        init();
        setOrdering(ordering);
    }

    virtual size_t innerAllocatedSize() const;

    /** Set the ordering (registry must be empty). */
    void setOrdering(Ordering* ordering);

    /**
       Add a reference to an object.
       \return registry's copy of the object
       \param object (this pointer should be discarded by client)
    */
    Object* add(Object* object);

    /**
       Remove a reference to an object.
       \param object utl::Object* returned by earlier call to add()
    */
    void remove(Object* object);

    /** Access the lock. */
    RWlock&
    lock()
    {
        return _lock;
    }

    /** Get begin iterator. */
    iterator
    begin()
    {
        return _objects.begin();
    }

    /** Get end iterator. */
    iterator
    end()
    {
        return _objects.end();
    }

private:
    RBtree _objects;
    mutable RWlock _lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
