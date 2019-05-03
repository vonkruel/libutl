#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/RandIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Random-access Array iterator.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ArrayIt : public RandIt
{
    UTL_CLASS_DECL(ArrayIt, RandIt);

public:
    /**
       Constructor.
       \param array associated array
       \param offset initial offset
    */
    ArrayIt(const Array* array, size_t offset)
        : _array(const_cast<Array*>(array))
        , _offset(offset)
    {
        IFDEBUG(FwdIt::setOwner(_array));
    }

    /**
       Compare with another ArrayIt.  self and rhs are equal if they iterate over the same array,
       and their offsets are equal.
    */
    virtual int compare(const Object& rhs) const;

    /**
       Copy another ArrayIt.  Make self's array and offset the same as the given ArrayIt.
    */
    virtual void copy(const Object& rhs);

    /**
       Move forward the given number of objects.  \b Note: if there are "holes" in the array,
       this may not be equivalent to adding dist to the current offset!
    */
    virtual void forward(size_t dist = 1);

    virtual Object*
    get() const
    {
        if (_offset >= _array->totalSize())
            return nullptr;
        return _array->get(_offset);
    }

    /**
       Get the object at the given offset.
       \return object at offset (nullptr if none)
       \param offset array offset
    */
    virtual Object*
    get(size_t offset) const
    {
        ASSERTD(!_array->hasHole());
        return _array->get(offset);
    }

    /** Get the associated array. */
    Array*
    getArray() const
    {
        return _array;
    }

    virtual size_t offset() const;

    virtual void reverse(size_t dist = 1);

    virtual void
    seek(size_t offset)
    {
        ASSERTD(!_array->hasHole());
        _offset = offset;
    }

    virtual void set(const Object* object);

    virtual void
    set(size_t offset, const Object* object)
    {
        ASSERTD(!isConst());
        _array->set(offset, object);
    }

    virtual size_t
    size() const
    {
        return _array->size();
    }

    virtual size_t subtract(const RandIt& it) const;

    /**
       Return an ArrayIt equivalent to self, but moved forward the given number of objects.
       \return result iterator
       \param dist number of objects to move forward
    */
    ArrayIt
    operator+(size_t dist) const
    {
        ArrayIt res = *this;
        res.forward(dist);
        return res;
    }

    /**
       Return copy of self moved backward the given number of objects
       \return result
       \param dist number of objects to move backward
    */
    ArrayIt
    operator-(size_t dist) const
    {
        ArrayIt res = *this;
        res.reverse(dist);
        return res;
    }

    /** Return the distance from the given iterator to self. */
    size_t
    operator-(const ArrayIt& it) const
    {
        return subtract(it);
    }

    ArrayIt&
    operator++()
    {
        forward();
        return *this;
    }

    ArrayIt
    operator++(int)
    {
        ArrayIt res = *this;
        forward();
        return res;
    }

    ArrayIt&
    operator--()
    {
        reverse();
        return *this;
    }

    ArrayIt
    operator--(int)
    {
        ArrayIt res = *this;
        reverse();
        return res;
    }

private:
    void
    init()
    {
        _array = nullptr;
        _offset = size_t_max;
    }
    void
    deInit()
    {
    }

private:
    Array* _array;
    size_t _offset;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

RandIt*
Array::beginNew() const
{
    auto it = new base_iterator(this, 0);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RandIt*
Array::beginNew()
{
    return new base_iterator(this, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RandIt*
Array::endNew() const
{
    auto it = new base_iterator(this, size_t_max);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RandIt*
Array::endNew()
{
    return new base_iterator(this, size_t_max);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
