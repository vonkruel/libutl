#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FlagsMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Vector;
class BitArray;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Ordering ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Object comparison abstraction.

   \author Adam McKee
   \ingroup abstract
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Ordering : public Object
{
    UTL_CLASS_DECL_ABC(Ordering, Object);
    UTL_CLASS_DEFID;

public:
    /**
       Compare two objects.
       \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
       \param lhs left-hand-side object
       \param rhs right-hand-side object
    */
    virtual int cmp(const Object* lhs, const Object* rhs) const = 0;

    virtual const Object& getObjectKey(const Object* object) const;

    /** An alternative to utl::cmp. */
    int
    operator()(const Object* lhs, const Object* rhs) const
    {
        return cmp(lhs, rhs);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// NaturalOrdering /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Natural object ordering.

   \see Object::compare
   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class NaturalOrdering : public Ordering
{
    UTL_CLASS_DECL(NaturalOrdering, Ordering);
    UTL_CLASS_DEFID;

public:
    virtual int cmp(const Object* lhs, const Object* rhs) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global instance of NaturalOrdering.
   \ingroup general
*/
extern const NaturalOrdering naturalOrdering;

////////////////////////////////////////////////////////////////////////////////////////////////////
// InvertedNaturalOrdering /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Inverted natural object ordering.

   \see Object::compare
   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class InvertedNaturalOrdering : public Ordering
{
    UTL_CLASS_DECL(InvertedNaturalOrdering, Ordering);
    UTL_CLASS_DEFID;

public:
    /**
       Invert the natural ordering of the two objects.
    */
    virtual int cmp(const Object* lhs, const Object* rhs) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global instance of InvertedNaturalOrdering.
   \ingroup general
*/
extern const InvertedNaturalOrdering invertedNaturalOrdering;

////////////////////////////////////////////////////////////////////////////////////////////////////
// KeyOrdering /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Key-based object ordering.

   \see Object::getKey
   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class KeyOrdering : public Ordering
{
    UTL_CLASS_DECL(KeyOrdering, Ordering);
    UTL_CLASS_DEFID;

public:
    virtual int cmp(const Object* lhs, const Object* rhs) const;

    virtual const Object& getObjectKey(const Object* object) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global instance of KeyOrdering.
   \ingroup general
*/
extern const KeyOrdering keyOrdering;

////////////////////////////////////////////////////////////////////////////////////////////////////
// InvertedKeyOrdering /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Inverted key-based object ordering.

   \see Object::getKey
   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class InvertedKeyOrdering : public Ordering
{
    UTL_CLASS_DECL(InvertedKeyOrdering, Ordering);
    UTL_CLASS_DEFID;

public:
    virtual int cmp(const Object* lhs, const Object* rhs) const;

    virtual const Object& getObjectKey(const Object* object) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global instance of KeyOrdering.
   \ingroup general
*/
extern const InvertedKeyOrdering invertedKeyOrdering;

////////////////////////////////////////////////////////////////////////////////////////////////////
// TypeOrdering ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Order objects by type name first, and use the given ordering to compare objects of the same type.

   \see Object::compare
   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class TypeOrdering : public Ordering
{
    UTL_CLASS_DECL(TypeOrdering, Ordering);

public:
    TypeOrdering(Ordering* ordering)
        : _ordering(ordering)
    {
    }

    virtual int cmp(const Object* lhs, const Object* rhs) const;

private:
    void
    init()
    {
        _ordering = nullptr;
    }
    void
    deInit()
    {
        delete _ordering;
    }

private:
    Ordering* _ordering;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global instance of TypeOrdering.
   \ingroup general
*/
extern const TypeOrdering typeOrdering;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Address-based object ordering.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class AddressOrdering : public Ordering
{
    UTL_CLASS_DECL(AddressOrdering, Ordering);
    UTL_CLASS_DEFID;

public:
    virtual int cmp(const Object* lhs, const Object* rhs) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global instance of AddressOrdering.
   \ingroup general
*/
extern const AddressOrdering addressOrdering;

////////////////////////////////////////////////////////////////////////////////////////////////////
// MultiKeyOrdering ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstract base for multi-key orderings.

   MultiKeyOrdering maintains an array of field ids, and ascending/descending flags for each
   comparison key.  It really only performs a simple book-keeping function.

   A derived class will override cmp() to look something like this:

   \code
   int res = 0;
   size_t numKeys = getNumKeys();
   size_t i;
   for (i = 0; i < numKeys; i++)
   {
       switch (getKeyId(i))
       {
           .
           .            // compare the keys of the two objects
           .
       }
       if (res != 0)
       {
           if (isDescending(i))
           {
               res = -res;
           }
           break;
       }
   }

   if (isDescending())
   {
       res = -res;
   }

   return res;
   \endcode

   \author Adam McKee
   \ingroup abstract
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class MultiKeyOrdering
    : public Ordering
    , protected FlagsMI
{
    UTL_CLASS_DECL_ABC(MultiKeyOrdering, Ordering);

public:
    virtual void copy(const Object& rhs);
    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /**
       Add a comparison key.
       \param fieldId field id
       \param ascending (optional : true) ascending order?
    */
    void addKey(uint_t fieldId, bool ascending = true);

    /** Clear all keys. */
    void
    clear()
    {
        _numKeys = 0;
    }

    /** Get the number of keys. */
    size_t
    numKeys() const
    {
        return _numKeys;
    }

    /** Get the key id for the given sort key. */
    uint_t keyId(uint_t idx) const;

    bool
    isAscending() const
    {
        return _ascending;
    }

    bool
    isDescending() const
    {
        return !_ascending;
    }

    /** Get the \b ascending flag for the given sort key. */
    bool isAscending(uint_t idx) const;

    /** Get the negation of the \b ascending flag for the given sort key. */
    bool
    isDescending(uint_t idx) const
    {
        return !isAscending(idx);
    }

private:
    void init();
    void deInit();
    size_t _numKeys;
    Vector<uint_t>* _fieldIds;
    BitArray* _ascendingFlags;
    bool _ascending;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
