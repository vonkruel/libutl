#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef new
#include <filesystem>
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Filesystem pathname.

   The various components of a pathname are illustrated by example:

   \code
   Pathname path("/foo/bar/foobar.txt");
   assert(path.directory() == "/foo/bar/");
   assert(path.filename() == "foobar.txt");
   assert(path.basename() == "/foo/bar/foobar");
   assert(path.firstComponent() == "/foo/");
   assert(path.lastComponent() == "foobar.txt");
   assert(path.suffix() == "txt");
   \endcode

   \author Adam McKee
   \ingroup filesystem
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Pathname : public String
{
    UTL_CLASS_DECL(Pathname, String);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param pathname path
    */
    Pathname(const String& pathname)
        : String(pathname)
    {
    }

    /**
       Constructor.
       \param pathname path
    */
    Pathname(const char* pathname)
        : String(pathname)
    {
    }

    /**
       Constructor.
       \param pathname path
    */
    Pathname(const std::filesystem::path& pathname);

    /** Convert to `std::filesystem::path`. */
    std::filesystem::path fs_path() const;

    /** Convert to `std::filesystem::path`. */
    operator std::filesystem::path() const;

    /** Convert to native pathname string. */
    String native() const;

    /** Get the directory. */
    Pathname directory() const;

    /** Chop out and get the directory. */
    Pathname chopDirectory();

    /** Get the filename. */
    Pathname filename() const;

    /** Chop out and get the filename. */
    Pathname chopFilename();

    /** Get the basename. */
    Pathname basename() const;

    /** Chop out and get the basename. */
    Pathname chopBasename();

    /** Get the suffix. */
    String suffix() const;

    /** Chop out and get the suffix. */
    String chopSuffix();

    /** Get the first component. */
    Pathname firstComponent() const;

    /** Chop out and get the first component. */
    Pathname chopFirstComponent();

    /** Get the last component. */
    Pathname lastComponent() const;

    /** Chop out and get the last component. */
    Pathname chopLastComponent();

    /** Absolute pathname? */
    bool isAbsolute() const;

    /** Relative pathname? */
    bool
    isRelative() const
    {
        return !isAbsolute();
    }

    /**
       If the pathname is empty or already ends in a separator, do nothing.
       Otherwise, append a separator ('/' under UNIX) to the path.
       \see HostOS::getPathSeparator
    */
    void appendSeparator();

    /**
       Remove any trailing separator from the path.
    */
    void removeTrailingSeparator();

    /**
       Normalize the path by removing extra separators and pointless round trips.
       For example: "/foo//bar///..//foobar.txt" -> "/foo/foobar.txt"
    */
    void normalize();

    /** Perform initialization at program startup (called by utl::init()). */
    static void utl_init();

public:
    constexpr static char separator = '/';
#if UTL_HOST_TYPE == UTL_HT_UNIX
    constexpr static char native_separator = '/';
#else
    constexpr static char native_separator = '\\';
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
