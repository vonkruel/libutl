#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for object->object mapping.

   <b>Attributes</b>

   \arg <b>owner flag</b> : If isOwner(), objects returned by map() are owned by the
   Mapping-derived object and do not need to be deleted by the client.  Otherwise (!isOwner()),
   the client is responsible for deleting objects returned by map().

   \author Adam McKee
   \ingroup abstract
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Mapping : public Object, protected FlagsMI
{
    UTL_CLASS_DECL_ABC(Mapping, Object);
    UTL_CLASS_DEFID;

public:
    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Set the \b owner flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /**
       Return the mapping for the given object.
       \return mapped object (nullptr if mapping undefined)
       \param object object to get mapping for
    */
    virtual Object* map(const Object* object) const = 0;

private:
    enum flg_t
    {
        flg_owner
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
