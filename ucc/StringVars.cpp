#include <libutl/libutl.h>
#include <libutl/Pair.h>
#include <libutl/RBtree.h>
#include <libutl/StringVars.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::StringVars);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
StringVars::compare(const utl::Object& rhs) const
{
    auto& vars = utl::cast<StringVars>(rhs);
    RBtree lhsVars(false), rhsVars(false);
    lhsVars = _vars;
    rhsVars = vars._vars;
    return lhsVars.compare(rhsVars);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringVars::copy(const utl::Object& rhs)
{
    auto& vars = utl::cast<StringVars>(rhs);
    _vars = vars._vars;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
StringVars::innerAllocatedSize() const
{
    return _vars.innerAllocatedSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringVars::serialize(utl::Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        _vars.clear();
        size_t numVars;
        utl::serialize(numVars, stream, io, mode);
        for (size_t i = 0; i < numVars; ++i)
        {
            String* var = new String();
            String* val = new String();
            SCOPE_FAIL
            {
                delete var;
                delete val;
            };
            var->serialize(stream, io, mode);
            val->serialize(stream, io, mode);
            setValue(var, val);
        }
    }
    else
    {
        size_t numVars = _vars.size();
        utl::serialize(numVars, stream, io, mode);
        RBtree vars(false);
        vars = _vars;
        RBtree::iterator it, lim = vars.end();
        for (it = vars.begin(); it != lim; ++it)
        {
            auto pair = utl::cast<Pair>(*it);
            pair->first()->serialize(stream, io, mode);
            pair->second()->serialize(stream, io, mode);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
StringVars::isNull(const String& name) const
{
    auto pair = utl::cast<Pair>(_vars.find(name));
    return (pair == nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringVars::setNull(const String& name)
{
    _vars.remove(name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const String&
StringVars::valueOf(const String& name) const
{
    auto pair = utl::cast<Pair>(_vars.find(name));
    if (pair == nullptr)
        return utl::emptyString;
    return utl::cast<String>(*pair->getSecond());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String&
StringVars::valueOf(const String& name, bool create)
{
    auto pair = utl::cast<Pair>(_vars.find(name));
    if (pair == nullptr)
    {
        if (create)
        {
            pair = new Pair(name.clone(), new String());
            _vars += pair;
        }
        else
        {
            return nonConstEmptyString;
        }
    }
    return utl::cast<String>(*pair->getSecond());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pair*
StringVars::setValue(String* name, String* value)
{
    ASSERTD(name != nullptr);

    // value is null -> remove argument
    if ((value == nullptr) || value->empty())
    {
        _vars.remove(*name);
        delete name;
        if (value != nullptr)
            delete value;
        return nullptr;
    }

    // value must own its chars
    value->assertOwner();

    // add/replace argument
    auto pair = utl::cast<Pair>(_vars.find(*name));
    if (pair == nullptr)
    {
        name->assertOwner();
        pair = new Pair(name, value);
        _vars += pair;
        return pair;
    }
    else
    {
        delete name;
        pair->setSecond(value);
        return nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringVars::add(const StringVars& vars)
{
    _vars.add(vars._vars);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
StringVars::intersect(const SortedCollection& rhs)
{
    // if rhs has same size as _vars then all variables must be relevant
    // .. (since rhs only includes relevant variables)
    if (_vars.size() == rhs.size())
        return;

    // intersect _vars with rhs
    RBtree newVars;
    newVars = _vars;
    _vars.clear();
    newVars.intersect(rhs);
    _vars.setOwner(false);
    _vars = newVars;
    _vars.setOwner(true);
    newVars.setOwner(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String StringVars::nonConstEmptyString;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
