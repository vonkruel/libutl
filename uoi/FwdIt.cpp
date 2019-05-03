#include <libutl/libutl.h>
#include <libutl/Collection.h>
#include <libutl/FwdIt.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::FwdIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
FwdIt::compare(const Object& rhs) const
{
    auto& it = utl::cast<FwdIt>(rhs);
    ASSERTD(hasSameOwner(it));
    return utl::compare(get(), it.get());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FwdIt::copy(const Object& rhs)
{
#ifdef DEBUG
    auto& it = utl::cast<FwdIt>(rhs);
    setOwner(it._owner, true);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
FwdIt::vclone(const Object& rhs)
{
    auto& rhsIt = utl::cast<FwdIt>(rhs);
    copy(rhsIt);
#ifdef DEBUG
    setConst(rhsIt.isConst());
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
bool
FwdIt::isValid(const utl::Object* owner) const
{
    if ((owner == nullptr) && (_owner == nullptr))
        return false;
    if ((owner != nullptr) && (_owner != owner))
        return false;
    if (!_notifyOwner)
        return true;
    if (!_owner->isA(Collection))
        return true;
    auto col = utl::cast<Collection>(_owner);
    return col->hasOwnedIt(this);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
FwdIt::setOwner(const Object* owner, bool notifyOwner) const
{
    if (owner == _owner)
        return;
    if ((_owner != nullptr) && (_notifyOwner))
    {
        _owner->removeOwnedIt(this);
    }
    _owner = owner;
    _notifyOwner = notifyOwner;
    if ((_owner != nullptr) && _notifyOwner)
    {
        _owner->addOwnedIt(this);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
FwdIt::init()
{
    _owner = nullptr;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
FwdIt::deInit()
{
    if ((_owner != nullptr) && (_notifyOwner))
    {
        _owner->removeOwnedIt(this);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
