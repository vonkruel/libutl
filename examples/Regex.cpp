#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Regex.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    for (;;)
    {
        Regex regex;
        cout << "Enter a regex: " << flush;
        if (cin.eofBlocking())
            break;
        cin >> regex;
        if (regex.empty())
            break;
        if (!regex.compile())
        {
            cout << "Not a valid regex: " << regex << endl;
            continue;
        }
        for (;;)
        {
            String str;
            cout << "Enter a string: " << flush;
            if (cin.eofBlocking())
                break;
            cin >> str;
            if (str.empty())
                break;
            Bool match = (regex == str);
            cout << "match = " << match << endl;
            if (match.get())
            {
                for (;;)
                {
                    String rep;
                    cout << "Enter a replacement string: " << flush;
                    if (cin.eofBlocking())
                        break;
                    cin >> rep;
                    if (rep.empty())
                        break;
                    String strCopy = str;
                    regex.searchReplace(strCopy, rep);
                    cout << "result = " << strCopy << endl;
                }
            }
        }
    }
    return 0;
}
