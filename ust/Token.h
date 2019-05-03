#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Scanned token.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Token : public String
{
    UTL_CLASS_DECL(Token, String);

public:
    /**
        Constructor.
        \param id id
        \param token string token
        \param lineNo line number where token was found
        \param colNo column number where token was found
    */
    Token(uint_t id, const String& token, size_t lineNo, size_t colNo)
        : String(token)
    {
        _id = id;
        _lineNo = lineNo;
        _colNo = colNo;
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    /// \name Accessors
    //@{
    uint_t
    id() const
    {
        return _id;
    }

    size_t
    colNo() const
    {
        return _colNo;
    }

    size_t
    lineNo() const
    {
        return _lineNo;
    }
    //@}
private:
    void
    init()
    {
        _id = 0;
    }
    void
    deInit()
    {
    }

private:
    uint_t _id;
    size_t _lineNo, _colNo;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
