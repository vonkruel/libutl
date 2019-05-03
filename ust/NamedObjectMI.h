#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Named object mix-in.

   Mix-in for classes that have a name.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class NamedObjectMI
{
public:
    /**
       Constructor.
    */
    NamedObjectMI()
    {
    }

    /**
       Constructor.
       \param name name of object
    */
    NamedObjectMI(const String& name)
    {
        setName(name);
    }

    /** Clear the name. */
    void
    clear()
    {
        _name.clear();
    }

    int
    compare(const NamedObjectMI& rhs) const
    {
        return _name.compare(rhs._name);
    }

    void
    copy(const NamedObjectMI& rhs)
    {
        _name = rhs._name;
    }

    void
    serialize(Stream& stream, uint_t io, uint_t mode = ser_default)
    {
        _name.serialize(stream, io, mode);
    }

    /** Get the name. */
    const String&
    getName() const
    {
        return _name;
    }

    /** Get the address of the name. */
    const String*
    getNamePtr() const
    {
        if (_name.empty())
            return nullptr;
        return &_name;
    }

    /** Set the name. */
    void
    setName(const String& name)
    {
        _name = name;
    }

private:
    String _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
