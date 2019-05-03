#include <libutl/libutl.h>
#include <libutl/DBresultField.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::DBresultField);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

DBresultField::DBresultField(const String& name, db_result_field_t type /*, uint_t size*/)
{
    _name = name;
    _type = type;
    /*if (size == uint_t_max)
    {
        switch (_type)
        {
        case dbf_string:
            _size = uint_t_max;
            break;
        case dbf_bool:
            _size = 1;
            break;
        case dbf_int16:
            _size = 2;
            break;
        case dbf_int32:
            _size = 4;
            break;
        case dbf_int64:
        case dbf_dateTime:
            _size = 8;
            break;
        default:
            ABORT();
        }
    }
    else
    {
        _size = size;
    }*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultField::copy(const Object& rhs)
{
    auto& field = utl::cast<DBresultField>(rhs);
    _name = field._name;
    _type = field._type;
    //_size = field._size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
DBresultField::compare(const Object& rhs) const
{
    auto& field = utl::cast<DBresultField>(rhs);
    auto res = _name.compare(field._name);
    if (res != 0)
        return res;
    res = utl::compare((uint_t)_type, (uint_t)field._type);
    /*if (res != 0)
        return res;
    res = utl::compare(_size, field._size);*/
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultField::clear()
{
    _name.clear();
    _type = dbf_undefined;
    //_size = uint_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
