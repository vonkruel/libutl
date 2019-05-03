#include <libutl/libutl.h>
#include <libutl/DBresult.h>
#include <libutl/DBresultRow.h>
#include <libutl/Ordering.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::DBresultRow);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultRow::copy(const utl::Object& rhs)
{
    auto& rr = utl::cast<DBresultRow>(rhs);
    _result = rr._result;
    _values = rr._values;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultRow::steal(utl::Object& rhs)
{
    auto& rr = utl::cast<DBresultRow>(rhs);
    _result = rr._result;
    _values = rr._values;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
DBresultRow::compare(const utl::Object& rhs) const
{
    auto& rr = utl::cast<DBresultRow>(rhs);
    int res = AddressOrdering().cmp(_result, rr._result);
    if (res != 0)
        return res;
    return utl::compare(_values, rr._values);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
DBresultRow::numFields() const
{
    ASSERTD(_result != nullptr);
    return _result->numFields();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
DBresultRow::isNull(uint_t fieldIdx) const
{
    ASSERTD(_result != nullptr);
    ASSERTD(fieldIdx < numFields());
    return _result->isNull(_values - _result->values() + fieldIdx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char*
DBresultRow::getString(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_string);
    auto res = reinterpret_cast<char*>(field(fieldIdx));
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
DBresultRow::getBool(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_bool);
    auto val = field(fieldIdx);
    auto valPtr = &val;
    return *reinterpret_cast<bool*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
DBresultRow::getInt32(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_int32);
    auto val = field(fieldIdx);
    auto valPtr = &val;
    return *reinterpret_cast<int*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int64_t
DBresultRow::getInt64(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_int64);
    auto val = field(fieldIdx);
    auto valPtr = &val;
    return *reinterpret_cast<int64_t*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
DBresultRow::getDouble(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_double);
    auto val = field(fieldIdx);
    auto valPtr = &val;
    return *reinterpret_cast<double*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char*
DBresultRow::getDecimal(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_decimal);
    auto res = reinterpret_cast<char*>(field(fieldIdx));
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

time_t
DBresultRow::getDateTime(uint_t fieldIdx) const
{
    ASSERTD(_result->field(fieldIdx).type() == dbf_dateTime);
    auto val = field(fieldIdx);
    auto valPtr = &val;
    return *reinterpret_cast<time_t*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultRow::init(const DBresult* result, uint64_t row)
{
    _result = result;
    _values = _result->values() + (row * _result->numFields());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
