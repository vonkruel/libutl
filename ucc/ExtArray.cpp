#include <libutl/libutl.h>
#include <libutl/ExtArray.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ExtArray);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ExtArray::open(const String& baseName, const RunTimeClass* rtc, uint_t serializeMode)
{
    close();

    _idxFile.open(baseName + ".idx", io_rdwr | fs_create);
    _objectFile.open(baseName + ".obj", io_rdwr | fs_create);
    _items = _idxFile.length() / sizeof(size_t);
    _rtc = rtc;
    _serializeMode = serializeMode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ExtArray::close()
{
    if (_items == size_t_max)
        return;

    _items = size_t_max;
    _idxFile.close();
    _objectFile.close();
    _rtc = nullptr;
    _serializeMode = uint_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ExtArray::add(const Object& object)
{
    _idxFile.seekEnd();
    _objectFile.seekEnd();
    size_t curPos = _objectFile.tell();
    utl::serialize(curPos, _idxFile, io_wr, ser_compact);
    if (_rtc == nullptr)
    {
        object.serializeOutBoxed(_objectFile, _serializeMode);
    }
    else
    {
        object.serializeOut(_objectFile, _serializeMode);
    }
    ++_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
ExtArray::get(size_t idx) const
{
    ASSERTD(isOpen());

    if (idx >= _items)
        return nullptr;

    _idxFile.seek(idx * sizeof(size_t));
    size_t pos;
    utl::serialize(pos, _idxFile, io_rd, ser_compact);
    _objectFile.seek(pos);
    Object* object;
    if (_rtc == nullptr)
    {
        object = serializeInBoxed(_objectFile, _serializeMode);
    }
    else
    {
        object = _rtc->create();
        object->serializeIn(_objectFile, _serializeMode);
    }
    return object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
