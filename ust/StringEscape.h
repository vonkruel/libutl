#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BitArray.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Character-based substitution.

   StringEscape replaces characters in a string with strings you provide.  The default for all
   characters is to pass them through without performing substitution.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class StringEscape : public Object
{
    UTL_CLASS_DECL(StringEscape, Object);
    UTL_CLASS_NO_COMPARE;
    UTL_CLASS_NO_COPY;

public:
    /**
       Add a substitution.
       \param ch character to perform substitution for
       \param str string to replace \b c with
       \param owner (optional : false) take ownership of s?
    */
    void add(char ch, const char* str, bool owner = false);

    /**
       Perform substitution.
       \param out output string
       \param in input string
       \param prefix (optional) write at the start of the output
       \param suffix (optional) write at the end of the output
    */
    void process(String& out,
                 const String& in,
                 const char* prefix = nullptr,
                 const char* suffix = nullptr) const;

private:
    void init();
    void deInit();

private:
    struct sub_t
    {
        const char* s;
        char buf[6];
        byte_t len;
        byte_t owner;
    };

private:
    BitArray _hasSub;
    sub_t _subs[256];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
