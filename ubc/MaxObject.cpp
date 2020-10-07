#include <libutl/libutl.h>
#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::MaxObject);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

const MaxObject maxObject;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
MaxObject::compare(const Object& rhs) const
{
    return rhs.isA(MaxObject) ? 0 : 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;