#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Stream.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinaryData;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Memory stream.

   MemStream allows you to stream data to/from a memory region.  An attempt to read past the end
   of the region causes a StreamEOFex to be thrown.  An attempt to write past the end of the region
   causes the region to be grown in size.

   <b>Attributes</b>

   \arg \b size : The initial size of the memory region.

   \arg \b increment : When the region is to be grown, the growth increment determines the amount
   of growth.  For example, if increment = 16, the region will always be grown by a multiple of 16.
   As a special case, if increment = size_t_max, the region will be doubled in size until it is
   >= the required size.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class MemStream : public Stream
{
    UTL_CLASS_DECL(MemStream, Stream);

public:
    /**
       Constructor.
       \param size initial size
       \param increment (optional : size_t_max) growth increment
    */
    MemStream(size_t size, size_t increment = size_t_max)
    {
        init(nullptr, size, true, increment);
    }

    /**
       Constructor.
       \param array memory region
       \param size memory region size
       \param owner (optional : true) \b owner flag
       \param increment (optional : size_t_max) growth increment
    */
    MemStream(byte_t* array, size_t size, bool owner = true, size_t increment = size_t_max)
    {
        init(array, size, owner, increment);
    }

    virtual void
    close()
    {
        clear();
    }

    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max);

    virtual void write(const byte_t* array, size_t num);

    /** Get address of allocated array. */
    const byte_t*
    get() const
    {
        return _array.get();
    }

    /** Get size of allocated array. */
    size_t
    arraySize() const
    {
        return _array.size();
    }

    /** Move written data to BinaryData object. */
    void takeData(BinaryData& data);

    /** Retrieve copy of data as string. */
    char* copyString() const;

    /** Convert to a string. */
    char* takeString();

    /** Seek the "get pointer" to the given position. */
    void
    seekg(size_t pos)
    {
        ASSERTD(pos <= _array.size());
        _readPos = pos;
    }

    /** Seek the "put pointer" to the given position. */
    void
    seekp(size_t pos)
    {
        _writePos = pos;
        if (_writePos > _array.size())
        {
            _array.grow(_writePos);
        }
    }

    /** Get the position of the "get pointer". */
    size_t
    tellg() const
    {
        return _readPos;
    }

    /** Get the position of the "put pointer". */
    size_t
    tellp() const
    {
        return _writePos;
    }

    /**
       Reserve allocation space.
       \param size required minimum size
       \param increment (optional : size_t_max) growth increment
    */
    void
    reserve(size_t size, size_t increment = size_t_max)
    {
        _array.reserve(size, increment);
    }

    /**
       Truncate to the given size (do nothing unless the new size is smaller).
       \param size new size
    */
    void truncate(size_t size);

    /**
       Set a new memory region.
       \param array memory region
       \param size memory region size
       \param owner (optional : true) \b owner flag
       \param increment (optional : size_t_max) growth increment
    */
    void set(byte_t* array, size_t size, bool owner = true, size_t increment = size_t_max);

    /** Set ownership flag. */
    void
    setOwner(bool owner)
    {
        _array.setOwner(owner);
    }

    /** Set the growth increment. */
    void
    setIncrement(size_t increment)
    {
        _array.setIncrement(increment);
    }

protected:
    virtual void
    clear()
    {
        set(nullptr, 0);
    }

private:
    void init(byte_t* array = nullptr,
              size_t size = KB(4),
              bool owner = true,
              size_t increment = size_t_max);
    void
    deInit()
    {
    }

private:
    Vector<byte_t> _array;
    size_t _readPos;
    size_t _writePos;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
