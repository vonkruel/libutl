#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

void
printTokens(const String& str, bool pq)
{
    cout << "tokenizing " << str << " (processQuotes = " << Bool(pq) << ")" << endl;
    size_t idx = 0;
    for (;;)
    {
        String tk = str.nextToken(idx, ',', pq);
        if (tk.empty())
            break;
        cout << "tk = '" << tk << "'" << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    const char* string_0 = "  abcd efgh  ";
    const char* string_1 = "abcd efgh";
    String test = string_0;
    test.trim();
    ASSERTD(test == string_1);

    test = "a ,  bb, cc, d'  ' , eee";
    printTokens(test, true);
    printTokens(test, false);
    return 0;
}
