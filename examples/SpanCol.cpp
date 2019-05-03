#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/SpanCol.h>
#include <libutl/TimeSpan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// TimeSlot ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class TimeSlot : public TimeSpan
{
    UTL_CLASS_DECL(TimeSlot, TimeSpan)
    UTL_CLASS_DEFID
public:
    TimeSlot(const Time& begin, const Time& end, const String& activity)
        : TimeSpan(begin, end)
    {
        _activity = activity;
    }
    virtual int compare(const Object& rhs) const;
    virtual void copy(const Object& rhs);
    virtual void
    dump(Stream& os, uint_t) const
    {
        os << *this << ": " << _activity << endl;
    }
    void
    setActivity(const String& activity)
    {
        _activity = activity;
    }

private:
    String _activity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(TimeSlot);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
TimeSlot::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(TimeSlot))
    {
        const TimeSlot& ts = (const TimeSlot&)rhs;
        res = TimeSpan::compare(ts);
        if (res != 0)
            return res;
        res = _activity.compare(ts._activity);
    }
    else
    {
        res = TimeSpan::compare(rhs);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TimeSlot::copy(const Object& rhs)
{
    if (rhs.isA(TimeSlot))
    {
        const TimeSlot& ts = (const TimeSlot&)rhs;
        TimeSpan::copy(ts);
        _activity = ts._activity;
    }
    else
    {
        TimeSpan::copy(rhs);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    SpanCol<Time, Duration> spanCol;
    spanCol += TimeSlot(Time::today(8), Time::today(12), "meeting");
    spanCol += TimeSlot(Time::today(12), Time::today(13), "lunch break");
    spanCol += TimeSlot(Time::today(13), Time::today(17), "programming");
    cout << "--- initial schedule" << endl;
    spanCol.dump(cout);
    spanCol += TimeSlot(Time::today(15), Time::today(16), "web-surfing");
    cout << "--- added a new activity from 3pm to 4pm" << endl;
    spanCol.dump(cout);
    spanCol += TimeSlot(Time::today(15), Time::today(19), "programming");
    cout << "--- extended the programming task" << endl;
    spanCol.dump(cout);
    spanCol.remove(TimeSpan(Time::today(11), Time::today(16)));
    cout << "--- removed from 11am to 4pm" << endl;
    spanCol.dump(cout);
    return 0;
}
