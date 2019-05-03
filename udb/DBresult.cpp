#include <libutl/libutl.h>
#include <libutl/DBresult.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::DBresult);
UTL_EXCEPTION_IMPL(utl, DatabaseEx, utl::Exception, "database exception");

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresult::initialize(uint64_t numRows, uint_t numFields, DBresultField** fields)
{
    // can't have rows without fields
    ASSERTD((numRows == 0) || (numFields > 0));

    // clear previous result
    clear();

    _status = dbr_ok;
    _numRows = numRows;
    _numFields = numFields;
    _fields = fields;
    size_t numValues = _numFields * _numRows;
    _values = new uint64_t[numValues];
    memset(_values, 0, numValues * sizeof(uint64_t));
    _nullArray.initialize(numValues, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresult::copy(const Object& rhs)
{
    auto& res = utl::cast<DBresult>(rhs);

    // clean up existing data
    clear();

    // copy simple attributes
    _status = res._status;
    _numRows = res._numRows;
    _errorMsg = res._errorMsg;

    // copy _fields
    _numFields = res._numFields;
    _fields = new DBresultField*[_numFields];
    for (uint_t fieldIdx = 0; fieldIdx != _numFields; ++fieldIdx)
    {
        _fields[fieldIdx] = utl::clone(res._fields[fieldIdx]);
    }

    // copy _values
    auto valuesSize = _numRows * _numFields;
    _values = new uint64_t[valuesSize];
    memcpy(_values, res._values, valuesSize * sizeof(uint64_t));
    auto valuesLim = _values + valuesSize;
    uint_t fieldIdx = 0;
    for (auto valuesPtr = _values; valuesPtr != valuesLim; ++valuesPtr)
    {
        auto type = _fields[fieldIdx]->type();
        if ((type == dbf_string) || (type == dbf_decimal))
        {
            auto str = *reinterpret_cast<char**>(valuesPtr);
            *reinterpret_cast<char**>(valuesPtr) = utl::strdup(str);
        }
    }

    // copy _nullArray
    _nullArray = res._nullArray;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresult::clear()
{
    if (_status == dbr_undefined)
    {
        ASSERTD(_numRows == 0);
        ASSERTD(_numFields == 0);
        ASSERTD(_fields == nullptr);
        ASSERTD(_values == nullptr);
        ASSERTD(_nullArray == BitArray());
        return;
    }

    DBresultField** fieldsPtr;
    DBresultField** fieldsLim = _fields + _numFields;
    auto ptr = _values;

    // _values
    uint64_t rowIdx;
    for (rowIdx = 0; rowIdx != _numRows; ++rowIdx)
    {
        for (fieldsPtr = _fields; fieldsPtr != fieldsLim; ++fieldsPtr)
        {
            auto v = *ptr++;
            auto type = (*fieldsPtr)->type();
            if (type == dbf_string)
            {
                auto str = reinterpret_cast<char*>(v);
                delete[] str;
            }
            else if (type == dbf_decimal)
            {
                auto str = reinterpret_cast<char*>(v);
                delete[] str;
            }
        }
    }
    delete[] _values;
    _values = nullptr;

    // _fields
    for (fieldsPtr = _fields; fieldsPtr != fieldsLim; ++fieldsPtr)
    {
        delete *fieldsPtr;
    }
    delete[] _fields;
    _fields = nullptr;
    _numFields = 0;

    // init misc. attributes
    _status = dbr_undefined;
    _numRows = 0;
    _errorMsg.clear();

    // clear nullptr map
    _nullArray.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char*
DBresult::getString(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_string);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<char**>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
DBresult::getBool(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_bool);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<bool*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
DBresult::getInt32(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_int32);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<int32_t*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int64_t
DBresult::getInt64(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_int64);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<int64_t*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

double
DBresult::getDouble(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_double);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<double*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const char*
DBresult::getDecimal(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_decimal);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<char**>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

time_t
DBresult::getDateTime(uint64_t rowIdx, uint_t fieldIdx) const
{
    ASSERTD(rowIdx < _numRows);
    ASSERTD(fieldIdx < _numFields);
    ASSERTD(_fields[fieldIdx]->type() == dbf_dateTime);
    auto val = _values[(rowIdx * _numFields) + fieldIdx];
    auto valPtr = &val;
    return *reinterpret_cast<time_t*>(valPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresult::init()
{
    _status = dbr_undefined;
    _numRows = 0;
    _numFields = 0;
    _fields = nullptr;
    _values = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresult::deInit()
{
    clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
