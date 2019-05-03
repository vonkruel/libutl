#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Hashtable.h>
#include <libutl/RBtree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class MemStream;
class Token;

////////////////////////////////////////////////////////////////////////////////////////////////////

enum bbcode_context_t
{
    bbc_block,
    bbc_inline,
    bbc_list,
    bbc_list_item
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   BBCode parser.

   This implementation is thread-safe and reasonably efficient.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeParser : public utl::Object
{
    UTL_CLASS_DECL(BBcodeParser, Object);

public:
    void addIsolatedTag(const String& name, const String& markup);

    String* parse(const String& input) const;

    String parseColor(const String& colorName) const;

    uint_t
    indentSize() const
    {
        return _indentSize;
    }

    void
    setIndentSize(uint_t indentSize)
    {
        _indentSize = indentSize;
    }

public:
    static String filter(const String& text);

private:
    void init();
    void deInit();
    utl::Deque* tokenize(const String& input) const;
    void parse(utl::MemStream& os, utl::Deque& tokens) const;
    bbcode_context_t
    processTag(utl::Deque& list, const String& name, uint_t lineNo, uint_t colNo) const;
    bbcode_context_t processTag(utl::Deque& list, const utl::Deque::iterator& startTagIt) const;
    bbcode_context_t findContext(utl::Deque& list) const;
    bool isIsolatedTag(const String& name) const;

private:
    static String toString(const utl::Token* tk, bool verbatim = false);
    static utl::Token* takeHead(utl::Deque& list);
    static void removeWShead(utl::Deque& list);
    static void removeWStail(utl::Deque& list);
    static void removeNLtail(utl::Deque& list);
    static void readStartTag(const String& startTag, String& tagName, String& tagParam);

private:
    utl::Hashtable _colorNames;
    utl::Hashtable _tags;
    BitArray _emoticonChars;
    BitArray _emoticonPostChars;
    uint_t _indentSize;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
