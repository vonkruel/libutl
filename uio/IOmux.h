#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/Stream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

/**
   Stream (de)-multiplexor.

   An IOmux stream has one or more associated streams.  When you write to an IOmux stream, you
   write to \b all of its associated streams.  When you read from an IOmux stream, you read
   from whichever associated stream has been selected for reading.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class IOmux : public Stream
{
    UTL_CLASS_DECL(IOmux, Stream);

public:
    /**
       Constructor.
       \param mode see utl::io_t
       \param owner \b owner flag for associated streams
    */
    IOmux(uint_t mode, bool owner = true)
    {
        init(mode, owner);
    }

    /** Revert to initial state. */
    virtual void close();

    virtual void copy(const Object& rhs);

    /** Add a stream. */
    bool
    add(const Stream* stream)
    {
        setError(false);
        return _streams.add(stream);
    }

    virtual void checkOK();

    /** Remove a stream. */
    bool remove(const Stream* stream);

    /** Select one of the associated streams for input. */
    bool setInput(const Stream* stream);

    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return _streams.isOwner();
    }

    /** Set the \b owner flag. */
    void
    setOwner(bool owner)
    {
        super::setOwner(owner);
        _streams.setOwner(owner);
    }

    /** Get the array of associated stream. */
    const TArray<Stream>&
    streams() const
    {
        return _streams;
    }

    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max);

    virtual void write(const byte_t* array, size_t num);

protected:
    size_t _input;
    TArray<Stream> _streams;

private:
    void init(uint_t mode = io_rdwr, bool owner = true);
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
