#include <libutl/libutl.h>
#include <libutl/TimeSpan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
TimeSpan::compare(const Object& rhs) const
{
    int res;

    if (rhs.isA(Time))
    {
        auto& t = utl::cast<Time>(rhs);
        // If the end is <= t, the time is greater
        if (_end <= t)
        {
            res = -1;
        }
        // Else if the time is < begin, the span is greater
        else if (t < _begin)
        {
            res = 1;
        }
        // Else the time falls within the span so they're equal
        else
        {
            res = 0;
        }
    }
    else if (rhs.isA(Duration))
    {
        // Compare the span's duration with the duration
        auto& d = utl::cast<Duration>(rhs);
        res = duration().compare(d);
    }
    else
    {
        res = super::compare(rhs);
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
TimeSpan::numSpanned(const Duration& _dur) const
{
    Time begin = _begin;
    begin.roundDown(_dur);
    size_t diff = (_end - begin);
    size_t dur = (size_t)_dur;
    size_t res = diff / dur;
    if ((res > 0) && (res * dur == diff))
    {
        --res;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
TimeSpan::numSpanned(uint_t unit) const
{
    Duration dur;
    dur.setUnit(unit);
    return numSpanned(dur);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::TimeSpan);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL2(utl::ObjectSpan, utl::Time, utl::Duration);
UTL_INSTANTIATE_TPL2(utl::Span, utl::Time, utl::Duration);
