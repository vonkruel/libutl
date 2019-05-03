#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Duration.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Day of week.
   \ingroup time
*/
enum tm_day_t
{
    tm_sunday,    /**< Sunday */
    tm_monday,    /**< Monday */
    tm_tuesday,   /**< Tuesday */
    tm_wednesday, /**< Wednesday */
    tm_thursday,  /**< Thursday */
    tm_friday,    /**< Friday */
    tm_saturday   /**< Saturday */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Month of year.
   \ingroup time
*/
enum tm_month_t
{
    tm_january,   /**< January */
    tm_february,  /**< February */
    tm_march,     /**< March */
    tm_april,     /**< April */
    tm_may,       /**< May */
    tm_june,      /**< June */
    tm_july,      /**< July */
    tm_august,    /**< August */
    tm_september, /**< September */
    tm_october,   /**< October */
    tm_november,  /**< November */
    tm_december   /**< December */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unit of time.
   \ingroup time
*/
enum tm_unit_t
{
    tm_second,    /**< second */
    tm_minute,    /**< minute */
    tm_half_hour, /**< half-hour */
    tm_hour,      /**< hour */
    tm_day,       /**< day */
    tm_week,      /**< week */
    tm_month      /**< month */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   DST flag.
   \ingroup time
*/
enum tm_dst_t
{
    tm_dst_off,  /**< DST off */
    tm_dst_on,   /**< DST on */
    tm_dst_auto, /**< DST auto select */
    tm_dst_leave /**< leave DST alone */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Date/time.

   <b>Getting a String Representation</b>

   A format string given to the toString() method contains special codes that are to be substituted
   with strings based on the represented time.  The codes are always preceded by a \b $ character
   so the parser can recognize them.  If you want to have a \b $ character in the format string
   without a format code, just put two in a row: \b $$.

   The formatting codes for toString() are:

   \arg \b d : day of month [1,31]
   \arg \b dd : day of month (2 digits)
   \arg \b dst : " DST" if DST enabled
   \arg \b h : hour [0,23]
   \arg \b hh : hour [0,11]
   \arg \b Mn : month name
   \arg \b mn : abbreviated month name (e.g. "Jan")
   \arg \b m : month [1,12]
   \arg \b mm : month (2 digits)
   \arg \b n : minute [0,59]
   \arg \b nn : minute (2 digits)
   \arg \b P : "AM" or "PM"
   \arg \b p : "am" or "pm"
   \arg \b s : second [0,59]
   \arg \b ss : second (2 digits)
   \arg \b Wn : day of week (e.g. "Monday")
   \arg \b wn : abbreviated day of week (e.g. "Mon")
   \arg \b y : year (e.g. 05 => 2005)
   \arg \b yyyy : year (4 digits - e.g. "2005")

   For example, suppose the represented time is June 15th 2000 5:43 pm DST. Here are some format
   strings and the corresponding results that toString() would provide.

   \arg "year = $yyyy, month = $Mn" => "year = 2000, month = June"
   \arg "$mn $d, $yyyy @ $hh:$mm" => "Jun 15, 2000 @ 17:43"
   \arg "$h:$mm $p $DST" => "5:43 pm DST"

   <b>Setting Time With a String</b>

   When setting the time with a string, you provide two strings to set().  The first string is the
   format string that tells the parser what to expect in the second string which contains the
   string representation of the time.  The formatting codes are not preceded by a \b $ character
   as they are with toString() because the format string contains \e only formatting codes.

   Here are the format strings recognized by set():

   \arg y : year (2 or 4 digits)
   \arg Mn : month name (e.g. "January")
   \arg mn : abbreviated month name (e.g. "Jan")
   \arg m : month [1,12]
   \arg d : day [1,31]
   \arg h : hour [0,23]
   \arg n : minute [0,59]
   \arg s : second [0,59]
   \arg p : am or pm (case-insensitive)
   \arg gmt : interpret as GMT

   \author Adam McKee
   \ingroup time
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Time : public Object
{
    UTL_CLASS_DECL(Time, Object);

public:
    /**
       Constructor.
       \param fmt format string
       \param str string representation
    */
    Time(const String& fmt, const String& str)
    {
        set(fmt, str);
    }

    /**
       Constructor.
       \param year year
       \param month (optional : 1) month [1,12]
       \param day (optional : 1) day [1,31]
       \param hour (optional : 0) hour [0,23]
       \param min (optional : 0) minute [0,59]
       \param sec (optional : 0) second [0,59]
       \param isGMT (optional : false) interpret as GMT ?
                    (otherwise interpret as local time)
    */
    Time(uint_t year,
         uint_t month = 1,
         uint_t day = 1,
         uint_t hour = 0,
         uint_t min = 0,
         double sec = 0,
         bool isGMT = false)
    {
        set(year, month, day, hour, min, sec, isGMT);
    }

    /**
       Constructor.
       \param t time_t value
    */
    Time(time_t t)
    {
        set(t);
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /// \name Accessors
    //@{
    /** Return a time_t representation. */
    operator time_t() const
    {
        return _dateTime;
    }

    /** Get the time as a time_t value. */
    time_t
    get() const
    {
        return _dateTime;
    }

    /** Get the year, month, and day. */
    void getComponents(uint_t* year,
                       uint_t* month,
                       uint_t* day,
                       uint_t* wday = nullptr,
                       uint_t* hour = nullptr,
                       uint_t* min = nullptr,
                       double* sec = nullptr,
                       bool* isDST = nullptr,
                       bool isGMT = false) const;

    /** Get the Null flag. */
    bool
    isNull() const
    {
        return (getYear() == 1970);
    }

    /** Set to null. */
    void
    setNull()
    {
        set((time_t)0);
    }

    /** Get the year. */
    uint_t
    getYear() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_year;
    }

    /** Get the month. */
    uint_t
    getMonth() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_mon;
    }

    /** Get the day. */
    uint_t
    getDay() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_mday;
    }

    /** Get the day of week [0 = Sunday, . . ., 6 = Saturday]. */
    uint_t
    getDayOfWeek() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_wday;
    }

    /** Get the hour [0,23]. */
    uint_t
    getHour() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_hour;
    }

    /** Get the minute [0,59]. */
    uint_t
    getMin() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_min;
    }

    /** Get the second [0,59]. */
    uint_t
    getSec() const
    {
        struct tm* tmP = localtime(&_dateTime);
        return tmP->tm_sec;
    }

    /** Get the number of seconds since midnight. */
    uint_t
    getTimeOfDay() const
    {
        uint_t h, m;
        double s;
        getComponents(nullptr, nullptr, nullptr, nullptr, &h, &m, &s);
        return (h * 3600) + (m * 60) + s;
    }

    /** Get the last day of this month. */
    uint_t maxDay() const;

    /** Set with a time_t value. */
    void
    set(time_t dateTime)
    {
        _dateTime = dateTime;
    }

    /**
       Set a new date/time.
       \param year year
       \param month (optional : 1) month [1,12]
       \param day (optional : 1) day [1,31]
       \param hour (optional : 0) hour [0,23]
       \param min (optional : 0) minute [0,59]
       \param sec (optional : 0) second [0,59]
       \param isGMT (optional : false) interpret as GMT ?
                    (interpret as local time if false)
    */
    void set(uint_t year,
             uint_t month,
             uint_t day,
             uint_t hour = 0,
             uint_t min = 0,
             double sec = 0,
             bool isGMT = false);

    /** Set to the current time. */
    Time&
    setCurrent()
    {
        set(time(nullptr));
        return *this;
    }
    //@}

    /// \name String Representation
    //@{
    /** Return a string representation of self. */
    virtual String toString() const;

    /**
       Return a string representation of self.
       \return string representation of self
       \param fmt format string
    */
    String toString(const String& fmt) const;

    /** Return a string represenation of self (in GMT). */
    String toGMTstring() const;

    /**
       Return a string representation of self.
       \return string representation of self
       \param fmt format string
    */
    String toGMTstring(const String& fmt) const;

    /**
       Set with a string.
       \param fmt format string
       \param str string representation
    */
    void set(const String& fmt, const String& str);
    //@}

    /// \name Rounding
    //@{
    /**
       Round down to the given unit
       \param unit unit (see utl::tm_unit_t)
    */
    Time& roundDown(uint_t unit);

    /**
       Round down to the given resolution.
       \param dur rounding resolution
    */
    Time& roundDown(const Duration& dur);

    /**
       Round up to the given unit
       \param unit unit (see utl::tm_unit_t)
    */
    Time& roundUp(uint_t unit);

    /**
       Round up to the given resolution.
       \param dur rounding resolution
    */
    Time& roundUp(const Duration& dur);
    //@}

    /// \name Operators
    //@{
    /** Assignment from time_t. */
    Time&
    operator=(time_t t)
    {
        set(t);
        return self;
    }

    /** Return self plus the given number of seconds. */
    Time
    operator+(int i) const
    {
        return operator+(Duration(i));
    }

    /** Return self plus the given duration. */
    Time
    operator+(const Duration& rhs) const
    {
        Time res = *this;
        res._dateTime += (time_t)rhs.get();
        return res;
    }

    /** Return self minus the given duration. */
    Time
    operator-(const Duration& rhs) const
    {
        Time res = *this;
        res._dateTime -= (time_t)rhs.get();
        return res;
    }

    /** Return self minus the given number of seconds. */
    Time
    operator-(int i) const
    {
        return *this - Duration(i);
    }

    /** Return self minus the given time. */
    Duration operator-(const Time& rhs) const;

    /** Add the given duration to self. */
    const Time&
    operator+=(const Duration& rhs)
    {
        _dateTime += (time_t)rhs.get();
        return *this;
    }

    /** Subtract the given duration from self. */
    const Time&
    operator-=(const Duration& rhs)
    {
        _dateTime -= (time_t)rhs.get();
        return *this;
    }
    //@}

    /// \name Current Time
    //@{
    /** Return the current time. */
    static Time
    current()
    {
        Time res;
        res.setCurrent();
        return res;
    }

    /**
       Return a Time object for the current date at the given time.
       \param hour (optional : 0) hour [0,23]
       \param min (optional : 0) minute [0,59]
       \param sec (optional : 0) second [0,59]
    */
    static Time today(uint_t hour = 0, uint_t min = 0, double sec = 0);
    //@}

    /** Abbreviated weekday names <pre>[0=Sunday,..,6=Saturday].</pre> */
    static const char* abbrevWeekDayName[7];
    /** Weekday names <pre>[0=Sunday,..,6=Saturday].</pre> */
    static const char* weekDayName[7];
    /** Abbreviated month names <pre>[0=January,..,11=December].</pre> */
    static const char* abbrevMonthName[12];
    /** Month names <pre>[0=January,..,11=December].</pre> */
    static const char* monthName[12];
    /** Number of days in each month <pre>[0=January,..,11=December].</pre> */
    static const uint_t monthDays[12];

private:
    void
    init()
    {
        setNull();
    }
    void
    deInit()
    {
    }

private:
    static int secondsFromGMT;
    time_t _dateTime;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_SERIALIZE(utl::Time);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
