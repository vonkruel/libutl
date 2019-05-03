#include <libutl/libutl.h>
#include <libutl/BitArray.h>
#include <libutl/Pair.h>
#include <libutl/RBtree.h>
#include <libutl/URI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::URI);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::copy(const Object& rhs)
{
    // copy a String by calling set()
    if (rhs.isA(String))
    {
        auto& s = utl::cast<String>(rhs);
        set(s);
        return;
    }

    // otherwise rhs must be a URI
    auto& uri = utl::cast<URI>(rhs);
    clear();
    _scheme = utl::clone(uri._scheme);
    _username = utl::clone(uri._username);
    _password = utl::clone(uri._password);
    _hostname = utl::clone(uri._hostname);
    _port = uri._port;
    _path = utl::clone(uri._path);
    _filename = utl::clone(uri._filename);
    _extension = utl::clone(uri._extension);
    _queryVars = utl::clone(uri._queryVars);
    _fragment = utl::clone(uri._fragment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::steal(Object& rhs)
{
    auto& rhsURI = utl::cast<URI>(rhs);
    _scheme = rhsURI._scheme;
    _username = rhsURI._username;
    _password = rhsURI._password;
    _hostname = rhsURI._hostname;
    _port = rhsURI._port;
    _path = rhsURI._path;
    _filename = rhsURI._filename;
    _extension = rhsURI._extension;
    _queryVars = rhsURI._queryVars;
    _fragment = rhsURI._fragment;
    rhsURI.init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
URI::compare(const Object& rhs) const
{
    // rhs is a String -> compare with URI
    // (and reverse the comparison which will help if rhs is a Regex for ex.)
    if (rhs.isA(String))
        return -1 * rhs.compare(get());

    // rhs is a URI -> compare attributes in normalized form
    auto& uri = utl::cast<URI>(rhs);
    int res;
    res = utl::compareNullable(_scheme, uri._scheme);
    if (res != 0)
        return res;
    res = utl::compareNullable(_username, uri._username);
    if (res != 0)
        return res;
    res = utl::compareNullable(_password, uri._password);
    if (res != 0)
        return res;
    res = utl::compareNullable(_hostname, uri._hostname);
    if (res != 0)
        return res;
    res = utl::compare(_port, uri._port);
    if (res != 0)
        return res;
    res = utl::compareNullable(_path, uri._path);
    if (res != 0)
        return res;
    res = utl::compareNullable(_filename, uri._filename);
    if (res != 0)
        return res;
    res = utl::compareNullable(_extension, uri._extension);
    if (res != 0)
        return res;
    res = utl::compareNullable(_queryVars, uri._queryVars);
    if (res != 0)
        return res;
    res = utl::compareNullable(_fragment, uri._fragment);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::toString() const
{
    return get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
URI::innerAllocatedSize() const
{
    size_t sz = 0;
    if (_scheme != nullptr)
        sz += _scheme->allocatedSize();
    if (_username != nullptr)
        sz += _username->allocatedSize();
    if (_password != nullptr)
        sz += _password->allocatedSize();
    if (_hostname != nullptr)
        sz += _hostname->allocatedSize();
    if (_path != nullptr)
        sz += _path->allocatedSize();
    if (_filename != nullptr)
        sz += _filename->allocatedSize();
    if (_extension != nullptr)
        sz += _extension->allocatedSize();
    if (_queryVars != nullptr)
        sz += _queryVars->allocatedSize();
    if (_fragment != nullptr)
        sz += _fragment->allocatedSize();
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::get() const
{
    String uri;
    if (!isRelative())
    {
        uri += scheme();
        uri += "://";
        uri += authority();
    }
    uri += percentEncode(path(), true);
    uri += percentEncode(filename());
    if (_extension != nullptr)
    {
        uri += '.';
        uri += percentEncode(extension());
    }
    if (!isRelative() && (_queryVars != nullptr))
    {
        uri += '?';
        uri += query();
    }
    if (!fragment().empty())
    {
        uri += '#';
        uri += percentEncode(fragment());
    }
    return uri;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::authority() const
{
    String auth = userinfo();
    auth += hostname();
    if (_port != 0)
    {
        auth += ':';
        auth += Uint(_port).toString();
    }
    return auth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::userinfo() const
{
    if ((_username == nullptr) && (_password == nullptr))
        return emptyString;
    String ui;
    if (_username != nullptr)
        ui += percentEncode(*_username);
    if (_password != nullptr)
    {
        ui += ":";
        ui += percentEncode(*_password);
    }
    ui += "@";
    return ui;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::fullPath() const
{
    return path() + fullFilename();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::fullFilename() const
{
    String fn = filename();
    if (_extension != nullptr)
    {
        fn += '.';
        fn += extension();
    }
    return fn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
URI::numSegments() const
{
    const String& path = this->path();
    if (path.empty())
        return 0;
    size_t n = 0;
    const char* c = path.get();
    const char* lim = c + path.length();
    if (*c == '/')
        ++c;
    while (c < lim)
    {
        if (*c++ == '/')
            ++n;
    }
    return n;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::segment(size_t id) const
{
    if ((id == 0) || (_path == nullptr))
        return emptyString;
    const String& path = this->path();
    size_t sid, ss, nextSlashIdx, len = path.length();
    for (sid = 1, ss = 0; ss < len; ++sid, ss = nextSlashIdx)
    {
        nextSlashIdx = path.find("/", ss + 1);
        if (id != sid)
            continue;
        if (path[ss] == '/')
            ++ss;
        return path.subString(ss, (nextSlashIdx == size_t_max) ? size_t_max : nextSlashIdx - ss);
    }
    return emptyString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::query() const
{
    // no vars?
    if (_queryVars == nullptr)
        return emptyString;

    // build the query string
    String query;
    for (auto pair_ : *_queryVars)
    {
        Pair& pair = utl::cast<Pair>(*pair_);
        auto var = utl::cast<String>(pair.first());
        auto val = utl::cast<String>(pair.second());
        if (!query.empty())
            query += '&';
        query += percentEncode(*var);
        query += '=';
        query += percentEncode(*val);
    }

    return query;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
URI::queryVarsCount() const
{
    return (_queryVars == nullptr) ? 0 : _queryVars->size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::queryVarNameValue(size_t id, String& name, String& value) const
{
    // no such variable?
    ASSERTD((id != 0) && (id <= queryVarsCount()));

    // find the variable
    Pair& pair = utl::cast<Pair>(*_queryVars->get(id - 1));
    name = *utl::cast<String>(pair.first());
    value = *utl::cast<String>(pair.second());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::queryVarsCopy(Collection& col) const
{
    if (_queryVars == nullptr)
        return;
    col.copyItems(*_queryVars);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::clear()
{
    delete _scheme;
    _scheme = nullptr;
    delete _username;
    _username = nullptr;
    delete _password;
    _password = nullptr;
    delete _hostname;
    _hostname = nullptr;
    delete _path;
    _path = nullptr;
    delete _filename;
    _filename = nullptr;
    delete _extension;
    _extension = nullptr;
    delete _queryVars;
    _queryVars = nullptr;
    delete _fragment;
    _fragment = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::set(const String& uri)
{
    clear();

    // parse authority
    size_t i = uri.find("://");
    if (i != size_t_max)
    {
        _scheme = uri.subString(0, i).clone();
        i += 3;

        // userinfo is given?
        size_t atIdx = uri.find("@", i);
        if (atIdx != size_t_max)
        {
            String userinfo = percentDecode(uri.subString(i, atIdx - i));
            i = atIdx + 1;
            size_t colonIdx = userinfo.find(":");
            _username = userinfo.subString(0, colonIdx).clone();
            if (colonIdx != size_t_max)
            {
                _password = userinfo.subString(colonIdx + 1).clone();
            }
        }

        // hostname
        size_t colonIdx = uri.find(":", i);
        size_t slashIdx = uri.find("/", i);
        if (slashIdx == size_t_max)
            throw IllegalValueEx(uri);
        if (colonIdx == size_t_max)
        {
            _hostname = uri.subString(i, slashIdx - i).clone();
        }
        else
        {
            _hostname = uri.subString(i, colonIdx - i).clone();
            i = colonIdx + 1;
            _port = Uint(uri.subString(i, slashIdx - i));
        }
        i = slashIdx;
    }
    else
    {
        i = 0;
    }

    // set queryIdx, fragmentIdx
    size_t queryIdx = uri.find("?", i);
    if (isRelative() && (queryIdx != size_t_max))
        throw IllegalValueEx(uri);
    size_t fragmentIdx = uri.find("#", i);
    if ((queryIdx != size_t_max) && (fragmentIdx < queryIdx))
    {
        throw IllegalValueEx(uri);
    }

    // parse path
    size_t pathEndIdx = utl::min(queryIdx, fragmentIdx);
    pathEndIdx = utl::min(uri.length(), pathEndIdx);
    String path = percentDecode(uri.subString(i, pathEndIdx - i));
    setFullPath(path);
    i = pathEndIdx;

    // parse query
    if (queryIdx != size_t_max)
    {
        ASSERTD(i == queryIdx);
        size_t queryEndIdx = utl::min(fragmentIdx, uri.length());
        while (i < queryEndIdx)
        {
            ++i;
            ++queryIdx;
            size_t semicolonIdx = uri.find(";", i);
            size_t ampersandIdx = uri.find("&", i);
            size_t pairEndIdx = min(semicolonIdx, ampersandIdx);
            pairEndIdx = min(pairEndIdx, queryEndIdx);
            String pair = uri.subString(i, pairEndIdx - i);
            size_t equalsIdx = pair.find("=");
            if (equalsIdx == size_t_max)
                throw IllegalValueEx(uri);
            String name = percentDecode(pair.subString(0, equalsIdx));
            String value = percentDecode(pair.subString(equalsIdx + 1));
            addQueryVariable(name, value);
            i = pairEndIdx;
        }
    }

    // parse fragment
    if (fragmentIdx != size_t_max)
    {
        _fragment = percentDecode(uri.subString(fragmentIdx + 1)).clone();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::appendSlash()
{
    if ((_filename == nullptr) && (_extension == nullptr))
        return;
    setFullPath(fullPath() + '/');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::removeSlash()
{
    if ((_filename != nullptr) || (_extension != nullptr))
        return;
    String fp = this->fullPath();
    setFullPath(fp.subString(0, fp.length() - 1));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::setFullPath(const String& fp)
{
    String path = fp, filename, extension;
    while (path.find("//") != size_t_max)
    {
        path.replace("//", "/");
    }
    path.replace("/./", "/");
    for (size_t idx = path.find("/../"); idx != size_t_max; idx = path.find("/../"))
    {
        size_t endIdx = idx + 4;
        const char* s = path.get();
        for (--idx; (idx != size_t_max) && (s[idx] != '/'); --idx)
            ;
        if (idx == size_t_max)
            break;
        ++idx;
        path.remove(idx, endIdx - idx);
    }
    delete _path;
    _path = nullptr;
    delete _filename;
    _filename = nullptr;
    delete _extension;
    _extension = nullptr;
    if (path.empty())
        throw IllegalValueEx(fp);
    if (path.lastChar() != '/')
    {
        int i;
        for (i = path.length() - 1; (i >= 0) && (path[i] != '/'); --i)
            ;
        if (i == -1)
        {
            filename = path;
            path.clear();
        }
        else
        {
            filename = path.subString(i + 1);
            path = path.subString(0, i + 1);
        }
        for (i = filename.length() - 1; (i >= 0) && (filename[i] != '.'); --i)
            ;
        if (i != -1)
        {
            extension = filename.subString(i + 1);
            filename = filename.subString(0, i);
        }
    }
    if (!path.empty())
        _path = path.clone();
    if (!filename.empty())
        _filename = filename.clone();
    if (!extension.empty())
        _extension = extension.clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::addQueryVariable(const String& name, const String& value)
{
    if (_queryVars == nullptr)
        _queryVars = new Array();
    String* nameCopy = name.clone();
    String* valueCopy = value.clone();
    nameCopy->assertOwner();
    valueCopy->assertOwner();
    *_queryVars += new Pair(nameCopy, valueCopy);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::init()
{
    _scheme = nullptr;
    _username = nullptr;
    _password = nullptr;
    _hostname = nullptr;
    _port = 0;
    _path = nullptr;
    _filename = nullptr;
    _extension = nullptr;
    _queryVars = nullptr;
    _fragment = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::deInit()
{
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::percentEncode(const String& str, bool isPath)
{
    String res;
    size_t len = str.length();
    for (size_t i = 0; i < len; ++i)
    {
        byte_t c = (byte_t)str[i];
        if (_unreservedChars->get(c) || (isPath && (c == '/')))
        {
            res += (char)c;
        }
        else
        {
            res += '%';
            String hex = Uint(c).toHex(2);
            hex.toUpper();
            res += hex;
        }
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
URI::percentDecode(const String& str)
{
    String res;
    size_t len = str.length();
    for (size_t i = 0; i < len; ++i)
    {
        byte_t c = (byte_t)str[i];
        if ((c == (byte_t)'%') && (i < (len - 2)))
        {
            Uint ui;
            ui.setHex(str.subString(i + 1, 2));
            c = (byte_t)ui.get();
            i += 2;
        }
        res += (char)c;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::utl_init()
{
    _unreservedChars = new BitArray(256, 1);
    size_t i;
    for (i = 0; i < 128; ++i)
    {
        // it's an unreserved char?
        char c = (char)i;
        bool isUnreserved =
            (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) ||
             ((c >= '0') && (c <= '9')) || (c == '-') || (c == '_') || (c == '.') || (c == '~'));
        _unreservedChars->set(i, isUnreserved);
    }

    // chars >= 128 are not unreserved
    for (; i < 256; ++i)
    {
        _unreservedChars->set(i, false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
URI::utl_deInit()
{
    delete _unreservedChars;
    _unreservedChars = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BitArray* URI::_unreservedChars;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
