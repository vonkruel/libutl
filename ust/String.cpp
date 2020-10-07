#include <libutl/libutl.h>
#include <libutl/BoyerMooreSearch.h>
#include <libutl/Stream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::String);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

String::String(const char* s, bool owner, bool duplicate, bool caseSensitive)
{
    ASSERTD(nullChar == '\0');
    if (!caseSensitive)
        setCaseSensitive(false);
    if (owner)
    {
        _s = &nullChar;
        _size = 0;
        _length = 0;
        set(s, true, duplicate);
    }
    else if ((s == nullptr) || (*s == nullChar))
    {
        _s = &nullChar;
        _size = 0;
        _length = 0;
    }
    else
    {
        _s = const_cast<char*>(s);
        _size = 0;
        _length = size_t_max;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String::String(size_t size)
{
    ASSERTD(nullChar == '\0');
    if (size == 0)
    {
        init();
    }
    else
    {
        size = utl::nextMultipleOfPow2((size_t)8, size);
        _s = new char[size];
        _s[0] = '\0';
        _size = size;
        _length = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
String::compare(const Object& rhs) const
{
    auto& string = utl::cast<String>(rhs);
    if (isCaseSensitive() && string.isCaseSensitive())
    {
        return ::strcmp(_s, string._s);
    }
    else
    {
        return ::strcasecmp(_s, string._s);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::copy(const Object& rhs)
{
    if (!rhs.isA(String))
    {
        this->copy(rhs.toString());
        return;
    }
    auto& string = utl::cast<String>(rhs);
    if (&string == this)
        return;

    // copy flags
    _flags = string._flags;

    // lhs will own (a copy of) rhs's string if lhs or rhs owns its string
    bool owner = isOwner() || string.isOwner();

    // set the new string in lhs (self)
    set(string._s, owner, owner);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::steal(Object& rhs_)
{
    auto& rhs = utl::cast<String>(rhs_);
    if (_size > 0)
        delete[] _s;
    copyFlags(rhs);
    _s = rhs._s;
    _size = rhs._size;
    _length = rhs._length;
    if (_size > 0)
        rhs.setOwner(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// FNV-1a hash
#define FNV_GCC_OPTIMIZATION
size_t
String::hash(size_t size) const
{
    if (empty())
        return 0;
    static const uint64_t FNV_offset_basis = 14695981039346656037ULL;
#ifndef FNV_GCC_OPTIMIZATION
    static const uint64_t FNV_prime = 1099511628211ULL;
#endif
    uint64_t h = FNV_offset_basis;
    auto ptr = reinterpret_cast<byte_t*>(_s);
    auto lim = ptr + length();
    for (; ptr != lim; ++ptr)
    {
        h ^= static_cast<uint64_t>(*ptr);
#ifdef FNV_GCC_OPTIMIZATION
        h += (h << 1) + (h << 4) + (h << 5) + (h << 7) + (h << 8) + (h << 40);
#else
        h *= FNV_prime;
#endif
    }
    return h % size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        stream >> self;
    }
    else
    {
        stream << self << '\n';
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
String::innerAllocatedSize() const
{
    return _size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::toString() const
{
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
String::compareSubstring(size_t begin, const String& rhs, size_t n)
{
    if (begin > length())
        begin = length();
    if (isCaseSensitive() && rhs.isCaseSensitive())
        return ::strncmp(_s + begin, rhs._s, n);
    return ::strncasecmp(_s + begin, rhs._s, n);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
String::comparePrefix(const String& rhs) const
{
    size_t rhsLen = rhs.length();
    if (isCaseSensitive() && rhs.isCaseSensitive())
        return ::strncmp(_s, rhs._s, rhsLen);
    return ::strncasecmp(_s, rhs._s, rhsLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
String::compareSuffix(const String& rhs) const
{
    size_t lhsLen = this->length();
    size_t rhsLen = rhs.length();
    if (lhsLen < rhsLen)
        return -1;
    if (isCaseSensitive() && rhs.isCaseSensitive())
    {
        return ::strncmp(_s + (lhsLen - rhsLen), rhs._s, rhsLen);
    }
    else
    {
        return ::strncasecmp(_s + (lhsLen - rhsLen), rhs._s, rhsLen);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::backslashEscaped(const String& specials) const
{
    // init escapeChars[]
    byte_t escapeChars[128];
    memset(escapeChars, 0, 128);
    size_t i, len = specials.length();
    for (i = 0; i != len; ++i)
    {
        ASSERTD(specials[i] >= 0);
        escapeChars[(byte_t)specials[i]] = 1;
    }
    escapeChars['\\'] = 1;

    // no escape chars -> just return str
    bool anyEscapeChars = false;
    len = length();
    for (i = 0; i != len; ++i)
    {
        char c = _s[i];
        if ((c >= 0) && escapeChars[(byte_t)c])
        {
            anyEscapeChars = true;
            break;
        }
    }
    if (!anyEscapeChars)
        return self;

    // escape special characters
    String res;
    for (i = 0; i != len; ++i)
    {
        char c = _s[i];
        if ((c >= 0) && escapeChars[(byte_t)c])
        {
            res += '\\';
        }
        res += c;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::backslashUnescaped() const
{
    String res;
    size_t i, len = length();
    for (i = 0; i != len; ++i)
    {
        char c = _s[i];
        if (c == '\\')
        {
            c = _s[++i];
        }
        res += c;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
String::find(const String& str, size_t begin) const
{
    size_t len = length(), strLen = str.length();

    // empty string is found
    if (strLen == 0)
        return begin;

    // a match is impossible -> return "not found"
    if ((begin >= len) || ((len - begin) < strLen))
        return size_t_max;

    // case-sensitive ?
    size_t maxI = len - strLen + 1;
    bool caseSensitive = isCaseSensitive();
    if ((strLen == 1) && caseSensitive)
    {
        char c = *str._s;
        auto lim = _s + len;
        for (auto p = _s + begin; p != lim; ++p)
        {
            if (*p == c)
                return (p - _s);
        }
    }
    else
    {
        if (caseSensitive)
        {
            // try to match at each possible location
            for (size_t i = begin; i != maxI; ++i)
            {
                if ((_s[i] == str._s[0]) && (_s[i + 1] == str._s[1]) &&
                    (::strncmp(_s + i + 2, str._s + 2, strLen - 2) == 0))
                {
                    return i;
                }
            }
        }
        else
        {
            // as above, but case-insensitive
            for (size_t i = begin; i != maxI; ++i)
            {
                if (::strncasecmp(_s + i, str._s, strLen) == 0)
                    return i;
            }
        }
    }

    return size_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
String::find(char c, size_t begin) const
{
    size_t len = length();
    const char* lim = _s + len;

    // a match is impossible -> return "not found"
    if (begin > len)
        return size_t_max;

    // case-sensitive ?
    bool caseSensitive = isCaseSensitive();
    if (caseSensitive)
    {
        for (const char* p = _s + begin; p <= lim; ++p)
        {
            if (*p == c)
                return (p - _s);
        }
    }
    else
    {
        c = tolower(c);
        for (const char* p = _s + begin; p <= lim; ++p)
        {
            if (tolower(*p) == c)
                return (p - _s);
        }
    }

    return size_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
String::findBM(const String& str, size_t begin) const
{
    size_t len = length(), strLen = str.length();

    // a match is impossible -> return "not found"
    if ((begin >= len) || (strLen == 0) || ((len - begin) < strLen))
    {
        return size_t_max;
    }

    // case-sensitive ?
    if (isCaseSensitive())
    {
        const char* match = boyerMooreSearch(_s + begin, len - begin, str._s, strLen);
        if (match == nullptr)
            return size_t_max;
        return match - _s;
    }
    else
    {
        String selfCopy = *this;
        String strCopy = str;
        selfCopy.toLower();
        strCopy.toLower();
        return selfCopy.findBM(strCopy, begin);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::subString(size_t begin, size_t len) const
{
    // set bounds
    size_t myLen = length(), end;
    setBounds(begin, end, len, myLen);

    // result is empty string if begin >= myLen
    if (len == 0)
        return String();

    // result is self if (begin == 0) and (len == myLen)
    if ((begin == 0) && (len == myLen))
        return self;

    char* str = new char[len + 1];
    strncpy(str, _s + begin, len);
    str[len] = '\0';

    return String(str, true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::reversed() const
{
    String res = self;
    res.reverse();
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::nextToken(size_t& idx, char delim, bool processQuotes) const
{
    String res;

    const char* s = _s + idx;
    const char* e;
    const char* lim = _s + length();

    // skip past leading whitespace
    while (isspace(*s))
        ++s;

    if (processQuotes)
    {
        res.reserve(64);
        while ((s != lim) && (*s != delim))
        {
            char c = *s;
            if ((c == '\'') || (c == '"'))
            {
                char q = c;
                ++s;
                while ((s != lim) && (*s != q))
                {
                    // find the first backslash or closing quote
                    for (e = s; (e != lim) && (*e != '\\') && (*e != q); ++e)
                        ;

                    // append the text up to the backslash or closing quote
                    res.append(s, e - s);
                    s = e;

                    // we stopped at a backslash?
                    if ((s != lim) && (*s == '\\'))
                    {
                        ++s;
                        res += *s++;
                    }
                }
                if (s != lim)
                    ++s;
            }
            else
            {
                for (e = s; (e != lim) && (*e != delim) && (*e != '\'') && (*e != '"'); ++e)
                    ;
                if ((e == lim) || (*e == delim))
                {
                    const char* ep = e;
                    for (--ep; (ep > s) && isspace(*ep); --ep)
                        ;
                    ++ep;
                    res.append(s, ep - s);
                }
                else
                {
                    res.append(s, e - s);
                }
                s = e;
            }
        }
    }
    else
    {
        for (e = s; (e != lim) && (*e != delim); ++e)
            ;
        const char* ep = e;
        for (--ep; (ep > s) && isspace(*ep); --ep)
            ;
        ++ep;
        res.append(s, ep - s);
        s = e;
    }

    idx = (s == lim) ? s - _s : s - _s + 1;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::excise()
{
    if (isOwner())
        delete[] _s;
    _s = &nullChar;
    _size = 0;
    _length = 0;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::economize()
{
    if (_size == 0)
        return;
    if (empty())
    {
        delete[] _s;
        _s = &nullChar;
        _size = 0;
        _length = 0;
        return;
    }
    size_t size = utl::nextMultipleOfPow2((size_t)8, length() + 1);
    if (_size == size)
        return;
    char* s = new char[size];
    memcpy(s, _s, size);
    delete[] _s;
    _s = s;
    _size = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::append(const char c)
{
    // do nothing?
    if (c == '\0')
        return self;

    // grow to a sufficient length
    size_t len = length();
    reserve(len + 2, size_t_max);

    // add the character
    _s[len] = c;
    _s[len + 1] = '\0';
    ++_length;

    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::append(const char* s, size_t len)
{
    ASSERTD(s != nullptr);
    ASSERTD(strlen(s) >= len);
    size_t myLen = length();

    // appending empty string -> just return self
    if (len == 0)
        return self;

    // concatenate s[0,len) to self
    reserve(myLen + len + 1, size_t_max);
    memcpy(_s + myLen, s, len);
    _length += len;
    _s[_length] = '\0';
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::chop(size_t begin, size_t len)
{
    // set bounds
    size_t myLen = length(), end;
    setBounds(begin, end, len, myLen);

    // the result is the substring
    String res = subString(begin, len);

    // chop out the result
    assertOwner();
    size_t num = myLen - end + 1;
    ::memmove(_s + begin, _s + end, num);
    _length -= (end - begin);

    // return the substring
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::remove(size_t begin, size_t len)
{
    // set bounds
    size_t myLen = length(), end;
    setBounds(begin, end, len, myLen);

    // chop out the result
    assertOwner();
    size_t num = myLen - end + 1;
    ::memmove(_s + begin, _s + end, num);
    _length -= (end - begin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::replace(const String& lhs, const String& rhs)
{
    // inefficient if many replacements are needed
    size_t lhsLen = lhs.length(), rhsLen = rhs.length();
    for (size_t idx = find(lhs); idx != size_t_max; idx = find(lhs, idx))
    {
        replace(idx, lhsLen, rhs);
        idx += rhsLen;
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::replace(char lhs, char rhs)
{
    auto myLen = length();
    auto lim = _s + myLen;
    for (auto ptr = _s; ptr != lim; ++ptr)
    {
        if (*ptr == lhs)
        {
            *ptr = rhs;
        }
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::replace(size_t begin, size_t len, const String& str)
{
    // set bounds
    size_t strLen = str.length();
    size_t myLen = length(), end;
    setBounds(begin, end, len, myLen);

    // grow to the required size
    ssize_t growth = (strLen - len);
    size_t newLen = myLen + growth;
    reserve(newLen + 1);

    // move characters at the end as needed
    if ((growth != 0) && (end < myLen))
    {
        ::memmove(_s + end + growth, _s + end, myLen - end);
    }

    // copy str into _s[]
    strncpy(_s + begin, str._s, strLen);
    _length += growth;
    _s[_length] = '\0';

    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::reverse()
{
    assertOwner();
    char* lp = _s;
    char* rp = _s + length() - 1;
    while (lp < rp)
    {
        char tmp = *lp;
        *lp++ = *rp;
        *rp-- = tmp;
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::set(const char* s_, bool owner, bool duplicate, size_t length)
{
    char* s = const_cast<char*>(s_);

    // do not make a copy if we will not own it
    if (!owner)
        duplicate = false;

    // s is nullptr or empty?
    if ((s == nullptr) || (*s == '\0'))
    {
        if (_size == 0)
        {
            _s = &nullChar;
        }
        else
        {
            *_s = '\0';
        }
        _length = 0;
        if (owner && !duplicate && (s != nullptr))
            delete[] s;
        return;
    }

    // calculate (or copy) the new length, and note the required allocation size
    _length = length = (length == size_t_max) ? strlen(s) : length;
    size_t reqSize = length + 1;

    // copy the string (or just copy its address)
    if (owner)
    {
        // possibly copy the string
        if (duplicate)
        {
            if (reqSize <= _size)
            {
                memcpy(_s, s, length);
            }
            else
            {
                // nuke existing string if we own it
                if (_size > 0)
                    delete[] _s;

                // make new size large enough to contain the string, and a multiple of 8
                _size = nextMultipleOfPow2((size_t)8, reqSize);
                _s = new char[_size];

                // copy into the new allocation
                memcpy(_s, s, length);
            }
            _s[length] = '\0';
        }
        else
        {
            if (_size > 0)
                delete[] _s;
            _s = s;
            _size = reqSize;
        }
    }
    else
    {
        if (_size > 0)
            delete[] _s;
        _s = s;
        _size = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::toLower(size_t begin, size_t len)
{
    assertOwner();

    // Set bounds
    size_t myLen = length(), end;
    setBounds(begin, end, len, myLen);

    // for each character in the substring...
    auto lim = _s + end;
    for (auto p = _s + begin; p != lim; ++p)
    {
        if ((*p >= 'A') && (*p <= 'Z'))
            *p = tolower(*p);
    }

    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::toUpper(size_t begin, size_t len)
{
    // set bounds
    size_t myLen = length(), end;
    setBounds(begin, end, len, myLen);

    // for each character in the substring...
    auto lim = _s + end;
    for (auto p = _s + begin; p != lim; ++p)
    {
        if ((*p >= 'a') && (*p <= 'z'))
            *p = toupper(*p);
    }

    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::padBegin(size_t len, char c)
{
    size_t myLen = length();
    if (len > myLen)
        self = repeat(c, len - myLen) + self;
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::padEnd(size_t len, char c)
{
    size_t myLen = length();
    if (len > myLen)
        self += repeat(c, len - myLen);
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::trim()
{
    if (empty())
        return self;
    trimBegin();
    trimEnd();
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::trimBegin()
{
    if (empty())
        return self;
    assertOwner();

    // chopSize = # of space characters @ start of string
    char* p;
    for (p = _s; isspace(*p); ++p)
        ;
    size_t chopSize = (p - _s);

    // chop out chopSize chars from start of string
    if (chopSize > 0)
    {
        length();
        memmove(_s, _s + chopSize, _length + 1 - chopSize);
        _length -= chopSize;
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
String::trimEnd()
{
    if (empty())
        return self;
    assertOwner();

    // get rid of space characters from the end of the string
    length();
    char* p;
    for (p = _s + _length - 1; (p >= _s) && isspace(*p); --p)
        ;
    *++p = '\0';
    _length = (p - _s);
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::repeat(char c, size_t num)
{
    String res;
    size_t i;
    for (i = 0; i != num; i++)
        res += c;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
String::spaces(size_t num)
{
    return repeat(' ', num);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::_assertOwner()
{
    ASSERTD(_size == 0);
    if (_length == size_t_max)
        _length = strlen(_s);
    size_t size = utl::nextMultipleOfPow2((size_t)8, _length + 1);
    char* s = new char[size];
    memcpy(s, _s, _length + 1);
    _s = s;
    _size = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
String::_assertOwner(size_t size, size_t increment)
{
    ASSERTD(size != 0);
    ASSERTD(increment != 0);

    // do nothing?
    if (size <= _size)
        return;

    // make sure ownership flag is set
    bool wasOwner = isOwner();
    setOwner(true);

    // determine new size of _s[]
    size_t newSize;
    if (increment == size_t_max)
    {
        newSize = nextPow2(size);
    }
    else
    {
        ASSERTD(increment == nextPow2(increment));
        newSize = nextMultipleOfPow2(increment, size);
    }

    // grow the character array to newSize
    ASSERTD(length() < newSize);
    auto s = new char[newSize];
    memcpy(s, _s, length() + 1);
    if (wasOwner)
        delete[] _s;
    _s = s;
    _size = newSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char String::nullChar = '\0';
const String emptyString;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
