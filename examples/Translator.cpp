#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Translator.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    StringTranslator st;

    // add translations
    st.add("foo", "bar");
    st.add("hexdigits", "0123456789abcdef");
    cout << "--- translations:" << endl;
    st.dump(cout);
    cout << endl;

    String str;
    str = "foo$(foo)!";
    cout << str << " -> " << st.translate(str) << endl;
    str = "the hex digits: $(hexdigits)";
    cout << str << " -> " << st.translate(str) << endl;
    return 0;
}
