#include <libutl/libutl.h>
#include <libutl/Base64encoder.h>
#include <libutl/MemStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Base64encode);
UTL_CLASS_IMPL(utl::Base64decode);
UTL_CLASS_IMPL(utl::Base64encoder);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MASK0 0x00fc0000UL
#define MASK1 0x0003f000UL
#define MASK2 0x00000fc0UL
#define MASK3 0x0000003fUL

static const char* base64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static byte_t base64rev[256];
static std::atomic_bool base64revInitialized(false);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Base64encode ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64encode::process(const byte_t* data, size_t dataLen)
{
    byte_t __dataLen = _dataLen;
    const byte_t* dataPtr = data;
    const byte_t* dataLim = data + dataLen;
    byte_t output[4];
    byte_t* outPtr = output;
    while (dataPtr < dataLim)
    {
        _data[__dataLen++] = *dataPtr++;
        if (__dataLen == 3)
        {
            uint32_t data =
                ((uint32_t)_data[0] << 16) | ((uint32_t)_data[1] << 8) | ((uint32_t)_data[2]);
            byte_t b0 = (data & MASK0) >> 18;
            byte_t b1 = (data & MASK1) >> 12;
            byte_t b2 = (data & MASK2) >> 6;
            byte_t b3 = (data & MASK3);
            outPtr = output;
            *outPtr++ = base64chars[b0];
            *outPtr++ = base64chars[b1];
            *outPtr++ = base64chars[b2];
            *outPtr++ = base64chars[b3];
            _os->write(output, 4);
            __dataLen = 0;
        }
    }
    _dataLen = __dataLen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64encode::finalize()
{
    byte_t __dataLen = _dataLen;
    if (__dataLen == 0)
        return;

    // pad input data with trailing 0 bits, outSize = how many base64 chars we'll write
    byte_t output[4];
    byte_t* outPtr = output;
    byte_t* outLim = output + 4;
    byte_t outSize;
    if (__dataLen == 1)
    {
        _data[1] = _data[2] = 0;
        outSize = 2;
    }
    else
    {
        ASSERTD(__dataLen == 2);
        _data[2] = 0;
        outSize = 3;
    }

    uint32_t data = ((uint32_t)_data[0] << 16) | ((uint32_t)_data[1] << 8) | ((uint32_t)_data[2]);
    byte_t b0 = (data & MASK0) >> 18;
    byte_t b1 = (data & MASK1) >> 12;
    *outPtr++ = base64chars[b0];
    *outPtr++ = base64chars[b1];
    if (outSize == 3)
    {
        byte_t b2 = (data & MASK2) >> 6;
        *outPtr++ = base64chars[b2];
    }
    while (outPtr < outLim)
        *outPtr++ = '='; // pad with '=' so output size is multiple of 4
    _os->write(output, 4);
    _dataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Base64decode ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64decode::process(const byte_t* data, size_t dataLen)
{
    // ignore trailing '='
    while ((dataLen > 0) && (data[dataLen - 1] == '='))
        --dataLen;

    uint32_t __data = _data;
    uint32_t __dataShift = _dataShift;
    const byte_t* dataPtr = data;
    const byte_t* dataLim = data + dataLen;
    byte_t output[3];
    byte_t* outPtr;
    while (dataPtr < dataLim)
    {
        byte_t ch = *dataPtr++;
        __data |= ((uint32_t)base64rev[ch] << __dataShift);
        if (__dataShift == 0)
        {
            outPtr = output;
            *outPtr++ = (__data & 0xff0000UL) >> 16;
            *outPtr++ = (__data & 0x00ff00UL) >> 8;
            *outPtr++ = (__data & 0x0000ffUL);
            _os->write(output, 3);
            __data = 0;
            __dataShift = 18;
        }
        else
        {
            __dataShift -= 6;
        }
    }
    _data = __data;
    _dataShift = __dataShift;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64decode::finalize()
{
    // nothing to do?
    if (_dataShift == 18)
        return;

    byte_t output[3];
    byte_t* outPtr = output;
    *outPtr++ = (_data & 0xff0000UL) >> 16;
    if (_dataShift == 12)
    {
        // we have 6/24 bits -> assume 1 byte of output (this case shouldn't happen)
        _os->write(output, 1);
    }
    else if (_dataShift == 6)
    {
        // we have 12/24 bits -> 1 byte of output
        _os->write(output, 1);
    }
    else
    {
        // we have 18/24 bits -> 2 bytes of output
        ASSERTD(_dataShift == 0);
        *outPtr++ = (_data & 0x00ff00UL) >> 8;
        _os->write(output, 2);
    }
    _data = 0;
    _dataShift = 18;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64decode::init()
{
    // not initialized escape chars yet -> do it
    if (!base64revInitialized.load(std::memory_order_acquire))
    {
        memset(base64rev, 0, 256);
        byte_t num = 0;
        for (char c = 'A'; c <= 'Z'; ++c)
            base64rev[(byte_t)c] = num++;
        for (char c = 'a'; c <= 'z'; ++c)
            base64rev[(byte_t)c] = num++;
        for (char c = '0'; c <= '9'; ++c)
            base64rev[(byte_t)c] = num++;
        base64rev[(byte_t)'+'] = 62;
        base64rev[(byte_t)'/'] = 63;
        base64revInitialized.store(true, std::memory_order_release);
    }
    _data = 0;
    _dataShift = 18;
    _owner = false;
    _os = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Base64encoder //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Base64encoder::encode(const byte_t* block, size_t num)
{
    _codec.encode->process(block, num);
    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Base64encoder::decode(byte_t* block, size_t num)
{
    // decode up to <num> bytes from the stream (even though this won't fill the buffer)
    auto ms = utl::cast<MemStream>(_codec.decode->stream());
    ms->seekp(0);
    size_t numRead = _stream->read(block, num, 1);
    _codec.decode->process(block, numRead);
    numRead = ms->tellp();
    return numRead;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64encoder::start(uint_t mode, Stream* stream, bool owner)
{
    clear();
    set(mode, stream, owner);
    if (isInput())
    {
        MemStream* ms = new MemStream(_iBuf.get(), _iBuf.size(), false);
        _codec.decode = new Base64decode(ms, true);
    }
    else
    {
        _codec.encode = new Base64encode(stream);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64encoder::clear()
{
    delete _codec.object;
    _codec.object = nullptr;
    Encoder::clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base64encoder::finishEncoding()
{
    ASSERTD(isOutput());
    ASSERTD(_codec.encode != nullptr);
    _codec.encode->finalize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Base64encoder::finishDecoding()
{
    auto ms = utl::cast<MemStream>(_codec.decode->stream());
    _codec.decode->finalize();
    size_t num = ms->tellp();
    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
