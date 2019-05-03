#include <libutl/libutl.h>
#include <libutl/HostOS.h>
#include <libutl/Tokenizer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::HostOS);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
HostOS::run(const String& cmd)
{
    // tokenize cmd
    StringTokenizer tokenizer;
    TokenizerTokens tokens;
    tokenizer.addTerminal(0, "\"(\\\\\"|[^\"])*\"");
    tokenizer.addTerminal(1, "'(\\'|[^'])*'");
    tokenizer.addTerminal(2, "[:graph:]+");
    tokenizer.scan(tokens, cmd);

    // convert tokens -> args[]
    TArray<String> args;
    while (!tokens.empty())
    {
        String tk = tokens.next();
        if ((tk[0] == '"') || (tk[0] == '\''))
        {
            tk = tk.subString(1, tk.length() - 2).backslashUnescaped();
        }
        args += tk;
    }

    // run the command
    int res = run(args(0), args);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HostOS::yield() const
{
    // probably good enough in many cases
    msleep(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
LinuxHostOS* hostOS;
#elif UTL_HOST_TYPE == UTL_HT_WINDOWS
WindowsHostOS* hostOS;
#else
HostOS* hostOS;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
