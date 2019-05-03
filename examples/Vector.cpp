#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
BufferedStream&
operator<<(BufferedStream& os, const utl::Vector<T>& v)
{
    os << "[";
    bool first = true;
    for (auto e : v)
    {
        if (!first)
            cout << ",";
        first = false;
        os << e;
    }
    os << "]";
    return os;
}

int
Test::run(int, char**)
{
    Vector<int> v;
    for (int i = 0; i < 10; ++i)
        v.append(i);
    cout << "v = " << v << endl;
    cout << "<insert [1] at position 1>" << endl;
    v.insert(1, 1, 1);
    cout << "v = " << v << endl;
    cout << "<remove 2 objects at position 1>" << endl;
    v.remove(1, 2);
    cout << "v = " << v << endl;
    return 0;
}
