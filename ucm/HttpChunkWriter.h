#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/BufferedStream.h>
#include <libutl/MemStream.h>
#include <libutl/NetServer.h>
#include <libutl/StringVars.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   HTTP Chunked Transfer Encoder.

   This encoder can be used to write an HTTP response that has no pre-determined length.

   \author Adam McKee
   \ingroup communication
*/
class HttpChunkWriter : public BufferedStream
{
    UTL_CLASS_DECL(HttpChunkWriter, BufferedStream);
    UTL_CLASS_DEFID;

public:
    HttpChunkWriter(Stream* stream, bool streamOwner = true);

private:
    virtual void underflow();

    virtual void overflow();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
