#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/RBtree.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Set of strings.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class StringSet : public TRBtree<String>
{
    UTL_CLASS_DECL(StringSet, TRBtree<String>);

public:
    /**
       Constructor.
       \param owner ownership flag
    */
    StringSet(bool owner);

    /**
       Add a string to the set.
       \return true if string added successfully, false if it already exists
       \param str string to be added
    */
    bool add(const String& str);

    /**
       Find a string in the set.
       \return found string (nullptr if none)
       \param key search key
    */
    String* find(const String& key) const;

private:
    void init(bool owner = true);
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
