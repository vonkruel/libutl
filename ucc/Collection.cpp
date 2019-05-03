#include <libutl/libutl.h>
#include <libutl/Bool.h>
#include <libutl/Collection.h>
#include <libutl/RBtree.h>
#include <libutl/AutoPtr.h>
#include <libutl/Uint.h>
#include <libutl/dlist.h>
#include <libutl/slist.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::Collection);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::assertOwner()
{
    if (isOwner())
        return;

    AutoPtr<BidIt> itPtr = beginNew();
    BidIt& it = *itPtr;
    for (;;)
    {
        const Object* object = it.get();
        if (object == nullptr)
            break;
        it.set(object->clone());
        it++;
    }

    setOwner(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::copy(const Object& rhs)
{
    auto& col = utl::cast<Collection>(rhs);
    if (&col == this)
        return;

    // clear our contents
    clear();

    // copy items
    copyItems(col);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::steal(Object& rhs_)
{
    auto& col = utl::cast<Collection>(rhs_);
    deInit();
    FlagsMI::copyFlags(col);
    _items = col._items;
    _ordering = col._ordering;
    col._items = 0;
    col._ordering = nullptr;
#ifdef DEBUG
    col.deInit();
    col._ownedIts = nullptr;
    _ownedIts = nullptr;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::vclone(const Object& rhs)
{
    auto& col = utl::cast<Collection>(rhs);
    ASSERTD(&col != this);
    ASSERTD(this->empty());

    // copy flags
    FlagsMI::copyFlags(col);

    // copy the ordering
    setOrdering(utl::clone(col.ordering()));

    // copy items
    copyItems(col);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::dump(Stream& os, uint_t level) const
{
    dump(os, level, false, false, 4, nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::serialize(const RunTimeClass* rtc, Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        clear();
    }
    if (io == io_rd)
    {
        // assume ownership (since we are allocating storage for objects)
        setOwner(true);

        // determine # of items
        size_t items;
        utl::serialize(items, stream, io, mode);

        // serialize objects
        AutoPtr<BidIt> out = beginNew();
        utl::serializeIn(*out, items, stream, mode, rtc);
    }
    else
    {
        // serialize # of items
        utl::serialize(_items, stream, io, mode);

        // serialize contained objects
        AutoPtr<BidIt> begin = beginNew();
        AutoPtr<BidIt> end = endNew();
        utl::serializeOut(*begin, *end, stream, mode, (rtc == nullptr));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Collection::innerAllocatedSize() const
{
    size_t sz = 0;

    // ordering
    if (_ordering != nullptr)
        sz += _ordering->allocatedSize();

    // contained objects (if we own them)
    if (isOwner())
    {
        AutoPtr<BidIt> it = beginNew();
        while (!it->isEnd())
        {
            Object* object = **it;
            sz += object->allocatedSize();
            it->forward();
        }
    }

    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Collection::update(const Object* object)
{
    bool res;
    AutoPtr<BidIt> itPtr = createIt();
    BidIt& it = *itPtr;
    findIt(*object, it);
    if (it.get() == nullptr)
    {
        res = false;
        if (isOwner())
            delete object;
    }
    else
    {
        it.set(object);
        res = true;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::setOrdering(Ordering* ordering, uint_t)
{
    if (ordering == _ordering)
        return;
    delete _ordering;
    _ordering = ordering;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Collection::addIt(const Object* object)
{
    // add the object
    bool addRes = add(object);

    // add failed -> return end iterator
    if (!addRes)
        return endNew();

    // Get an iterator for the object.
    // NOTE: if self is a multi-set, this still works because:
    //    1. add() will add the new object before others with matching keys
    //    2. findIt() will find the first object with the given key
    AutoPtr<BidIt> it = createIt();
    findIt(*object, *it);
    return it.release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Collection::addOrFind(const Object* object)
{
    iterator it;
    findIt(*object, it);
    if (it.get() == nullptr)
    {
        add(object);
        return const_cast<Object*>(object);
    }
    else
    {
        if (isOwner())
            delete object;
        return *it;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Collection::addOrUpdate(const Object* object)
{
    iterator it;
    findIt(*object, it);
    if (it.get() == nullptr)
    {
        add(object);
        return true;
    }
    else
    {
        it.set(object);
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection&
Collection::copyItems(const Collection* src, const Predicate* pred, bool predVal)
{
    // create iterators
    AutoPtr<BidIt> inBegin = src->beginNew();
    AutoPtr<BidIt> inEnd = src->endNew();
    AutoPtr<BidIt> outIt = endNew();

    // utl::copy does the hot object copying action
    utl::copy(*outIt, *inBegin, *inEnd, isOwner(), pred, predVal);

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection&
Collection::copyItems(const ListNode* src, const Predicate* pred, bool predVal)
{
    for_each_ln(src, Object, object);
    if ((pred == nullptr) || (pred->eval(object) == predVal))
    {
        add(object);
    }
    for_each_end;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection&
Collection::copyItems(const SlistNode* src, const Predicate* pred, bool predVal)
{
    for_each_sln(src, Object, object);
    if ((pred == nullptr) || (pred->eval(object) == predVal))
    {
        add(object);
    }
    for_each_end;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection&
Collection::stealItems(Collection* src)
{
    ASSERTD(src != nullptr);
    ASSERTD(isOwner() && src->isOwner());
    setOwner(false);
    src->setOwner(false);
    copyItems(src);
    src->clear();
    setOwner(true);
    src->setOwner(true);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::dump(Stream& os,
                 uint_t level,
                 bool key,
                 bool printClassName,
                 uint_t indent,
                 const char* separator) const
{
    // create iterators
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();

    // utl::dump does it
    utl::dump(*begin, *end, os, level, key, printClassName, indent, separator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Collection::toString() const
{
    return "{ " + toString(", ") + " }";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Collection::toString(bool key) const
{
    return "{ " + toString(", ", key) + " }";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Collection::toString(const String& sep, bool key) const
{
    // create iterators
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();

    // utl::toString does it
    return utl::toString(*begin, *end, sep, key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection::iterator
Collection::begin() const
{
    BidIt* it = beginNew();
    IFDEBUG(it->setConst(true));
    iterator res(it);
    IFDEBUG(res.setConst(true));
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection::iterator
Collection::begin()
{
    BidIt* it = beginNew();
    return iterator(it);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Collection::createIt() const
{
    return endNew();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Collection::createIt()
{
    return endNew();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection::iterator
Collection::end() const
{
    BidIt* it = endNew();
    IFDEBUG(it->setConst(true));
    iterator res(it);
    IFDEBUG(res.setConst(true));
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection::iterator
Collection::end()
{
    BidIt* it = endNew();
    return iterator(it);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Collection::contains(const Object& key) const
{
    const Object* object = find(key);
    bool res = (object != nullptr);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Collection::count(const Predicate* pred, bool predVal) const
{
    // create iterators
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();

    if (pred == nullptr)
    {
        return _items;
    }
    else
    {
        return utl::count(*begin, *end, pred, predVal);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Collection::find(const Object& key) const
{
    AutoPtr<BidIt> itPtr = createIt();
    BidIt& it = *itPtr;
    findIt(key, it);
    Object* res = *it;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::findIt(const Object& key, BidIt& it)
{
    AutoPtr<BidIt> begin = this->beginNew();
    AutoPtr<BidIt> end = this->endNew();
    linearSearch(*begin, *end, key, it, ordering());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Collection::first() const
{
    AutoPtr<BidIt> it = beginNew();
    Object* res = **it;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Collection::last() const
{
    AutoPtr<BidIt> itPtr = endNew();
    BidIt& it = *itPtr;
    --it;
    Object* res = *it;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Collection::remove(const Object& key)
{
    AutoPtr<BidIt> itPtr = createIt();
    BidIt& it = *itPtr;
    findIt(key, it);
    bool res = (*it != nullptr);
    if (res)
        removeIt(it);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::removeIt(BidIt& it)
{
    it.set(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Collection::take(BidIt& it)
{
    Object* res = *it;
    ASSERTD(res != nullptr);
    FlagGuard fg(this, flg_owner, false);
    removeIt(it);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Collection::sanityCheck() const
{
    // check for correct # of items
    size_t itemsFound = 0;
    AutoPtr<FwdIt> it = beginNew();
    while (!it->isEnd())
    {
        ++itemsFound;
        it->forward();
    }
    ASSERT(itemsFound == _items);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Collection::addOwnedIt(const FwdIt* it) const
{
    _ownedItsLock.lock();
    if (_ownedIts == nullptr)
    {
        _ownedIts = new RBtree(false);
        _ownedIts->setOrdering(new AddressOrdering());
    }
    //_ownedIts->sanityCheck();
    ASSERTFNP(_ownedIts->add(it));
    //_ownedIts->sanityCheck();
    _ownedItsLock.unlock();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
bool
Collection::hasOwnedIt(const FwdIt* it) const
{
    return _ownedIts->contains(*it);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Collection::removeOwnedIt(const FwdIt* it) const
{
    _ownedItsLock.lock();
    ASSERT(_ownedIts != nullptr);
    //_ownedIts->sanityCheck();
    ASSERTFNP(_ownedIts->remove(*it));
    //_ownedIts->sanityCheck();
    _ownedItsLock.unlock();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Collection::exciseOwnedIts() const
{
    _ownedItsLock.lock();
    if (_ownedIts != nullptr)
        _ownedIts->sanityCheck();
    delete _ownedIts;
    _ownedIts = nullptr;
    _ownedItsLock.unlock();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::init()
{
    _items = 0;
    _ordering = nullptr;
    setMultiSet(true);
#ifdef DEBUG
    _ownedIts = nullptr;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Collection::deInit()
{
    delete _ordering;
#ifdef DEBUG
    if (_ownedIts != nullptr)
    {
        // invalidate all owned iterators
        for (auto it = _ownedIts->begin(false); *it != nullptr; ++it)
        {
            auto& ownedIt = utl::cast<FwdIt>(**it);
            ownedIt.invalidate();
        }
        delete _ownedIts;
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
