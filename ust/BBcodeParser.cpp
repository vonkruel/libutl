#include <libutl/libutl.h>
#include <libutl/Deque.h>
#include <libutl/MemStream.h>
#include <libutl/Token.h>
#include <libutl/URI.h>
#include <libutl/BBcodeParser.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag (and derived) ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

enum bbcode_token_t
{
    bbc_text,
    bbc_ws,
    bbc_nl,
    bbc_tag,
    bbc_endtag
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* colorNames[] = {"AliceBlue",
                                   "AntiqueWhite",
                                   "Aqua",
                                   "Aquamarine",
                                   "Azure",
                                   "Beige",
                                   "Bisque",
                                   "Black",
                                   "BlanchedAlmond",
                                   "Blue",
                                   "BlueViolet",
                                   "Brown",
                                   "BurlyWood",
                                   "CadetBlue",
                                   "Chartreuse",
                                   "Chocolate",
                                   "Coral",
                                   "CornflowerBlue",
                                   "Cornsilk",
                                   "Crimson",
                                   "Cyan",
                                   "DarkBlue",
                                   "DarkCyan",
                                   "DarkGoldenRod",
                                   "DarkGray",
                                   "DarkGrey",
                                   "DarkGreen",
                                   "DarkKhaki",
                                   "DarkMagenta",
                                   "DarkOliveGreen",
                                   "DarkOrange",
                                   "DarkOrchid",
                                   "DarkRed",
                                   "DarkSalmon",
                                   "DarkSeaGreen",
                                   "DarkSlateBlue",
                                   "DarkSlateGray",
                                   "DarkSlateGrey",
                                   "DarkTurquoise",
                                   "DarkViolet",
                                   "DeepPink",
                                   "DeepSkyBlue",
                                   "DimGray",
                                   "DimGrey",
                                   "DodgerBlue",
                                   "FireBrick",
                                   "FloralWhite",
                                   "ForestGreen",
                                   "Fuchsia",
                                   "Gainsboro",
                                   "GhostWhite",
                                   "Gold",
                                   "GoldenRod",
                                   "Gray",
                                   "Grey",
                                   "Green",
                                   "GreenYellow",
                                   "HoneyDew",
                                   "HotPink",
                                   "IndianRed",
                                   "Indigo",
                                   "Ivory",
                                   "Khaki",
                                   "Lavender",
                                   "LavenderBlush",
                                   "LawnGreen",
                                   "LemonChiffon",
                                   "LightBlue",
                                   "LightCoral",
                                   "LightCyan",
                                   "LightGoldenRodYellow",
                                   "LightGray",
                                   "LightGrey",
                                   "LightGreen",
                                   "LightPink",
                                   "LightSalmon",
                                   "LightSeaGreen",
                                   "LightSkyBlue",
                                   "LightSlateGray",
                                   "LightSlateGrey",
                                   "LightSteelBlue",
                                   "LightYellow",
                                   "Lime",
                                   "LimeGreen",
                                   "Linen",
                                   "Magenta",
                                   "Maroon",
                                   "MediumAquaMarine",
                                   "MediumBlue",
                                   "MediumOrchid",
                                   "MediumPurple",
                                   "MediumSeaGreen",
                                   "MediumSlateBlue",
                                   "MediumSpringGreen",
                                   "MediumTurquoise",
                                   "MediumVioletRed",
                                   "MidnightBlue",
                                   "MintCream",
                                   "MistyRose",
                                   "Moccasin",
                                   "NavajoWhite",
                                   "Navy",
                                   "OldLace",
                                   "Olive",
                                   "OliveDrab",
                                   "Orange",
                                   "OrangeRed",
                                   "Orchid",
                                   "PaleGoldenRod",
                                   "PaleGreen",
                                   "PaleTurquoise",
                                   "PaleVioletRed",
                                   "PapayaWhip",
                                   "PeachPuff",
                                   "Peru",
                                   "Pink",
                                   "Plum",
                                   "PowderBlue",
                                   "Purple",
                                   "Red",
                                   "RosyBrown",
                                   "RoyalBlue",
                                   "SaddleBrown",
                                   "Salmon",
                                   "SandyBrown",
                                   "SeaGreen",
                                   "SeaShell",
                                   "Sienna",
                                   "Silver",
                                   "SkyBlue",
                                   "SlateBlue",
                                   "SlateGray",
                                   "SlateGrey",
                                   "Snow",
                                   "SpringGreen",
                                   "SteelBlue",
                                   "Tan",
                                   "Teal",
                                   "Thistle",
                                   "Tomato",
                                   "Turquoise",
                                   "Violet",
                                   "Wheat",
                                   "White",
                                   "WhiteSmoke",
                                   "Yellow",
                                   "YellowGreen",
                                   nullptr};

////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag : public utl::Object
{
    UTL_CLASS_DECL(BBcodeTag, Object);
    UTL_CLASS_NO_COPY;

public:
    BBcodeTag(const String& name,
              const String& preText,
              const String& postText,
              bbcode_context_t context,
              bool isVerbatim = false,
              bool isIsolated = false);

    virtual int compare(const utl::Object& rhs) const;

    virtual size_t hash(size_t size) const;

    void process(const BBcodeParser* parser,
                 String& os,
                 const String& text,
                 const String& param,
                 bbcode_context_t context,
                 bool indent) const;

    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

    bbcode_context_t
    context() const
    {
        return _context;
    }

    virtual bool
    isVerbatim() const
    {
        return _isVerbatim;
    }

    virtual bool
    isIsolated() const
    {
        return _isIsolated;
    }

protected:
    bool checkContext(bbcode_context_t parentContext) const;
    virtual void fallback(MemStream& os, const String& text, const String& param) const;

protected:
    String _name;
    const String* _preText;
    const String* _postText;
    bbcode_context_t _context;
    bool _isVerbatim;
    bool _isIsolated;

private:
    void init();
    void deInit();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

BBcodeTag::BBcodeTag(const String& name,
                     const String& preText,
                     const String& postText,
                     bbcode_context_t context,
                     bool isVerbatim,
                     bool isIsolated)
{
    init();
    _name = name;
    if (!preText.empty())
        _preText = preText.clone();
    if (!postText.empty())
        _postText = postText.clone();
    _context = context;
    _isVerbatim = isVerbatim;
    _isIsolated = isIsolated;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
BBcodeTag::compare(const utl::Object& rhs) const
{
    if (rhs.isA(BBcodeTag))
    {
        auto& tag = utl::cast<BBcodeTag>(rhs);
        return _name.compare(tag._name);
    }
    else
    {
        auto& str = utl::cast<String>(rhs);
        return _name.compare(str);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
BBcodeTag::hash(size_t size) const
{
    return _name.hash(size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag::process(const BBcodeParser* parser,
                   String& os,
                   const String& text,
                   const String& param,
                   bbcode_context_t context,
                   bool indent) const
{
    MemStream ms;
    if (checkContext(context))
    {
        process(parser, ms, text, param, context, indent);
    }
    else
    {
        fallback(ms, text, param);
    }
    os.set(ms.takeString(), true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag::process(const BBcodeParser* parser,
                   MemStream& os,
                   const String& text,
                   const String& param,
                   bbcode_context_t context,
                   bool indent) const
{
    if (_isIsolated)
    {
        os << *_preText;
    }
    else
    {
        os << *_preText;
        if (indent)
            os.indent(parser->indentSize());
        os << text;
        if (indent)
            os.unindent(parser->indentSize());
        os << *_postText;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BBcodeTag::checkContext(bbcode_context_t parentContext) const
{
    switch (parentContext)
    {
    case bbc_block:
        return ((_context == bbc_block) || (_context == bbc_inline) || (_context == bbc_list));
    case bbc_inline:
        return (_context == bbc_inline);
    case bbc_list:
        return (_context == bbc_list_item);
    case bbc_list_item:
        return ((_context == bbc_inline) || (_context == bbc_list));
    default:
        ABORT();
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag::fallback(MemStream& os, const String& text, const String& param) const
{
    os << "[" << _name;
    if (!param.empty())
    {
        os << "=" << BBcodeParser::filter(param);
    }
    os << ']';
    if (!_isIsolated)
    {
        os << text;
        os << "[/" << _name << "]";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag::init()
{
    _preText = &utl::emptyString;
    _postText = &utl::emptyString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag::deInit()
{
    if (_preText != &emptyString)
        delete _preText;
    if (_postText != &emptyString)
        delete _postText;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_center ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_center : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_center, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_center::process(const BBcodeParser* parser,
                          MemStream& os,
                          const String& text,
                          const String& param,
                          bbcode_context_t context,
                          bool indent) const
{
    os << "<div style='text-align:center'>";
    os.indent(parser->indentSize());
    os << text;
    os.unindent(parser->indentSize());
    os << "</div>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_center::init()
{
    _name = "center";
    _context = bbc_block;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_color ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_color : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_color, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_color::process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const
{
    String color = parser->parseColor(param);
    if (color.empty())
    {
        fallback(os, text, param);
        return;
    }
    os << "<span style='color:" << color << "'>";
    os.indent(parser->indentSize());
    os << text;
    os.unindent(parser->indentSize());
    os << "</span>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_color::init()
{
    _name = "color";
    _context = bbc_inline;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_img //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_img : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_img, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_img::process(const BBcodeParser* parser,
                       MemStream& os,
                       const String& text,
                       const String& param,
                       bbcode_context_t context,
                       bool) const
{
    String url = param.empty() ? text : param;
    try
    {
        url = utl::URI(url);
    }
    catch (utl::Exception&)
    {
        fallback(os, text, param);
        return;
    }
    os << "<img src='" << url << "' alt='' />";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_img::init()
{
    _name = "img";
    _context = bbc_inline;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_list /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_list : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_list, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_list::process(const BBcodeParser* parser,
                        MemStream& os,
                        const String& text,
                        const String& param,
                        bbcode_context_t context,
                        bool indent) const
{
    bool ordered = false;
    String style = "disc";
    if (param == "1")
    {
        ordered = true;
        style = "decimal";
    }
    else if (param == "01")
    {
        ordered = true;
        style = "decimal-leading-zero";
    }
    else if (param == "A")
    {
        ordered = true;
        style = "upper-alpha";
    }
    else if (param == "a")
    {
        ordered = true;
        style = "lower-alpha";
    }
    else if (param == "I")
    {
        ordered = true;
        style = "upper-roman";
    }
    else if (param == "i")
    {
        ordered = true;
        style = "lower-roman";
    }
    else if (param == "greek")
    {
        ordered = false;
        style = "lower-greek";
    }
    else if (param == "circle")
    {
        ordered = false;
        style = "circle";
    }
    else if (param == "disc")
    {
        ordered = false;
        style = "disc";
    }
    else if (param == "square")
    {
        ordered = false;
        style = "square";
    }
    else
    {
        fallback(os, text, param);
        return;
    }
    if (ordered)
        os << "<ol ";
    else
        os << "<ul ";
    os << "style='list-style-type:" << style << "'>";
    if (indent)
        os.indent(parser->indentSize());
    os << text;
    if (indent)
        os.unindent(parser->indentSize());
    if (ordered)
        os << "</ol>";
    else
        os << "</ul>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_list::init()
{
    _name = "list";
    _context = bbc_list;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_listItem /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_listItem : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_listItem, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_listItem::process(const BBcodeParser* parser,
                            MemStream& os,
                            const String& text,
                            const String& param,
                            bbcode_context_t context,
                            bool indent) const
{
    os << "<li>";
    if (indent)
        os.indent(parser->indentSize());
    os << text;
    if (indent)
        os.unindent(parser->indentSize());
    os << "</li>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_listItem::init()
{
    _name = "*";
    _context = bbc_list_item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_quote ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_quote : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_quote, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_quote::process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool) const
{
    os << "<div class='quotation'>" << endl;
    os.indent(parser->indentSize());
    os << "<div class='quotation-header'>";
    if (param.empty())
    {
        os << "Quote:";
    }
    else
    {
        os << param << " wrote:";
    }
    os << "</div>" << endl;
    os << "<div class='quotation-body'>" << endl;
    os.indent(parser->indentSize());
    os << text;
    os.unindent(parser->indentSize());
    os << "</div>" << endl;
    os.unindent(parser->indentSize());
    os << "</div>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_quote::init()
{
    _name = "quote";
    _context = bbc_block;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_size /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_size : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_size, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_size::process(const BBcodeParser* parser,
                        MemStream& os,
                        const String& text,
                        const String& param,
                        bbcode_context_t context,
                        bool indent) const
{
    uint_t size;
    try
    {
        size = Uint(param);
    }
    catch (Exception&)
    {
        fallback(os, text, param);
        return;
    }
    size = utl::max(size, 5U);
    size = utl::min(size, 30U);
    String sizeStr = Uint(size).toString() + "px";
    os << "<span style='font-size:" << sizeStr << "'>";
    if (indent)
        os.indent(parser->indentSize());
    os << text;
    if (indent)
        os.unindent(parser->indentSize());
    os << "</span>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_size::init()
{
    _name = "size";
    _context = bbc_inline;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeTag_url //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BBcodeTag_url : public BBcodeTag
{
    UTL_CLASS_DECL(BBcodeTag_url, BBcodeTag);

public:
    virtual void process(const BBcodeParser* parser,
                         MemStream& os,
                         const String& text,
                         const String& param,
                         bbcode_context_t context,
                         bool indent) const;

private:
    void init();
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_url::process(const BBcodeParser* parser,
                       MemStream& os,
                       const String& text,
                       const String& param,
                       bbcode_context_t context,
                       bool) const
{
    String url = param.empty() ? text : param;
    try
    {
        url = utl::URI(url);
    }
    catch (utl::Exception&)
    {
        fallback(os, text, param);
        return;
    }
    os << "<a href='" << url << "'>" << text << "</a>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeTag_url::init()
{
    _name = "url";
    _context = bbc_inline;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::BBcodeParser);
UTL_CLASS_IMPL(utl::BBcodeTag);
UTL_CLASS_IMPL(utl::BBcodeTag_center);
UTL_CLASS_IMPL(utl::BBcodeTag_color);
UTL_CLASS_IMPL(utl::BBcodeTag_img);
UTL_CLASS_IMPL(utl::BBcodeTag_list);
UTL_CLASS_IMPL(utl::BBcodeTag_listItem);
UTL_CLASS_IMPL(utl::BBcodeTag_quote);
UTL_CLASS_IMPL(utl::BBcodeTag_size);
UTL_CLASS_IMPL(utl::BBcodeTag_url);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BBcodeParser ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::addIsolatedTag(const String& name, const String& markup)
{
    _tags += new BBcodeTag(name, markup, emptyString, bbc_inline, false, true);

    char name0 = name[0];
    size_t len = name.length();
    if ((len >= 2) && (len <= 3) && (isdigit(name0) || ispunct(name0)))
    {
        _emoticonChars.set((byte_t)name0, 1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String*
BBcodeParser::parse(const String& input) const
{
    utl::Deque* tokens = tokenize(input);
    MemStream os;
    parse(os, *tokens);
    delete tokens;
    return new String(os.takeString(), true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
BBcodeParser::parseColor(const String& p_colorName) const
{
    // HTML color name?
    String colorName(p_colorName);
    colorName.setCaseSensitive(false);
    auto color = utl::cast<String>(_colorNames.find(colorName));
    if (color != nullptr)
        return *color;

    // 3- or 6-digit hex code?
    if (colorName[0] == '#')
        colorName = colorName.subString(1);
    colorName.toLower();
    size_t len = colorName.length();
    if ((len != 3) && (len != 6))
        return emptyString;
    const char* c = colorName.get();
    const char* lim = c + len;
    while (c != lim)
        if (!isxdigit(*c++))
            return emptyString;
    return "#" + colorName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
BBcodeParser::filter(const String& text)
{
    String output;
    output.reserve(text.length());
    const char* ptr = text.get();
    const char* lim = ptr + text.length();
    for (; ptr != lim; ++ptr)
    {
        char c = *ptr;
        if (c == '<')
        {
            output += "&lt;";
        }
        else if (c == '>')
        {
            output += "&gt;";
        }
        else if (c == '&')
        {
            output += "&amp;";
        }
        else
        {
            output += c;
        }
    }
    return output;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::init()
{
    // init _colorNames
    const char** colorName;
    for (colorName = colorNames; *colorName != nullptr; ++colorName)
    {
        String* str = new String(*colorName, false);
        _colorNames += str;
    }

    // init _tags
    _tags += new BBcodeTag("b", "<b>", "</b>", bbc_inline);
    _tags += new BBcodeTag("code", "<code>", "</code>", bbc_inline);
    _tags += new BBcodeTag("i", "<i>", "</i>", bbc_inline);
    _tags += new BBcodeTag("pre", "<pre>", "</pre>", bbc_block, true);
    _tags += new BBcodeTag("s", "<s>", "</s>", bbc_inline);
    _tags += new BBcodeTag("u", "<u>", "</u>", bbc_inline);
    _tags += new BBcodeTag_center();
    _tags += new BBcodeTag_color();
    _tags += new BBcodeTag_img();
    _tags += new BBcodeTag_list();
    _tags += new BBcodeTag_listItem();
    _tags += new BBcodeTag_quote();
    _tags += new BBcodeTag_size();
    _tags += new BBcodeTag_url();

    // isolated tags
    addIsolatedTag("br", "<br/>");

    // init _emoticonChars
    _emoticonChars.initialize(256);

    // init _emoticonPostChars (characters that can follow short emoticons)
    _emoticonPostChars.initialize(256);
    _emoticonPostChars.set((uint_t)'\0', 1);
    _emoticonPostChars.set((uint_t)'.', 1);
    _emoticonPostChars.set((uint_t)'?', 1);
    _emoticonPostChars.set((uint_t)'!', 1);
    _emoticonPostChars.set((uint_t)')', 1);
    for (uint_t c = 0; c != 256; ++c)
    {
        if (isspace(c))
        {
            _emoticonPostChars.set(c, 1);
        }
    }

    // default indent is 4 spaces
    _indentSize = 4;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::deInit()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

utl::Deque*
BBcodeParser::tokenize(const String& input) const
{
    utl::Deque* tokens = new utl::Deque();
    const char* c = input.get();
    const char* lim = c + input.length();
    size_t line = 1, col = 1;
    while (c < lim)
    {
        String token;
        bbcode_token_t tokenType;
        size_t tokenLine = line, tokenCol = col;
        if (isblank(*c))
        {
            while (isblank(*c))
            {
                token += *c++;
                ++col;
            }
            tokenType = bbc_ws;
        }
        else if (*c == '\n')
        {
            token = *c++;
            ++line;
            col = 1;
            tokenType = bbc_nl;
        }
        else if (*c == '[')
        {
            ++c;
            ++col;
            bool isEndTag = (*c == '/');
            if (isEndTag)
            {
                ++c;
                ++col;
            }
            String tagName;
            while (isgraph(*c) && (*c != '=') && (*c != '[') && (*c != ']'))
            {
                tagName += *c;
                token += *c;
                ++c;
                ++col;
            }
            while (isgraph(*c) && (*c != '[') && (*c != ']'))
            {
                if (isEndTag)
                    tagName += *c;
                token += *c;
                ++c;
                ++col;
            }
            tagName.toLower();
            if (*c == ']')
            {
                ++c;
                ++col;
                if (_tags.has(tagName))
                {
                    tokenType = isEndTag ? bbc_endtag : bbc_tag;
                }
                else
                {
                    token = filter(token);
                    tokenType = bbc_text;
                    if (isEndTag)
                        token = "[/" + token + "]";
                    else
                        token = "[" + token + "]";
                }
            }
            else
            {
                tokenType = bbc_text;
                if (isEndTag)
                    token = "[/" + token + "]";
                else
                    token = "[" + token + "]";
            }
        }
        else if (!iscntrl(*c) && !isspace(*c))
        {
            tokenType = bbc_text;
            while (!isspace(*c) && !iscntrl(*c) && (*c != '['))
            {
                token += *c;
                ++c;
                ++col;
            }

            // pick out emoticons
            char chars[4] = {':', ')', 0, 0};
            String name;
            size_t len = token.length();
            if (len < 2)
                goto add;
            char* begin = token.get();
            char* end = begin + len;
            char* unp = begin;
            char* p = begin;
            char* lim = end - 1;
            while (p < lim)
            {
                // p doesn't point to first character of emoticon -> skip
                if (!_emoticonChars.get((byte_t)*p))
                {
                    ++p;
                    continue;
                }

                // skip past '://' so we don't mess up URLs
                if ((p[0] == ':') && (p[1] == '/') && (p[2] == '/'))
                {
                    p += 3;
                    continue;
                }

                // :emoticon: ?
                size_t tagLen = 0;
                if (p[0] == ':')
                {
                    char* colonPtr;
                    for (colonPtr = p + 1; isalnum(*colonPtr); ++colonPtr)
                        ;
                    if (*colonPtr == ':')
                    {
                        *colonPtr = '\0';
                        name.set(p + 1);
                        *colonPtr = ':';
                        name.toLower();
                        if (isIsolatedTag(name))
                        {
                            tagLen = name.length() + 2;
                        }
                    }
                }

                // 2- or 3-character emoticon?
                if (tagLen == 0)
                {
                    chars[0] = *p;
                    chars[1] = *(p + 1);
                    chars[2] = '\0';
                    ASSERTD(chars[0] != '\0');
                    ASSERTD(chars[1] != '\0');
                    name.set(chars, false);
                    if (isIsolatedTag(name) && _emoticonPostChars.get(*(p + 2)))
                    {
                        tagLen = 2;
                    }
                    else if (*(p + 2) != '\0')
                    {
                        chars[2] = *(p + 2);
                        ASSERTD(chars[3] == '\0');
                        name.lengthInvalidate();
                        if (isIsolatedTag(name) && _emoticonPostChars.get(*(p + 3)))
                        {
                            tagLen = 3;
                        }
                    }
                }

                if (tagLen == 0)
                {
                    ++p;
                    continue;
                }

                // found an emoticon
                // .. preceding text is at [unp,p)
                // .. emoticon is at [p,p+tagLen)
                // .. next unp is (p+tagLen)

                // emit text before emoticon
                if (unp < p)
                {
                    size_t i = (unp - begin);
                    size_t len = (p - unp);
                    tokens->add(
                        new Token(bbc_text, token.subString(i, len), tokenLine, tokenCol + i));
                }

                // emit token
                name.assertOwner();
                tokens->add(new Token(bbc_tag, name, tokenLine, tokenCol + (p - begin)));

                // adjust unp, p
                unp = (p + tagLen);
                p += tagLen;
            }
            if (unp == end)
                continue;
            if (unp > begin)
            {
                tokenCol += (unp - begin);
                token = token.subString(unp - begin);
            }
        }
        else
        {
            ++c;
            ++col;
            continue;
        }
    add:
        utl::Token* tk = new Token(tokenType, token, tokenLine, tokenCol);
        IFDEBUG(tk->length());
        tokens->add(tk);
    }
    return tokens;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::parse(MemStream& os, utl::Deque& tokens) const
{
    Deque list;
    bbcode_context_t context = bbc_block;
    while (!tokens.empty())
    {
        Token* tk = takeHead(tokens);

        if (tk->id() == bbc_text)
        {
            *tk = toString(tk);
            list.add(tk);
        }
        else if (tk->id() == bbc_ws)
        {
            *tk = toString(tk);
            list.add(tk);
        }
        else if (tk->id() == bbc_nl)
        {
            *tk = toString(tk);
            removeWStail(list);
            removeWShead(tokens);
            list.add(tk);
        }
        else if (tk->id() == bbc_tag)
        {
            removeWShead(tokens);

            // read start tag
            String tagName, tagParam;
            readStartTag(*tk, tagName, tagParam);

            // find tag
            auto tag = utl::cast<BBcodeTag>(_tags.find(tagName));
            ASSERTD(tag != nullptr);

            // auto-close list_item tag?
            if ((context == bbc_list_item) && (tag->context() == bbc_list_item))
            {
                context = processTag(list, "*", tk->lineNo(), tk->colNo());
            }

            if (tag->isIsolated())
            {
                // isolated tag -> just process it
                String text;
                tag->process(this, text, *tk, tagParam, context, false);
                list.add(new Token(bbc_text, text, tk->lineNo(), tk->colNo()));
                delete tk;
            }
            else if (tag->isVerbatim())
            {
                String text;
                bool first = true;
                while (!tokens.empty())
                {
                    Token* t = takeHead(tokens);
                    bool isEnd = ((t->id() == bbc_endtag) && (*t == tagName));
                    if (isEnd)
                    {
                        delete t;
                        break;
                    }
                    if (first && (t->id() == bbc_nl))
                        text += "\n";
                    else
                        text += toString(*t, true);
                    first = false;
                    delete t;
                }
                tag->process(this, text, text, tagParam, context, false);
                list.add(new Token(bbc_text, text, tk->lineNo(), tk->colNo()));
                delete tk;
            }
            else
            {
                list.add(tk);
                context = tag->context();
            }
        }
        else if (tk->id() == bbc_endtag)
        {
            removeWStail(list);

            // ignore [/*]
            if (*tk == "*")
            {
                delete tk;
                continue;
            }

            // auto-close list_item tag?
            if ((context == bbc_list_item) && (*tk == "list"))
            {
                context = processTag(list, "*", tk->lineNo(), tk->colNo());
            }

            context = processTag(list, *tk, tk->lineNo(), tk->colNo());
            delete tk;
        }
    }

    // collect output
    for (auto tk_ : list)
    {
        Token& tk = utl::cast<Token>(*tk_);
        if ((tk.id() == bbc_tag) || (tk.id() == bbc_endtag))
        {
            os << toString(tk);
        }
        else
        {
            os << tk;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bbcode_context_t
BBcodeParser::processTag(utl::Deque& list, const String& name, uint_t lineNo, uint_t colNo) const
{
    bbcode_context_t context;
    // search backwards for matching start tag (any other start tag is inappropriate)
    utl::Deque::iterator it = list.end();
    bool done = list.empty(), processed = false;
    for (--it; !done; --it)
    {
        done = (it == list.begin());
        Token* t = (Token*)*it;
        if (t->id() == bbc_tag)
        {
            done = true;
            String tagName, tagParam;
            readStartTag(*t, tagName, tagParam);
            if (tagName == name)
            {
                processed = true;
                context = processTag(list, it);
            }
        }
    }
    if (!processed)
    {
        list.add(new Token(bbc_text, "[/" + name + "]", lineNo, colNo));
    }
    return context;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bbcode_context_t
BBcodeParser::processTag(utl::Deque& list, const utl::Deque::iterator& startTagIt) const
{
    // read start tag
    const Token* tk = (Token*)*startTagIt;
    uint_t lineNo = tk->lineNo(), colNo = tk->colNo();
    String tagName, tagParam;
    readStartTag(*tk, tagName, tagParam);

    // find the tag
    auto tag = utl::cast<BBcodeTag>(_tags.find(tagName));
    ASSERTD(tag != nullptr);

    // collect intermediate tokens -> text
    MemStream os;
    auto it = startTagIt;
    ++it;
    size_t numTokens = 1;
    bool first = true, leadingNL = false;
    while (it != list.end())
    {
        tk = (Token*)*it;
        if (first && (tk->id() == bbc_nl))
        {
            os << "\n";
            leadingNL = true;
        }
        else
        {
            os << *tk;
        }
        ++it;
        ++numTokens;
        first = false;
    }
    String text;
    text.set(os.takeString(), true, false);

    // truncate list to remove [startTagIt,end)
    while (numTokens--)
    {
        list.removeBack();
    }

    // get rid of trailing line break in list item?
    bool addNL = false;
    if ((tag->context() == bbc_list_item) && (text.compareSuffix("<br/>\n") == 0))
    {
        size_t len = text.length();
        if (leadingNL)
        {
            text[len - 6] = '\n';
            text[len - 5] = '\0';
        }
        else
            text[len - 6] = '\0';
        text.lengthInvalidate();
        addNL = true;
    }

    // find the enclosing context
    bbcode_context_t context = findContext(list);

    // process the tag
    bool indent = leadingNL || (tag->context() == bbc_list_item);
    tag->process(this, text, text, tagParam, context, indent);
    if (addNL)
        text += '\n';
    tk = new Token(bbc_text, text, lineNo, colNo);
    list.add(tk);

    // return the new context
    return context;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bbcode_context_t
BBcodeParser::findContext(utl::Deque& list) const
{
    bbcode_context_t context = bbc_block;
    utl::Deque::iterator it = list.end();
    bool done = list.empty();
    for (--it; !done; --it)
    {
        done = (it == list.begin());
        Token* tk = (Token*)*it;
        if (tk->id() != bbc_tag)
        {
            done = (it == list.begin());
            continue;
        }
        done = true;
        String tagName, tagParam;
        readStartTag(*tk, tagName, tagParam);
        auto tag = utl::cast<BBcodeTag>(_tags.find(tagName));
        ASSERTD(tag != nullptr);
        context = tag->context();
    }
    return context;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BBcodeParser::isIsolatedTag(const String& name) const
{
    auto tag = utl::cast<BBcodeTag>(_tags.find(name));
    return ((tag != nullptr) && tag->isIsolated());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
BBcodeParser::toString(const utl::Token* tk, bool verbatim)
{
    String text;
    switch (tk->id())
    {
    case bbc_text:
        text = filter(*tk);
        break;
    case bbc_ws:
        if (verbatim)
            text = String::repeat(' ', tk->length());
        else
            text = " ";
        break;
    case bbc_nl:
        text = "<br/>\n";
        break;
    case bbc_tag:
        text = "[" + filter(*tk) + "]";
        break;
    case bbc_endtag:
        text = "[/" + filter(*tk) + "]";
        break;
    }
    return text;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

utl::Token*
BBcodeParser::takeHead(utl::Deque& list)
{
    return utl::cast<Token>(list.popFront());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::removeWShead(utl::Deque& list)
{
    if (list.empty())
        return;
    auto tk = utl::cast<Token>(list.front());
    if (tk->id() == bbc_ws)
    {
        list.removeFront();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::removeWStail(utl::Deque& list)
{
    if (list.empty())
        return;
    auto tk = utl::cast<Token>(list.back());
    if (tk->id() == bbc_ws)
    {
        list.removeBack();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::removeNLtail(utl::Deque& list)
{
    if (list.empty())
        return;
    auto tk = utl::cast<Token>(list.back());
    if (tk->id() == bbc_nl)
    {
        list.removeBack();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BBcodeParser::readStartTag(const String& startTag, String& tagName, String& tagParam)
{
    size_t equalsIdx = startTag.find("=");
    if (equalsIdx == size_t_max)
    {
        tagName = startTag;
        tagParam.clear();
    }
    else
    {
        tagName = startTag.subString(0, equalsIdx);
        tagParam = startTag.subString(equalsIdx + 1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
