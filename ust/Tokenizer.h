#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/List.h>
#include <libutl/Token.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Stream;
class TokenizerTokens;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Tokenizer ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Stream tokenizer.

   A Tokenizer is configured by defining a set of "terminals" described by regular expressions.
   Tokenizer can then scan an input stream for strings that match one of these expressions
   (ignoring any intervening characters).

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Tokenizer : public Object
{
    UTL_CLASS_DECL(Tokenizer, Object);
    UTL_CLASS_NO_COMPARE;

public:
    virtual void copy(const Object& rhs);

    /** Re-set to initial configuration. */
    void clear();

    /**
       Add a terminal.
       \param id id
       \param regex regular expression
    */
    void addTerminal(uint_t id, const String& regex);

    /**
       Scan an input stream.
       \param tokens output tokens
       \param is input stream
       \param owner input stream ownership flag
    */
    void scan(TokenizerTokens& tokens, Stream* is, bool owner = true) const;

protected:
    Array _terminals;
    bool _reverse;

private:
    void
    init()
    {
        _reverse = false;
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringTokenizer /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   String tokenizer.

   This is a simple specialization of Tokenizer that can tokenize
   an input string instead of an input stream.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class StringTokenizer : public Tokenizer
{
    UTL_CLASS_DECL(StringTokenizer, Tokenizer);
    UTL_CLASS_DEFID;

public:
    /**
       Scan an input string.
       \param tokens output tokens
       \param s input string
    */
    void scan(TokenizerTokens& tokens, const String* s) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// TokenizerTokens /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Store tokens collected by Tokenizer.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class TokenizerTokens : public Object, protected FlagsMI
{
    UTL_CLASS_DECL(TokenizerTokens, Object);
    UTL_CLASS_DEFID;

public:
    void
    clear()
    {
        _tokens.clear();
    }

    void
    add(String* tk)
    {
        _tokens.pushBack(tk);
    }

    /**
       No more tokens?
       \return true iff token queue is empty
    */
    bool
    empty() const
    {
        return _tokens.empty();
    }

    /**
       Peek at the next token.
       \return next token (empty string if queue is empty)
    */
    Token peek() const;

    /**
       Get the next token.
       \return next token (empty string if queue is empty)
    */
    Token next();

    /** Skip past the given number of tokens. */
    void skip(size_t numTokens);

    /** Get \b reverse flag. */
    bool
    isReverse() const
    {
        return getFlag(flg_reverse);
    }

    /** Set \b reverse flag. */
    void
    setReverse(bool reverse)
    {
        setFlag(flg_reverse, reverse);
    }

private:
    enum flg_t
    {
        flg_reverse
    };

private:
    List _tokens;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
