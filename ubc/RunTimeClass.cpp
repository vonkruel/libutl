#include <libutl/libutl.h>
#include <libutl/Array.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef new
#include <vector>
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

struct SortedRunTimeClass : public Object
{
    virtual int compare(const Object& rhs) const;
    const char* className;
    const RunTimeClass* rtc;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static const uint_t maxRTCs = 1024;
static uint_t numRTCs = 0;
static const char* rtcName[maxRTCs];
static const RunTimeClass* rtcInstance[maxRTCs];

////////////////////////////////////////////////////////////////////////////////////////////////////

int
SortedRunTimeClass::compare(const Object& rhs) const
{
    auto& rtc = utl::cast<SortedRunTimeClass>(rhs);
    return strcmp(className, rtc.className);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RunTimeClass::printClassNames()
{
    for (uint_t i = 0; i < numRTCs; ++i)
    {
        printf("%s\n", rtcName[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RunTimeClass::add(RunTimeClass* rtc)
{
    // too many classes?
    ASSERTD(numRTCs < maxRTCs);

    // find insertion point
    const char* className = rtc->name();

    // find insertion point (first index where rtc is <)
    uint_t i;
    for (i = 0; i < numRTCs; ++i)
    {
        int res = strcmp(className, rtcName[i]);
        if (res == 0)
            return;
        if (res < 0)
            break;
    }

    // make room
    uint_t j;
    for (j = numRTCs; j > i; --j)
    {
        rtcName[j] = rtcName[j - 1];
        rtcInstance[j] = rtcInstance[j - 1];
    }

    // insert at position ip
    rtcName[i] = className;
    rtcInstance[i] = rtc;
    ++numRTCs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const RunTimeClass*
RunTimeClass::find(const char* className)
{
    uint_t low = 0;
    uint_t high = numRTCs - 1;
    while ((low <= high) && (high != uint_t_max))
    {
        uint_t mid = (low + high) / 2;
        int cmp = strcmp(className, rtcName[mid]);
        if (cmp < 0)
        {
            high = mid - 1;
        }
        else if (cmp > 0)
        {
            low = mid + 1;
        }
        else
        {
            return rtcInstance[mid];
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void
setBases(const RunTimeClass* rtc, std::vector<const RunTimeClass*>& bases)
{
    bases.push_back(rtc);
    auto base = rtc->baseClass();
    if (base == rtc)
    {
        bases.push_back(nullptr);
    }
    else
    {
        setBases(base, bases);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RunTimeClass::_isA(const RunTimeClass* rtc) const
{
    auto bases = _bases.load(std::memory_order_relaxed);
    if (bases == nullptr)
    {
        std::vector<const RunTimeClass*> basesV;
        setBases(this, basesV);
        if (basesV.size() <= 8)
        {
            bases = _basesInline;
            memcpy(bases, basesV.data(), basesV.size() * sizeof(void*));
            _bases.store(bases, std::memory_order_release);
        }
        else
        {
            bases = (const RunTimeClass**)malloc(basesV.size() * sizeof(void*));
            memcpy(bases, basesV.data(), basesV.size() * sizeof(void*));
            const RunTimeClass** expected = nullptr;
            if (!_bases.compare_exchange_strong(expected, bases, std::memory_order_release,
                                                std::memory_order_acquire))
            {
                free(bases);
                bases = expected;
            }
        }
    }
    auto it = bases;
    auto base = *it;
    while (base != nullptr)
    {
        if (base == rtc)
            return true;
        base = *++it;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RunTimeClass::utl_deInit()
{
    auto lim = rtcInstance + numRTCs;
    for (auto ptr = rtcInstance; ptr != lim; ++ptr)
    {
        auto rtc = *ptr;
        auto& rtcBases = rtc->_bases;
        if ((rtcBases != nullptr) && (rtcBases != rtc->_basesInline))
        {
            free(rtcBases);
            rtcBases = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
