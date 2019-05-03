#include <libutl/libutl.h>
#include <libutl/BinaryData.h>
#include <libutl/MemStream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::MemStream);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
MemStream::read(byte_t* array, size_t maxBytes, size_t minBytes)
{
    ASSERTD(isInput());
    // ASSERTD((maxBytes == 0) || (minBytes != 0));

    // fix minBytes
    if (minBytes > maxBytes)
        minBytes = maxBytes;

    // how many bytes can we read?
    size_t num = min(_array.size() - _readPos, maxBytes);

    // not enough bytes to satisfy the request?
    if (num < minBytes)
        throwStreamEOFex();

    // copy into the caller's buffer
    memcpy(array, _array + _readPos, num);

    // advance read position
    _readPos += num;

    return num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemStream::write(const byte_t* array, size_t num)
{
    size_t newWritePos = _writePos + num;
    _array.grow(newWritePos);
    memcpy(_array + _writePos, array, num);
    _writePos = newWritePos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemStream::takeData(BinaryData& data)
{
    ASSERTD(_array.isOwner());
    data.set(_array, _array.size(), true, 1);
    _array.setOwner(false);
    _array.set(nullptr, 0, true, 1);
    _readPos = _writePos = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char*
MemStream::copyString() const
{
    // empty -> return nullptr
    size_t size = _array.size();
    if (size == 0)
        return nullptr;
    if ((size == 1) && (_array[0] == '\0'))
        return nullptr;
    char* res;
    if (_array[size - 1] == '\0')
    {
        res = new char[size];
        memcpy(res, _array, _array.size());
    }
    else
    {
        ++size;
        res = new char[size];
        memcpy(res, _array, _array.size());
        res[size - 1] = '\0';
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char*
MemStream::takeString()
{
    ASSERTD(_array.isOwner());

    // need to NUL-terminate?
    size_t size = _array.size();
    if ((size == 0) || (_array[size - 1] != '\0'))
        put('\0');

    // take ownership from _array
    auto res = const_cast<char*>(reinterpret_cast<const char*>(get()));
    _array.setOwner(false);
    _array.set(nullptr, 0, true, _array.increment());
    _readPos = _writePos = 0;

    // return the string
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemStream::truncate(size_t size)
{
    if (size >= _array.size())
        return;
    _readPos = utl::min(_readPos, size);
    _writePos = utl::min(_writePos, size);
    _array.setSize(size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemStream::set(byte_t* array, size_t size, bool owner, size_t increment)
{
    super::clear();
    setMode(io_rdwr);
    setError(false);
    _array.excise();
    if (array == nullptr)
    {
        _array.reserve(size, increment);
    }
    else
    {
        _array.set(array, size, owner, increment);
    }
    _readPos = _writePos = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemStream::init(byte_t* array, size_t size, bool owner, size_t increment)
{
    // this is like set(), except we don't call super::clear() or _array.clear()
    setMode(io_rdwr);
    setError(false);
    if (array == nullptr)
    {
        _array.reserve(size, increment);
    }
    else
    {
        _array.set(array, size, owner, increment);
    }
    _readPos = _writePos = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
