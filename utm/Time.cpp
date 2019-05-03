#include <libutl/libutl.h>
#include <math.h>
#include <libutl/Bool.h>
#include <libutl/Int.h>
#include <libutl/Translator.h>
#include <libutl/TimeSpan.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// TimeTranslator /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeTranslator : public Mapping
{
    UTL_CLASS_DECL(TimeTranslator, Mapping);

public:
    TimeTranslator(const Time* time, bool asGMT = false)
    {
        _time = time;
        _asGMT = asGMT;
        _time->getComponents(&_year, &_month, &_day, &_wday, &_hour, &_min, &_sec, &_isDST, _asGMT);
    }

    virtual Object* map(const Object* object) const;

private:
    void
    init()
    {
        _time = nullptr;
    }
    void
    deInit()
    {
    }

private:
    const Time* _time;
    uint_t _year, _month, _day, _wday, _hour, _min;
    double _sec;
    bool _isDST;
    bool _asGMT;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Time ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
Time::compare(const Object& rhs) const
{
    int res;
    // if rhs is a Time also
    if (rhs.isA(Time))
    {
        auto& tm = utl::cast<Time>(rhs);
        res = utl::compare(_dateTime, tm._dateTime);
    }
    // else if rhs is a TimeSpan
    else
    {
        auto& ts = utl::cast<TimeSpan>(rhs);
        // if we are before the time span, result is -1
        if (*this < ts.begin())
        {
            res = -1;
        }
        // if we are after the time span, result is 1
        else if (ts.end() <= *this)
        {
            res = 1;
        }
        // else we are within the time span -> result is 0
        else
        {
            res = 0;
        }
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Time::copy(const Object& rhs)
{
    auto& tm = utl::cast<Time>(rhs);
    _dateTime = tm._dateTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Time::serialize(Stream& stream, uint_t io, uint_t mode)
{
    // serialize flags

    if (mode & ser_default)
    {
        mode |= getSerializeMode();
        mode &= ~ser_default;
    }

    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            // serialize members
            String str;
            str.serializeIn(stream, mode);

            uint_t year = 1, month = 1, day = 1, hour = 0, min = 0;
            double sec = 0.0;
            sscanf(str, "%u/%u/%u %u:%u:%lf", &year, &month, &day, &hour, &min, &sec);
            set(year, month, day, hour, min, sec, true);
        }
        else
        {
            // serialize members
            toGMTstring().serializeOut(stream, mode);
        }
    }
    else // ser_compact or ser_nonportable
    {
        utl::serialize(_dateTime, stream, io, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Time::getComponents(uint_t* year,
                    uint_t* month,
                    uint_t* day,
                    uint_t* wday,
                    uint_t* hour,
                    uint_t* min,
                    double* sec,
                    bool* isDST,
                    bool isGMT) const
{
    struct tm* tmP;
    if (isGMT)
    {
        tmP = gmtime(&_dateTime);
    }
    else
    {
        tmP = localtime(&_dateTime);
    }

    if (year != nullptr)
    {
        *year = tmP->tm_year + 1900;
    }
    if (month != nullptr)
    {
        *month = tmP->tm_mon + 1;
    }
    if (day != nullptr)
    {
        *day = tmP->tm_mday;
    }
    if (wday != nullptr)
    {
        *wday = tmP->tm_wday;
    }
    if (hour != nullptr)
    {
        *hour = tmP->tm_hour;
    }
    if (min != nullptr)
    {
        *min = tmP->tm_min;
    }
    if (sec != nullptr)
    {
        *sec = tmP->tm_sec;
    }
    if (isDST != nullptr)
    {
        *isDST = (tmP->tm_isdst > 0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
Time::maxDay() const
{
    uint_t res;
    uint_t year, month;

    // get current year, month, day
    getComponents(&year, &month, nullptr);

    res = monthDays[month - 1];

    // February is a special case
    if (res == 0)
    {
        Uint yr(year);
        if (yr.isDivisibleBy(4))
        {
            if (yr.isDivisibleBy(100))
            {
                if (yr.isDivisibleBy(400))
                {
                    res = 29;
                }
                else
                {
                    res = 28;
                }
            }
            else
            {
                res = 29;
            }
        }
        else
        {
            res = 28;
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Time::set(uint_t year, uint_t month, uint_t day, uint_t hour, uint_t min, double sec, bool isGMT)
{
    // populate tm struct
    struct tm tms;
    memset(&tms, 0, sizeof(struct tm));
    tms.tm_year = year - 1900;
    tms.tm_mon = month - 1;
    tms.tm_mday = day;
    tms.tm_hour = hour;
    tms.tm_min = min;
    tms.tm_sec = (int)sec;
    tms.tm_isdst = -1;

    // get time_t value from tm struct
    if (isGMT)
    {
#if UTL_HOST_TYPE == UTL_HT_UNIX
        _dateTime = timegm(&tms);
#else
        ABORT();
#endif
    }
    else
    {
        _dateTime = mktime(&tms);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Time::toString() const
{
    char* str = new char[64];

    if (isNull())
    {
        strcpy(str, "<null>");
    }
    else
    {
        uint_t year, month, day, hour, min;
        double sec;
        bool isDST;
        getComponents(&year, &month, &day, nullptr, &hour, &min, &sec, &isDST);
        sprintf(str, "%u-%02u-%02u %02u:%02u:%02.0f", year, month, day, hour, min, sec);
        if (isDST)
        {
            strcat(str, " DST");
        }
    }

    return String(str, true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Time::toString(const String& fmt) const
{
    if (isNull())
        return "<null>";

    StringTranslator st(new TimeTranslator(this));
    return st.translate(fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Time::toGMTstring() const
{
    char* str = new char[64];

    if (isNull())
    {
        strcpy(str, "<null>");
    }
    else
    {
        uint_t year, month, day, hour, min;
        double sec;
        bool isDST;
        getComponents(&year, &month, &day, nullptr, &hour, &min, &sec, &isDST, true);
        sprintf(str, "%u-%02u-%02u %02u:%02u:%02.0f", year, month, day, hour, min, sec);
    }

    return String(str, true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Time::toGMTstring(const String& fmt) const
{
    if (isNull())
        return "<null>";

    StringTranslator st(new TimeTranslator(this, true));
    return st.translate(fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Time::set(const String& fmt, const String& str)
{
    int i;

    StringTokenizer fmtST;
    TokenizerTokens fmtTokens;
    fmtST.addTerminal(0, "\\S+");
    fmtST.scan(fmtTokens, fmt);

    StringTokenizer strST;
    TokenizerTokens strTokens;
    strST.addTerminal(0, "[:alnum:]+");
    strST.scan(strTokens, str);

    String fmtToken, strToken;

    uint_t year = 1971, month = 1, day = 1, hour = 0, min = 0;
    double sec = 0;
    bool isGMT = false;

    for (;;)
    {
        // get the format-specifier token
        fmtToken = fmtTokens.next();
        // get the format string
        strToken = strTokens.next();

        // terminate conversion?
        if (fmtToken.empty() && strToken.empty())
            break;

        // "y" -> year
        if (fmtToken == "y")
        {
            year = Int(strToken);
            if (strToken.length() == 2)
            {
                year += 2000;
            }
            if (year < 1900)
                throw IllegalValueEx(strToken);
        }
        // "Mn" -> month name
        else if (fmtToken == "Mn")
        {
            for (i = 0; i != 12; i++)
            {
                if (strToken == monthName[i])
                {
                    month = i + 1;
                    break;
                }
            }
            throw IllegalValueEx(strToken);
        }
        // "mn" -> abbreviated month name
        else if (fmtToken == "mn")
        {
            for (i = 0; i != 12; i++)
            {
                if (strToken == abbrevMonthName[i])
                {
                    month = i + 1;
                    break;
                }
            }
            throw IllegalValueEx(strToken);
        }
        // "m" -> month
        else if (fmtToken == "m")
        {
            month = Int(strToken);
            if ((month == 0) || (month > 12))
                throw IllegalValueEx(strToken);
        }
        // "d" -> day
        else if (fmtToken == "d")
        {
            day = Int(strToken);
            if ((day == 0) || (day > 31))
                throw IllegalValueEx(strToken);
        }
        // "h" -> hour
        else if (fmtToken == "h")
        {
            hour = Int(strToken);
            if (hour > 23)
                throw IllegalValueEx(strToken);
        }
        // "n" -> minute
        else if (fmtToken == "n")
        {
            min = Int(strToken);
            if (min > 59)
                throw IllegalValueEx(strToken);
        }
        // "s" -> second
        else if (fmtToken == "s")
        {
            sec = Float(strToken);
            if (sec > 59)
                throw IllegalValueEx(strToken);
        }
        // "p" -> "am" or "pm" (case-insensitive)
        else if (fmtToken == "p")
        {
            // if we're in PM and hour < 12, add 12 hours
            if ((strToken.toLower() == "pm") && (hour < 12))
            {
                hour += 12;
            }
        }
        // "gmt" -> interpret as GMT
        else if (fmtToken == "gmt")
        {
            isGMT = true;
        }
    }

    // Set the time
    set(year, month, day, hour, min, sec, isGMT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Time&
Time::roundDown(uint_t unit)
{
    switch (unit)
    {
    case tm_second:
        roundDown(Duration::oneSecond);
        break;
    case tm_minute:
        roundDown(Duration::oneMinute);
        break;
    case tm_half_hour:
        roundDown(Duration::oneHalfHour);
        break;
    case tm_hour:
        roundDown(Duration::oneHour);
        break;
    case tm_day:
        roundDown(Duration::oneDay);
        break;
    case tm_week:
        roundDown(Duration::oneDay);
        *this -= Duration(getDayOfWeek() * (24 * 3600));
        break;
    case tm_month:
        roundDown(Duration::oneDay);
        *this -= Duration((getDay() - 1) * (24 * 3600));
        break;
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Time&
Time::roundDown(const Duration& dur)
{
    if (dur == 0)
        return self;
    int curTimeOfDay = (int)this->getTimeOfDay();
    int newTimeOfDay = utl::roundDown(curTimeOfDay, (int)dur.get());
    int diff = newTimeOfDay - curTimeOfDay;
    _dateTime += diff;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Time&
Time::roundUp(uint_t unit)
{
    switch (unit)
    {
    case tm_second:
        roundUp(Duration::oneSecond);
        break;
    case tm_minute:
        roundUp(Duration::oneMinute);
        break;
    case tm_half_hour:
        roundUp(Duration::oneHalfHour);
        break;
    case tm_hour:
        roundUp(Duration::oneHour);
        break;
    case tm_day:
        roundUp(Duration::oneDay);
        break;
    case tm_week:
        roundUp(Duration::oneDay);
        *this += Duration((7 - getDayOfWeek()) * (24 * 60 * 60));
        break;
    case tm_month:
        roundUp(Duration::oneDay);
        *this += Duration((maxDay() + 1 - getDay()) * (24 * 3600));
        break;
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Time&
Time::roundUp(const Duration& dur)
{
    if (dur == 0)
        return self;
    int curTimeOfDay = (int)this->getTimeOfDay();
    int newTimeOfDay = utl::roundUp(curTimeOfDay, (int)dur.get());
    int diff = newTimeOfDay - curTimeOfDay;
    _dateTime += diff;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Duration
Time::operator-(const Time& rhs) const
{
    // compare the julian date and timeOfDay
    time_t diff = _dateTime - rhs._dateTime;
    return Duration(diff);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Time
Time::today(uint_t hour, uint_t min, double sec)
{
    Time res;
    res.setCurrent();
    res.roundDown(tm_day);
    res += Duration((hour * 60 * 60) + (min * 60) + sec);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Time::abbrevWeekDayName[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Time::weekDayName[7] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                                    "Thursday", "Friday", "Saturday"};

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Time::abbrevMonthName[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Time::monthName[12] = {"January",   "February", "March",    "April",
                                   "May",       "June",     "July",     "August",
                                   "September", "October",  "November", "December"};

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint_t Time::monthDays[12] = {31,
                                    0, // depends on year
                                    31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// TimeTranslator /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
TimeTranslator::map(const Object* object) const
{
    auto& token = utl::cast<String>(*object);
    ASSERTD(!token.empty());

    char c0 = token[0];
    char c1 = token[1];
    AutoPtr<> resPtr = new String;
    String& res = (String&)*resPtr;

    switch (c0)
    {
    case 'd':
        if (token == "d")
        {
            res = Int(_day).toString();
        }
        else if (token == "dd")
        {
            res = Int(_day).toDecimal(2);
        }
        else if (token == "dst")
        {
            if (_isDST)
            {
                res = " DST";
            }
        }
        break;
    case 'h':
        if (token == "h")
        {
            if (_hour > 12)
            {
                res = Int(_hour - 12).toString();
            }
            else
            {
                res = Int(_hour).toString();
            }
        }
        else if (token == "hh")
        {
            res = Int(_hour).toDecimal(2);
        }
        break;
    case 'M':
        if ((c1 == 'n') && (token[2] == '\0'))
        {
            res = Time::monthName[_month - 1];
        }
        break;
    case 'm':
        if (c1 == '\0')
        {
            res = Int(_month).toString();
        }
        else if ((c1 == 'm') && (token[2] == '\0'))
        {
            res = Int(_month).toDecimal(2);
        }
        else if ((c1 == 'n') && (token[2] == '\0'))
        {
            res = Time::abbrevMonthName[_month - 1];
        }
        break;
    case 'n':
        if (c1 == '\0')
        {
            res = Int(_min).toString();
        }
        else if ((c1 == 'n') && (token[2] == '\0'))
        {
            res = Int(_min).toDecimal(2);
        }
        break;
    case 'P':
        if (c1 == '\0')
        {
            res = (_hour < 12) ? "AM" : "PM";
        }
        break;
    case 'p':
        if (c1 == '\0')
        {
            res = (_hour < 12) ? "am" : "pm";
        }
        break;
    case 's':
        if ((c1 == 's') && (token[2] == '\0'))
        {
            if (Float(_sec).isInt())
            {
                res = Float(_sec).toString("%02.0f");
            }
            else
            {
                res = Float(_sec).toString("%02f");
            }
        }
        break;
    case 'W':
        if ((c1 == 'n') && (token[2] == '\0'))
        {
            res = Time::weekDayName[_wday];
        }
        break;
    case 'w':
        if ((c1 == 'n') && (token[2] == '\0'))
        {
            res = Time::abbrevWeekDayName[_wday];
        }
        break;
    case 'y':
        if ((c1 == 'y') && (token[2] == '\0'))
        {
            res = Int(_year).mod(100).toString();
        }
        else if (token == "yyyy")
        {
            res = Int(_year).toString();
        }
        break;
    }

    // no match?
    if (res.empty())
        return nullptr;

    resPtr.release();
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Time);
UTL_CLASS_IMPL(utl::TimeTranslator);
