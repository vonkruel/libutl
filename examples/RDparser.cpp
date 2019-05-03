#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Float.h>
#include <libutl/RDparser.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

enum prod_t
{
    pr_expr,
    pr_term,
    pr_atom
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum term_t
{
    tm_number,
    tm_plus_or_minus,
    tm_mult_or_div,
    tm_open_paren,
    tm_close_paren
};

////////////////////////////////////////////////////////////////////////////////////////////////////

double
eval(const ParseNode* pn)
{
    double res;
    TArray<ParseNode> edges(false);
    edges.copyItems(pn->edges());
    const ParseNode* opPN;
    double n;

    switch (pn->productionId())
    {
    case pr_expr:
        ASSERTD(edges.items() == 2);
        res = eval(edges[0]);
        pn = edges[1];
        if (pn->edges() != nullptr)
        {
            pn = (const ParseNode*)pn->edges()->get();
            edges.clear();
            edges.copyItems(pn->edges());
            opPN = edges[0];
            n = eval(edges[1]);
            if (opPN->token() == "+")
                res += n;
            else
                res -= n;
        }
        break;
    case pr_term:
        ASSERTD(edges.items() == 2);
        res = eval(edges[0]);
        pn = edges[1];
        if (pn->edges() != nullptr)
        {
            pn = (const ParseNode*)pn->edges()->get();
            edges.clear();
            edges.copyItems(pn->edges());
            opPN = edges[0];
            n = eval(edges[1]);
            if (opPN->token() == "*")
                res *= n;
            else
                res /= n;
        }
        break;
    case pr_atom:
        if (edges.items() == 0)
        {
            res = Float(pn->token());
        }
        else
        {
            res = eval(edges[1]);
        }
        break;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    RDparser parser;

    // add productions (a.k.a. non-terminals)
    parser.addProduction(pr_expr, "expression", "term [ +_or_- expression ]");
    parser.addProduction(pr_term, "term", "atom [ *_or_/ term ]");
    parser.addProduction(pr_atom, "atom", "number");
    parser.addProduction(pr_atom, "atom", "( expression )");

    // add terminals
    parser.addTerminal(tm_number, "number", "\\d+(\\.\\d*)?");
    parser.addTerminal(tm_plus_or_minus, "+_or_-", "\\+|^-");
    parser.addTerminal(tm_mult_or_div, "*_or_/", "\\*|^/");
    parser.addTerminal(tm_open_paren, "(", "\\(");
    parser.addTerminal(tm_close_paren, ")", "\\)");

    // compile the grammar tree
    parser.compile();

    // read an expression
    for (;;)
    {
        String expr;
        cout << "enter an expression: " << flush;

        try
        {
            cin >> expr;
        }
        catch (StreamEOFex&)
        {
            break;
        }

        if (expr.empty())
        {
            break;
        }

        // parse and evaluate the expression
        try
        {
            Graph* parseTree = parser.parse(expr);
            cout << eval((ParseNode*)(parseTree->getStart())) << endl;
            delete parseTree;
        }
        // catch a ScanEx or ParseEx
        catch (RDparserEx& ex)
        {
            ex.dump(cerr);
        }
    }

    return 0;
}
