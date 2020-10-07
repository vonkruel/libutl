#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FlagsMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Character string.

   A string is a <b>nul</b>-terminated array of characters.

   <b>Attributes</b>

   \arg <b><i>owner</i> flag</b> : True iff self owns the underlying string.

   \arg <b><i>case-sensitive</i> flag</b> : When two strings are compared, the comparison will be
   case-sensitive iff both strings have the \b case-sensitive flag asserted.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class String
    : public Object
    , public FlagsMI
{
    UTL_CLASS_DECL(String, Object);

public:
    /**
       Constructor.
       \param s character array
       \param owner (optional : true) ownership flag
       \param duplicate (optional : true) make a copy? (implies ownership)
       \param caseSensitive (optional : true) case sensitivity flag
    */
    String(const char* s, bool owner = true, bool duplicate = true, bool caseSensitive = true);

    /**
       Constructor
       \param size initial size of character array
    */
    String(size_t size);

    /**
       Constructor.
       \param ch character
    */
    String(char ch)
    {
        _s = new char[8];
        _s[0] = ch;
        _s[1] = '\0';
        _size = 8;
        _length = size_t_max;
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    virtual size_t hash(size_t size) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual size_t innerAllocatedSize() const;

    virtual String toString() const;

    /// \name Flags
    //@{
    /** Get the ownership flag. */
    bool
    isOwner() const
    {
        return (_size != 0);
    }

    /** Set the ownership flag (without doing anything else). */
    void
    setOwner(bool owner)
    {
        if (owner)
        {
            if (_size == 0)
                _size = length() + 1;
        }
        else
        {
            _size = 0;
        }
    }

    /** Get the \b case-sensitive flag. */
    bool
    isCaseSensitive() const
    {
        return !getFlag(flg_caseInsensitive);
    }

    /** Set the \b case-sensitive flag. */
    void
    setCaseSensitive(bool caseSensitive)
    {
        setFlag(flg_caseInsensitive, !caseSensitive);
    }

    /** Get the \b marked flag. */
    bool
    isMarked() const
    {
        return getFlag(flg_marked);
    }

    /** Set the \b marked flag. */
    void
    setMarked(bool marked = true)
    {
        setFlag(flg_marked, marked);
    }
    //@}

    /// \name Comparison
    //@{
    /** Compare the given string against a substring of self. */
    int compareSubstring(size_t begin, const String& rhs, size_t n);

    /** Compare the given string against a substring of self. */
    int
    compareSubstring(size_t begin, const char* rhs, size_t len)
    {
        return compareSubstring(begin, String(rhs, false), len);
    }

    /** Compare the given string against the beginning of self. */
    int comparePrefix(const String& rhs) const;

    /** Compare the given string against the beginning of self. */
    int
    comparePrefix(const char* rhs) const
    {
        return comparePrefix(String(rhs, false));
    }

    /** Compare the given string against the end of self. */
    int compareSuffix(const String& rhs) const;

    /** Compare the given string against the end of self. */
    int
    compareSuffix(const char* rhs) const
    {
        return compareSuffix(String(rhs, false));
    }

    /** Compare two strings (case sensitive comparison iff isCaseSensitive()). */
    inline int
    strcmp(const char* lhs, const char* rhs) const
    {
        if (isCaseSensitive())
        {
            int d = (int)*lhs - (int)*rhs;
            if (d != 0)
                return d;
            return ::strcmp(lhs + 1, rhs + 1);
        }
        else
        {
            int d = tolower(*lhs) - tolower(*rhs);
            if (d != 0)
                return d;
            return ::strcasecmp(lhs + 1, rhs + 1);
        }
    }

    /** Get the string comparison function (strcmp or strcasecmp). */
    inline std::function<int(const char*, const char*)>
    strcmp() const
    {
        return isCaseSensitive() ? ::strcmp : ::strcasecmp;
    }

    /** Compare (up to) the first n bytes of two strings (case sensitive iff isCaseSensitive()). */
    inline int
    strncmp(const char* lhs, const char* rhs, size_t n) const
    {
        return isCaseSensitive() ? ::strncmp(lhs, rhs, n) : ::strncasecmp(lhs, rhs, n);
    }

    /** Get the length-limited string comparison function (\b strncmp or \b strncasecmp). */
    inline std::function<int(const char*, const char*, size_t)>
    strncmp() const
    {
        return isCaseSensitive() ? ::strncmp : ::strncasecmp;
    }
    //@}

    /// \name Read-only (const) Operations
    //@{
    /** Backslash-escape a string. */
    String
    backslashEscaped(const char* specials) const
    {
        return backslashEscaped(String(specials, false));
    }

    /** Backslash-escape a string. */
    String backslashEscaped(const String& specials) const;

    /** Backslash-unescape a string. */
    String backslashUnescaped() const;

    /** Is the string empty? */
    bool
    empty() const
    {
        return (length() == 0);
    }

    /**
       Find the first instance of the given string in self.
       \return index of match (size_t_max if no match)
       \param str search string
       \param begin search start index
    */
    size_t find(const String& str, size_t begin = 0) const;

    /**
       Find the first instance of the given string in self.
       \return index of match (size_t_max if no match)
       \param str search string
       \param begin search start index
    */
    size_t
    find(const char* str, size_t begin = 0) const
    {
        return find(String(str, false), begin);
    }

    /**
       Find the first instance of the given character in self.
       \return index of match (size_t_max if no match)
       \param ch search character
       \param begin search start index
    */
    size_t find(char ch, size_t begin = 0) const;

    /**
       Use Boyer-Moore algorithm to find the first instance of the given string.
       \return index of match (size_t_max if no match)
       \see BoyerMooreSearch
    */
    size_t findBM(const String& str, size_t begin = 0) const;

    /**
       Use Boyer-Moore algorithm to find the first instance of the given string.
       \return index of match (size_t_max if no match)
       \see BoyerMooreSearch
    */
    size_t
    findBM(const char* str, size_t begin = 0) const
    {
        return findBM(String(str, false), begin);
    }

    /** Get the first character in the string (\b nul if empty). */
    char
    firstChar() const
    {
        return *_s;
    }

    /** Get the last character in the string (\b nul if empty). */
    char
    lastChar() const
    {
        size_t len = length();
        if (len == 0)
            return '\0';
        return _s[len - 1];
    }

    /** Get const char*. */
    const char*
    get() const
    {
        return _s;
    }

    /** Get the length of the string. */
    size_t
    length() const
    {
        if (_length == size_t_max)
            _length = strlen(_s);
        ASSERTD(_length == strlen(_s));
        return _length;
    }

    /** Invalidate the cached length of the string. */
    void
    lengthInvalidate() const
    {
        _length = size_t_max;
    }

    /**
       Get the first \b n characters of the string.
       \return prefix string
    */
    String
    prefix(size_t n) const
    {
        return subString(0, n);
    }

    /** Get the size of the allocated character array. */
    size_t
    size() const
    {
        return _size;
    }

    /**
       Get a substring.
       \return specified substring
       \param begin first character of substring
       \param len length of substring
    */
    String subString(size_t begin, size_t len = size_t_max) const;

    /**
       Get the last \b n characters of the string.
       \return suffix string
    */
    String
    suffix(size_t n) const
    {
        size_t len = length();
        if (n >= len)
            return self;
        return subString(len - n, n);
    }

    /** Get a copy of self with the characters in reverse order. */
    String reversed() const;

    /**
       Return the next token in a delimited string.
       \param idx starting position
       \param delim delimiter
       \param processQuotes (optional : false) process quoted strings?
    */
    String nextToken(size_t& idx, char delim = ' ', bool processQuotes = false) const;
    //@}

    /// \name Modification (and non-const)
    //@{
    /** Reset to empty string. */
    String&
    clear()
    {
        if (isOwner())
            *_s = '\0';
        else
            _s = &nullChar;
        _length = 0;
        return self;
    }

    /**
       Like clear(), but de-allocate the array instead of writing a \b nul character at position 0.
    */
    String& excise();

    /** Get char*. */
    char*
    get()
    {
        return _s;
    }

    /** Get the specified character. */
    char
    get(size_t i) const
    {
        ASSERTD(i <= length());
        return _s[i];
    }

    /** Make sure self has its own copy of the string. */
    inline String&
    assertOwner()
    {
        if (_size == 0)
            _assertOwner();
        return *this;
    }

    /**
       Make sure self has its own copy of the string.
       \param size the required minimum size
       \param increment growth increment (if increment = size_t_max, make size a power of 2)
    */
    inline void
    assertOwner(size_t size, size_t increment = 8)
    {
        if (_size < size)
            _assertOwner(size, increment);
    }

    /** Make sure no extra space is allocated. */
    void economize();

    /**
       Grow to the specified size.
       \param size the required minimum size
       \param increment growth increment (if increment = size_t_max, make size a power of 2)
    */
    void
    reserve(size_t size, size_t increment = 8)
    {
        if (_size < size)
            _assertOwner(size, increment);
    }

    /** Append the given character. */
    String& append(char c);

    /** Append the given string. */
    String&
    append(const String& s)
    {
        return append(s._s, s.length());
    }

    /** Append the given string. */
    String&
    append(const char* s)
    {
        ASSERTD(s != nullptr);
        return append(s, strlen(s));
    }

    /**
       Append the given string.
       \param s address of first character to append
       \param len number of characters to append
    */
    String& append(const char* s, size_t len);

    /**
       Chop out and return a sub-string.
       \return chopped out portion of self
       \param begin index of first character to chop out
       \param len number of characters to chop out
    */
    String chop(size_t begin, size_t len = size_t_max);

    /**
       Remove a sub-string (like chop(), but doesn't return the removed sub-string).
       \param begin index of first character to chop out
       \param len number of characters to chop out
    */
    void remove(size_t begin, size_t len = size_t_max);

    /** Replace all instances of lhs with rhs. */
    String& replace(const String& lhs, const String& rhs);

    /** Replace all instances of lhs with rhs. */
    String& replace(char lhs, char rhs);

    /** Replace all instances of lhs with rhs. */
    String&
    replace(const char* lhs, const char* rhs)
    {
        return replace(String(lhs, false), String(rhs, false));
    }

    /** Replace all instances of lhs with rhs. */
    String&
    replace(const char* lhs, const String& rhs)
    {
        return replace(String(lhs, false), rhs);
    }

    /** Replace all instances of lhs with rhs. */
    String&
    replace(const String& lhs, const char* rhs)
    {
        return replace(lhs, String(rhs, false));
    }

    /**
       Replace a substring with a given string.
       \return reference to self
       \param begin first character of substring to replace
       \param len length of substring to replace
       \param str string that will replace specified substring
    */
    String& replace(size_t begin, size_t len, const String& str);

    /** Reverse the ordering of the characters. */
    String& reverse();

    /**
       Set a new string.
       \param s character array
       \param owner (optional : true) ownership flag
       \param duplicate (optional : true) make a copy? (implies ownership)
       \param length (optional) provide length of string
    */
    void set(const char* s, bool owner = true, bool duplicate = true, size_t length = size_t_max);

    /**
       Convert characters to lowercase.
       \return reference to self
       \param begin character to convert
       \param len length of substring to convert
    */
    String& toLower(size_t begin = 0, size_t len = size_t_max);

    /**
       Convert characters to uppercase.
       \return reference to self
       \param begin character to convert
       \param len length of substring to convert
    */
    String& toUpper(size_t begin = 0, size_t len = size_t_max);

    /**
       Pad with spaces (or another character) at the beginning.
       \return reference to self
       \param len padding length
       \param ch padding character
    */
    String& padBegin(size_t len, char ch = ' ');

    /**
       Pad with spaces (or another character) at the end.
       \return reference to self
       \param len padding length
       \param ch padding character
    */
    String& padEnd(size_t len, char ch = ' ');

    /** Trim whitespace from the beginning and end. */
    String& trim();

    /** Trim whitespace from the beginning. */
    String& trimBegin();

    /** Trim whitespace from the end. */
    String& trimEnd();
    //@}

    /// \name Static Methods
    //@{
    /**
       Return a string consisting of a repeated character
       \return character \b c repeated \b num times
       \param ch character to repeat
       \param num number of repetitions
    */
    static String repeat(char ch, size_t num);

    /** Return a string consisting of the given number of spaces. */
    static String spaces(size_t num);
    //@}

    /// \name Operators
    //@{
    /** Get a copy of self with the given character appended. */
    inline String
    operator+(char c) const
    {
        String res(this->length() + 2);
        res.append(self);
        res.append(c);
        return res;
    }

    /** Get a copy of self with the given string appended. */
    inline String
    operator+(const char* str) const
    {
        ASSERTD(str != nullptr);
        return self + String(str, false);
    }

    /** Get a copy of self with the given string appended. */
    inline String
    operator+(const String& rhs) const
    {
        String res(this->length() + rhs.length() + 1);
        res = self;
        res += rhs;
        return res;
    }

    /** Append the given character to self. */
    inline String&
    operator+=(char c)
    {
        return append(c);
    }

    /** Append the given string to self. */
    inline String&
    operator+=(const char* rhs)
    {
        return append(String(rhs, false));
    }

    /** Append the given string to self. */
    inline String&
    operator+=(const String& rhs)
    {
        return append(rhs);
    }

    /** Make self equal to a copy of the given character array. */
    inline String&
    operator=(const char* str)
    {
        set(str);
        return self;
    }

    /** Make self equal to the given character. */
    inline String&
    operator=(char c)
    {
        clear();
        append(c);
        return self;
    }

    /** Array access operator. */
    inline char& operator[](size_t i)
    {
        ASSERTD(i <= length());
        return _s[i];
    }

    /** Array access operator. */
    inline const char& operator[](size_t i) const
    {
        ASSERTD(i <= length());
        return _s[i];
    }

    /** Conversion to char*. */
    inline operator char*()
    {
        return _s;
    }

    /** Conversion to const char*. */
    inline operator const char*()
    {
        return _s;
    }

    /** Conversion to const char*. */
    inline operator const char*() const
    {
        return _s;
    }

    /** Conversion to void*. */
    inline operator void*()
    {
        return _s;
    }

    /** Conversion to const void*. */
    inline operator const void*()
    {
        return _s;
    }

    /** Conversion to const void*. */
    inline operator const void*() const
    {
        return _s;
    }

    inline bool
    operator<(const Object& rhs) const
    {
        return (compare(rhs) < 0);
    }
    inline bool
    operator<=(const Object& rhs) const
    {
        return (compare(rhs) <= 0);
    }
    inline bool
    operator>(const Object& rhs) const
    {
        return (compare(rhs) > 0);
    }
    inline bool
    operator>=(const Object& rhs) const
    {
        return (compare(rhs) >= 0);
    }
    inline bool
    operator==(const Object& rhs) const
    {
        return (compare(rhs) == 0);
    }
    inline bool
    operator!=(const Object& rhs) const
    {
        return (compare(rhs) != 0);
    }

    inline bool
    operator<(const String& rhs) const
    {
        return (compare(rhs) < 0);
    }
    inline bool
    operator<=(const String& rhs) const
    {
        return (compare(rhs) <= 0);
    }
    inline bool
    operator>(const String& rhs) const
    {
        return (compare(rhs) > 0);
    }
    inline bool
    operator>=(const String& rhs) const
    {
        return (compare(rhs) >= 0);
    }
    inline bool
    operator==(const String& rhs) const
    {
        return (compare(rhs) == 0);
    }
    inline bool
    operator!=(const String& rhs) const
    {
        return (compare(rhs) != 0);
    }
    //@}
protected:
    static void
    setBounds(size_t& begin, size_t& end, size_t& len, size_t myLen)
    {
        if (begin > myLen)
            begin = myLen;
        if (len > myLen)
            len = myLen;
        end = begin + len;
        if (end > myLen)
            end = myLen;
        len = end - begin;
    }

protected:
    char* _s;
    size_t _size;
    mutable size_t _length;

private:
    enum flg_t
    {
        flg_marked,
        flg_caseInsensitive
    };

private:
    void
    init(bool caseSensitive = true)
    {
        _s = &nullChar;
        _size = 0;
        _length = 0;
        if (!caseSensitive)
            setCaseSensitive(false);
    }

    void
    deInit()
    {
// check length in DEBUG mode, and allow an easy breakpoint
#ifdef DEBUG
        bool lengthOK = ((_length == size_t_max) || (_length == strlen(_s)));
        ASSERT(lengthOK);
        ASSERT(nullChar == '\0');
#endif

        // if we own the string, delete it
        if (isOwner())
            delete[] _s;
    }

    void _assertOwner();
    void _assertOwner(size_t size, size_t increment);

private:
    static char nullChar;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   An empty string.
   \ingroup string
*/
extern const String emptyString;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator==(const utl::String& lhs_, const char* rhs)
{
    ASSERTD(rhs != nullptr);
    const char* lhs = lhs_.get();
    return (lhs_.strcmp(lhs, rhs) == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator!=(const utl::String& lhs_, const char* rhs)
{
    ASSERTD(rhs != nullptr);
    const char* lhs = lhs_.get();
    return (lhs_.strcmp(lhs, rhs) != 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator==(const char* lhs, const utl::String& rhs_)
{
    ASSERTD(lhs != nullptr);
    const char* rhs = rhs_.get();
    return (rhs_.strcmp(lhs, rhs) == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator!=(const char* lhs, const utl::String& rhs_)
{
    ASSERTD(lhs != nullptr);
    const char* rhs = rhs_.get();
    return (rhs_.strcmp(lhs, rhs) != 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator==(const utl::String& lhs_, const char rhs)
{
    ASSERTD(rhs != '\0');
    const char* lhs = lhs_.get();
    if (lhs_.isCaseSensitive())
        return (*lhs == rhs) && (lhs[1] == '\0');
    return (tolower(*lhs) == tolower(rhs)) && (lhs[1] == '\0');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator!=(const utl::String& lhs_, const char rhs)
{
    ASSERTD(rhs != '\0');
    const char* lhs = lhs_.get();
    if (lhs_.isCaseSensitive())
        return (*lhs != rhs) || (lhs[1] != '\0');
    return (tolower(*lhs) != tolower(rhs)) || (lhs[1] != '\0');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator==(char lhs, const utl::String& rhs_)
{
    ASSERTD(lhs != '\0');
    const char* rhs = rhs_.get();
    if (rhs_.isCaseSensitive())
        return (lhs == *rhs) && (rhs[1] == '\0');
    return (tolower(lhs) == tolower(*rhs)) && (rhs[1] == '\0');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool
operator!=(char lhs, const utl::String& rhs_)
{
    ASSERTD(lhs != '\0');
    const char* rhs = rhs_.get();
    if (rhs_.isCaseSensitive())
        return (lhs != *rhs) || (rhs[1] != '\0');
    return (tolower(lhs) != tolower(*rhs)) || (rhs[1] != '\0');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline utl::String
operator+(const char* lhs, const utl::String& rhs)
{
    return utl::String(lhs, false) + rhs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline utl::String
operator+(char lhs, const utl::String& rhs)
{
    utl::String res(rhs.length() + 2);
    res.append(lhs);
    res.append(rhs);
    return res;
}
