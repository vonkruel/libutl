#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Graph.h>
#include <libutl/Queue.h>
#include <libutl/Regex.h>
#include <libutl/Token.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class GrammarNode;
class Production;
class ParseNode;
class RDparserState;
class Terminal;

////////////////////////////////////////////////////////////////////////////////////////////////////
// RDparser ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Recursive-descent parser.

   This is a fairly simple parser that should be adequate for many purposes.  RDparser allows you
   to define a grammar and then parse text that satisfies the grammar.  A grammar is defined by its
   productions and terminals.

   <b>Productions</b>

   A grammar production describes a rule for translating a production symbol into one or more
   production or terminal symbols.  A production symbol may also be referred to as a non-terminal
   symbol.

   The rules for writing a valid grammar production are themselves best described by writing a
   grammar, but don't worry I'll try to make it clear!

   \code
   PRODUCTION ::= BRANCH { \| BRANCH }
   BRANCH     ::= ATOM { ATOM }
   ATOM       ::= PRODUCTION
                | TERMINAL
                | \[ PRODUCTION \]
                | \{ PRODUCTION \}
   \endcode

   Let's examine each of these three productions:

   <ul>
   <li>A \b PRODUCTION consists of a \b BRANCH followed by zero or
   more additional <b>BRANCH</b>es.
   <li>A \b BRANCH consists of an \b ATOM followed by zero or more
   additional <b>ATOM</b>s.
   <li>An \b ATOM consists of one of the following:
   <ol>
   <li>a \b PRODUCTION
   <li>a \b TERMINAL
   <li>open-square-bracket \b PRODUCTION close-square-bracket
   <li>open-curly-bracket \b PRODUCTION close-curly-bracket
   </ol>
   </ul>

   As you can see:

   <ul>
   <li>Symbols enclosed in square brackets must be matched zero or one times.
   <li>Symbols enclosed in curly brackets must be matched zero or more times.
   <li>A symbol can be escaped with the '\' character to suppress its normal meaning.
   The \b ATOM production contains examples of this.  The square and curly brackets had to be
   escaped otherwise the rule's meaning is rather different, given that those symbols carry
   special meaning in the grammar for writing production rules.
   <li>The '|' character separates multiple possible translations, any of which can satisfy the
   production rule.
   </ul>

   Left-recursion is not supported.  That is, you cannot write a production rule whose first
   symbol is the name of the production itself.

   <b>Terminals</b>

   At some point we have to describe the actual characters we expect to see.  The "leaves" of
   the grammar consist of terminal symbols which are specified by regular expressions (Regex).
   For example, you might have a terminal symbol to represent a number, with a regular
   expression like <b>"\\d+(\\.\\d*)?"</b>.

   <b>General Instructions</b>

   After defining the grammar by defining productions (addProduction()) and terminals
   (addTerminal()), call compile() to create the state machine that will be used to parse the
   grammar.  Then you're ready to parse text (parse()).  To see a full example of RDparser in
   action, look at the example program that parses simple math expressions.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RDparser : public Object, protected FlagsMI
{
    UTL_CLASS_DECL(RDparser, Object);

public:
    void clear();

    virtual void copy(const Object& rhs);

    /**
       Add a production rule.
       \param id unique production rule id
       \param name production name
       \param rhs right-hand-side of production
    */
    void addProduction(uint_t id, const String& name, const String& rhs);

    /**
       Add a terminal symbol.
       \param id unique terminal symbol id
       \param name terminal name
       \param regex regular expression for terminal
    */
    void addTerminal(uint_t id, const String& name, const String& regex);

    /**
       Compile the state machine to parse the grammar.
       \return true if compilation successful, false otherwise
    */
    bool compile();

    /**
       Determine whether a properly defined grammar has been compiled.
       \return true if grammar compiled successfully, false otherwise
    */
    bool
    ok() const
    {
        return getFlag(flg_ok);
    }

    /**
       Parse text from the given stream until EOF.
       \return parse tree (Graph of ParseNode)
       \param stream input stream
       \param prod (optional) root production
    */
    Graph* parse(Stream& stream, const String* prod = nullptr);

    /**
       Parse the given string.
       \return parse tree (Graph of ParseNode)
       \param str input string
       \param prod (optional) root production
    */
    Graph* parse(const String& str, const String* prod = nullptr);

private:
    enum flg_t
    {
        flg_ok
    };

private:
    void init();
    void
    deInit()
    {
    }
    static String getToken(const TDequeIt<String>& it);
    static bool isSymbol(char c);
    static bool isSymbol(const TDequeIt<String>& it);
    ParseNode*
    parse(RDparserState& state, TDequeIt<Token>& it, Graph* parseTree, const GrammarNode* gn);
    GrammarNode* parseExpressionI(TDequeIt<String>& it, GrammarNode* expr = nullptr);
    GrammarNode* parseBranchI(TDequeIt<String>& it);
    GrammarNode* parseAtomI(TDequeIt<String>& it);
    void scan(RDparserState& state, Stream& stream);
    void scanI(Queue<String>& queue, const String& str);
    void
    setOK(bool _ok)
    {
        setFlag(flg_ok, _ok);
    }
    ParseNode*
    tryParse(RDparserState& state, TDequeIt<Token>& it, Graph* parseTree, const GrammarNode* gn);

private:
    TRBtree<Production> _productions;
    TRBtree<Terminal> _terminals;
    TArray<Terminal> _terminalsArray;
    String _rootProd;
    Graph _gramTree;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ParseNode ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Parse-tree node.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ParseNode : public Vertex
{
    UTL_CLASS_DECL(ParseNode, Vertex);

public:
    virtual void copy(const Object& rhs);

    /** Determine whether self is a production node. */
    bool
    isProduction() const
    {
        return (_prodId != uint_t_max);
    }

    /** Get the production id. */
    uint_t
    productionId() const
    {
        return _prodId;
    }

    /** Determine whether self is a terminal node. */
    bool
    isToken() const
    {
        return (_token != nullptr);
    }

    /** Get the token. */
    const Token&
    token() const
    {
        ASSERTD(_token != nullptr);
        return *_token;
    }

    /** Set the production id. */
    void
    setProductionId(uint_t prodId)
    {
        _prodId = prodId;
    }

    /** Set the token. */
    void
    setToken(const Token* token)
    {
        delete _token;
        _token = token;
        if (_token != nullptr)
            _token = _token->clone();
    }

private:
    void
    init()
    {
        _prodId = uint_t_max;
        _token = nullptr;
    }
    void
    deInit()
    {
        delete _token;
    }

private:
    uint_t _prodId;
    const Token* _token;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Base class for exceptions thrown by RDparser.
   \ingroup string
*/
UTL_EXCEPTION_DECL(RDparserEx, Exception);

////////////////////////////////////////////////////////////////////////////////////////////////////
// ParseEx /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Parse exception.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ParseEx : public RDparserEx
{
    UTL_CLASS_DECL(ParseEx, RDparserEx);

public:
    ParseEx(const Token* token);
    virtual void copy(const Object& rhs);
    virtual void dump(Stream& os) const;

private:
    void
    init()
    {
        _token = nullptr;
    }
    void deInit();

private:
    const Token* _token;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ScanEx //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Scan exception.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ScanEx : public RDparserEx
{
    UTL_CLASS_DECL(ScanEx, RDparserEx);
    UTL_CLASS_DEFID;

public:
    ScanEx(String* line, size_t lineNo, size_t colNo);
    virtual void copy(const Object& rhs);
    virtual void dump(Stream& os) const;

private:
    size_t _lineNo, _colNo;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
