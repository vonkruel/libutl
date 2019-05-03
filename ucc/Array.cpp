#include <libutl/libutl.h>
#include <libutl/Array.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Array);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

Array::Array(
    size_t size, size_t increment, bool owner, bool multiSet, bool keepSorted, Ordering* ordering)
{
    init(size, increment, owner, multiSet, keepSorted, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Array::Array(bool owner, bool multiSet, bool keepSorted, Ordering* ordering)
{
    init(4, size_t_max, owner, multiSet, keepSorted, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::copy(const Object& rhs)
{
    auto& col = utl::cast<Collection>(rhs);
    if (&col == this)
        return;

    // clear our contents
    clear();

    // reserve space before copying objects
    reserve(col.size());
    copyItems(col);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::steal(Object& rhs_)
{
    auto& rhs = utl::cast<Array>(rhs_);
    super::steal(rhs);
    _firstNull = rhs._firstNull;
    _array.steal(rhs._array);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Array::innerAllocatedSize() const
{
    size_t sz = super::innerAllocatedSize();
    sz += _array.innerAllocatedSize();
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::removeHoles(size_t low, size_t high)
{
    // low parameter not given -> default to _firstNull
    low = max(low, _firstNull);

    // high parameter not given -> default to _array.size
    high = min(high, _array.size());

    // nothing to do -> return immediately
    if ((low == high) || (high <= (_firstNull + 1)))
        return;

    // look for the first null
    Object** lhs = _array + low;
    Object** lim = _array + high;
    for (; (lhs < lim) && (*lhs != nullptr); ++lhs)
        ;
    if (lhs == lim)
        return;

    // look for the first non-null after it
    Object** rhs;
    for (rhs = lhs + 1; (rhs < lim) && (*rhs == nullptr); ++rhs)
        ;
    if (rhs == lim)
        return;

    // now copy
    while (rhs != lim)
    {
        *lhs++ = *rhs++;
        for (; (rhs != lim) && (*rhs == nullptr); ++rhs)
            ;
    }

    // fix _firstNull, _array.size
    size_t nullIdx = (lhs - _array.get());
    if (_firstNull >= low)
        _firstNull = nullIdx;
    if (high == _array.size())
    {
        _array.setSize(nullIdx);
    }
    else
    {
        for (; lhs < lim; ++lhs)
            *lhs = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::set(size_t idx, const Object* object, size_t firstNull)
{
    if (object == nullptr)
    {
        if (idx < _array.size())
        {
            _remove(idx);
            _firstNull = min(_firstNull, idx);
            if ((_array.size() > 0) && (idx == (_array.size() - 1)))
                fixArraySize();
        }
    }
    else
    {
        _array.grow(idx + 1);
        if (object == _array[idx])
            return;
        _remove(idx);
        _array[idx] = const_cast<Object*>(object);
        ++_items;

        // fix _firstNull
        if (idx == _firstNull)
        {
            if (firstNull == size_t_max)
            {
                setFirstNull(_firstNull + 1);
            }
            else
            {
                _firstNull = firstNull;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::shuffle(size_t low, size_t high, randutils::mt19937_64_rng* rng)
{
    // fix high
    if (high == size_t_max)
    {
        high = items();
    }

    // sanity check
    ASSERTD(!hasHole());
    ASSERTD(low <= high);
    ASSERTD(low <= items());
    ASSERTD(high <= items());

    // shuffle
    auto lowIt = _array.begin() + low;
    auto highIt = lowIt + high;
    if (rng == nullptr)
    {
        randutils::mt19937_64_rng rng;
        rng.shuffle(lowIt, highIt);
    }
    else
    {
        rng->shuffle(lowIt, highIt);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Array::sort(uint_t)
{
    // don't try to sort an array with holes in it
    if (hasHole())
        removeHoles();
    ASSERTD(_array.size() == _items);

    std::sort(_array.begin(), _array.end(), utl::lessThan<>(this->ordering()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Array::find(const Object& key) const
{
    // find the index of the matching object
    auto idx = findIndex(key);

    // no match?
    if (idx == size_t_max)
        return nullptr;

    return _array[idx];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Array::findIndex(const Object& key, uint_t findType, size_t low, size_t high) const
{
    // fix up findType
    if (findType == uint_t_max)
    {
        findType = isMultiSet() ? find_first : find_any;
    }

    size_t res;

    // high parameter not given -- default to _array.size
    if (high > _array.size())
        high = _array.size();

    //
    // search for the key
    //

    // sorted, no hole -> do a binary search
    bool sorted = isSorted();
    if (!hasHole() && sorted)
    {
        if (findType == find_last)
        {
            base_iterator lowIt(this, low);
            base_iterator highIt(this, high);
            res = binarySearch(lowIt, highIt, key, this->ordering(), find_last);
        }
        else // find_any, find_first, find_ip
        {
            auto beginIt = this->begin();
            auto lowIt = beginIt + low;
            auto highIt = beginIt + high;
            auto it = std::lower_bound(lowIt, highIt, &key, utl::lessThan<>(this->ordering()));
            if (findType == find_ip)
            {
                res = it - beginIt;
            }
            else
            {
                if ((it != highIt) && (compareObjects(*it, key) == 0))
                {
                    res = it - beginIt;
                }
                else
                {
                    res = size_t_max;
                }
            }
        }
    }
    // sorted, but has a hole -> do a slightly faster version of linear search
    else if (sorted)
    {
        base_iterator lowIt(this, low);
        base_iterator highIt(this, high);
        base_iterator it;
        linearSearchSorted(lowIt, highIt, key, it, ordering(), findType);
        res = it.offset();
        if ((res == high) && (findType != find_ip))
            res = size_t_max;
    }
    // worst case... linear search
    else
    {
        base_iterator lowIt(this, low);
        base_iterator highIt(this, high);
        base_iterator it;
        linearSearch(lowIt, highIt, key, it, ordering(), findType);
        res = it.offset();
        if ((res == high) && (findType != find_ip))
            res = size_t_max;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Array::iterator
Array::findIt(const Object& key) const
{
    auto idx = findIndex(key);
    return (idx == size_t_max) ? this->end() : this->begin() + idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::findIt(const Object& key, BidIt& it)
{
    iterator nit = findIt(key);
    it = base_iterator(this, indexOf(nit));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Array::iterator
Array::findFirstIt(const Object& key, bool insert) const
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::findFirstIt(const Object& key, BidIt& it, bool insert)
{
    iterator nit = findIt(key, insert ? find_ip : find_first);
    it = base_iterator(this, indexOf(nit));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Array::iterator
Array::findLastIt(const Object& key) const
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::findLastIt(const Object& key, BidIt& it)
{
    iterator nit = findIt(key, find_last);
    it = base_iterator(this, indexOf(nit));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Array::add(const Object* object, bool keepSorted)
{
    size_t idx;
    size_t newFirstNull = size_t_max;

    // if we are keeping the array sorted
    if (keepSorted)
    {
        // find the insertion point
        idx = findIndex(*object, find_ip);

        // if we don't allow multiples, we might disallow the addition
        Object* idxObject = (idx < _array.size()) ? get(idx) : nullptr;
        if (!isMultiSet() && (idxObject != nullptr) && (compareObjects(idxObject, object) == 0))
        {
            // we can't add the object (since multiSet == FALSE)
            if (isOwner())
                delete object;
            return false;
        }

        // ensure idx is an unused entry in the array.
        if (idxObject != nullptr)
        {
            // maybe the previous index is unused
            if (hasHole() && (idx > 0) && (get(idx - 1) == nullptr))
            {
                --idx;
            }
            else
            {
                if (idx < _firstNull)
                    newFirstNull = _firstNull + 1;
                add(idx);
            }
        }
    }
    // we are not keeping the array sorted
    else
    {
        // if multiples are not allowed and an equivalent object exists
        if (!isMultiSet() && (find(*object)) != nullptr)
        {
            // we can't add the object
            if (isOwner())
                delete object;
            return false;
        }
        idx = _firstNull;
    }

    // set the new entry to point to the newly added object
    set(idx, object, newFirstNull);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::add(size_t idx, size_t num)
{
    // nothing to do -> return immediately
    if (idx >= _array.size())
        return;

    // insert nulls @ idx
    _array.insert(idx, num);

    // fix unused-indexes
    _firstNull = min(_firstNull, idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::add(size_t idx, const Object* object)
{
    // insert an empty entry at idx
    add(idx);

    // put the object into the new entry
    set(idx, object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::insert(const Object* object, iterator it)
{
    IFDEBUG(checkInsert(object, base_iterator(this, indexOf(it)), isSorted()));
    add(indexOf(it), object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::insert(const Object* object, const BidIt& it_)
{
    auto& it = utl::cast<base_iterator>(it_.getProxiedObject());
    IFDEBUG(checkInsert(object, it, isSorted()));
    size_t offset = min(it.offset(), _array.size());
    add(offset, object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::clear()
{
    if (isOwner())
    {
        auto lim = this->end();
        for (auto it = this->begin(); it != lim; ++it)
        {
            delete *it;
        }
    }
    _array.clear();
    _items = 0;
    _firstNull = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Array::remove(const Object& key, bool relocate, bool preserveOrder)
{
    // find the index of the key
    auto idx = findIndex(key);
    if (idx == size_t_max)
        return false;

    // remove the found object
    removeItems(idx, 1, relocate, preserveOrder);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::remove(size_t idx, bool relocate, bool preserveOrder)
{
    // nothing to do -> return immediately
    if (idx >= _array.size())
        return;

    // if we are relocating, pass the buck
    if (relocate)
    {
        removeItems(idx, 1, true, preserveOrder);
    }
    else if (_array[idx] != nullptr)
    {
        // remove the object from the array
        _remove(idx);
        // fix first-null & _array.size
        _firstNull = min(_firstNull, idx);
        if (idx == (_array.size() - 1))
            fixArraySize();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::removeIt(iterator it, bool relocate, bool preserveOrder)
{
    remove(indexOf(it), relocate, preserveOrder);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::removeIt(iterator begin, iterator end, bool relocate, bool preserveOrder)
{
    removeItems(indexOf(begin), end - begin, relocate, preserveOrder);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::removeIt(BidIt& it_)
{
    auto& it = utl::cast<base_iterator>(it_.getProxiedObject());
    ASSERTD(!it.isConst());
    ASSERTD(it.isValid(this));
    remove(it.offset(), true, true);
    if (*it == nullptr)
        ++it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::removeIt(BidIt& begin_, BidIt& end_)
{
    auto& bbegin = utl::cast<base_iterator>(begin_.getProxiedObject());
    auto& bend = utl::cast<base_iterator>(end_.getProxiedObject());
    ASSERTD(!bbegin.isConst());
    ASSERTD(!bend.isConst());
    ASSERTD(bbegin.isValid(this));
    ASSERTD(bend.isValid(this));
    removeItems(bbegin.offset(), bend.offset() - bbegin.offset(), true, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::removeItems(size_t idx, size_t num, bool relocate, bool preserveOrder)
{
    size_t i, lim;

    // nothing to do -> return immediately
    if ((idx >= _array.size()) || (num == 0))
        return;

#ifdef DEBUG
    // remember whether there was a hole before we started
    bool hole = hasHole();
#endif

    // remove the elements in [idx,idx + num)
    size_t size = _array.size();
    num = min(num, size - idx);
    lim = idx + num;
    for (i = idx; i < lim; ++i)
    {
        if (_array[idx] != nullptr)
            _remove(i);
    }

    // if we removed the last object, just fix array size
    if (lim == size)
    {
        fixArraySize(idx);
        return;
    }

    // relocating elements?
    if (relocate)
    {
        // if the caller wants to prevent a hole, assume there was no hole before this operation
        ASSERTD(!hole);

        // preserve existing order of objects?
        if (preserveOrder)
        {
            _array.remove(idx, num);
            ASSERTD(_array.size() == (size - num));
            size = _array.size();
            ASSERTD(_firstNull - num == size);
            _firstNull = size;
        }
        else
        {
            // move items from the end to fill the gap
            size_t numFill = min(num, size - lim);
            size_t fillIdx = size - numFill;
            ASSERTD(numFill > 0);
            Object** lhsIt = _array.begin() + idx;
            iterator rhsIt = this->begin() + fillIdx;
            iterator rhsLim = this->begin() + size;
            for (; rhsIt != rhsLim; ++lhsIt, ++rhsIt)
            {
                *lhsIt = *rhsIt;
            }
            fixArraySize(fillIdx);
        }
    }
    else
    {
        _firstNull = min(_firstNull, idx);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::replace(iterator it, const Object* newObject, bool keepSorted)
{
    ASSERTD(it >= this->begin());
    ASSERTD(it < this->end());
    ASSERTD(*it != nullptr);
    ASSERTD(newObject != nullptr);

    if (keepSorted && isSorted())
    {
        // find out where to re-insert
        size_t destIdx = findIndex(*newObject, find_ip);

        // replace object @ it with newObject
        if (isOwner())
            delete *it;
        *const_cast<Object**>(it) = const_cast<Object*>(newObject);

        // relocate object @ idx to destIdx (moving everything in between over one position)
        _array.relocate(destIdx, indexOf(it));
    }
    else
    {
        if (isOwner())
            delete *it;
        *const_cast<Object**>(it) = const_cast<Object*>(newObject);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::replace(base_iterator& it, const Object* newObject)
{
    replace(this->begin() + it.offset(), newObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Array::replace(const Object* oldObject, const Object* newObject)
{
    ASSERTD(oldObject != nullptr);
    ASSERTD(newObject != nullptr);
    ASSERTD(oldObject != newObject);

    // find oldObject
    auto it = findIt(*oldObject);
    if (it == end())
    {
        if (isOwner())
            delete newObject;
        return false;
    }
    else
    {
        replace(it, newObject);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Array::sanityCheck() const
{
    isSorted() ? super::sanityCheck() : Collection::sanityCheck();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::init(
    size_t size, size_t increment, bool owner, bool multiSet, bool keepSorted, Ordering* ordering)
{
    setOwner(owner);
    setMultiSet(multiSet);
    setKeepSorted(keepSorted);
    if (ordering != nullptr)
        super::setOrdering(ordering, sort_none);

    // set up the Object* array
    _array.setAutoInit(true);
    _array.setIncrement(increment);
    _array.reserve(size);

    _firstNull = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::deInit()
{
    // if we own elements, we must delete them
    if (isOwner())
        clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Array::iterator
Array::findIt(const Object& key, uint_t findType) const
{
    auto idx = findIndex(key, findType);
    if (idx == size_t_max)
        return this->end();
    return this->begin() + idx;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::_remove(size_t idx)
{
    ASSERTD(idx < _array.size());
    if (_array[idx] == nullptr)
        return;
    if (isOwner())
        delete _array[idx];
    _array[idx] = nullptr;
    --_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::setFirstNull(size_t idx)
{
    Object** it = _array + idx;
    Object** lim = _array + _array.size();
    for (; (it < lim) && (*it != nullptr); ++it)
        ;
    _firstNull = it - _array.get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Array::fixArraySize(size_t size)
{
    // make _array exactly long enough to contain the last non-null entry
    Object** it = _array + size - 1;
    Object** begin = _array;
    for (; (it >= begin) && (*it == nullptr); --it)
        ;
    _array.setSize((it + 1) - begin);
    _firstNull = min(_firstNull, _array.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
