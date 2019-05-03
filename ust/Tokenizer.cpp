#include <libutl/libutl.h>
#include <libutl/MemStream.h>
#include <libutl/Regex.h>
#include <libutl/AutoPtr.h>
#include <libutl/Tokenizer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Tokenizer //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
Tokenizer::copy(const Object& rhs)
{
    auto& tokenizer = utl::cast<Tokenizer>(rhs);
    _terminals = tokenizer._terminals;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Tokenizer::clear()
{
    _terminals.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Tokenizer::addTerminal(uint_t id, const String& p_regex)
{
    Regex* regex = new Regex("^" + p_regex);
    _terminals.set(id, regex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Tokenizer::scan(TokenizerTokens& tokens, Stream* is, bool owner) const
{
    Regex whitespace("^\\s+");

    AutoPtr<> isPtr;
    if (owner)
        isPtr = is;

    // Scan tokens until input stream is at EOF
    tokens.clear();
    for (size_t line = 0;; ++line)
    {
        String str;
        try
        {
            *is >> str;
            str += '\n';
        }
        catch (StreamEOFex&)
        {
            break;
        }
        size_t col = 0;
        size_t strLen = str.length();
        while (col < strLen)
        {
            // curStr = unscanned remainder of current line
            String curStr(str.get() + col, false);

            // what terminal matches curStr?
            bool match = false;
            uint_t tokenId = uint_t_max;
            for (auto regex_ : _terminals)
            {
                Regex& regex = utl::cast<Regex>(*regex_);
                ++tokenId;
                RegexMatch m;
                if (regex.match(curStr, m))
                {
                    Token* tk = new Token(tokenId, m.replaceString("&"), line, col);
                    tokens.add(tk);
                    col += tk->length();
                    match = true;
                    break;
                }
            }

            // skip whitespace
            if (!match)
            {
                RegexMatch m;
                if (whitespace.match(curStr, m))
                {
                    col += m.matchSpan(0).size();
                }
                else
                {
                    ++col;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// StringTokenizer ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringTokenizer::scan(TokenizerTokens& tokens, const String* s) const
{
    // s is nullptr or empty -> nothing to do
    if ((s == nullptr) || s->empty())
    {
        tokens.clear();
        return;
    }

    // create an input stream from the string
    Stream* memStream = new MemStream((byte_t*)(s->get()), s->length(), 0, false);
    Tokenizer::scan(tokens, memStream, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// TokenizerTokens ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Token
TokenizerTokens::peek() const
{
    Token tk;
    if (_tokens.empty())
        return tk;
    if (isReverse())
        tk = *_tokens.back();
    else
        tk = *_tokens.front();
    return tk;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Token
TokenizerTokens::next()
{
    Token tk;
    if (_tokens.empty())
        return tk;
    if (isReverse())
    {
        tk = *_tokens.back();
        _tokens.removeBack();
    }
    else
    {
        tk = *_tokens.front();
        _tokens.removeFront();
    }
    return tk;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
TokenizerTokens::skip(size_t numTokens)
{
    while (numTokens-- > 0)
        next();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Tokenizer);
UTL_CLASS_IMPL(utl::StringTokenizer);
UTL_CLASS_IMPL(utl::TokenizerTokens)

////////////////////////////////////////////////////////////////////////////////////////////////////

template class utl::AutoPtr<utl::TokenizerTokens>;
