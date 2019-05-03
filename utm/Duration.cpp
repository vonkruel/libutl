#include <libutl/libutl.h>
#include <libutl/Int.h>
#include <libutl/Duration.h>
#include <libutl/TimeSpan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Duration);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Duration::toString() const
{
    if (_n < 0)
    {
        return "-" + Duration(-_n).toString();
    }

    uint_t numDays = days(), numHours = hours(), numMinutes = minutes();
    double numSeconds = seconds();

    String res;
    if (_n == 0)
    {
        res = "0s";
        return res;
    }

    bool space = false;
    if (numDays > 0)
    {
        res += Int(numDays).toString();
        res += "d";
        space = true;
    }
    if (numHours > 0)
    {
        if (space)
            res += " ";
        res += Int(numHours).toString();
        res += "h";
        space = true;
    }
    if (numMinutes > 0)
    {
        if (space)
            res += " ";
        res += Int(numMinutes).toString();
        res += "m";
        space = true;
    }
    if (numSeconds > 0)
    {
        if (space)
            res += " ";
        res += Float(numSeconds).toString();
        res += "s";
        space = true;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Number<double>&
Duration::set(const String& str)
{
    _n = 0;

    // Create a tokenizer for the format string
    StringTokenizer st;
    TokenizerTokens tokens;
    st.addTerminal(0, "\\S+");
    st.scan(tokens, str);

    for (;;)
    {
        // Get a token
        String token = tokens.next();

        // No token -> we're done
        if (token.empty())
            break;

        // The unit measurement
        String unit;
        if (!isdigit(token[token.length() - 1]))
        {
            unit = token.chop(token.length() - 1);
            unit.toLower();
        }
        // n is the number associated with the unit
        double num = (double)Float(token);

        // Add seconds to the duration based on unit and n
        if (unit == "d")
        {
            _n += num * oneDay;
        }
        else if (unit == "h")
        {
            _n += num * oneHour;
        }
        else if (unit == "m")
        {
            _n += num * oneMinute;
        }
        else
        {
            _n += num;
        }
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Duration::set(uint_t days, uint_t hours, uint_t minutes, double seconds)
{
    _n = (double)(days * 24 * 60 * 60) + (hours * 60 * 60) + (minutes * 60) + seconds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Duration::setUnit(uint_t unit, uint_t num)
{
    double n;
    switch (unit)
    {
    case tm_second:
        n = Duration::oneSecond;
        break;
    case tm_minute:
        n = Duration::oneMinute;
        break;
    case tm_half_hour:
        n = Duration::oneHalfHour;
        break;
    case tm_hour:
        n = Duration::oneHour;
        break;
    case tm_day:
        n = Duration::oneDay;
        break;
    case tm_week:
        n = 7 * Duration::oneDay;
        break;
    default:
        ABORT();
    }
    _n = n * num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Duration Duration::oneDay(24 * 60 * 60);
const Duration Duration::oneHour(60 * 60);
const Duration Duration::oneHalfHour(30 * 60);
const Duration Duration::oneMinute(60);
const Duration Duration::oneSecond(1);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
