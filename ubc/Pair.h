#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

/**
   Simple container for two objects.

   Pair represents a pair of objects.  The first object is designated the key (see getKey()).
   Pair provides a useful way of associating two objects without writing a special class.  When
   used in conjunction with a container class, Pair can be used to provide a mapping from one set
   of objects to another.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Pair
    : public Object
    , protected FlagsMI
{
    UTL_CLASS_DECL(Pair, Object);

public:
    /**
       Constructor.
       \param first first object
       \param second second object
       \param firstOwner \b owner flag for first object (make a copy if true)
       \param secondOwner \b owner flag for second object (make a copy if true)
    */
    Pair(const Object& first,
         const Object& second,
         bool firstOwner = true,
         bool secondOwner = true);

    /**
       Constructor.
       \param first first object
       \param second second object
       \param firstOwner \b owner flag for first object
       \param secondOwner \b owner flag for second object
    */
    Pair(const Object* first,
         const Object* second,
         bool firstOwner = true,
         bool secondOwner = true);

    /** Disassociate both objects. */
    void
    clear()
    {
        setFirst(nullptr);
        setSecond(nullptr);
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    virtual void vclone(const Object& rhs);

    /** Get the key (the first object). */
    virtual const Object& getKey() const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual String toString() const;

    virtual size_t innerAllocatedSize() const;

    /** Return the first object. */
    Object*
    first() const
    {
        return _first;
    }

    /** Return the second object. */
    Object*
    second() const
    {
        return _second;
    }

    /** Return the first object. */
    Object*
    getFirst() const
    {
        return _first;
    }

    /** Return the second object. */
    Object*
    getSecond() const
    {
        return _second;
    }

    /** Set the first object. */
    void setFirst(const Object* first);

    /** Set the second object. */
    void setSecond(const Object* second);

    /** Set the first object (make a copy if isFirstOwner()). */
    void
    setFirst(const Object& first)
    {
        if (isFirstOwner())
            setFirst(first.clone());
        else
            setFirst(&first);
    }

    /** Set the second object (make a copy if isSecondOwner()). */
    void
    setSecond(const Object& second)
    {
        if (isSecondOwner())
            setSecond(second.clone());
        else
            setSecond(&second);
    }

    /** Return the \b owner flag for the first object. */
    bool
    isFirstOwner() const
    {
        return getFlag(flg_firstOwner);
    }

    /** Return the \b owner flag for the second object. */
    bool
    isSecondOwner() const
    {
        return getFlag(flg_secondOwner);
    }

    /** Set the \b owner flag for the first object. */
    void
    setFirstOwner(bool firstOwner)
    {
        setFlag(flg_firstOwner, firstOwner);
    }

    /** Set the \b owner flag for the second object. */
    void
    setSecondOwner(bool secondOwner)
    {
        setFlag(flg_secondOwner, secondOwner);
    }

private:
    enum flg_t
    {
        flg_firstOwner,
        flg_secondOwner
    };

private:
    void
    init()
    {
        _first = _second = nullptr;
        setFirstOwner(true);
        setSecondOwner(true);
    }
    void
    deInit()
    {
        clear();
    }

private:
    Object* _first;
    Object* _second;
    byte_t _flags;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
