#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/BitArray.h>
#include <libutl/BoyerMooreSearch.h>
#include <libutl/Queue.h>
#include <libutl/Span.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class REnode;
class RegexMatch;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Regular expression.

   A regular expression (RE) is a pattern that describes a set of matching strings.  The grammar
   for a RE (as supported in this implementation) is as follows:

   \code
   REGEX      ::= BRANCH { \| BRANCH }
   BRANCH     ::= PIECE { PIECE }
   PIECE      ::= ATOM [ * | + | ? | \{ NUMBER [, NUMBER] \} ]
   ATOM       ::= ( EXPRESSION )
                | ^
                | $
                | .
                | CHAR{CHAR}
                | \\ CHAR
                | \[ BRACKET_EXPR \]
   \endcode

   A \b REGEX consists of one or more <b>BRANCH</b>es separated by a '|'.
   The RE matches any string that matches any one of the branches.

   A \b BRANCH consists of one or more <b>PIECE</b>s concatenated together.  It matches a match
   for the first piece, followed by a match for the second, etc...

   A \b PIECE consists of an \b ATOM optionally followed by one of:
   <ul>
   <li><code>*</code> : match the atom zero or more times
   <li><code>+</code> : match the atom one or more times
   <li><code>?</code> : match the atom zero or one times
   <li><code>{m}</code> : match the atom exactly \c m times
   <li><code>{m,}</code> : match the atom \c m or more times
   <li><code>{m,n}</code> : match the atom between \c m and \c n times
   </ul>

   An \b ATOM matches any of the following:
   <ul>
   <li>a \b REGEX enclosed in <code>'()'</code> (matching the expression)
   <li><code>^</code> : beginning of line
   <li><code>$</code> : end of line
   <li><code>.</code> : any character
   <li>a special character, including:
       <ul>
       <li><code>\\a</code> : match any alphabetic character
       <li><code>\\d</code> : match any digit
       <li><code>\\s</code> : match any whitespace character
       <li><code>\\w</code> : match any alpha-numeric or underscore character
       <li><code>\\A</code> : inverse of <code>\\a</code>
       <li><code>\\D</code> : inverse of <code>\\d</code>
       <li><code>\\S</code> : inverse of <code>\\s</code>
       <li><code>\\W</code> : inverse of <code>\\w</code>
       </ul>
   <li>a string of characters
       <br>You can suppress the normal significance of the characters <code>'^$[](){}|*+?\\'</code>
       by preceding them with a <code>'\'</code>.
       <br>For example, if you wanted to match a <code>'|'</code>, you'd have to escape it like
       this: <code>'\|'</code>.
   <li>a list of characters enclosed in <code>'[]'</code>, matching any character in the list
       <br>If the first character in the list is a <code>'^'</code>, match any character \e not
       in the list.
       <br>If two characters in the list are separated by a <code>'-'</code>,
       match any character between the two (inclusive).
       <br>POSIX character classes are supported within bracket expressions:
       <ul>
           <li><code>[:alnum:]</code> : alphanumeric characters:
               <code>[a-zA-Z0-9]</code>
           <li><code>[:alpha:]</code> : alphabetic characters:
               <code>[a-zA-Z]</code>
           <li><code>[:blank:]</code> : space and tab:
               <code>[ \\t]</code>
           <li><code>[:cntrl:]</code> : control characters:
               <code>[\\x00-\\x1f\\x7f]</code>
           <li><code>[:digit:]</code> : decimal digits:
               <code>[0-9]</code>
           <li><code>[:graph:]</code> : visible characters:
               <code>[\\x21-\\x7e]</code>
           <li><code>[:lower:]</code> : lowercase characters:
               <code>[a-z]</code>
           <li><code>[:odigit:]</code> : octal digits:
               <code>[0-7]</code> (actually not POSIX, I added this one)
           <li><code>[:print:]</code> : visible characters and spaces:
               <code>[\\x20-\\x7e]</code>
           <li><code>[:punct:]</code> : punctuation characters:
               <code>[-!"#$%&'()*+,./:;<=>?@[\\\\\\]_`{|}~]</code>
           <li><code>[:space:]</code> : whitespace characters:
               <code>[ \\t\\r\\n\\v\\f]</code>
           <li><code>[:upper:]</code> : uppercase characters:
               <code>[A-Z]</code>
           <li><code>[:word:]</code> : alphanumeric characters plus
               <code>'_'</code>: <code>[a-zA-Z0-9_]</code>
           <li><code>[:xdigit:]</code> : hexadecimal digits:
               <code>[0-9a-fA-F]</code>
       </ul>
       A few examples:
       <ul>
           <li><code>[x-z[:punct:]]</code> : lowercase x, y, or z, or any punctuation character
           <li><code>[[:upper:]yz]</code> : any uppercase character or lowercase y or z
           <li><code>[[:graph:][^,.]]</code> : any visible character except <code>','</code>
               or <code>'.'</code>
           <li><code>[[:graph:][^:lower:][^.]]</code> : any visible character except lowercase
               or <code>'.'</code>
       </ul>
   </ul>

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Regex : public String
{
    UTL_CLASS_DECL(Regex, String);

public:
    /** Constructor. */
    Regex(const String& regex)
        : String(regex)
    {
        init();
        compile();
    }

    /**
       Constructor.
       \param str regex
    */
    Regex(const char* str)
        : String(str)
    {
        init();
        compile();
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    void clear();

    /** Compile the regex. */
    bool compile();

    /**
       Attempt to match the given string against the regex.
       \return true if \b str matches regex, false otherwise
       \param str string to match against
       \param m reference to RegexMatch structure
       \param idx starting search index
    */
    bool match(const String& str, RegexMatch* m = nullptr, size_t idx = 0) const;

    /**
       At each position within \b str, attempt to match the regex.
       If a match is found, the matching substring is replaced by the replacement string.
       \return number of matches (and substitutions)
       \param str search/replace string
       \param rep replacement string
    */
    size_t searchReplace(String& str, const String& rep);

    /** Determine whether the regex is successfully compiled. */
    bool
    ok() const
    {
        return getFlag(flg_ok);
    }

private:
    void init();
    void deInit();

    static String getToken(const TDequeIt<String>& it);

    bool
    isAnchored() const
    {
        return getFlag(flg_anchored);
    }

    static bool isSymbol(const TDequeIt<String>& it);

    bool match(RegexMatch& m, size_t& idx, const REnode* node) const;

    REnode* parseExpression(TDequeIt<String>& it, bool paren = false);
    REnode* parseBranch(TDequeIt<String>& it);
    REnode* parsePiece(TDequeIt<String>& it);
    REnode* parseAtom(TDequeIt<String>& it);
    void parseBracketExpr(TDequeIt<String>& it, BitArray& charSet, bool tv);

    void scan(Queue<String>& queue);

    void
    setAnchored(bool anchored)
    {
        setFlag(flg_anchored, anchored);
    }

    bool setCharSet(const REnode* node, BitArray& charSet);

    void
    setOK(bool ok)
    {
        setFlag(flg_ok, ok);
    }

    void setTail(REnode* chain, REnode* tail);

private:
    REnode* _start;
    TArray<REnode>* _regex;
    uint_t _parenNo;
    char _firstChar;
    BoyerMooreSearch<>* _mustStr;
    enum flg_t
    {
        flg_ok = 4,
        flg_anchored = 5
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// RegexMatch //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   Regular expression match information.

   RegexMatch stores information about matches that were found when comparing a String
   against a Regex.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegexMatch : public utl::Object
{
    UTL_CLASS_DECL(RegexMatch, Object);
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_COPY;

public:
    /** Was there a match? */
    bool
    ok() const
    {
        return _matches != nullptr;
    }

    /** Reset to initial configuration. */
    void
    clear()
    {
        _matchStr.clear();
        delete _matches;
        _matches = nullptr;
    }

    /** Set match information. */
    void
    set(const String& matchStr, const TArray<Span<size_t>>* matches)
    {
        delete _matches;
        _matchStr = matchStr;
        _matches = matches;
    }

    /** Get the string that matched the regex pattern. */
    const String&
    matchString() const
    {
        return _matchStr;
    }

    /** Get the number of match-spans. */
    size_t
    numMatchSpans() const
    {
        ASSERTD(ok());
        return _matches->size();
    }

    /** Get the span of the given match (0 = whole expression). */
    Span<size_t>&
    matchSpan(size_t idx)
    {
        ASSERTD(ok());
        return (*_matches)(idx);
    }

    /** Get the span of the given match (0 = whole expression). */
    const Span<size_t>&
    matchSpan(size_t idx) const
    {
        ASSERTD(ok());
        return (*_matches)(idx);
    }

    /**
       Return \b str with all match references replaced by the
       match strings.
    */
    String replaceString(const String& str) const;

private:
    void
    init()
    {
        _matches = nullptr;
    }
    void
    deInit()
    {
        delete _matches;
    }

private:
    String _matchStr;
    const TArray<Span<size_t>>* _matches;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
