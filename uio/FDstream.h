#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Stream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Stream with file descriptor.

   FDstream is a base class for streams that have an associated file descriptor.  Examples of
   streams that have file descriptors include disk files (FileStream) and TCP sockets (TCPsocket).

   FDstream \b is a concrete class though, and if the exact kind of stream is unknown, it can
   be used without specializing it.

   <b>Attributes</b>

   \arg \b fd : The associated file descriptor.

   \arg \b minRead : underflow() will stop reading when it has read \b minRead bytes.  For a TTY
   device, you may want to set \b minRead = 1.  This is done automatically for utl::cin.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class FDstream : public Stream
{
    friend class BufferedFDstream;
    UTL_CLASS_DECL(FDstream, Stream);
    UTL_CLASS_NO_COPY;

public:
    /**
       Constructor.
       \param fd file descriptor
       \param mode access mode (see utl::io_t)
    */
    FDstream(int fd, uint_t mode);

    /** Close the file. */
    virtual void close();

    /** Detach from the file. */
    void
    detach()
    {
        _fd = -1;
        clear();
    }

    /** Get the associated file descriptor. */
    int
    fd() const
    {
        return _fd;
    }

#if UTL_HOST_TYPE == UTL_HT_UNIX
    /** Determine whether the file descriptor is associated with a TCP socket. */
    bool isSocket() const;
#endif

    /** Determine whether the file descriptor is associated with a TTY device. */
    bool isTTY() const;

    /**
       Set the file descriptor and access mode.
       \param fd file descriptor
       \param mode access mode (see utl::io_t)
    */
    virtual void setFD(int fd, uint_t mode = io_rd | io_wr);

    virtual size_t read(byte_t* array, size_t maxBytes, size_t minBytes = size_t_max);

    virtual void write(const byte_t* array, size_t num);

#if UTL_HOST_TYPE == UTL_HT_UNIX
    /** Is blocking I/O enabled on the file descriptor? */
    bool
    blockingIO() const
    {
        return !getFlag(flg_nonBlocking);
    }

    /** Enable/disable blocking I/O on the file descriptor. */
    void setBlockingIO(bool v);

    /**
       Block until data can be read.
       \return true iff data available for reading
       \param usec (optional) wait time (in microseconds)
    */
    bool blockRead(uint32_t usec = 0);

    /**
       Block until data can be written.
       \return true iff the file descriptor is ready for writing
       \param usec (optional) wait time (in microseconds)
    */
    bool blockWrite(uint32_t usec = 0);
#endif
protected:
    /** Clear/reset the object. */
    virtual void clear();

    /** Set non-blocking mode. */
    virtual void setModes();

protected:
    int _fd;

private:
    enum flg_t
    {
        flg_nonBlocking = 4
    };

private:
    void
    init()
    {
        _fd = -1;
    }
    void
    deInit()
    {
        close();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
