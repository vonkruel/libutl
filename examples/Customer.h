#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Uint.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

class Customer : public Object
{
    UTL_CLASS_DECL(Customer, Object);
    UTL_CLASS_DEFID;

public:
    Customer(uint64_t id, const String& name)
    {
        _id = id;
        _name = name;
    }
    virtual int compare(const Object& rhs) const;
    virtual void copy(const Object& rhs);
    virtual void dump(Stream& os, uint_t level = uint_t_max) const;
    const Object&
    getKey() const
    {
        return _id;
    }
    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);
    virtual String toString() const;
    uint_t
    getID() const
    {
        return _id;
    }
    const String&
    getName() const
    {
        return _name;
    }
    void
    setID(uint_t id)
    {
        _id = id;
    }
    void
    setName(const String& name)
    {
        _name = name;
    }

private:
    Uint _id;
    String _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Customer_cpp"
