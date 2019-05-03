#include <libutl/libutl.h>
#include <libutl/FileStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::FileStream);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

FileStream::FileStream(int fd, uint_t mode)
{
    open(fd, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FileStream::FileStream(const Pathname& path, uint_t mode, uint_t createMode)
{
    open(path, mode, createMode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ssize_t
FileStream::length() const
{
    ssize_t curPos = tell();
    const_cast_this->seekEnd();
    ssize_t res = tell();
    const_cast_this->seek(curPos);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FileStream::open(int fd, uint_t mode)
{
    // close any currently open file
    close();

    setFD(fd);
    setMode(mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FileStream::open(const Pathname& path, uint_t mode, uint_t createMode)
{
    int fd;

    // close any currently open file
    close();

    if (mode & fs_clobber)
    {
        mode |= io_wr;
        mode |= fs_create;
        mode |= fs_trunc;
    }

    // figure out the flags
    int flags = 0;
    if (((mode & io_rd) != 0) && (mode & io_wr) != 0)
    {
        flags |= O_RDWR;
    }
    else if ((mode & io_rd) != 0)
    {
        flags |= O_RDONLY;
    }
    else if ((mode & io_wr) != 0)
    {
        flags |= O_WRONLY;
    }

    if ((mode & fs_create) != 0)
    {
        flags |= O_CREAT;
    }

    if ((mode & fs_append) != 0)
    {
        flags |= O_APPEND;
    }

    if ((mode & fs_trunc) != 0)
    {
        flags |= O_TRUNC;
    }

    if ((mode & fs_create) != 0)
    {
        if (createMode == uint_t_max)
        {
            createMode = 0664;
        }
        fd = ::open(path, flags, createMode);
    }
    else
    {
        fd = ::open(path, flags);
    }

    // ensure ::open succeeded
    if (fd < 0)
        errToEx(path);

    // set the name
    setName(path);

    // set the FD
    setFD(fd, mode & io_rdwr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ssize_t
FileStream::seekCur(ssize_t offset)
{
    ASSERTD(_fd >= 0);
    ssize_t res = lseek(_fd, offset, SEEK_CUR);
    if (res < 0)
        errToEx(getNamePtr());
    setEOF(false);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ssize_t
FileStream::seekStart(ssize_t offset)
{
    ASSERTD(_fd >= 0);
    ssize_t res = lseek(_fd, offset, SEEK_SET);
    if (res < 0)
        errToEx(getNamePtr());
    setEOF(false);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ssize_t
FileStream::seekEnd(ssize_t offset)
{
    ASSERTD(_fd >= 0);
    ssize_t res = lseek(_fd, offset, SEEK_END);
    if (res < 0)
        errToEx(getNamePtr());
    setEOF(false);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ssize_t
FileStream::tell() const
{
    ASSERTD(_fd >= 0);
    ssize_t res = lseek(_fd, 0, SEEK_CUR);
    if (res < 0)
        errToEx(getNamePtr());
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FileStream::truncate(ssize_t length)
{
    ASSERTD(_fd >= 0);

    // set length to current position if it wasn't specified
    ssize_t curPos = tell();
    if (length == ssize_t_max)
    {
        length = curPos;
    }

    // ensure our current position is <= length
    if (curPos > length)
    {
        seek(length);
    }

#if UTL_CC == UTL_CC_MSVC
    int res = _chsize_s(_fd, length);
#else
    int res = ftruncate(_fd, length);
#endif
    if (res < 0)
        errToEx(getNamePtr());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
