#include <libutl/libutl.h>
#include <libutl/DBconnection.h>
#include <libutl/DBresultPagingRange.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::DBconnection);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBconnection::setResultOwner(bool owner)
{
    if (owner == _resultOwner)
        return;
    if (owner)
    {
        _resultOwner = true;
        ASSERTD(_result == nullptr);
    }
    else
    {
        _resultOwner = false;
        clearResult();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const DBresult*
DBconnection::exec(const char* stmt)
{
    auto res = vexec(stmt);
    if (_resultOwner)
        setResult(res);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const DBresult*
DBconnection::declareCursor(const char* name, const char* query, bool scroll, bool hold)
{
    String stmt;
    stmt.reserve(128);
    stmt += "DECLARE ";
    stmt += name;
    stmt += " ";
    if (!scroll)
        stmt += "NO ";
    stmt += "SCROLL ";
    stmt += "CURSOR ";
    if (hold)
    {
        stmt += "WITH ";
    }
    else
    {
        stmt += "WITHOUT ";
    }
    stmt += "HOLD FOR ";
    stmt += query;
    return this->exec(stmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const DBresult*
DBconnection::openCursor(const char* name)
{
    String stmt = "OPEN ";
    stmt += name;
    stmt += ";";
    return this->exec(stmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const DBresult*
DBconnection::closeCursor(const char* name)
{
    String stmt = "CLOSE ";
    stmt += name;
    stmt += ";";
    return this->exec(stmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const DBresult*
DBconnection::fetchForward(const char* cursorName, uint_t count)
{
    String query = "FETCH FORWARD " + Uint(count).toString() + " FROM " + cursorName + ";";
    return this->exec(query);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DBresultPagingRange
DBconnection::pagingRange(const char* query, const char* cursorName, uint_t pageSize)
{
    return DBresultPagingRange(this, query, cursorName, pageSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBconnection::throwOnError(const DBresult* result, bool deleteResult)
{
    bool ok = (result->status() == dbr_ok);
    deleteResult = deleteResult && (!_resultOwner || (_result != result));
    if (ok)
    {
        if (deleteResult)
            delete result;
    }
    else
    {
        auto errorMsg = result->errorMsg();
        if (deleteResult)
            delete result;
        throw DatabaseEx(errorMsg);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBconnection::setResult(const DBresult* result)
{
    ASSERTD(_resultOwner);
    delete _result;
    _result = result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DBresult*
DBconnection::successResult()
{
    auto res = new DBresult();
    res->status() = dbr_ok;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DBresult*
DBconnection::disconnectedResult()
{
    auto res = new DBresult();
    res->errorMsg() = "connection failure";
    res->status() = dbr_fatal_error;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBconnection::init()
{
    _connected = false;
    _resultOwner = true;
    _result = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DBconnection::deInit()
{
    delete _result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
