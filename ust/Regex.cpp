#include <libutl/libutl.h>
#include <libutl/Regex.h>
#include <libutl/AutoPtr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// REnode /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class REnode : public Object
{
    UTL_CLASS_DECL(REnode, Object);

public:
    REnode(uint_t op, REnode* operand = nullptr)
    {
        init(op, operand);
    }

    const BitArray&
    charSet() const
    {
        return *_charSet;
    }

    REnode*
    next() const
    {
        return _next;
    }

    REnode*
    operand() const
    {
        return _operand;
    }

    uint_t
    op() const
    {
        return _operator;
    }

    uint_t
    parenNo() const
    {
        return _parenNo;
    }

    const String&
    string() const
    {
        return *_str;
    }

    uint_t
    minReps() const
    {
        return _minReps;
    }

    uint_t
    maxReps() const
    {
        return _maxReps;
    }

    String&
    string()
    {
        return *_str;
    }

    void
    setCharSet(BitArray* charSet)
    {
        delete _charSet;
        _charSet = charSet;
    }

    void
    setNext(REnode* next)
    {
        _next = next;
    }

    void
    setOperand(REnode* operand)
    {
        _operand = operand;
    }

    void
    setOp(uint_t op)
    {
        _operator = op;
    }

    void
    setParenNo(uint_t parenNo)
    {
        _parenNo = parenNo;
    }

    void
    setString(const String* str)
    {
        delete _str;
        _str = str->clone();
    }

    void
    setReps(uint_t minReps, uint_t maxReps = uint_t_max)
    {
        _minReps = minReps;
        _maxReps = maxReps;
    }

public:
    enum re_node_t
    {
        re_invalid,
        re_open_paren,
        re_close_paren,
        re_branch,
        re_end,
        re_line_begin,
        re_line_end,
        re_any_char,
        re_repeat,
        re_range_any,
        re_string
    };

private:
    void
    init(uint_t op = re_invalid, REnode* operand = nullptr)
    {
        _operator = op;
        _operand = operand;
        _next = nullptr;
        _str = nullptr;
        _charSet = nullptr;
        _parenNo = 0;
        _minReps = _maxReps = uint_t_max;
    }

    void
    deInit()
    {
        delete _str;
        delete _charSet;
    }

private:
    uint_t _operator;
    REnode *_operand, *_next;
    String* _str;
    BitArray* _charSet;
    uint_t _parenNo;
    uint_t _minReps;
    uint_t _maxReps;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Regex //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
Regex::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(Regex))
    {
        auto& rgx = utl::cast<Regex>(rhs);
        res = super::compare(rgx);
    }
    else if (rhs.isA(String))
    {
        auto& string = utl::cast<String>(rhs);
        res = match(string) ? 0 : -1;
    }
    else
    {
        res = match(rhs.toString()) ? 0 : -1;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::copy(const Object& rhs)
{
    auto& string = utl::cast<String>(rhs);
    super::copy(string);
    compile();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::steal(Object& rhs)
{
    auto& rhsRegex = utl::cast<Regex>(rhs);

    if (!_regex->empty())
        _regex->clear();
    if (_mustStr != nullptr)
        delete _mustStr;

    super::steal(rhsRegex);
    _start = rhsRegex._start;
    _regex = rhsRegex._regex;
    _parenNo = rhsRegex._parenNo;
    _firstChar = rhsRegex._firstChar;
    _mustStr = rhsRegex._mustStr;

    // make sure rhsRegex can be safely destructed
    rhsRegex._regex = nullptr;
    rhsRegex._mustStr = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::clear()
{
    _start = nullptr;
    _regex->clear();
    _parenNo = 0;
    _firstChar = '\0';
    delete _mustStr;
    _mustStr = nullptr;
    setOK(false);
    setAnchored(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Regex::compile()
{
    // scan tokens
    Queue<String> queue;
    scan(queue);

    // clear old parse information
    clear();

    // parse
    setOK(true);
    TDequeIt<String> it = queue.begin();
    ++_parenNo;
    _start = parseExpression(it);

    // if parse was successful
    if (ok())
    {
        // we need to setCharSet() for certain nodes
        for (auto node : *_regex)
        {
            uint_t op = node->op();
            if (op == REnode::re_branch)
            {
                BitArray* curChars = new BitArray(256);
                setCharSet(node->operand(), *curChars);
                node->setCharSet(curChars);
            }
            else if (op == REnode::re_repeat)
            {
                BitArray* nextChars = new BitArray(256);
                setCharSet(node->next(), *nextChars);
                node->setCharSet(nextChars);
            }
        }
        // try to optimize
        const REnode* node = _start;
        if (node->next()->op() == REnode::re_end)
        {
            node = node->operand();
            if (node->op() == REnode::re_line_begin)
            {
                setAnchored(true);
            }
            else if (node->op() == REnode::re_string)
            {
                _firstChar = node->string()[0];
            }
            size_t curLen = 1U;
            for (; node != nullptr; node = node->next())
            {
                if (node->op() == REnode::re_string)
                {
                    size_t strLen = node->string().length();
                    if (strLen > curLen)
                    {
                        curLen = strLen;
                        char* str = utl::strdup(node->string().get());
                        delete _mustStr;
                        _mustStr = new BoyerMooreSearch<>(true, str, strLen);
                    }
                }
            }
        }
    }
    else
    {
        clear();
    }

    return ok();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Regex::match(const String& str, RegexMatch* p_m, size_t idx) const
{
    if (p_m != nullptr)
        p_m->clear();

    if (!ok())
        return false;

    // must have the must string (if there is one)
    if ((_mustStr != nullptr) && (_mustStr->find(str.get(), str.length()) == nullptr))
    {
        return false;
    }

    // create our own temporary RegexMatch, or use the one given
    AutoPtr<RegexMatch> matchPtr;
    if (p_m == nullptr)
    {
        p_m = new RegexMatch();
        matchPtr = p_m;
    }
    RegexMatch& m = *p_m;
    TArray<Span<size_t>>* matches = new TArray<Span<size_t>>();
    for (uint_t i = 0; i < _parenNo; ++i)
    {
        *matches += new Span<size_t>();
    }
    m.set(str, matches);

    size_t i, j;
    Span<size_t>* matchSpan = m.matchSpan(0);

    if (isAnchored())
    {
        if (idx > 0)
        {
            m.clear();
            return false;
        }
        i = 0;
        if (match(m, i, _start))
        {
            matchSpan->set(0, i);
            return true;
        }
    }
    else
        for (size_t i = idx; str[i] != '\0'; i++)
        {
            // current char must match firstChar (if there is one)
            if ((_firstChar != '\0') && (str[i] != _firstChar))
                continue;

            j = i;
            if (match(m, j, _start))
            {
                matchSpan->set(i, j);
                return true;
            }
        }

    // match failed
    m.clear();
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Regex::searchReplace(String& str, const String& rep)
{
    size_t numReplaced = 0;
    for (size_t pos = 0; pos < str.length();)
    {
        RegexMatch m;
        bool match = this->match(String(str.get(), false), m, pos);
        if (match == false)
            break;
        Span<size_t>& matchSpan = m.matchSpan(0);
        if (matchSpan.size() == 0)
        {
            ++pos;
            continue;
        }
        String replaceString = m.replaceString(rep);
        str.replace(matchSpan.begin(), matchSpan.size(), replaceString);
        m.matchString().lengthInvalidate();
        pos = matchSpan.begin() + replaceString.length();
        ++numReplaced;
    }
    return numReplaced;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::init()
{
    _regex = new TArray<REnode>();
    _mustStr = nullptr;
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::deInit()
{
    delete _regex;
    delete _mustStr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Regex::getToken(const TDequeIt<String>& it)
{
    String res;
    String* tk = *it;
    if (tk != nullptr)
    {
        res = *tk;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Regex::isSymbol(const TDequeIt<String>& it)
{
    bool res = false;
    String* tk = *it;
    if (tk != nullptr)
        res = tk->isMarked();
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Regex::match(RegexMatch& m, size_t& idx, const REnode* node) const
{
    Vector<size_t>* saveIdxs;
    size_t len, saveIdx;
    uint_t reps, minReps, maxReps, parenNo;
    Span<size_t>* matchSpan;
    const BitArray* charSet;
    const String* str;
    const char* matchStr = m.matchString();
    while (node != nullptr)
    {
        const REnode* next = node->next();
        switch (node->op())
        {
        case REnode::re_open_paren:
            parenNo = node->parenNo();
            matchSpan = m.matchSpan(parenNo);
            saveIdx = idx;
            if (match(m, idx, next))
            {
                matchSpan->setBegin(saveIdx);
                return true;
            }
            else
            {
                matchSpan->setBegin(0);
                return false;
            }
            break;
        case REnode::re_close_paren:
            parenNo = node->parenNo();
            matchSpan = m.matchSpan(parenNo);
            saveIdx = idx;
            if (match(m, idx, next))
            {
                matchSpan->setEnd(saveIdx);
                return true;
            }
            else
            {
                matchSpan->setEnd(0);
                return false;
            }
            break;
        case REnode::re_branch:
            if (next->op() != REnode::re_branch)
            {
                next = node->operand();
            }
            else
            {
                while (node->op() == REnode::re_branch)
                {
                    if (node->charSet().get(matchStr[idx]))
                    {
                        saveIdx = idx;
                        if (match(m, idx, node->operand()))
                            return true;
                        idx = saveIdx;
                    }
                    node = node->next();
                }
                return false;
            }
            break;
        case REnode::re_end:
            return true;
            break;
        case REnode::re_line_begin:
            if (idx != 0)
                return false;
            break;
        case REnode::re_line_end:
            if (matchStr[idx] != '\0')
                return false;
            break;
        case REnode::re_any_char:
            if (matchStr[idx] != '\0')
                ++idx;
            else
                return false;
            break;
        case REnode::re_repeat:
            minReps = node->minReps();
            maxReps = node->maxReps();
            saveIdxs = new Vector<size_t>(256, 256);

            // match the operand up to <maxReps> times
            for (reps = 0; reps < maxReps; ++reps)
            {
                saveIdxs->set(reps, idx);
                if (!match(m, idx, node->operand()))
                {
                    idx = saveIdxs->get((size_t)reps);
                    break;
                }
            }
            saveIdxs->set(reps, idx);

            // work backwards, to a minimum of <minReps> matches
            charSet = node->charSet();
            for (; (reps >= minReps) && (reps != uint_t_max); --reps)
            {
                // only try a match if it's possible
                idx = saveIdxs->get(reps);
                if (charSet->get(matchStr[idx]) && match(m, idx, next))
                {
                    delete saveIdxs;
                    return true;
                }
            }

            // fail
            delete saveIdxs;
            return false;
            break;
        case REnode::re_range_any:
            charSet = node->charSet();
            if (charSet->get(matchStr[idx]) == false)
                return false;
            idx++;
            break;
        case REnode::re_string:
            str = node->string();
            len = str->length();
            if (strncmp(str->get(), matchStr + idx, len) == 0)
            {
                idx += len;
            }
            else
            {
                return false;
            }
            break;
        }
        node = next;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// EXPRESSION ::= BRANCH { | BRANCH } ...
// (implements 'OR')
REnode*
Regex::parseExpression(TDequeIt<String>& it, bool paren)
{
    REnode *expr, *prevNode, *branch;
    uint_t parenNo;

    if (paren)
    {
        expr = new REnode(REnode::re_open_paren);
        parenNo = _parenNo++;
        expr->setParenNo(parenNo);
        *_regex += expr;
        prevNode = expr;
    }
    else
    {
        prevNode = nullptr;
    }

    for (;;)
    {
        TDequeIt<String> saveIt = it;
        branch = new REnode(REnode::re_branch);
        branch->setOperand(parseBranch(it));

        if (branch->operand() == nullptr)
        {
            delete branch;
            it = saveIt;
            return nullptr;
        }

        *_regex += branch;

        // link with previous node
        if (prevNode == nullptr)
        {
            expr = branch;
        }
        else
        {
            prevNode->setNext(branch);
        }
        prevNode = branch;

        // if there's another branch, next token is "|" symbol
        if (isSymbol(it) && (getToken(it) == "|"))
        {
            ++it;
        }
        // if parenthesized, we need to find the closing paren
        else if ((!paren && (*it == nullptr)) || (paren && (getToken(it) == ")")))
        {
            break;
        }
        // else the parse has failed
        else
        {
            setOK(false);
            return nullptr;
        }
    }

    // link a terminating node to the chain
    REnode* end;
    if (paren)
    {
        end = new REnode(REnode::re_close_paren);
        end->setParenNo(parenNo);
    }
    else
    {
        end = new REnode(REnode::re_end);
    }
    *_regex += end;
    prevNode->setNext(end);

    // link tail of each branch to the end node
    for (const REnode* cur = (paren ? expr->next() : expr); cur != end; cur = cur->next())
    {
        setTail((REnode*)(cur->operand()), end);
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// BRANCH ::= PIECE { PIECE } ...
// (implements concatenation)
REnode*
Regex::parseBranch(TDequeIt<String>& it)
{
    REnode *branch = nullptr, *prevNode = nullptr, *piece;
    for (;;)
    {
        // parse a piece
        TDequeIt<String> saveIt = it;
        piece = parsePiece(it);
        if (piece == nullptr)
        {
            it = saveIt;
            if (prevNode != nullptr)
            {
                setOK(true);
            }
            break;
        }

        // link with previous
        if (prevNode == nullptr)
        {
            branch = prevNode = piece;
        }
        else
        {
            setTail(prevNode, piece);
        }
        if ((isSymbol(it) && (getToken(it) == ")")) || (*it == nullptr))
        {
            break;
        }
    }

    return branch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// PIECE ::= ATOM { * | + | ? | \{ NUMBER [, NUMBER] \} }
REnode*
Regex::parsePiece(TDequeIt<String>& it)
{
    // parse an atom
    TDequeIt<String> saveIt = it;
    REnode* atom = parseAtom(it);
    if (!ok())
    {
        it = saveIt;
        return nullptr;
    }

    // next must be a symbol
    if (!isSymbol(it))
        return atom;

    bool fixString = (atom->op() == REnode::re_string) && (atom->string().length() > 1);

    REnode* piece;
    String tk = getToken(it);
    if (tk == "*")
    {
        ++it;
        piece = new REnode(REnode::re_repeat, atom);
        piece->setReps(0);
        *_regex += piece;
    }
    else if (tk == "+")
    {
        ++it;
        piece = new REnode(REnode::re_repeat, atom);
        piece->setReps(1);
        *_regex += piece;
    }
    else if (tk == "?")
    {
        ++it;
        piece = new REnode(REnode::re_repeat, atom);
        piece->setReps(0, 1);
        *_regex += piece;
    }
    else if (tk == "{")
    {
        ++it;
        String repsSpec = getToken(it);
        ++it;
        if (!isSymbol(it) || (getToken(it) != "}"))
        {
            delete atom;
            setOK(false);
            return nullptr;
        }
        ++it;
        size_t commaIdx = repsSpec.find(",");
        String minRepsStr = repsSpec.subString(0, commaIdx);
        String maxRepsStr = repsSpec.subString(commaIdx + 1);
        uint_t minReps = Uint(minRepsStr);
        uint_t maxReps;
        if (maxRepsStr.empty())
        {
            if (commaIdx == size_t_max)
                maxReps = minReps;
            else
                maxReps = uint_t_max;
        }
        else
        {
            maxReps = Uint(maxRepsStr);
        }
        piece = new REnode(REnode::re_repeat, atom);
        piece->setReps(minReps, maxReps);
        *_regex += piece;
    }
    else
    {
        piece = atom;
        fixString = false;
    }

    // break off the last char in the string operand?
    if (fixString)
    {
        auto& str = atom->string();
        size_t strLen = str.length();
        String ch = str.chop(strLen - 1, strLen);

        // create atom2, link with piece
        REnode* atom2 = new REnode(REnode::re_string);
        atom2->setString(ch);
        *_regex += atom2;
        piece->setOperand(atom2);

        // link string atom with piece, return it
        atom->setNext(piece);
        return atom;
    }

    return piece;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// ATOM ::= ( EXPRESSION )
//        | ^
//        | $
//        | .
//        | [ BRACKET_EXPRESSION ]
//        | { char }
REnode*
Regex::parseAtom(TDequeIt<String>& it)
{
    REnode* atom = nullptr;
    if (isSymbol(it))
    {
        String tk = getToken(it);
        if (tk == "(")
        {
            TDequeIt<String> saveIt = it;
            ++it;
            atom = parseExpression(it, true);
            if (!ok() || !isSymbol(it) || (getToken(it) != ")"))
            {
                it = saveIt;
                atom = nullptr;
                goto fail;
            }
            ++it;
            return atom;
        }
        else if (tk == "^")
        {
            ++it;
            atom = new REnode(REnode::re_line_begin);
        }
        else if (tk == "$")
        {
            ++it;
            atom = new REnode(REnode::re_line_end);
        }
        else if (tk == ".")
        {
            ++it;
            atom = new REnode(REnode::re_any_char);
        }
        else if (tk == "[")
        {
            ++it;

            // translate bracket expression -> charSet
            atom = new REnode(REnode::re_range_any);
            BitArray& charSet = *new BitArray(256);
            atom->setCharSet(charSet);

            // if matching inverse, mark all characters as allowed to start
            bool tv = !(isSymbol(it) && (getToken(it) == "^"));
            if (!tv)
                charSet.setAll(true);
            parseBracketExpr(it, charSet, true);
            charSet[0] = false;
            if (!ok())
                goto fail;
        }
        else if ((tk == "\\a") || (tk == "\\A"))
        {
            ++it;
            bool tv = (tk == "\\a");
            atom = new REnode(REnode::re_range_any);
            BitArray& charSet = *new BitArray(256);
            atom->setCharSet(charSet);
            for (uint_t c = 0; c < 256; c++)
                charSet[c] = isalpha(c) ? tv : !tv;
        }
        else if ((tk == "\\d") || (tk == "\\D"))
        {
            ++it;
            bool tv = (tk == "\\d");
            atom = new REnode(REnode::re_range_any);
            BitArray& charSet = *new BitArray(256);
            atom->setCharSet(charSet);
            for (uint_t c = 0; c < 256; c++)
                charSet[c] = isdigit(c) ? tv : !tv;
        }
        else if ((tk == "\\s") || (tk == "\\S"))
        {
            ++it;
            bool tv = (tk == "\\s");
            atom = new REnode(REnode::re_range_any);
            BitArray& charSet = *new BitArray(256);
            atom->setCharSet(charSet);
            for (uint_t c = 0; c < 256; c++)
                charSet[c] = isspace(c) ? tv : !tv;
        }
        else if ((tk == "\\w") || (tk == "\\W"))
        {
            ++it;
            bool tv = (tk == "\\w");
            atom = new REnode(REnode::re_range_any);
            BitArray& charSet = *new BitArray(256);
            atom->setCharSet(charSet);
            for (uint_t c = 0; c < 256; c++)
                charSet[c] = (isalnum(c) || (c == '_')) ? tv : !tv;
        }
        else
        {
            goto fail;
        }
    }
    else
    {
        String tk = getToken(it);
        if (tk.empty())
            goto fail;
        atom = new REnode(REnode::re_string);
        atom->setString(tk);
        ++it;
    }
    *_regex += atom;
    return atom;
fail:
    delete atom;
    setOK(false);
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::parseBracketExpr(TDequeIt<String>& it, BitArray& charSet, bool tv)
{
    uint_t c = uint_t_max;

    // invert ?
    if (isSymbol(it) && (getToken(it) == '^'))
    {
        tv = !tv;
        ++it;
    }

    while (ok())
    {
        String tk = getToken(it);
        size_t tklen = tk.length();
        bool symbol = isSymbol(it);
        if (symbol && (tk == '['))
        {
            ++it;
            parseBracketExpr(it, charSet, tv);
        }
        else if (symbol && (tk == ']'))
        {
            ++it;
            return;
        }
        else if ((tklen >= 6) && (tk[0] == ':') && (tk[tklen - 1] == ':'))
        {
            ++it;
            if (getToken(it) != "]")
                goto fail;
            tklen -= 2;
            tk = tk.subString(1, tklen);
            if (tk == "alnum")
            {
                for (c = 1; c < 256; ++c)
                    if (isalnum(c))
                        charSet[c] = tv;
            }
            else if (tk == "alpha")
            {
                for (c = 1; c < 256; ++c)
                    if (isalpha(c))
                        charSet[c] = tv;
            }
            else if (tk == "blank")
            {
                for (c = 1; c < 256; ++c)
                    if (isblank(c))
                        charSet[c] = tv;
            }
            else if (tk == "cntrl")
            {
                for (c = 1; c <= 0x1f; ++c)
                    charSet[c] = tv;
                charSet[0x7f] = tv;
            }
            else if (tk == "digit")
            {
                for (c = 1; c < 256; ++c)
                    if (isdigit(c))
                        charSet[c] = tv;
            }
            else if (tk == "graph")
            {
                for (c = 1; c < 256; ++c)
                    if (isgraph(c))
                        charSet[c] = tv;
            }
            else if (tk == "lower")
            {
                for (c = 1; c < 256; ++c)
                    if (islower(c))
                        charSet[c] = tv;
            }
            else if (tk == "odigit")
            {
                for (c = '0'; c <= '7'; ++c)
                    charSet[c] = tv;
            }
            else if (tk == "print")
            {
                for (c = 1; c < 256; ++c)
                    if (isprint(c))
                        charSet[c] = tv;
            }
            else if (tk == "punct")
            {
                for (c = 1; c < 256; ++c)
                    if (ispunct(c))
                        charSet[c] = tv;
            }
            else if (tk == "space")
            {
                for (c = 1; c < 256; ++c)
                    if (isspace(c))
                        charSet[c] = tv;
            }
            else if (tk == "upper")
            {
                for (c = 1; c < 256; ++c)
                    if (isupper(c))
                        charSet[c] = tv;
            }
            else if (tk == "word")
            {
                for (c = 1; c < 256; ++c)
                    if (isalnum(c) || (c == '_'))
                        charSet[c] = tv;
            }
            else if (tk == "xdigit")
            {
                for (c = 1; c < 256; ++c)
                    if (isxdigit(c))
                        charSet[c] = tv;
            }
            else
            {
                goto fail;
            }
        }
        else
        {
            // \a, \d, \s, \w are disallowed in bracketed expression
            if (isSymbol(it) && (tklen > 1))
                goto fail;

            ++it;
            uint_t last = uint_t_max;
            bool range = false;
            for (size_t i = 0; i < tklen; ++i)
            {
                c = tk[i];
                if (range)
                {
                    range = false;
                    for (uint_t j = last + 1; j < c; ++j)
                    {
                        charSet[j] = tv;
                    }
                    last = uint_t_max;
                }
                else if ((c == '-') && (last != uint_t_max) && (i < (tklen - 1)))
                {
                    range = true;
                    continue;
                }
                charSet[c] = tv;
                last = c;
            }
        }
    }
fail:
    setOK(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::scan(Queue<String>& queue)
{
    // fast lookup for symbol-ness
    byte_t isSymbol[256];
    memset(isSymbol, 0, 256);
    isSymbol[(byte_t)'^'] = 1;
    isSymbol[(byte_t)'$'] = 1;
    isSymbol[(byte_t)'|'] = 1;
    isSymbol[(byte_t)'.'] = 1;
    isSymbol[(byte_t)'*'] = 1;
    isSymbol[(byte_t)'+'] = 1;
    isSymbol[(byte_t)'?'] = 1;
    isSymbol[(byte_t)'('] = 1;
    isSymbol[(byte_t)')'] = 1;
    isSymbol[(byte_t)'{'] = 1;
    isSymbol[(byte_t)'}'] = 1;
    isSymbol[(byte_t)'['] = 1;
    isSymbol[(byte_t)']'] = 1;

    size_t i = 0, len = length();
    String token;
    while (i < len)
    {
        char c = get(i++);
        if ((c == '\\') && (i < len))
        {
            c = get(i++);
            switch (c)
            {
            case '\\':
                token += '\\';
                break;
            case 'f':
                token += '\f';
                break;
            case 'n':
                token += '\n';
                break;
            case 'r':
                token += '\r';
                break;
            case 't':
                token += '\t';
                break;
            case 'v':
                token += '\v';
                break;
            case 'x':
                if ((i + 1) < len)
                {
                    uint_t c1 = tolower(get(i++));
                    uint_t c2 = tolower(get(i++));
                    if (c1 >= 'a')
                        c1 = 10 + (c1 - 'a');
                    else
                        c1 -= '0';
                    if (c2 >= 'a')
                        c2 = 10 + (c2 - 'a');
                    else
                        c2 -= '0';
                    c = (c1 << 4) | c2;
                }
                else
                {
                    c = 'x';
                }
                token += c;
                break;
            default:
                if (isSymbol[(byte_t)c])
                {
                    token += c;
                }
                else
                {
                    if (!token.empty())
                    {
                        queue.enQ(token);
                    }
                    token = '\\';
                    token += c;
                    token.setMarked(true);
                    queue.enQ(token);
                    token.clear();
                    token.setMarked(false);
                }
            }
        }
        else if (isSymbol[(byte_t)c])
        {
            if (!token.empty())
            {
                queue.enQ(token);
            }
            token = c;
            token.setMarked(true);
            queue.enQ(token);
            token.clear();
            token.setMarked(false);
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
        queue.enQ(token);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//
// If we start matching at the given node, what is the set of possible characters?
//
bool
Regex::setCharSet(const REnode* node, BitArray& charSet)
{
    uint_t i;
    for (;;)
    {
        if (node == nullptr)
        {
            charSet.setAll(true);
            return true;
        }
        const BitArray* nodeCharSet;
        bool ct;
        switch (node->op())
        {
        case REnode::re_branch:
            ct = true;
            for (;;)
            {
                ct = ct && setCharSet(node->operand(), charSet);
                node = node->next();
                if (node->op() != REnode::re_branch)
                {
                    if (ct)
                    {
                        return true;
                    }
                    else
                    {
                        return setCharSet(node, charSet);
                    }
                }
            }
            break;
        case REnode::re_line_end:
            charSet.set(0, true);
            return true;
            break;
        case REnode::re_any_char:
            charSet.setAll(true);
            return true;
            break;
        case REnode::re_repeat:
            setCharSet(node->operand(), charSet);
            if (node->minReps() == 0)
            {
                return setCharSet(node->next(), charSet);
            }
            return true;
            break;
        case REnode::re_range_any:
            nodeCharSet = node->charSet();
            for (i = 0; i < 256; i++)
            {
                if (nodeCharSet->get(i))
                    charSet.set(i, true);
            }
            return true;
            break;
        case REnode::re_string:
            charSet[node->string()[0]] = true;
            return true;
            break;
        }
        node = node->next();
    }
    // should never get here...
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Regex::setTail(REnode* chain, REnode* tail)
{
    REnode* cur = chain;
    while (cur->next() != nullptr)
    {
        cur = cur->next();
    }
    cur->setNext(tail);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
RegexMatch::replaceString(const String& str) const
{
    String res;
    size_t i, j, len = str.length();
    const Span<size_t>* matchSpan;
    for (i = 0; i < len;)
    {
        char c = str[i];
        if (c == '&')
        {
            matchSpan = this->matchSpan(0);
            res += _matchStr.subString(matchSpan->begin(), matchSpan->end() - matchSpan->begin());
            i++;
        }
        else if (c == '\\')
        {
            c = str[++i];
            if (isdigit(c))
            {
                char numStr[16];
                for (j = 0; isdigit(str[i]); i++, j++)
                {
                    if (j < 15)
                        numStr[j] = c;
                }
                numStr[min((size_t)15, j)] = '\0';
                uint_t parenNo = atoi(numStr);
                ASSERTD(parenNo < this->numMatchSpans());
                matchSpan = this->matchSpan(parenNo);
                res +=
                    _matchStr.subString(matchSpan->begin(), matchSpan->end() - matchSpan->begin());
            }
            else
            {
                res += '\\';
                res += c;
                i++;
            }
        }
        else
        {
            res += c;
            i++;
        }
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::REnode);
UTL_CLASS_IMPL(utl::Regex);
UTL_CLASS_IMPL(utl::RegexMatch);

////////////////////////////////////////////////////////////////////////////////////////////////////

template class utl::AutoPtr<utl::RegexMatch>;
UTL_INSTANTIATE_TPL(utl::TArray, utl::REnode);
