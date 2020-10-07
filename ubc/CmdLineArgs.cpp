#include <libutl/libutl.h>
#include <libutl/CmdLineArgs.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

class CmdLineArg
    : public Object
    , protected FlagsMI
{
    UTL_CLASS_DECL(CmdLineArg, Object);

public:
    CmdLineArg(size_t idx, const String* sw, bool valFlag = false)
    {
        init(idx, sw, valFlag);
    }

    virtual const Object&
    getKey() const
    {
        return *_sw;
    }

    size_t
    getIdx() const
    {
        return _idx;
    }

    const String&
    getSwitch() const
    {
        return *_sw;
    }

    bool
    getValFlag() const
    {
        return getFlag(flg_val);
    }

private:
    enum flg_t
    {
        flg_val
    };

private:
    void
    init(size_t idx = 0, const String* sw = nullptr, bool valFlag = false)
    {
        _idx = idx;
        _sw = const_cast<String*>(sw);
        setFlag(flg_val, valFlag);
    }
    void
    deInit()
    {
        delete _sw;
    }

private:
    size_t _idx;
    String* _sw;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void
CmdLineArgs::clear()
{
    _idx = 0;
    _array.clear();
    _args.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
CmdLineArgs::isSet(const String& sw, String* val, size_t* swIdx)
{
    CmdLineArg* arg = _args.findT(sw);
    if (arg == nullptr)
        return false;
    if ((val != nullptr) && (arg->getValFlag()))
    {
        val->clear();
        size_t idx = arg->getIdx() + 1;
        if (idx < items())
        {
            String& nextArg = _array(idx);
            if (!nextArg.empty() && nextArg[0] != '-')
            {
                *val = nextArg;
                if (idx == _idx)
                {
                    ++_idx;
                }
            }
        }
    }
    if (swIdx != nullptr)
    {
        *swIdx = arg->getIdx();
    }
    _args.remove(sw);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
CmdLineArgs::parse(int argc, char** argv)
{
    clear();
    _idx = 1;
    size_t i;
    for (i = 0; i < (size_t)argc; i++)
    {
        _array += new String(argv[i]);
    }
    for (i = 1; i < (size_t)argc; i++)
    {
        const String& str = (*this)(i);
        size_t strLen = str.length();
        if (strLen < 2)
            continue;
        if (str[0] != '-')
            continue;
        _idx = i + 1;
        if (str == "--")
            break;
        String sw;
        if (str[1] == '-')
        {
            sw = str.subString(2);
            _args.addOrUpdate(new CmdLineArg(i, sw.clone(), true));
            _idx = max(_idx, i + 1);
        }
        else
        {
            bool valFlag = (strLen == 2);
            for (size_t j = 1; j < strLen; j++)
            {
                sw = str.subString(j, 1);
                _args.addOrUpdate(new CmdLineArg(i, sw.clone(), valFlag));
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
CmdLineArgs::parse(const Array& args)
{
    size_t argc = args.size();
    char** argv = new char*[argc];
    SCOPE_EXIT
    {
        delete[] argv;
    };
    for (size_t i = 0; i < argc; ++i)
    {
        auto& arg = utl::cast<String>(args(i));
        argv[i] = arg.get();
    }
    parse(argc, argv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
CmdLineArgs::printErrors(Stream& os, const String* prefix)
{
    if (ok())
        return false;
    for (auto arg : _args)
    {
        if (prefix == nullptr)
        {
            os << "unknown switch: ";
        }
        else
        {
            os << *prefix;
        }
        const String& sw = arg->getSwitch();
        if (sw.length() == 1)
        {
            os << "-";
        }
        else
        {
            os << "--";
        }
        os << arg->getSwitch() << endl;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::CmdLineArg);
UTL_CLASS_IMPL(utl::CmdLineArgs);
UTL_INSTANTIATE_TPL(utl::TRBtree, utl::CmdLineArg);
UTL_INSTANTIATE_TPL(utl::TRBtreeIt, utl::CmdLineArg);
