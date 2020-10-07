#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/RBtree.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class CmdLineArg;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Parse command-line arguments.

   CmdLineArgs simplifies the task of parsing command-line arguments.  It can accept
   one-character switches (e.g. \b -q) as well as long switches (e.g. \b --quiet).
   It can also recognize and report any unknown switches that were given.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class CmdLineArgs
    : public Object
    , protected FlagsMI
{
    UTL_CLASS_DECL(CmdLineArgs, Object);

public:
    /**
       Constructor.  Invokes parse() for the given arguments.
       \param argc number of arguments (as in main())
       \param argv array of arguments (as in main())
    */
    CmdLineArgs(int argc, char** argv)
    {
        init(argc, argv);
    }

    /**
       Constructor.  Invokes parse() for the given arguments.
       \param args Array of String's
    */
    CmdLineArgs(const Array& args)
    {
        init(args);
    }

    /** See Object::clear. */
    void clear();

    /** Get the argument array. */
    const TArray<String>&
    getArray() const
    {
        return _array;
    }

    /** Return the index of the first non-switch argument. */
    size_t
    idx() const
    {
        return _idx;
    }

    /**
       Determine whether a switch was given.
       \return true if switch given, false otherwise
       \param sw switch to test for
       \param val (optional) value which was given for the switch
       \param swIdx (optional) index of argument where switch was found
    */
    bool isSet(const String& sw, String* val = nullptr, size_t* swIdx = nullptr);

    /** Get the ownership flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Set the ownership flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /** Return the number of arguments. */
    size_t
    items() const
    {
        return _array.items();
    }

    /**
       Determine whether any unknown switches were given.  This method \b must be called after
       isSet() has been called for each known switch.
    */
    bool
    ok() const
    {
        return _args.empty();
    }

    /**
       Parse the given arguments.
       \param argc number of arguments (as in main())
       \param argv array of arguments (as in main())
    */
    void parse(int argc, char** argv);

    /**
       Parse the given arguments.
       \param args Array of String's
    */
    void parse(const Array& args);

    /**
       Print unknown switches to the given stream.
       \return true if errors were found, false otherwise
       \param os stream to print errors to
       \param prefix prefix for each error report
    */
    bool printErrors(Stream& os, const String* prefix = nullptr);

    /**
       Return the argument at the given index.
       \see Array::operator[]
    */
    const String* operator[](size_t idx) const
    {
        return _array.operator[](idx);
    }

    /** Same as above, but takes a signed argument. */
    const String* operator[](int idx) const
    {
        return _array.operator[]((size_t)idx);
    }

    /**
       Return the argument at the given index.
       \see Array::operator()
    */
    const String&
    operator()(size_t idx) const
    {
        return _array.operator()(idx);
    }

    /** Same as above, but takes a signed argument. */
    const String&
    operator()(int idx) const
    {
        return _array.operator()((size_t)idx);
    }

private:
    enum flg_t
    {
        flg_owner
    };

private:
    void
    init()
    {
        setOwner(true);
        clear();
    }

    void
    init(int argc, char** argv)
    {
        setOwner(true);
        parse(argc, argv);
    }

    void
    init(const Array& args)
    {
        setOwner(true);
        parse(args);
    }

    void
    deInit()
    {
    }

private:
    size_t _idx;
    TArray<String> _array;
    TRBtree<CmdLineArg> _args;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
