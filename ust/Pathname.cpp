#include <libutl/libutl.h>
#include <libutl/HostOS.h>
#include <libutl/Pathname.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Pathname);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

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
    // can't turn empty path into root
    size_t len = length();
    if (len == 0)
        return;

    // don't add a slash if there already is one
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
    if ((str[0] == separator) && (str[1] == '\0'))
        return;
    if ((str[1] == ':') && (str[2] == separator) && (str[3] == '\0'))
        return;
    for (size_t len = length(); (len != 0) && (str[len - 1] == separator); str[len-- - 1] = '\0')
        ;
    lengthInvalidate();
    length();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pathname::normalize()
{
    // coalesce multiple consecutive slashes into one
    String sep = separator;
    String sepsep = super::repeat(separator, 2);
    while (find(sepsep) != size_t_max)
    {
        replace(sepsep, sep);
    }

    // "c:" and "c:\" are already normalized
    char* s = get();
    if ((s[0] == separator) && (s[1] == '\0'))
        return;
    if (isalpha(s[0]) && (s[1] == ':') && (s[2] == separator) && (s[3] == '\0'))
        return;

    // add a separator to the end
    appendSeparator();

    // replace "/./" with "/"
    String sepdotsep = sep + "." + sep;
    replace(sepdotsep, sep);

    // replace "/foo/../" with "/"
    String sepdotdotsep = sep + ".." + sep;
    for (size_t idx = find(sepdotdotsep); idx != size_t_max; idx = find(sepdotdotsep))
    {
        size_t endIdx = idx + 4;
        for (s = get(), --idx; (idx != size_t_max) && (s[idx] != separator); --idx)
            ;
        if (idx == size_t_max)
            break;
        ++idx;
        chop(idx, endIdx - idx);
    }

    // finally, remove trailing separator
    removeTrailingSeparator();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr char Pathname::separator;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
