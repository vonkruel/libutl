#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Array.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Char.h>
#include <libutl/RBtree.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    RBtree lhs;
    RBtree rhs;
    Array out(false);

    // set up lhs
    lhs += Char('a');
    lhs += Char('b');
    lhs += Char('d');
    lhs += Char('f');
    lhs += Char('g');

    // set up rhs
    rhs += Char('b');
    rhs += Char('c');
    rhs += Char('e');
    rhs += Char('g');
    rhs += Char('h');

    // show contents of lhs, rhs
    cout << "lhs: " << lhs << endl;
    cout << "rhs: " << rhs << endl;
    cout << "---" << endl;

    // difference
    lhs.difference(rhs, out);
    cout << "lhs items not in rhs: " << out << endl;
    out.clear();

    rhs.difference(lhs, out);
    cout << "rhs items not in lhs: " << out << endl;
    out.clear();

    // symmetricDifference
    lhs.symmetricDifference(rhs, out);
    cout << "symmetric difference: " << out << endl;
    out.clear();

    // intersection
    lhs.intersection(rhs, out);
    cout << "intersection: " << out << endl;
    out.clear();

    // merge (a.k.a. union)
    lhs.merge(rhs, out);
    cout << "union: " << out << endl;
    out.clear();

    // multiKeyQuickSort
    out.setOwner(true);
    out += new String("zabc");
    out += new String("cdba");
    out += new String("cdab");
    out += new String("abce");
    out += new String("abcd");
    cout << "multiKeyQuickSort: " << out;
    out.multiKeyQuickSort(false);
    cout << " = " << out << endl;
    out.clear();
    out.setOwner(false);

    return 0;
}
