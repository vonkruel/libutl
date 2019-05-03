#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BufferedFileStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Externally stored array of objects.

   ExtArray is intended for write-once/read-many (WORM) applications.  Removal of objects isn't
   supported.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ExtArray : public Object
{
    UTL_CLASS_DECL(ExtArray, Object);

public:
    /**
       Constructor.
       \param baseName base pathname for object and index files
       \param rtc class of all objects in the array (nullptr if unknown)
       \param serializeMode serialization mode (see utl::serialize_t)
    */
    ExtArray(const String& baseName,
             const RunTimeClass* rtc = nullptr,
             uint_t serializeMode = uint_t_max)
    {
        init();
        open(baseName, rtc, serializeMode);
    }

    /** Get number of items. */
    size_t
    items() const
    {
        return _items;
    }

    /**
       Open a new array.
       \param baseName base pathname for object and index files
       \param rtc class of all objects in the array (nullptr if unknown)
       \param serializeMode serialization mode (see utl::serialize_t)
    */
    void open(const String& baseName,
              const RunTimeClass* rtc = nullptr,
              uint_t serializeMode = ser_default);

    /** Close the currently open array. */
    void close();

    /**
       Add an object.
       \param object object to add
    */
    void add(const Object& object);

    /**
       Get the object at the given index.
       The caller is responsible for deleting the returned object.
       \return found object (nullptr if none)
       \param idx array index
    */
    Object* get(size_t idx) const;

    /**
       Get the object at the given index.
       \return found object (nullptr if none)
       \param idx array index
    */
    Object* operator[](size_t idx) const
    {
        return get(idx);
    }

private:
    void
    init()
    {
        _items = size_t_max;
    }
    void
    deInit()
    {
    }
    bool
    isOpen() const
    {
        return (_items != size_t_max);
    }

private:
    size_t _items;
    mutable BufferedFileStream _idxFile;
    mutable BufferedFileStream _objectFile;
    const RunTimeClass* _rtc;
    uint_t _serializeMode;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
