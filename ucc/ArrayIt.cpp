#include <libutl/libutl.h>
#include <libutl/Array.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ArrayIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
ArrayIt::compare(const Object& p_rhs) const
{
    auto& ai = utl::cast<ArrayIt>(p_rhs);
    ASSERTD(hasSameOwner(ai));
    size_t lhs = _offset, rhs = ai._offset;
    size_t maxOffset = _array->totalSize();
    bool lhsEnd = (lhs >= maxOffset), rhsEnd = (rhs >= maxOffset);
    if (lhsEnd && rhsEnd)
        return 0;
    return utl::compare(lhs, rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArrayIt::copy(const Object& rhs)
{
    auto& ai = utl::cast<ArrayIt>(rhs);
    super::copy(ai);
    _array = ai._array;
    _offset = ai._offset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArrayIt::forward(size_t dist)
{
    ASSERTD(isValid(_array));
    if (dist == 0)
        return;
    size_t maxOffset = _array->totalSize();
    if (_array->hasHole())
    {
        size_t offset = (_offset >= maxOffset) ? maxOffset : _offset;
        size_t last = offset;
        while ((dist > 0) && (offset < maxOffset))
        {
            ++offset;
            if (_array->get(offset) != nullptr)
            {
                --dist;
                last = offset;
            }
        }
        _offset = last;
        if (_offset >= maxOffset)
            _offset = size_t_max;
    }
    else
    {
        if (_offset < size_t_max)
            _offset += dist;
        if (_offset >= maxOffset)
            _offset = size_t_max;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
ArrayIt::offset() const
{
    return min(_array->totalSize(), _offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArrayIt::reverse(size_t dist)
{
    ASSERTD(isValid(_array));
    if (dist == 0)
        return;
    size_t maxOffset = _array->totalSize();
    if (_array->hasHole())
    {
        size_t offset = (_offset >= maxOffset) ? maxOffset : _offset;
        size_t last = offset;
        while ((dist > 0) && (offset > 0))
        {
            --offset;
            if (_array->get(offset) != nullptr)
            {
                --dist;
                last = offset;
            }
        }
        _offset = last;
        if (_offset >= maxOffset)
            _offset = size_t_max;
    }
    else
    {
        if (_offset >= maxOffset)
            _offset = maxOffset;
        if (dist >= _offset)
        {
            _offset = 0;
        }
        else
        {
            _offset -= dist;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ArrayIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_array));
    if (_offset >= _array->totalSize())
    {
        ASSERTD(object != nullptr);
        _array->add(object);
    }
    else
    {
        if (object == nullptr)
        {
            _array->removeIt((ArrayIt&)*this);
        }
        else
        {
            _array->set(_offset, object);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
ArrayIt::subtract(const RandIt& it) const
{
    auto& ait = utl::cast<ArrayIt>(it);
    ASSERTD(isValid(_array));
    ASSERTD(hasSameOwner(ait));
    ASSERTD(!_array->hasHole());
    size_t myOffset = _offset;
    size_t itOffset = ait._offset;
    size_t maxOffset = _array->totalSize();
    if (myOffset >= maxOffset)
        myOffset = maxOffset;
    if (itOffset >= maxOffset)
        itOffset = maxOffset;
    ASSERTD(myOffset >= itOffset);
    return myOffset - itOffset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
