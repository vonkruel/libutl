#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/AutoPtr.h>
#include <libutl/Encoder.h>
#include <libutl/Hashtable.h>
#include <libutl/Mapping.h>
#include <libutl/MemStream.h>
#include <libutl/Pair.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Translator //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Simple translator.

   Translator is a specialization of Encoder that allows you to define a set of (string) => (string)
   mappings, and then apply those mappings to some text.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Translator : public Encoder
{
    UTL_CLASS_DECL(Translator, Encoder);

public:
    /**
       Constructor.
       \param stream input stream
       \param owner (optional : true) input stream ownership flag
       \param mapping (optional) dynamic mapping
       \param mappingOwner (optional : true) mapping ownership flag
       \param rememberMappings (optional : false) record successful mappings
                               for fast lookup next time?
       \param prefix prefix character for substitutable strings
    */
    Translator(Stream* stream,
               bool owner = true,
               Mapping* mapping = nullptr,
               bool mappingOwner = true,
               bool rememberMappings = false,
               char prefix = '$')
    {
        set(stream, owner, mapping, mappingOwner, rememberMappings, prefix);
    }

    /**
       Add a mapping.
       \param lhs left-hand-side string
       \param rhs right-hand-side string
    */
    void
    add(const String& lhs, const String& rhs)
    {
        _xlats += new Pair(lhs.clone(), rhs.clone());
    }

    void clear();

    virtual void
    dump(Stream& os, uint_t level = uint_t_max) const
    {
        _xlats.dump(os, level);
    }

    virtual size_t encode(const byte_t* block, size_t num);

    virtual size_t
    decode(byte_t* block, size_t num)
    {
        ABORT();
        return 0;
    }

    /** Get the mapping. */
    const Mapping*
    getMapping() const
    {
        return _mapping;
    }

    /** Get the \b rememberMappings flag. */
    bool
    getRememberMappings() const
    {
        return getFlag(flg_rememberMappings);
    }

    /** Get the \b mappingOwner flag. */
    bool
    isMappingOwner() const
    {
        return getFlag(flg_mappingOwner);
    }

    /**
       Set a new configuration.
       \param stream input stream
       \param owner (optional : true) input stream ownership flag
       \param mapping (optional) dynamic mapping
       \param mappingOwner (optional : true) mapping ownership flag
       \param rememberMappings (optional : false) record successful mappings
                               for fast lookup next time
       \param prefix prefix character for substitutable strings
    */
    void set(Stream* stream,
             bool owner = true,
             Mapping* mapping = nullptr,
             bool mappingOwner = true,
             bool rememberMappings = false,
             char prefix = '$');

    /**
       Set the mapping object.
       \param mapping (optional) dynamic mapping
       \param mappingOwner (optional : true) mapping ownership flag
       \param rememberMappings (optional : false) record successful mappings
                               for fast lookup next time?
    */
    void
    setMapping(Mapping* mapping, bool mappingOwner = true, bool rememberMappings = false)
    {
        if (isMappingOwner())
            delete _mapping;
        _mapping = mapping;
        setFlag(flg_mappingOwner, mappingOwner);
        setFlag(flg_rememberMappings, rememberMappings);
    }

private:
    void
    init()
    {
        _mapping = nullptr;
    }

    void
    deInit()
    {
        setMapping(nullptr);
    }

    const String* translate(const String& token);

private:
    enum flg_t
    {
        flg_mappingOwner = 12,
        flg_rememberMappings
    };

private:
    String _prefix;
    Hashtable _xlats;
    Mapping* _mapping;
    AutoPtr<> _mappedObject;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// StringTranslator ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   String translator.

   This is a wrapper for Translator that can perform translations on an input string instead of
   an input stream.

   \author Adam McKee
   \ingroup string
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class StringTranslator : public Object
{
    UTL_CLASS_DECL(StringTranslator, Object);

public:
    StringTranslator(Mapping* mapping,
                     bool mappingOwner = true,
                     bool rememberMappings = true,
                     char prefix = '$')
    {
        set(mapping, mappingOwner, rememberMappings, prefix);
    }

    /**
       Set a new configuration.
       \param mapping (optional) dynamic mapping
       \param mappingOwner (optional : true) mapping ownership flag
       \param rememberMappings (optional : false) record successful mappings
                               for fast lookup next time?
       \param prefix prefix character for substitutable strings
    */
    void set(Mapping* mapping,
             bool mappingOwner = true,
             bool rememberMappings = true,
             char prefix = '$');

    /**
       Add a mapping.
       \param lhs left-hand-side string
       \param rhs right-hand-side string
    */
    void
    add(const String& lhs, const String& rhs)
    {
        _translator.add(lhs, rhs);
    }

    virtual void
    dump(Stream& os, uint_t level = uint_t_max) const
    {
        _translator.dump(os, level);
    }

    /**
       Translate the given string.
       \return translated string
       \param str input string
    */
    String translate(const String& str);

private:
    void
    init()
    {
        set(nullptr);
    }

    void
    deInit()
    {
        _translator.set(nullptr);
    }

private:
    Translator _translator;
    MemStream _ms;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
