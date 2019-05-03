#include <libutl/libutl.h>
#include <libutl/BinaryData.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BinaryData);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinaryData::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        // clear existing data
        clear();

        // read & set size
        setOwner(true);
        size_t size;
        utl::serialize(size, stream, io, mode);
        setSize(size);

        // read binary data (ignore serialization mode)
        stream.read(_array, size);
    }
    else
    {
        // write size
        utl::serialize(_size, stream, io, mode);

        // write binary data (ignore serialization mode)
        stream.write(_array, _size);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
