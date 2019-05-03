#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FDstream.h>
#include <libutl/Pathname.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   FileStream flags.
   \ingroup io
*/
enum fs_flags_t
{
    fs_create = 4,  /**< FileStream::open() : create a new file */
    fs_append = 8,  /**< FileStream::open() : open in \b append mode */
    fs_trunc = 16,  /**< FileStream::open() : truncate existing file */
    fs_clobber = 32 /**< same as <b>io_wr | fs_create | fs_trunc</b> */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Disk file stream.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class FileStream : public FDstream
{
    UTL_CLASS_DECL(FileStream, FDstream);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param fd file descriptor obtained from call to open(2)
       \param mode see utl::io_t and utl::fs_flags_t
    */
    FileStream(int fd, uint_t mode = io_rdwr);

    /**
       Constructor.
       \param path file pathname
       \param mode (optional : io_rdwr) see utl::io_t and utl::fs_flags_t
       \param createMode (optional) permissions when new file is created
    */
    FileStream(const Pathname& path, uint_t mode = io_rdwr, uint_t createMode = uint_t_max);

    /** Get file length. */
    ssize_t length() const;

    /**
       Close the currently open file, and attach to an open file.
       \param fd file descriptor obtained from call to open(2)
       \param mode see utl::io_t and utl::fs_flags_t
    */
    void open(int fd, uint_t mode = io_rdwr);

    /**
       Close the currently open file, and open a new file.
       \param path file pathname
       \param mode (optional : io_rdwr) see utl::io_t and utl::fs_flags_t
       \param createMode (optional) permissions when new file is created
    */
    void open(const Pathname& path, uint_t mode = io_rdwr, uint_t createMode = uint_t_max);

    /** Seek to the start of the file. */
    void
    rewind()
    {
        seek(0);
    }

    /**
       Seek to the given file offset.
       \param offset given offset
       \return resulting position
    */
    ssize_t
    seek(ssize_t offset)
    {
        return seekStart(offset);
    }

    /**
       Seek to the current position plus \b offset bytes.
       \param offset given offset
       \return resulting position
    */
    ssize_t seekCur(ssize_t offset);

    /**
       Seek to the start of the file plus \b offset bytes.
       \param offset given offset
       \return resulting position
    */
    ssize_t seekStart(ssize_t offset = 0);

    /**
       Seek to the end of the file plus \b offset bytes.
       \param offset given offset
       \return resulting position
    */
    ssize_t seekEnd(ssize_t offset = 0);

    /** Get the current position. */
    ssize_t tell() const;

    /**
       Truncate the file to the given length.
       If no length is given, the file is truncated to the current position.
       \param length (optional)
    */
    void truncate(ssize_t length = ssize_t_max);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
