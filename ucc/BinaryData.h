#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Vector of bytes.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinaryData : public Vector<byte_t>
{
    UTL_CLASS_DECL(BinaryData, Vector<byte_t>);

public:
    /**
       Constructor
       \param size array size
       \param increment (optional : size_t_max) growth increment
    */
    BinaryData(size_t size, size_t increment = 1)
        : Vector<byte_t>(size, increment)
    {
    }

    /**
       Constructor
       \param array associated array
       \param size array size
       \param owner (optional : true) \b owner flag
       \param increment (optional : size_t_max) growth increment
    */
    BinaryData(byte_t* array, size_t size, bool owner = true, size_t increment = 1)
        : Vector<byte_t>(array, size, owner, increment)
    {
    }

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

private:
    void
    init()
    {
        setIncrement(1);
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
