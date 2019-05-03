#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Duration.h>
#include <libutl/Span.h>
#include <libutl/Time.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Time span.

   TimeSpan is a specialization of Span for Time values.

   \author Adam McKee
   \ingroup time
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeSpan : public ObjectSpan<Time, Duration>
{
    UTL_CLASS_DECL_NT_TPL2(TimeSpan, ObjectSpan, Time, Duration);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param rhs Span instance to copy
    */
    TimeSpan(const Span<Time, Duration>& rhs)
    {
        _begin = rhs.begin();
        _end = rhs.end();
    }

    /**
       Constructor.
       \param begin begin time
       \param end end time
    */
    TimeSpan(const Time& begin, const Time& end)
    {
        set(begin, end);
    }

    /** Compare with another instance. */
    virtual int compare(const Object& rhs) const;

    /** Get the duration of the span. */
    Duration
    duration() const
    {
        return _end - _begin;
    }

    /** Get the number of times the given duration is spanned. */
    uint_t numSpanned(const Duration& dur) const;

    /**
       Get the number of times the given unit is spanned.
       \see tm_unit_t
    */
    uint_t numSpanned(uint_t unit) const;

    /** Add the given duration to the begin and end times. */
    void
    shift(const Duration& rhs)
    {
        _begin += rhs;
        _end += rhs;
    }

    /**
       Determine whether self spans the given multiple of the given unit.
       \see tm_unit_t
    */
    bool
    spans(uint_t unit, uint_t num = 1) const
    {
        return (numSpanned(unit) >= num);
    }

    /** Add the given duration to the begin and end times. */
    const TimeSpan&
    operator+=(const Duration& rhs)
    {
        shift(rhs);
        return *this;
    }

    /** Subtract the given duration from the begin and end times. */
    const TimeSpan&
    operator-=(const Duration& rhs)
    {
        shift(-rhs);
        return *this;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
