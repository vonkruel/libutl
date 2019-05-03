#include <libutl/libutl.h>
#include <libutl/DBresultPagingIterator.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::DBresultPagingIterator);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
DBresultPagingIterator::compare(const Object& rhs) const
{
    auto& it = utl::cast<DBresultPagingIterator>(rhs);
    ASSERTD(_cursorName == it._cursorName);
    return utl::compare(this->rowIdx(), it.rowIdx());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::copy(const Object& rhs)
{
    auto& it = utl::cast<DBresultPagingIterator>(rhs);
    setResult(nullptr);
    _result = it._result;
    _page = it._page;
    _row = it._row;
    _pageSize = it._pageSize;
    _resultOwner = it._resultOwner;
    _cursorName = it._cursorName;
    _connection = it._connection;
    if (_resultOwner)
        _result = utl::clone(_result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::steal(Object& rhs)
{
    auto& it = utl::cast<DBresultPagingIterator>(rhs);
    setResult(nullptr);
    _result = it._result;
    _page = it._page;
    _row = it._row;
    _pageSize = it._pageSize;
    _resultOwner = it._resultOwner;
    _cursorName = it._cursorName;
    _connection = it._connection;
    it._resultOwner = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::next()
{
    if (isEnd())
        return;
    ++_row;
    if ((_result == nullptr) || (_row == _result->numRows()))
    {
        nextPage();
        ASSERTD(_result != nullptr);
        if (++_row == _result->numRows()) // no more rows?
        {
            // become end iterator
            _connection = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::init()
{
    _result = nullptr;
    _connection = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::deInit()
{
    if (_resultOwner)
        delete _result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::nextPage()
{
    ASSERTD(!isEnd());
    auto sql = "FETCH FORWARD " + Uint(_pageSize).toString() + " FROM " + _cursorName + ";";
    setResult(_connection->exec(sql));
    if (!ok())
    {
        throw DatabaseEx(errorMsg());
    }
    ++_page;
    _row = uint64_t_max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBresultPagingIterator::setResult(const DBresult* result)
{
    ASSERTD(_connection != nullptr);
    if (_resultOwner)
        delete _result;
    _result = result;
    _resultOwner = !_connection->isResultOwner();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
