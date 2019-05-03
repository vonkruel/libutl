#include <libutl/libutl.h>
#include <libutl/Translator.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Translator /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
Translator::clear()
{
    super::clear();
    _prefix.clear();
    _xlats.clear();
    delete _mapping;
    _mapping = nullptr;
    _mappedObject.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Translator::encode(const byte_t* block, size_t num)
{
    checkOK();

    Vector<char> blk(num + 1);
    if (num > 0)
        memcpy(blk.get(), block, num);
    blk[num] = '\0';

    String token, tokenStr, str;

    size_t i = 0;
    while (i < num)
    {
        size_t startI = i;

        str.set(blk.get() + i, false);
        size_t strLen = str.length();
        size_t eos = i + strLen;

        // empty string -> write NUL character, skip ahead one char
        if (str.empty())
        {
            // write NUL
            ++i;
            *_stream << '\0';
            continue;
        }

        // look for the next instance of the prefix (e.g. '$')
        size_t prefixIdx = str.find(_prefix);
        if (prefixIdx == size_t_max)
            prefixIdx = strLen;

        // intervening characters?
        if (prefixIdx > 0)
        {
            *_stream << str.subString(0, prefixIdx);
            i += prefixIdx;
            continue;
        }

        ++i; // skip past $

        // at end of input?
        if (i == num)
            return startI;

        // escaped prefix char?
        if (blk[i] == _prefix[0])
        {
            ++i; // skip past 2nd $
            *_stream << _prefix;
            continue;
        }

        // identify the token
        String token;
        bool bracket = (blk[i] == '(');
        if (bracket)
        {
            ++i; // skip past "("

            // find closing bracket
            size_t bracketIdx = str.find(")", 2);
            if (bracketIdx == size_t_max)
                return startI;

            // extract the token, adjust i
            token = str.subString(2, bracketIdx - 2);
            i += token.length(); // skip past token
            ++i;                 // skip past ")"
        }
        else
        {
            for (; i < eos; ++i)
            {
                char c = blk[i];
                if (!isalpha(c))
                    break;
                token += c;
            }
        }

        const String* xlat = translate(token);
        if (xlat == nullptr)
        {
            // at end of input?
            if ((i == num) && !bracket)
                return startI;
        }
        else
        {
            *_stream << *xlat;
        }
    }
    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Translator::set(Stream* stream,
                bool owner,
                Mapping* mapping,
                bool mappingOwner,
                bool rememberMappings,
                char prefix)
{
    super::set(io_wr, stream, owner);
    setMapping(mapping, mappingOwner, rememberMappings);
    _mappedObject = nullptr;
    _prefix = prefix;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const String*
Translator::translate(const String& token)
{
    auto pair = utl::cast<Pair>(_xlats.find(token));
    if (pair != nullptr)
        return utl::cast<String>(pair->getSecond());
    if (_mapping == nullptr)
        return nullptr;

    auto res = utl::cast<String>(_mapping->map(token));
    if (res != nullptr)
    {
        if (getRememberMappings())
        {
            _xlats += new Pair(token.clone(), res, true, !_mapping->isOwner());
        }
        else if (!_mapping->isOwner())
        {
            _mappedObject = res;
        }
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// StringTranslator ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringTranslator::set(Mapping* mapping, bool mappingOwner, bool rememberMappings, char prefix)
{
    _translator.set(_ms, false, mapping, mappingOwner, rememberMappings, prefix);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
StringTranslator::translate(const String& str)
{
    _translator.write((byte_t*)str.get(), str.length());
    _translator.flush();
    return String(_ms.takeString(), true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Translator);
UTL_CLASS_IMPL(utl::StringTranslator);
