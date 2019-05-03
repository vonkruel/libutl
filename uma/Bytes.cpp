#include <libutl/libutl.h>
#include <libutl/Bytes.h>
#include <libutl/Float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Bytes);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Bytes::toString() const
{
    if (_n >= GB(1))
    {
        return Float(gigabytes()).toString(2) + " GB";
    }
    else if (_n >= MB(1))
    {
        return Float(megabytes()).toString(2) + " MB";
    }
    else if (_n >= KB(1))
    {
        return Float(kilobytes()).toString(2) + " KB";
    }
    else
    {
        return super::toString();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Bytes Bytes::oneKilobyte(KB(1));
const Bytes Bytes::oneMegabyte(MB(1));
const Bytes Bytes::oneGigabyte(GB(1));

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
