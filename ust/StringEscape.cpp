#include <libutl/libutl.h>
#include <libutl/StringEscape.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::StringEscape);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringEscape::add(char c, const char* s, bool owner)
{
    auto len = strlen(s);
    ASSERTD(len < 256);
    auto index = (byte_t)c;
    _hasSub.set(index, 1);
    auto& sub = _subs[index];
    if (len <= 6)
    {
        sub.s = sub.buf;
        memcpy(sub.buf, s, len);
        if (owner)
        {
            delete[] s;
            owner = false;
        }
    }
    else
    {
        sub.s = s;
    }
    sub.len = len;
    sub.owner = owner ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringEscape::process(String& out, const String& in, const char* prefix, const char* suffix) const
{
    size_t prefixLen = (prefix == nullptr) ? 0 : strlen(prefix);
    size_t suffixLen = (suffix == nullptr) ? 0 : strlen(suffix);

    // make a note of where substitions occur, and compute new length
    utl::Vector<size_t> v((size_t)0, size_t_max);
    auto len = in.length(), newLen = out.length() + prefixLen + len + suffixLen;
    auto b = in.get();
    auto p = b;
    auto lim = b + len;
    for (; p != lim; ++p)
    {
        // no substitution?
        auto c = *p;
        auto index = (byte_t)c;
        if (!_hasSub.get(index))
            continue;

        // reference the substitution
        auto& sub = _subs[index];
        auto s = sub.s;
        if (s == nullptr)
            continue;

        // remember where we saw the character that needed substitution
        v.append(p - b);

        // adjust the length of the output string
        newLen += ((int)sub.len - 1);
    }

    // make room in the output string
    out.reserve(newLen + 1);
    ASSERTD(out.isOwner());

    // prefix
    if (prefix != nullptr)
        out.append(prefix, prefixLen);

    // no substitions?
    if (v.empty())
    {
        out.append(in);
    }
    else
    {
        // construct the result
        size_t pos = 0;
        for (auto eci : v)
        {
            if (eci > pos)
                out.append(b + pos, eci - pos);
            byte_t c = (byte_t)b[eci];
            auto& sub = _subs[c];
            out.append(sub.s, sub.len);
            pos = eci + 1;
        }
        out.append(b + pos, len - pos);
    }

    // suffix
    if (suffix != nullptr)
        out.append(suffix, suffixLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringEscape::init()
{
    _hasSub.initialize(256);
    memset(_subs, 0, sizeof(_subs));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringEscape::deInit()
{
    auto lim = _subs + 256;
    for (auto it = _subs; it != lim; ++it)
    {
        auto& sub = *it;
        if (sub.owner)
            delete[] sub.s;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
