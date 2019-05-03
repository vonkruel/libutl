#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BufferedFDstream.h>
#include <libutl/FileStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Buffered disk file stream.

   BufferedFileStream provides buffering for a FileStream, and has the same external interface as
   FileStream.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BufferedFileStream : public BufferedFDstream
{
    UTL_CLASS_DECL(BufferedFileStream, BufferedFDstream);

public:
    BufferedFileStream(FileStream* fileStream)
    {
        setStream(fileStream);
    }

    BufferedFileStream(int fd, uint_t mode = io_rdwr)
    {
        setStream(new FileStream(fd, mode));
    }

    BufferedFileStream(const Pathname& path, uint_t mode = io_rdwr, uint_t createMode = uint_t_max)
    {
        setStream(new FileStream(path, mode, createMode));
    }

    size_t
    length() const
    {
        return pget()->length();
    }

    void
    open(int fd, uint_t mode = io_rdwr)
    {
        setStream(new FileStream(fd, mode));
    }

    void
    open(const Pathname& path, uint_t mode = io_rdwr, uint_t createMode = uint_t_max)
    {
        setStream(new FileStream(path, mode, createMode));
    }

    void
    rewind()
    {
        seek(0);
    }

    ssize_t
    seek(size_t offset)
    {
        return seekStart(offset);
    }

    ssize_t
    seekCur(ssize_t offset)
    {
        flush(io_rdwr);
        return pget()->seekCur(offset);
    }

    ssize_t
    seekStart(ssize_t offset = 0)
    {
        flush(io_rdwr);
        return pget()->seekStart(offset);
    }

    ssize_t
    seekEnd(long offset = 0)
    {
        flush(io_rdwr);
        return pget()->seekEnd(offset);
    }

    ssize_t
    tell() const
    {
        return pget()->tell();
    }

    void
    truncate(ssize_t length = ssize_t_max)
    {
        flush(io_rdwr);
        return pget()->truncate(length);
    }

private:
    void
    init()
    {
        setStream(new FileStream());
    }

    void
    deInit()
    {
    }

    const FileStream*
    pget() const
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<FileStream>(_stream);
    }

    FileStream*
    pget()
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<FileStream>(_stream);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
