#include <libutl/libutl.h>
#include <libutl/Predicate.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::Predicate);
UTL_CLASS_IMPL(utl::IsA);
UTL_CLASS_IMPL_ABC(utl::UnaryPredicate);
UTL_CLASS_IMPL_ABC(utl::BinaryPredicate);
UTL_CLASS_IMPL_ABC(utl::ComparisonPredicate);
UTL_CLASS_IMPL(utl::Not);
UTL_CLASS_IMPL(utl::And);
UTL_CLASS_IMPL(utl::Or);
UTL_CLASS_IMPL(utl::EqualTo);
UTL_CLASS_IMPL(utl::LessThan);
UTL_CLASS_IMPL(utl::LessThanOrEqualTo);
UTL_CLASS_IMPL(utl::GreaterThan);
UTL_CLASS_IMPL(utl::GreaterThanOrEqualTo);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Predicate //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
Predicate::compare(const Object& rhs) const
{
    // Predicate is equal to the object iff eval(object) is TRUE
    return eval(rhs) ? 0 : -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// UnaryPredicate /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

UnaryPredicate::UnaryPredicate(const Object& object, bool owner)
{
    init();
    setOwner(owner);
    setObject(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UnaryPredicate::UnaryPredicate(const Object* object, bool owner)
{
    _object = const_cast<Object*>(object);
    setOwner(owner);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnaryPredicate::copy(const Object& rhs)
{
    auto& up = utl::cast<UnaryPredicate>(rhs);
    clear();
    Predicate::copy(up);
    _object = up._object;
    if (isOwner())
    {
        _object = utl::clone(_object);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnaryPredicate::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        clear();
        Predicate::serialize(stream, io, mode);
        _object = serializeInNullable(stream, mode);
        setOwner(true);
    }
    else
    {
        Predicate::serialize(stream, io, mode);
        serializeOutNullable(_object, stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// BinaryPredicate ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

BinaryPredicate::BinaryPredicate(const Object& lhs, const Object& rhs, bool owner)
{
    init();
    setOwner(owner);
    setLHS(lhs);
    setRHS(rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinaryPredicate::BinaryPredicate(const Object* lhs, const Object* rhs, bool owner)
{
    _lhs = const_cast<Object*>(lhs);
    _rhs = const_cast<Object*>(rhs);
    setOwner(owner);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinaryPredicate::copy(const Object& rhs)
{
    auto& bp = utl::cast<BinaryPredicate>(rhs);
    clear();
    Predicate::copy(bp);
    _lhs = bp._lhs;
    _rhs = bp._rhs;
    if (isOwner())
    {
        _lhs = utl::clone(_lhs);
        _rhs = utl::clone(_rhs);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinaryPredicate::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        clear();
        Predicate::serialize(stream, io, mode);
        _lhs = serializeInNullable(stream, mode);
        _rhs = serializeInNullable(stream, mode);
        setOwner(true);
    }
    else
    {
        Predicate::serialize(stream, io, mode);
        serializeOutNullable(_lhs, stream, mode);
        serializeOutNullable(_rhs, stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
