#include <libutl/libutl.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Uint);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Uint::serialize(Stream& stream, uint_t io, uint_t mode)
{
    Object::serialize(stream, io, mode);
    utl::serialize(_n, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
