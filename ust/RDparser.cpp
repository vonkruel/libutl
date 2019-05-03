#include <libutl/libutl.h>
#include <libutl/MemStream.h>
#include <libutl/RDparser.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ParseNode //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
ParseNode::copy(const Object& rhs)
{
    auto& pn = utl::cast<ParseNode>(rhs);
    Vertex::copy(pn);
    _prodId = pn._prodId;
    setToken(pn._token);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Production /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class Production : public Object, protected FlagsMI
{
    UTL_CLASS_DECL(Production, Object);

public:
    Production(uint_t id, const String& name, const String& rhs)
    {
        _id = id;
        _name = name;
        _rhs = rhs;
    }

    virtual void copy(const Object& rhs);

    virtual const Object&
    getKey() const
    {
        return _name;
    }

    uint_t
    id() const
    {
        return _id;
    }

    const String&
    name() const
    {
        return _name;
    }

    const String&
    rhs() const
    {
        return _rhs;
    }

    void
    setName(const String& name)
    {
        _name = name;
    }

    void
    setRHS(const String& rhs)
    {
        _rhs = rhs;
    }

private:
    void
    init(uint_t id = 0)
    {
        _id = id;
    }
    void
    deInit()
    {
    }

private:
    uint_t _id;
    String _name;
    String _rhs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Production::copy(const Object& rhs)
{
    if (rhs.isA(Production))
    {
        auto& prod = utl::cast<Production>(rhs);
        FlagsMI::copyFlags(prod);
        _id = prod._id;
        _name = prod._name;
        _rhs = prod._rhs;
    }
    else
    {
        ABORT();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Terminal ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class Terminal : public Regex
{
    UTL_CLASS_DECL(Terminal, Regex);

public:
    Terminal(uint_t id, const String& name, const String& regex)
        : Regex(regex)
    {
        init(id);
        _name = name;
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual const Object&
    getKey() const
    {
        return _name;
    }

    uint_t
    id() const
    {
        return _id;
    }

    const String&
    name() const
    {
        return _name;
    }

    void
    setName(const String& name)
    {
        _name = name;
    }

private:
    void
    init(uint_t id = 0)
    {
        _id = id;
    }
    void
    deInit()
    {
    }

private:
    uint_t _id;
    String _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Terminal::compare(const Object& rhs) const
{
    if (rhs.isA(Terminal))
    {
        auto& term = utl::cast<Terminal>(rhs);
        return _name.compare(term._name);
    }
    else
    {
        return super::compare(rhs);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Terminal::copy(const Object& rhs)
{
    super::copy(rhs);
    if (rhs.isA(Terminal))
    {
        auto& term = utl::cast<Terminal>(rhs);
        _id = term._id;
        _name = term._name;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// GrammarNode ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

enum gnode_t
{
    gn_invalid,
    gn_and,
    gn_or,
    gn_zero_or_one,
    gn_zero_plus,
    gn_production,
    gn_terminal
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class GrammarNode : public Vertex
{
    UTL_CLASS_DECL(GrammarNode, Vertex);

public:
    GrammarNode(uint_t op)
    {
        init(op);
    }

    virtual void copy(const Object& rhs);

    virtual const Object&
    getKey() const
    {
        if (_production != nullptr)
            return _production->name();
        if (_terminal != nullptr)
            return _terminal->name();
        return emptyString;
    }

    uint_t
    op() const
    {
        return _op;
    }

    const Production&
    production() const
    {
        return *_production;
    }

    const Terminal&
    terminal() const
    {
        return *_terminal;
    }

    bool
    isProduction() const
    {
        return (_production != nullptr);
    }

    bool
    isTerminal() const
    {
        return (_terminal != nullptr);
    }

    void
    setOp(uint_t op)
    {
        _op = op;
    }

    void
    setProduction(Production* production)
    {
        _production = production;
    }

    void
    setTerminal(Terminal* terminal)
    {
        _terminal = terminal;
    }

private:
    void
    init(uint_t op = gn_invalid)
    {
        _op = op;
        _production = nullptr;
        _terminal = nullptr;
    }

    void
    deInit()
    {
    }

private:
    uint_t _op;
    Production* _production;
    Terminal* _terminal;
    static String emptyString;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
GrammarNode::copy(const Object& rhs)
{
    if (rhs.isA(GrammarNode))
    {
        auto& gn = utl::cast<GrammarNode>(rhs);
        Vertex::copy(gn);
        _op = gn._op;
        _production = gn._production;
        _terminal = gn._terminal;
    }
    else
    {
        ABORT();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RDparserState //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class RDparserState
{
public:
    RDparserState()
    {
        lastToken = tokenQ.end();
        lastTokenOK = tokenQ.end();
        ok = true;
    }

public:
    Queue<Token> tokenQ;
    TDequeIt<Token> lastToken;
    TDequeIt<Token> lastTokenOK;
    bool ok;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

String GrammarNode::emptyString;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RDparser ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::clear()
{
    _terminals.clear();
    _terminalsArray.clear();
    _rootProd.clear();
    _gramTree.clear();
    setOK(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::copy(const Object& rhs)
{
    if (rhs.isA(RDparser))
    {
        const RDparser& parser = (const RDparser&)(rhs);
        clear();
        _productions = parser._productions;
        _terminals = parser._terminals;
        _terminalsArray = parser._terminalsArray;
        _rootProd = parser._rootProd;
        _gramTree = parser._gramTree;
    }
    else
    {
        ABORT();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::addProduction(uint_t id, const String& name, const String& rhs)
{
    Production* prod = _productions.findT(name);
    if (prod == nullptr)
    {
        _productions += new Production(id, name, rhs);
    }
    else
    {
        prod->setRHS(prod->rhs() + " | " + rhs);
    }
    if (_rootProd.empty())
        _rootProd = name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::addTerminal(uint_t id, const String& name, const String& regex)
{
    Terminal* terminal = new Terminal(id, name, String("^") + regex);
    ASSERTD(terminal->ok());
    _terminals += terminal;
    _terminalsArray += terminal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RDparser::compile()
{
    setOK(true);
    _gramTree.clear();

    // create vertices for productions
    for (auto prod : _productions)
    {
        GrammarNode* gn = new GrammarNode(gn_production);
        gn->setProduction(prod);
        _gramTree += gn;
    }

    // create vertices for terminals
    for (auto term : _terminals)
    {
        GrammarNode* gn = new GrammarNode(gn_terminal);
        gn->setTerminal(term);
        _gramTree += gn;
    }

    // create the grammar tree
    TDeque<GrammarNode> prodNodes(false);
    for (auto gn_ : _gramTree)
    {
        GrammarNode& gn = utl::cast<GrammarNode>(*gn_);
        prodNodes += gn;
    }
    for (auto gn : prodNodes)
    {
        if (gn->op() == gn_production)
        {
            Queue<String> tokenQ;
            scanI(tokenQ, gn->production().rhs());
            TDequeIt<String> it = tokenQ.begin();
            parseExpressionI(it, gn);
            if (!ok())
                break;
        }
    }

    if (!ok())
        clear();
    return ok();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Graph*
RDparser::parse(Stream& stream, const String* prod)
{
    if (!ok())
        return nullptr;

    // find the starting production
    if (prod == nullptr)
        prod = &_rootProd;
    auto gn = utl::cast<GrammarNode>(_gramTree.find(*prod));
    if (gn == nullptr)
        return nullptr;

    // scan input
    RDparserState state;
    scan(state, stream);

    // create the parseTree
    auto parseTree = new Graph;
    parseTree->setMultiSet(true);
    auto it = state.tokenQ.begin();
    parseTree->setStart(parse(state, it, parseTree, gn));

    // detect trailing garbage (we must end on the "<eof>" token)
    if (state.ok)
    {
        Token* tk = *state.lastTokenOK;
        if ((tk == nullptr) || (tk->id() != uint_t_max))
        {
            state.ok = false;
        }
    }

    // clean up if the parse failed
    if (!state.ok)
    {
        delete parseTree;
        throw ParseEx(utl::clone(*state.lastToken));
    }

    return parseTree;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Graph*
RDparser::parse(const String& str, const String* prod)
{
    MemStream ms((byte_t*)str.get(), str.length() + 1, 0, false);
    return parse(ms, prod);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::init()
{
    _gramTree.setMultiSet(true);
    _terminals.setOrdering(new KeyOrdering);
    _terminalsArray.setOwner(false);
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
RDparser::getToken(const TDequeIt<String>& it)
{
    String res;
    String* tk = *it;
    if (tk != nullptr)
        res = *tk;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RDparser::isSymbol(char c)
{
    return (c == '|') || (c == '{') || (c == '}') || (c == '[') || (c == ']');
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
RDparser::isSymbol(const TDequeIt<String>& it)
{
    bool res = false;
    String* tk = *it;
    if (tk != nullptr)
        res = tk->isMarked();
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParseNode*
RDparser::parse(RDparserState& state, TDequeIt<Token>& it, Graph* parseTree, const GrammarNode* gn)
{
    TDeque<GrammarNode> edges(false);
    TDequeIt<Token> saveIt = it;
    TDequeIt<Token> saveLastTokenOK = state.lastTokenOK;
    ParseNode* pn = new ParseNode;
    ParseNode* newPN = nullptr;
    uint_t op = gn->op();
    uint_t numEdges;

    if (gn->isTerminal())
    {
        const Token* tk = *it;
        if (tk == nullptr)
            goto fail;
        const Terminal& term = gn->terminal();
        // possibly set state.lastToken
        bool setLastToken = false;
        const Token* lastToken = *state.lastToken;
        if ((lastToken == nullptr) || (tk->lineNo() > lastToken->lineNo()) ||
            ((tk->lineNo() == lastToken->lineNo()) && (tk->colNo() > lastToken->colNo())))
        {
            setLastToken = true;
        }
        if (tk->id() == term.id())
        {
            ++it;
            state.lastTokenOK = it;
            if (setLastToken)
                state.lastToken = it;
            pn->setToken(tk);
            parseTree->add(pn);
            return pn;
        }
        else
        {
            if (setLastToken)
                state.lastToken = it;
            goto fail;
        }
    }

    if (gn->isProduction())
    {
        pn->setProductionId(gn->production().id());
    }

    edges.copyItems(gn->edges());
    GrammarNode* subGN;
    ParseNode* subPN;
    switch (op)
    {
    case gn_and:
        for (auto edge : edges)
        {
            subPN = parse(state, it, parseTree, edge);
            if (!state.ok)
                goto fail;
            ASSERTD(subPN != nullptr);
            pn->addDirectedEdge(subPN);
        }
        break;
    case gn_or:
        for (auto edge : edges)
        {
            subPN = tryParse(state, it, parseTree, edge);
            if (!state.ok)
            {
                state.ok = true;
                continue;
            }
            ASSERTD(subPN != nullptr);
            pn->addDirectedEdge(subPN);
            goto succeed;
        }
        goto fail;
        break;
    case gn_zero_or_one:
        subGN = utl::cast<GrammarNode>(edges.first());
        subPN = tryParse(state, it, parseTree, subGN);
        state.ok = true;
        if (subPN != nullptr)
            pn->addDirectedEdge(subPN);
        break;
    case gn_zero_plus:
        subGN = utl::cast<GrammarNode>(edges.first());
        for (;;)
        {
            subPN = tryParse(state, it, parseTree, subGN);
            state.ok = true;
            if (subPN == nullptr)
                break;
            pn->addDirectedEdge(subPN);
        }
        break;
    }

succeed:
    numEdges = (pn->edges() == nullptr) ? 0 : slist_items(pn->edges());
    if (numEdges == 1)
        newPN = utl::cast<ParseNode>(pn->edges()->get());

    // only one edge -> possibly cut out the "middleman" (pn)
    if ((newPN != nullptr) && ((op == gn_and) || (op == gn_or)) &&
        !(pn->isProduction() && newPN->isProduction()))
    {
        if (pn->isProduction())
        {
            ASSERTD(!newPN->isProduction());
            newPN->setProductionId(pn->productionId());
        }
        delete pn;
        pn = newPN;
    }
    else
    {
        parseTree->add(pn);
    }
    return pn;

fail:
    delete pn;
    it = saveIt;
    state.lastTokenOK = saveLastTokenOK;
    state.ok = false;
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// EXPRESSION ::= BRANCH { | BRANCH } ...
// (implements 'OR')
GrammarNode*
RDparser::parseExpressionI(TDequeIt<String>& it, GrammarNode* expr)
{
    bool exprGiven = (expr != nullptr);
    if (!exprGiven)
        expr = new GrammarNode();
    expr->setOp(gn_or);

    for (;;)
    {
        TDequeIt<String> saveIt = it;
        GrammarNode* branch = parseBranchI(it);
        if (branch == nullptr)
        {
            it = saveIt;
            if (expr->edges() == nullptr)
            {
                if (!exprGiven)
                {
                    delete expr;
                }
                return nullptr;
            }
            else
            {
                setOK(true);
                break;
            }
        }
        expr->addDirectedEdge(branch);

        if (isSymbol(it) && (getToken(it) == "|"))
        {
            it++;
        }
    }

    if (!exprGiven)
    {
        _gramTree += expr;
    }

    return expr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// BRANCH ::= ATOM { ATOM } ...
// (implements concatenation)
GrammarNode*
RDparser::parseBranchI(TDequeIt<String>& it)
{
    GrammarNode* branch = new GrammarNode(gn_and);

    for (;;)
    {
        TDequeIt<String> saveIt = it;
        GrammarNode* atom = parseAtomI(it);
        if (atom == nullptr)
        {
            it = saveIt;
            if (branch->edges() == nullptr)
            {
                delete branch;
                return nullptr;
            }
            else
            {
                setOK(true);
                break;
            }
        }
        branch->addDirectedEdge(atom);
    }

    _gramTree += branch;

    return branch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// ATOM ::= EXPRESSION
//        | TERMINAL
//        | [ EXPRESSION ]
//        | { EXPRESSION }
GrammarNode*
RDparser::parseAtomI(TDequeIt<String>& it)
{
    GrammarNode *atom, *expr;

    if (isSymbol(it))
    {
        TDequeIt<String> saveIt = it;
        String tk = getToken(it);
        if (tk == "[")
        {
            it++;
            atom = new GrammarNode(gn_zero_or_one);
            _gramTree += atom;
            expr = parseExpressionI(it);
            if (!ok() || !isSymbol(it) || (getToken(it) != "]"))
            {
                it = saveIt;
                goto fail;
            }
            it++;
            atom->addDirectedEdge(expr);
        }
        else if (tk == "{")
        {
            it++;
            atom = new GrammarNode(gn_zero_plus);
            _gramTree += atom;
            expr = parseExpressionI(it);
            if (!ok() || !isSymbol(it) || (getToken(it) != "}"))
            {
                it = saveIt;
                goto fail;
            }
            it++;
            atom->addDirectedEdge(expr);
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
        auto gn = utl::cast<GrammarNode>(_gramTree.find(tk));
        if (gn == nullptr)
        {
            goto fail;
        }
        if (gn->isProduction() || gn->isTerminal())
        {
            it++;
            atom = gn;
        }
        else
        {
            goto fail;
        }
    }

    return atom;
fail:
    setOK(false);
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::scan(RDparserState& state, Stream& stream)
{
    Regex whitespace("^\\s+");

    // Scan tokens until the stream is at EOF
    for (size_t line = 0;; ++line)
    {
        String str;
        try
        {
            stream >> str;
        }
        catch (StreamEOFex&)
        {
            state.tokenQ += new Token(uint_t_max, "<eof>", line, 0);
            break;
        }
        size_t i = 0;
        size_t strLen = str.length();
        while (i < strLen)
        {
            String curStr(str.get() + i, false);
            // figure out what token matches the current input
            bool match = false;
            for (auto terminal : _terminalsArray)
            {
                RegexMatch m;
                if (terminal->match(curStr, m))
                {
                    Token* tk = new Token(terminal->id(), m.replaceString("&"), line, i);
                    state.tokenQ += tk;
                    i += tk->length();
                    match = true;
                    break;
                }
            }

            // is it whitespace?
            if (!match)
            {
                RegexMatch m;
                if (whitespace.match(curStr, m))
                {
                    i += m.matchSpan(0).size();
                    match = true;
                }
            }

            // throw an exception if no valid token found
            if (!match)
                throw ScanEx(str, line, i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
RDparser::scanI(Queue<String>& queue, const String& str)
{
    size_t i = 0, len = str.length();
    String token;
    while (i < len)
    {
        char c = str.get(i++);
        if (isspace(c))
        {
            if (!token.empty())
            {
                queue.enQ(token);
                token.clear();
            }
        }
        else if (c == '\\')
        {
            c = str.get(i++);
            if (c == '\\')
            {
                token += '\\';
            }
            else
            {
                if (!token.empty())
                    queue.enQ(token);
                if (isSymbol(c))
                {
                    token = c;
                    queue.enQ(token);
                    token.clear();
                }
                else
                {
                    token = '\\';
                    token += c;
                    token.setMarked(true);
                    queue.enQ(token);
                    token.clear();
                    token.setMarked(false);
                }
            }
        }
        else if (isSymbol(c))
        {
            if (!token.empty())
                queue.enQ(token);
            token = c;
            token.setMarked(true);
            queue.enQ(token);
            token.clear();
            token.setMarked(false);
            continue;
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
    {
        queue.enQ(token);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ParseNode*
RDparser::tryParse(RDparserState& state,
                   TDequeIt<Token>& it,
                   Graph* parseTree,
                   const GrammarNode* gn)
{
    Graph subParseTree;
    subParseTree.setMultiSet(true);
    ParseNode* pn = parse(state, it, subParseTree, gn);
    if (state.ok)
    {
        // parseTree gets subParseTree's elements
        parseTree->setOwner(false);
        Collection& subParseTreeCol = subParseTree;
        parseTree->add(subParseTreeCol);
        parseTree->setOwner(true);
        // subParseTree loses ownership of elements
        subParseTree.setOwner(false);
    }
    return pn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ParseEx ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

ParseEx::ParseEx(const Token* token)
{
    setName(String("parse failed", false, false));
    _token = token->clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ParseEx::copy(const Object& rhs)
{
    if (rhs.isA(ParseEx))
    {
        auto& ex = utl::cast<ParseEx>(rhs);
        Exception::copy(ex);
        _token = ex._token->clone();
    }
    else
    {
        ABORT();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ParseEx::dump(Stream& os) const
{
    os << name() << ": "
       << "line " << _token->lineNo() + 1 << ", column " << _token->colNo() + 1 << ", at ";
    if (_token->id() == uint_t_max)
    {
        os << *_token;
    }
    else
    {
        os << "'" << *_token << "'";
    }
    os << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ParseEx::deInit()
{
    delete _token;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ScanEx /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

ScanEx::ScanEx(String* line, size_t lineNo, size_t colNo)
{
    setName(String("scan failed", false, false));
    setObject(*line);
    _lineNo = lineNo;
    _colNo = colNo;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ScanEx::copy(const Object& rhs)
{
    if (rhs.isA(ScanEx))
    {
        auto& ex = utl::cast<ScanEx>(rhs);
        Exception::copy(ex);
        _lineNo = ex._lineNo;
        _colNo = ex._colNo;
    }
    else
    {
        ABORT();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ScanEx::dump(Stream& os) const
{
    os << name() << ": ";
    os << "line " << _lineNo + 1 << ", column " << _colNo + 1 << endl;
    os << "    " << *utl::cast<String>(object()) << endl;
    os << "    ";
    for (uint_t i = 0; i != _colNo; i++)
    {
        os << " ";
    }
    os << "^" << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ParseNode);
UTL_CLASS_IMPL(utl::Production);
UTL_CLASS_IMPL(utl::Terminal);
UTL_CLASS_IMPL(utl::GrammarNode);
UTL_CLASS_IMPL(utl::RDparser);
UTL_CLASS_IMPL(utl::ParseEx);
UTL_CLASS_IMPL(utl::ScanEx);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_EXCEPTION_IMPL(utl, RDparserEx, utl::Exception, nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TArray, utl::ParseNode);
UTL_INSTANTIATE_TPL(utl::TArray, utl::Terminal);
UTL_INSTANTIATE_TPL(utl::TDeque, utl::GrammarNode);
UTL_INSTANTIATE_TPL(utl::TDequeIt, utl::GrammarNode);
UTL_INSTANTIATE_TPL(utl::TDeque, utl::Token);
UTL_INSTANTIATE_TPL(utl::TDequeIt, utl::Token);
UTL_INSTANTIATE_TPL(utl::Queue, utl::Token);
UTL_INSTANTIATE_TPL(utl::TRBtree, utl::Production);
UTL_INSTANTIATE_TPL(utl::TRBtreeIt, utl::Production);
UTL_INSTANTIATE_TPL(utl::TRBtree, utl::Terminal);
UTL_INSTANTIATE_TPL(utl::TRBtreeIt, utl::Terminal);
