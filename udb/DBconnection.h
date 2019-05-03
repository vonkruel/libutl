#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/DBresult.h>
#include <libutl/Float.h>
#include <libutl/Int.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBresultPagingRange;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Database connection.

   This abstract base class represents a client connection to an RDBMS server.  DBconnection
   provides methods for executing SQL commands/queries on the server and accessing the results.

   \author Adam McKee
   \ingroup database
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class DBconnection : public Object
{
    UTL_CLASS_DECL_ABC(DBconnection, Object);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_SERIALIZE;

protected:
    struct StringArg
    {
        const char* str;
        bool owner;
    };

    using arg_vector_t = std::vector<StringArg>;

public:
    /// \name Connection
    //@{
    /** Connected? */
    bool
    isConnected() const
    {
        return _connected;
    }

    /** Make a connection. */
    virtual bool connect(const char* connectStr) = 0;

    /** Break the connection. */
    virtual void disconnect() = 0;
    //@}

    /// \name Owned Result
    //@{
    /** Get the owned result. */
    const DBresult*
    result() const
    {
        return _result;
    }

    /** Query result ownership flag. */
    bool
    isResultOwner() const
    {
        return _resultOwner;
    }

    /** Enable/disable result ownership (enabled by default). */
    void setResultOwner(bool owner);

    /** Clear result. */
    void
    clearResult()
    {
        delete _result;
        _result = nullptr;
    }

    /** Release the owned result. */
    const DBresult*
    releaseResult()
    {
        auto res = _result;
        _result = nullptr;
        return res;
    }
    //@}

    /// \name Simple Query
    //@{
    /**
       Execute a statement.
       \return SQL statement execution result
       \param stmt SQL statement
    */
    const DBresult* exec(const char* stmt);

    /**
       Execute a statement (virtual).
       \return SQL statement execution result
       \param stmt SQL statement
    */
    virtual const DBresult* vexec(const char* stmt) = 0;
    //@}

    /// \name Statements w/ Arguments
    //@{
    /**
       Execute a statement with provided arguments.
       \param stmt SQL statement
       \param args arguments
    */
    template <typename... R>
    const DBresult*
    execParams(const char* stmt, R... args)
    {
        arg_vector_t vargs;
        vargs.reserve(8);
        prepareArgs(vargs, args...);
        SCOPE_EXIT
        {
            cleanupArgs(vargs);
        };
        auto res = vexecParams(stmt, vargs);
        if (_resultOwner)
            setResult(res);
        return res;
    }

    /**
       Execute a statement with provided arguments (virtual).
       \param stmt SQL statement
       \param args arguments
    */
    virtual const DBresult* vexecParams(const char* stmt, const arg_vector_t& args) = 0;
    //@}

    /// \name Prepared Statements
    //@{
    /**
       Prepare a SQL statement for execution.
       \param stmtName SQL statement name
       \param stmt SQL statement
       \param numParams number of parameters
       \param paramTypes parameter types
    */
    virtual const DBresult* prepare(const char* stmtName,
                                    const char* stmt,
                                    int numParams,
                                    db_result_field_t* paramTypes = nullptr) = 0;

    /**
       Execute a prepared SQL statement.
       \param stmtName SQL statement name
       \param args arguments
    */
    template <typename... R>
    const DBresult*
    execPrepared(const char* stmtName, R... args)
    {
        arg_vector_t vargs;
        vargs.reserve(8);
        prepareArgs(vargs, args...);
        SCOPE_EXIT
        {
            cleanupArgs(vargs);
        };
        auto res = vexecPrepared(stmtName, vargs);
        if (_resultOwner)
            setResult(res);
        return res;
    }

    /**
       Execute a prepared SQL statement (virtual).
       \param stmtName SQL statement name
       \param args arguments
    */
    virtual const DBresult* vexecPrepared(const char* stmtName, const arg_vector_t& args) = 0;
    //@}

    /// \name Cursors
    //@{
    /**
       Declare a SQL cursor for the result rows of a given query.
       \param name SQL cursor name
       \param query SQL query
       \param scroll (optional : false) allow non-sequential access to rows?
       \param hold (optional : false) allow the cursor to outlive the current transaction?
    */
    virtual const DBresult*
    declareCursor(const char* name, const char* query, bool scroll = false, bool hold = false);

    /**
       Open the named SQL cursor.
       \param name SQL cursor name
    */
    virtual const DBresult* openCursor(const char* name);

    /**
       Close the named SQL cursor.
       \param name SQL cursor name
    */
    virtual const DBresult* closeCursor(const char* name);

    /**
       Fetch result rows from the named SQL cursor.
       \param cursorName SQL cursor name
       \param count the number of result rows to retrieve
    */
    virtual const DBresult* fetchForward(const char* cursorName, uint_t count = 1000);
    //@}

    /**
       Return a paging range for the given query.
       \param query SQL query
       \param cursorName (optional : nullptr) SQL cursor name
       \param pageSize (optional : 1000) number of rows to retrieve at a time
    */
    DBresultPagingRange
    pagingRange(const char* query, const char* cursorName = nullptr, uint_t pageSize = 1000);

    /**
      Translate an error result to an exception (optionally deleting the query result).
      \param result query result (DatabaseEx will be thrown if result->status() != dbr_ok)
      \param deleteResult (optional : false) delete the result unconditionally?
    */
    void throwOnError(const DBresult* result, bool deleteResult = false);

protected:
    void
    setConnected(bool connected)
    {
        _connected = connected;
    }

    void setResult(const DBresult* result);

    DBresult* successResult();

    DBresult* disconnectedResult();

    /// \name Argument Preparation
    //@{
    template <typename... R>
    void
    prepareArgs(arg_vector_t&)
    {
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, const char* arg, R... args)
    {
        vargs.push_back(StringArg{arg, false});
        prepareArgs(vargs, args...);
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, const std::string& arg, R... args)
    {
        vargs.push_back(StringArg{arg.c_str(), false});
        prepareArgs(vargs, args...);
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, bool arg, R... args)
    {
        auto str = arg ? "1" : "0";
        vargs.push_back(StringArg{str, false});
        prepareArgs(vargs, args...);
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, int32_t arg, R... args)
    {
        auto str = Int(arg).toString();
        str.setOwner(false);
        vargs.push_back(StringArg{str.get(), true});
        prepareArgs(vargs, args...);
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, int64_t arg, R... args)
    {
        auto str = Int(arg).toString();
        str.setOwner(false);
        vargs.push_back(StringArg{str.get(), true});
        prepareArgs(vargs, args...);
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, double arg, R... args)
    {
        auto str = Float(arg).toString(uint_t_max);
        str.setOwner(false);
        vargs.push_back(StringArg{str.get(), true});
        prepareArgs(vargs, args...);
    }

    template <typename... R>
    void
    prepareArgs(arg_vector_t& vargs, const Decimal& arg, R... args)
    {
        auto str = arg.toString();
        str.setOwner(false);
        vargs.push_back(StringArg{str.get(), true});
        prepareArgs(vargs, args...);
    }

    void
    cleanupArgs(arg_vector_t& args)
    {
        for (auto arg : args)
        {
            if (arg.owner)
                delete[] arg.str;
        }
    }
    //@}

private:
    void init();
    void deInit();

private:
    bool _connected;
    bool _resultOwner;
    const DBresult* _result;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
