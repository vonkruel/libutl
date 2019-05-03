#include <libutl/libutl.h>
#include <libutl/AutoPtr.h>
#include <libutl/Bool.h>
#include <libutl/SortedCollection.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::SortedCollection);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::clobber(const SortedCollection* rhs)
{
    ASSERTD(isOwner());
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    utl::clobber(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, ordering());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
SortedCollection::compare(const Object& rhs) const
{
    auto& cont = utl::cast<SortedCollection>(rhs);
    if (&cont == this)
        return 0;
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = cont.beginNew();
    AutoPtr<BidIt> rhsEnd = cont.endNew();
    return utl::compare(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, ordering());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
SortedCollection::addOrFind(const Object* object)
{
    AutoPtr<BidIt> itPtr = createIt();
    BidIt& it = *itPtr;
    findFirstIt(*object, it, true);
    if (!it.isEnd() && (compareObjects(it.get(), object) == 0))
    {
        if (isOwner())
            delete object;
        return it.get();
    }
    else
    {
        insert(object, it);
        return const_cast<Object*>(object);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SortedCollection::addOrUpdate(const Object* object)
{
    AutoPtr<BidIt> itPtr = createIt();
    BidIt& it = *itPtr;
    findFirstIt(*object, it, true);
    if (!it.isEnd() && (compareObjects(it.get(), object) == 0))
    {
        it.set(object);
        return false;
    }
    else
    {
        insert(object, it);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::insert(const Object* object, const BidIt&)
{
    ABORT();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::reverse()
{
    // utl::reverse does it
    FlagGuard fg(this, flg_owner, false);
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();
    utl::reverse(*begin, *end);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::setOrdering(Ordering* ordering, uint_t algorithm)
{
    super::setOrdering(ordering);

    // sort with the given algorithm
    if ((algorithm != sort_none) && !empty())
    {
        sort(algorithm);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::findEqualRange(const Object& key, BidIt& begin, BidIt& end)
{
    findFirstIt(key, begin);
    if (begin.isEnd())
    {
        end = begin;
    }
    else
    {
        AutoPtr<BidIt> myEnd = this->endNew();
        linearSearchSorted(begin, *myEnd, key, end, ordering(), find_last);
        ++end;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::findFirstIt(const Object& key, BidIt& it, bool insert)
{
    AutoPtr<BidIt> begin = this->beginNew();
    AutoPtr<BidIt> end = this->endNew();
    if (insert)
    {
        linearSearchSorted(*begin, *end, key, it, ordering(), find_ip);
    }
    else
    {
        linearSearch(*begin, *end, key, it, ordering(), find_first);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::findLastIt(const Object& key, BidIt& it)
{
    AutoPtr<BidIt> begin = this->beginNew();
    AutoPtr<BidIt> end = this->endNew();
    linearSearch(*begin, *end, key, it, ordering(), find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
SortedCollection::findLinear(const Object& key) const
{
    Object* res;

    // Create iterators
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();
    AutoPtr<BidIt> outIt = beginNew();

    // utl::linearSearch does it
    utl::linearSearch(*begin, *end, key, *outIt);
    res = **outIt;

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
SortedCollection::findLinearSorted(const Object& key) const
{
    Object* res;

    // Create iterators
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();
    AutoPtr<BidIt> outIt = beginNew();

    // utl::linearSearchSorted does it
    utl::linearSearchSorted(*begin, *end, key, *outIt);
    res = **outIt;

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::removeIt(BidIt& begin, const BidIt& end)
{
    // utl::remove does it
    utl::remove(begin, end, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection*
SortedCollection::difference(const SortedCollection* rhs, Collection* out) const
{
    // If no destination container was given, create one
    if (out == nullptr)
    {
        out = create();
        out->setOwner(false);
    }

    // create iterators
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    AutoPtr<BidIt> outIt;
    bool outIsSelf = (out == this);
    if (outIsSelf)
    {
        outIt = const_cast_this->beginNew();
    }
    else
    {
        outIt = out->endNew();
    }

    // utl::difference does it
    utl::difference(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, *outIt, ordering(), out->isOwner(),
                    outIsSelf);

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection*
SortedCollection::intersection(const SortedCollection* rhs, Collection* out, bool multiSet) const
{
    // If no destination container was given, create one
    if (out == nullptr)
    {
        out = create();
        out->setOwner(false);
    }

    // create iterators
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    AutoPtr<BidIt> outIt;
    bool outIsSelf = (out == this);
    if (outIsSelf)
    {
        outIt = const_cast_this->beginNew();
    }
    else
    {
        outIt = out->endNew();
    }

    // utl::intersect does it
    utl::intersect(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, *outIt, ordering(), out->isOwner(),
                   multiSet, outIsSelf);

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
SortedCollection::intersectCard(const SortedCollection* rhs) const
{
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    size_t res = utl::intersectCard(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, ordering());
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SortedCollection::intersects(const SortedCollection* rhs) const
{
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    bool res = utl::intersects(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, ordering());
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SortedCollection::isSubSet(const SortedCollection* rhs) const
{
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    bool res = utl::isSubSet(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, ordering());
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection*
SortedCollection::merge(const SortedCollection* rhs, Collection* out) const
{
    // if no destination container was given, create one
    if (out == nullptr)
    {
        out = create();
        out->setOwner(false);
    }

    // create iterators
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    AutoPtr<BidIt> outIt = out->endNew();

    // utl::merge does it
    utl::merge(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, *outIt, ordering(), out->isOwner());

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection*
SortedCollection::symmetricDifference(const SortedCollection* rhs, Collection* out) const
{
    // If no destination container was given, create one
    if (out == nullptr)
    {
        out = create();
        out->setOwner(false);
    }

    // create iterators
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    AutoPtr<BidIt> outIt = out->endNew();

    // utl::symmetricDifference does it
    utl::symmetricDifference(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, *outIt, ordering(),
                             out->isOwner());

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::symmetricDifference(const SortedCollection* rhs,
                                      Collection* lhsOut,
                                      Collection* rhsOut) const
{
    // Create iterators
    AutoPtr<BidIt> lhsBegin = beginNew();
    AutoPtr<BidIt> lhsEnd = endNew();
    AutoPtr<BidIt> rhsBegin = rhs->beginNew();
    AutoPtr<BidIt> rhsEnd = rhs->endNew();
    AutoPtr<BidIt> lhsOutIt = lhsOut->endNew();
    AutoPtr<BidIt> rhsOutIt = rhsOut->endNew();

    // utl::symmetricDifference does it
    utl::symmetricDifference(*lhsBegin, *lhsEnd, *rhsBegin, *rhsEnd, *lhsOutIt, *rhsOutIt,
                             ordering(), lhsOut->isOwner(), rhsOut->isOwner());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Collection*
SortedCollection::unique(Collection* out)
{
    // If no destination container was given, create one
    if (out == nullptr)
    {
        out = create();
        out->setOwner(false);
    }

    // create iterators
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();
    AutoPtr<BidIt> outIt;
    bool outIsSelf = (out == this);
    if (outIsSelf)
    {
        outIt = const_cast_this->beginNew();
    }
    else
    {
        outIt = out->endNew();
    }

    // utl::unique does it
    utl::unique(*begin, *end, *outIt, ordering(), out->isOwner(), outIsSelf);

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SortedCollection::testSorted() const
{
    AutoPtr<BidIt> beginPtr = beginNew();
    AutoPtr<BidIt> endPtr = endNew();
    return utl::isSorted(*beginPtr, *endPtr, ordering());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::multiKeyQuickSort(bool key, bool reverse)
{
    // utl::multiKeyQuickSort does it
    AutoPtr<BidIt> beginPtr = beginNew();
    AutoPtr<BidIt> endPtr = endNew();
    FlagGuard fg(this, flg_owner, false);
    utl::multiKeyQuickSort(*beginPtr, *endPtr, key, reverse, _items);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SortedCollection::sort(uint_t algorithm)
{
    FlagGuard fg(this, flg_owner, false);
    AutoPtr<BidIt> begin = beginNew();
    AutoPtr<BidIt> end = endNew();
    utl::sort(*begin, *end, ordering(), algorithm, _items);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SortedCollection::sanityCheck() const
{
    // check for properly ordered items, and correct # of items
    size_t itemsFound = 0;
    AutoPtr<FwdIt> it = beginNew();
    const Object* prevObject = nullptr;
    while (!it->isEnd())
    {
        const Object* nextObject = *it;
        if (prevObject != nullptr)
        {
            ASSERT(compareObjects(prevObject, nextObject) <= 0);
        }
        prevObject = nextObject;
        ++itemsFound;
        it->forward();
    }
    ASSERT(itemsFound == _items);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SortedCollection::checkInsert(const Object* object, const BidIt& it, bool sorted) const
{
    ASSERT(&it == &it.getProxiedObject());
    ASSERT(it.isValid(this));
    if (empty())
        return;
    if (sorted)
    {
        // iterator object must be >= object
        if (!it.isEnd())
        {
            const Object* itObject = it.get();
            int cmp = compareObjects(itObject, object);
            if (isMultiSet())
            {
                ASSERT(cmp >= 0);
            }
            else
            {
                ASSERT(cmp > 0);
            }
        }

        // predecessor of iterator object must be <= object
        AutoPtr<BidIt> myIt = it.clone();
        const Object* oldObject = myIt->get();
        myIt->reverse();
        const Object* myItObject = myIt->get();
        if (myItObject == oldObject)
            return;
        int cmp = compareObjects(myItObject, object);
        if (isMultiSet())
        {
            ASSERT(cmp <= 0);
        }
        else
        {
            ASSERT(cmp < 0);
        }
    }
    else
    {
        ASSERT(isMultiSet() || !contains(object));
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
