#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Hashtable.h>
#include <libutl/Pair.h>
#include <libutl/SortedCollection.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class Pair;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A set of (variable-name, variable-value) tuples.

   \ingroup collection
   \author Adam McKee
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class StringVars : public utl::Object
{
    UTL_CLASS_DECL(StringVars, Object);

public:
    virtual int compare(const utl::Object& rhs) const;

    virtual void copy(const utl::Object& rhs);

    virtual void serialize(utl::Stream& stream, uint_t io, uint_t mode = utl::ser_default);

    virtual size_t innerAllocatedSize() const;

    /** Empty? */
    bool
    empty() const
    {
        return _vars.empty();
    }

    /** Is the given variable null? */
    bool
    isNull(const char* name) const
    {
        return isNull(String(name, false));
    }

    /** Is the given variable null */
    bool isNull(const String& name) const;

    /** Remove the given variable (make it null). */
    void
    setNull(const char* name)
    {
        setNull(String(name, false));
    }

    /** Remove the given variable (make it null). */
    void setNull(const String& name);

    /**
       Get the value of a variable.
       \return variable value (empty string if not found)
       \param name variable name
    */
    const String&
    valueOf(const char* name) const
    {
        return valueOf(String(name, false));
    }

    /**
       Get the value of a variable.
       \return variable value (empty string if not found)
       \param name variable name
    */
    const String& valueOf(const String& name) const;

    /**
       Get the value of a variable (non-const version).
       \return variable value (empty string if not found)
       \param name variable name
       \param create create the variable if it doesn't exist?
    */
    String&
    valueOf(const char* name, bool create)
    {
        return valueOf(String(name, false), create);
    }

    /**
       Get the value of a variable (non-const version).
       \return variable value (empty string if not found)
       \param name variable name
       \param create create the variable if it doesn't exist?
    */
    String& valueOf(const String& name, bool create);

    /**
       Set a variable.
       \return utl::Pair* if the name wasn't known, nullptr otherwise
       \param name variable name
       \param value variable value
    */
    Pair*
    setValue(const char* name, const char* value)
    {
        return setValue(new String(name, false), new String(value, false));
    }

    /**
       Set a variable.
       \return utl::Pair* if the name wasn't known, nullptr otherwise
       \param name variable name
       \param value variable value
    */
    Pair*
    setValue(const String& name, const char* value)
    {
        return setValue(name.clone(), new String(value, false));
    }

    /**
       Set a variable.
       \return utl::Pair* if the name wasn't known, nullptr otherwise
       \param name variable name
       \param value variable value
    */
    Pair*
    setValue(const char* name, const String& value)
    {
        return setValue(new String(name, false), value.clone());
    }

    /**
       Set a variable.
       \return utl::Pair* if the name wasn't known, nullptr otherwise
       \param name variable name
       \param value variable value
    */
    Pair*
    setValue(const String& name, const String& value)
    {
        return setValue(name.clone(), value.clone());
    }

    /**
       Set a variable.
       \return utl::Pair* if the name wasn't known, nullptr otherwise
       \param name variable name
       \param value variable value
    */
    Pair* setValue(String* name, String* value);

    /** Clear out contents. */
    void
    clear()
    {
        _vars.clear();
    }

    /** Get the container. */
    utl::Collection&
    container()
    {
        return _vars;
    }

    /** Get the container. */
    const utl::Collection&
    container() const
    {
        return _vars;
    }

    /** Add variables from another instance. */
    void add(const StringVars& vars);

    /** Intersect variables with given collection. */
    void intersect(const utl::SortedCollection& vars);

private:
    void
    init()
    {
    }
    void
    deInit()
    {
        ASSERTD(nonConstEmptyString.empty());
    }

private:
    static String nonConstEmptyString;
    utl::Hashtable _vars;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
