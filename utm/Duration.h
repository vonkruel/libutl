#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Time;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Duration of time.

   <b>String Representation Formatting</b>

   The format string takes the form:

   \code
   [<numDays> d] [<numHours> h] [<numMinutes> m] [<numSeconds> s]
   \endcode

   The following are examples of valid format strings:

   \arg "1d 4m 5s" => one day, 4 minutes, 5 seconds

   \arg "3h 15s" => three hours, 15 seconds

   \author Adam McKee
   \ingroup time
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Duration : public Float
{
    UTL_CLASS_DECL(Duration, Float);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param seconds number of seconds
    */
    Duration(double seconds)
    {
        Number<double>::set(seconds);
    }

    /**
       Constructor.
       \param str format string
    */
    Duration(const String& str)
    {
        set(str);
    }

    /**
       Constructor.
       \param days number of days
       \param hours number of hours
       \param minutes number of minutes
       \param seconds number of seconds
    */
    Duration(uint_t days, uint_t hours, uint_t minutes, double seconds)
    {
        set(days, hours, minutes, seconds);
    }

    virtual String toString() const;

    /** Get the number of days. */
    uint_t
    days() const
    {
        return (uint_t)(_n / (24 * 60 * 60));
    }

    /** Get the number of hours. */
    uint_t
    hours() const
    {
        double res = fmod(_n, 24 * 60 * 60);
        res /= (60 * 60);
        return (uint_t)res;
    }

    /** Get the number of minutes. */
    uint_t
    minutes() const
    {
        double res = fmod(_n, 60 * 60);
        res /= 60;
        return (uint_t)res;
    }

    /** Get the number of seconds. */
    double
    seconds() const
    {
        return fmod(_n, 60);
    }

    /** Set with a format string. */
    virtual Number<double>& set(const String& str);

    /**
       Set a new duration.
       \param days number of days
       \param hours number of hours
       \param minutes number of minutes
       \param seconds number of seconds
    */
    void set(uint_t days, uint_t hours, uint_t minutes, double seconds);

    /**
       Set to a multiple of a given unit.
       \param unit (see utl::tm_unit_t)
       \param num multiple of the given unit
    */
    void setUnit(uint_t unit, uint_t num = 1);

    /**
       Add the given duration to self and return the result.
       \return self plus rhs
       \param rhs duration to add
    */
    Duration
    operator+(const Duration& rhs) const
    {
        return (double)*this + (double)rhs;
    }

    /**
       Subtract the given duration from self and return the result.
       \return self minus rhs
       \param rhs duration to subtract
    */
    Duration
    operator-(const Duration& rhs) const
    {
        return (double)*this - (double)rhs;
    }

    /** Get duration in number of days. */
    static Duration
    days(double numDays)
    {
        return Duration(numDays * 24.0 * 60.0 * 60.0);
    }

    /** Get duration in number of hours. */
    static Duration
    hours(double numHours)
    {
        return Duration(numHours * 60.0 * 60.0);
    }

    /** Get duration in number of minutes. */
    static Duration
    minutes(double numMinutes)
    {
        return Duration(numMinutes * 60.0);
    }

    /** Get duration in number of seconds. */
    static Duration
    seconds(uint64_t numSeconds)
    {
        return Duration(numSeconds);
    }

public:
    /** Duration of one day. */
    static const Duration oneDay;
    /** Duration of one hour. */
    static const Duration oneHour;
    /** Duration of half an hour. */
    static const Duration oneHalfHour;
    /** Duration of one minute. */
    static const Duration oneMinute;
    /** Duration of one second. */
    static const Duration oneSecond;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
