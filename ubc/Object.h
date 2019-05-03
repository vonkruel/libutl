#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Stream;
class String;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Root of UTL++ class hierarchy.

   Object provides a standard interface for functionality that is
   very commonly required.  Most notably, Object defines virtual
   methods for functionality such as:

   \arg copying
   \arg comparison
   \arg serialization
   \arg dumping (e.g. for debugging purposes)
   \arg run-time type identification (RTTI)

   When you use the \ref UTL_CLASS_DECL and \ref UTL_CLASS_IMPL macros (macros.h) with your
   classes, the following methods are implemented for you:

   \arg default constructor - calls init()
   \arg copy constructor - calls init(), then copy()
   \arg virtual destructor - calls deInit()
   \arg clone - create copy of self (via copy())
   \arg create - create new instance of same class
   \arg operator= - copy another object (calls copy())
   \arg pointer conversion - conversion to pointer
   \arg const pointer conversion - conversion to const pointer
   \arg getClass (virtual) - get self's class
   \arg getClassName (virtual) - get the name of self's class
   \arg getThisClass - get self's class
   \arg getThisClassName - get the name of self's class
   \arg getBaseClass - get the base class
   \arg getBaseClassName - get the name of the base class

   \author Adam McKee
   \ingroup abstract
   \see RunTimeClass
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Object
{
    UTL_CLASS_DECL_ABC(Object, Object);
    UTL_CLASS_DEFID;

public:
    /** Revert to initial state. */
    void clear();

    /**
       Compare with another object.  If no overridden version succeeds in doing the comparison,
       then an attempt will be made to re-start the comparison process using one or both of the
       objects' keys.  Usually, an override of compare() should call the superclass's compare()
       if it doesn't know how to compare itself with the rhs object.
       \see getKey
       \return < 0 if self < rhs, 0 if self = rhs, > 0 if self > rhs
       \param rhs object to compare with
    */
    virtual int compare(const Object& rhs) const;

    /**
       Copy another instance.  When you override copy(), you should usually call the superclass's
       copy().
       \param rhs object to copy
    */
    virtual void copy(const Object& rhs);

    /**
       Make an exact copy of another instance.  The default implementation just calls copy(),
       so you have to override this if you want different behavior for copy construction
       and cloning.
    */
    virtual void vclone(const Object& rhs);

    /**
       "Steal" the internal representation from another instance.  The default implementation
       just calls vclone(), so you have to override this if you want a "move" capability.
    */
    virtual void steal(Object& rhs);

    /**
       Dump a human-readable representation of self to the given output stream.  A common use of
       this is for debugging & testing.
       \param os stream to dump to
       \param level level of 'verbosity' for the dump
    */
    virtual void dump(Stream& os, uint_t level = uint_t_max) const;

    /**
       Front-end for dump() that prints the object's class name.
       \param os stream to dump to
       \param indent (optional : 4) indent the object?
       \param level level of 'verbosity' for the dump
    */
    void dumpWithClassName(Stream& os, uint_t indent = 4, uint_t level = uint_t_max) const;

    /**
       Get the key for this object.  The key for an object is used to re-start the comparison
       process when all overridden compare() have failed.  Usually an object's key uniquely
       identifies the object, though it doesn't necessarily need to.  By default, a class has no
       associated key.
       \see compare hasKey
       \return object's key (= self if the object has no key)
    */
    virtual const Object& getKey() const;

    /**
       Determine whether or not the object has a key.
       \see compare getKey
       \return true iff the object has a key.
    */
    bool
    hasKey() const
    {
        return (&(getKey()) != this);
    }

    /** Get the proxied object (= self if none). */
    virtual const Object& getProxiedObject() const;

    /** Get the proxied object (= self if none). */
    virtual Object& getProxiedObject();

    /**
       Get the hash code for the object.  The default implementation will return the hash code of
       the object's key, or abort if the object has none.
       \return the hash code
       \param size hash table size
    */
    virtual size_t hash(size_t size) const;

    /**
       Determine whether self's class is a descendent of the given class.  The macro isA provides
       a more convenient usage.
       \see RunTimeClass
       \return true iff self's class is a descendent of the given class
       \param runTimeClass class we are comparing with
    */
    bool
    _isA(const RunTimeClass* runTimeClass) const
    {
        return getClass()->_isA(runTimeClass);
    }

    /// \name Serialization
    //@{
    /**
       Serialize from an input stream.
       \param is input stream to serialize from
       \param mode see utl::serialize_t
    */
    void
    serializeIn(Stream& is, uint_t mode = ser_default)
    {
        serialize(is, io_rd, mode);
    }

    /**
       Serialize to an output stream.
       \param os output stream to serialize to
       \param mode see utl::serialize_t
    */
    void
    serializeOut(Stream& os, uint_t mode = ser_default) const
    {
        const_cast_this->serialize(os, io_wr, mode);
    }

    /**
       Serialize to or from a stream.  This is the \b only virtual method for serialization.
       You must override this in any class that has data to be serialized, and ensure that the
       superclass's serialize() gets called.
       \param stream stream to serialize from/to
       \param io see utl::io_t
       \param mode see utl::serialize_t
    */
    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /**
       Serialize a nullptr-able object from an input stream.
       \see serializeInBoxed
       \return newly created/serialized object (nullptr if none)
       \param is input stream to serialize from
       \param mode see utl::serialize_t
    */
    static Object* serializeInNullable(Stream& is, uint_t mode = ser_default);

    /**
       Serialize a nullptr-able object to an output stream.
       \see serializeOutBoxed
       \param object object to serialize (can be nullptr)
       \param os output stream to serialize to
       \param mode see utl::serialize_t
    */
    static void serializeOutNullable(const Object* object, Stream& os, uint_t mode = ser_default);

    /**
       Serialize a nullptr-able object to or from a stream.
       \see serializeBoxed
       \param object object to serialize (can be nullptr)
       \param stream stream to serialize from/to
       \param io see utl::io_t
       \param mode see utl::serialize_t
    */
    static void
    serializeNullable(Object*& object, Stream& stream, uint_t io, uint_t mode = ser_default);

    /**
       Serialize a boxed object from an input stream.  Create a new object from scratch, given the
       classname and serialized data read from the input stream.
       \return newly created/serialized object (nullptr if none)
       \param is input stream to serialize from
       \param mode see utl::serialize_t
    */
    static Object* serializeInBoxed(Stream& is, uint_t mode = ser_default);

    /**
       Serialize a boxed object to an output stream.  The object's classname is written to the
       stream first, so that serializeInBoxed() will be able to re-create the object from scratch.
       \param os stream to serialize to
       \param mode see utl::serialize_t
    */
    void serializeOutBoxed(Stream& os, uint_t mode = ser_default) const;

    /**
       Serialize a boxed object to or from a stream.
       \param object object to be serialized
       \param stream stream to serialize from/to
       \param io see utl::io_t
       \param mode see utl::serialize_t
    */
    static void
    serializeBoxed(Object*& object, Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        if (io == io_rd)
            object = serializeInBoxed(stream, mode);
        else
            object->serializeOutBoxed(stream, mode);
    }
    //@}

    /** Return a string representation of self. */
    virtual String toString() const;

    /** Conversion to String. */
    operator String() const;

    /** Get the total allocated size of this object. */
    size_t allocatedSize() const;

    /** Get the "inner" allocated size. */
    virtual size_t innerAllocatedSize() const;

    /// \name Comparison Operators
    //@{
    /** Less-than operator. */
    bool
    operator<(const Object& rhs) const
    {
        return (compare(rhs) < 0);
    }
    /** Less-than-or-equal-to operator. */
    bool
    operator<=(const Object& rhs) const
    {
        return (compare(rhs) <= 0);
    }
    /** Greater-than operator. */
    bool
    operator>(const Object& rhs) const
    {
        return (compare(rhs) > 0);
    }
    /** Greater-than-or-equal-to operator. */
    bool
    operator>=(const Object& rhs) const
    {
        return (compare(rhs) >= 0);
    }
    /** Equal-to operator. */
    bool
    operator==(const Object& rhs) const
    {
        return (compare(rhs) == 0);
    }
    /** Unequal-to operator. */
    bool
    operator!=(const Object& rhs) const
    {
        return (compare(rhs) != 0);
    }
    //@}

#ifdef DEBUG
    /** Notify self that it owns the given iterator. */
    virtual void addOwnedIt(const class FwdIt* it) const;

    /** Notify self that the given owned iterator has been destroyed. */
    virtual void removeOwnedIt(const class FwdIt* it) const;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
