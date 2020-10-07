#include <libutl/libutl.h>
#include <libutl/HostOS.h>
#include <libutl/Pathname.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace fs = std::filesystem;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Pathname);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname::Pathname(const fs::path& pathname)
{
    set(pathname.generic_u8string().c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

fs::path
Pathname::fs_path() const
{
    return fs::path(get(), fs::path::generic_format);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname::operator fs::path() const
{
    return fs::path(get(), fs::path::generic_format);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Pathname::native() const
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    return self;
#else
    return String(self).replace(separator, native_separator);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::directory() const
{
    if (empty())
        return Pathname();

    const char* ptr;
    const char* str = get();
    const char* strEnd = str + length();
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != separator); ptr--)
        ;

    if (*ptr == separator)
    {
        return Pathname(subString(0, ptr - str + 1));
    }

    return Pathname();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::chopDirectory()
{
    if (empty())
        return Pathname();

    const char* ptr;
    const char* str = get();
    const char* strEnd = str + length();
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != separator); ptr--)
        ;

    if (*ptr == separator)
    {
        return chop(0, ptr - str + 1);
    }

    return Pathname();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::filename() const
{
    if (empty())
        return Pathname();

    const char* ptr;
    const char* str = get();
    const char* strEnd = str + length();
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != separator); ptr--)
        ;

    if (*ptr == separator)
    {
        return subString(ptr - str + 1, strEnd - ptr);
    }
    else
    {
        return self;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::chopFilename()
{
    if (empty())
        return Pathname();

    const char* ptr;
    const char* str = get();
    const char* strEnd = str + length();
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != separator); ptr--)
        ;

    if (*ptr == separator)
    {
        return chop(ptr - str + 1, strEnd - ptr);
    }
    else
    {
        Pathname filename = self;
        self.clear();
        return filename;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::basename() const
{
    if (empty())
        return Pathname();

    size_t len = length();
    const char* ptr;
    const char* str = get();
    const char* strEnd = str + len;
    size_t dotPos = len;
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != separator); ptr--)
    {
        if ((*ptr == '.') && (dotPos == len))
        {
            dotPos = ptr - str;
        }
    }

    return subString(0, dotPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::chopBasename()
{
    if (empty())
        return Pathname();

    size_t len = length();
    const char* ptr;
    const char* str = get();
    const char* strEnd = str + len;
    size_t dotPos = len;
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != separator); ptr--)
    {
        if ((*ptr == '.') && (dotPos == len))
        {
            dotPos = ptr - str;
        }
    }

    return chop(0, dotPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::firstComponent() const
{
    if (empty())
        return Pathname();

    const char* str = get();
    const char* ptr = str;

    // skip past leading slash
    if (*ptr == separator)
        ++ptr;

    const char* ptrLim = str + length();
    for (; (ptr != ptrLim) && (*ptr != separator); ptr++)
        ;

    if (*ptr == separator)
    {
        return subString(0, ptr - str + 1);
    }
    else
    {
        return self;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::chopFirstComponent()
{
    if (empty())
        return Pathname();

    const char* str = get();
    const char* ptr = str;

    // skip past leading slash
    if (*ptr == separator)
        ++ptr;

    const char* ptrLim = str + length();
    for (; (ptr != ptrLim) && (*ptr != separator); ptr++)
        ;

    if (*ptr == separator)
    {
        return chop(0, ptr - str + 1);
    }
    else
    {
        Pathname firstComponent = self;
        self.clear();
        return firstComponent;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::lastComponent() const
{
    if (empty())
        return Pathname();

    const char* str = get();
    const char* strEnd = str + length();
    const char* ptr = strEnd - 1;

    // skip past trailing slash
    if (*ptr == separator)
        --ptr;

    for (; (ptr != str) && (*ptr != separator); ptr--)
        ;

    if (*ptr == separator)
    {
        return subString(ptr - str + 1, strEnd - ptr - 1);
    }
    else
    {
        return self;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pathname
Pathname::chopLastComponent()
{
    if (empty())
        return Pathname();

    const char* str = get();
    const char* strEnd = str + length();
    const char* ptr = strEnd - 1;

    // skip past trailing slash
    if (*ptr == separator)
        --ptr;

    for (; (ptr != str) && (*ptr != separator); ptr--)
        ;

    if (*ptr == separator)
    {
        return chop(ptr - str + 1, strEnd - ptr - 1);
    }
    else
    {
        Pathname lastComponent = self;
        self.clear();
        return lastComponent;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Pathname::isAbsolute() const
{
    const char* s = get();
    if (s[0] == separator)
        return true;
#if UTL_HOST_OS == UTL_OS_WINDOWS
    if (isalpha(s[0]) && (s[1] == ':') && (s[2] == separator))
        return true;
#endif
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Pathname::suffix() const
{
    String suffix;

    // empty path -> empty suffix
    if (empty())
        return suffix;

    const char* ptr;
    const char* str = get();
    const char* strEnd = str + length();
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != '.'); ptr--)
        ;

    if (*ptr == '.')
    {
        return subString(ptr - str + 1, strEnd - ptr);
    }

    return Pathname();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Pathname::chopSuffix()
{
    String suffix;

    // empty path -> empty suffix
    if (empty())
        return suffix;

    const char* ptr;
    const char* str = get();
    const char* strEnd = str + length();
    for (ptr = strEnd - 1; (ptr != str) && (*ptr != '.'); ptr--)
        ;

    if (*ptr == '.')
    {
        return chop(ptr - str + 1, strEnd - ptr);
    }

    return Pathname();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pathname::appendSeparator()
{
    // can't turn empty path into root path
    size_t len = length();
    if (len == 0)
        return;

    // don't add a separator if there already is one
    if (self[len - 1] == separator)
        return;

    // append separator
    append(separator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pathname::removeTrailingSeparator()
{
    char* str = get();
    auto len = length();

    // "" is left as-is
    if (len == 0)
        return;

    // "/" is left as-is
    if ((str[0] == separator) && (str[1] == '\0'))
        return;

        // Windows: "x:/" is left as-is
#if UTL_HOST_TYPE == UTL_HT_WINDOWS
    if ((str[1] == ':') && (str[2] == separator) && (str[3] == '\0'))
        return;
#endif

    // idx = index of the last non-separator character
    size_t idx;
    for (idx = len - 1; str[idx] == separator; --idx)
        ;

    // idx = index of first separator in string of trailing separators
    ++idx;

    // idx is the new length
    str[idx] = '\0';
    _length = idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pathname::normalize()
{
    // Windows: replace '\' with '/'
#if UTL_HOST_TYPE == UTL_HT_WINDOWS
    replace(native_separator, separator);
#endif

    // coalesce multiple consecutive separators into one
    char sep[2] = {separator, '\0'};
    char sepsep[3] = {separator, separator, '\0'};
    while (find(sepsep) != size_t_max)
    {
        replace(sepsep, separator);
    }

    // "/" is normalized
    char* s = get();
    if ((s[0] == separator) && (s[1] == '\0'))
        return;

    // "x:/" is normalized
    if (isalpha(s[0]) && (s[1] == ':') && (s[2] == separator) && (s[3] == '\0'))
        return;

    // add a separator to the end
    appendSeparator();

    // replace "/./" with "/"
    char sepdotsep[4] = {separator, '.', separator, '\0'};
    replace(sepdotsep, sep);

    // iteratively replace "foo/../" with ""
    char sepdotdotsep[5] = {separator, '.', '.', separator, '\0'};
    size_t findPos;
    for (size_t idx = find(sepdotdotsep); idx != size_t_max; idx = find(sepdotdotsep, findPos))
    {
        // "foo/bar/../"
        //         ^   ^
        //         |   |
        //       idx   endIdx
        size_t endIdx = idx + 4;

        // "foo/bar/../"
        //      ^      ^
        //      |      |
        //    idx      endIdx
        for (s = get(), --idx; (idx != size_t_max) && (s[idx] != separator); --idx)
            ;
        ++idx;

        // found a preceding name to cancel out "/../" ?
        if (((endIdx - idx) > 4) && (s[idx] != '.'))
        {
            chop(idx, endIdx - idx);
            findPos = idx;
        }
        else
        {
            findPos = endIdx;
        }
    }

    // finally, remove trailing separator
    removeTrailingSeparator();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr char Pathname::separator;
constexpr char Pathname::native_separator;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
