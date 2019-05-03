#include <libutl/libutl.h>
#include <libutl/Pair.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Pair);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

Pair::Pair(const Object& first, const Object& second, bool firstOwner, bool secondOwner)
{
    init();
    setFirstOwner(firstOwner);
    setSecondOwner(secondOwner);
    setFirst(first);
    setSecond(second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Pair::Pair(const Object* first, const Object* second, bool firstOwner, bool secondOwner)
{
    _first = const_cast<Object*>(first);
    _second = const_cast<Object*>(second);
    setFirstOwner(firstOwner);
    setSecondOwner(secondOwner);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Pair::compare(const Object& rhs) const
{
    if (rhs.isA(Pair))
    {
        auto& pair = utl::cast<Pair>(rhs);
        if (&pair == this)
            return 0;
        int res = _first->compare(*(pair._first));
        if (res != 0)
            return res;
        res = _second->compare(*(pair._second));
        return res;
    }
    else
    {
        ASSERTD(_first != nullptr);
        return _first->compare(rhs);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pair::copy(const Object& rhs)
{
    auto& pair = utl::cast<Pair>(rhs);
    if (&pair == this)
        return;
    clear();
    _first = pair._first;
    _second = pair._second;
    if (isFirstOwner())
        _first = utl::clone(_first);
    if (isSecondOwner())
        _second = utl::clone(_second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pair::vclone(const Object& rhs)
{
    auto& pair = utl::cast<Pair>(rhs);
    copyFlags(pair);
    _first = pair._first;
    _second = pair._second;
    if (isFirstOwner())
        _first = utl::clone(_first);
    if (isSecondOwner())
        _second = utl::clone(_second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pair::steal(Object& rhs)
{
    auto& pair = utl::cast<Pair>(rhs);
    copyFlags(pair);
    _first = pair._first;
    _second = pair._second;
    pair._first = nullptr;
    pair._second = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Object&
Pair::getKey() const
{
    ASSERTD(_first != nullptr);
    return *_first;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pair::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        clear();
        _first = serializeInNullable(stream, mode);
        _second = serializeInNullable(stream, mode);
        setFirstOwner(true);
        setSecondOwner(true);
    }
    else
    {
        serializeOutNullable(_first, stream, mode);
        serializeOutNullable(_second, stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Pair::toString() const
{
    return String("(") + _first->toString() + ',' + _second->toString() + ')';
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Pair::innerAllocatedSize() const
{
    size_t sz = 0;
    if (isFirstOwner() && (_first != nullptr))
        sz += _first->allocatedSize();
    if (isSecondOwner() && (_second != nullptr))
        sz += _second->allocatedSize();
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pair::setFirst(const Object* first)
{
    if (isFirstOwner())
    {
        delete _first;
    }
    _first = const_cast<Object*>(first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Pair::setSecond(const Object* second)
{
    if (isSecondOwner())
    {
        delete _second;
    }
    _second = const_cast<Object*>(second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
