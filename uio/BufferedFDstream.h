#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BufferedStream.h>
#include <libutl/FDstream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Buffered stream with file descriptor.

   BufferedFDstream provides buffering for a FDstream, and has the same external interface.

   \author Adam McKee
   \ingroup io
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BufferedFDstream : public BufferedStream
{
    UTL_CLASS_DECL(BufferedFDstream, BufferedStream);
    UTL_CLASS_DEFID;

public:
    BufferedFDstream(FDstream* fdStream)
    {
        setStream(fdStream);
    }

    BufferedFDstream(int fd, uint_t mode)
    {
        setStream(new FDstream(fd, mode));
    }

    int
    fd() const
    {
        return pget()->fd();
    }

    void
    setFD(int fd, uint_t mode)
    {
        pget()->setFD(fd, mode);
    }

#if UTL_HOST_TYPE == UTL_HT_UNIX
    bool
    isSocket() const
    {
        return pget()->isSocket();
    }
#endif

    bool
    isTTY() const
    {
        return pget()->isTTY();
    }

#if UTL_HOST_TYPE == UTL_HT_UNIX
    bool blockRead(uint32_t usec = 0);
#endif
private:
    const FDstream*
    pget() const
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<FDstream>(_stream);
    }
    FDstream*
    pget()
    {
        ASSERTD(_stream != nullptr);
        return utl::cast<FDstream>(_stream);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Standard input.
   \ingroup io
*/
extern BufferedFDstream cin;

/**
   Standard output.
   \ingroup io
*/
extern BufferedFDstream cout;

/**
   Standard error.
   \ingroup io
*/
extern BufferedFDstream cerr;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
