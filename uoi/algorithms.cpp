#include <libutl/libutl.h>
#include <libutl/algorithms_inl.h>
#include <libutl/Heap.h>
#include <libutl/AutoPtr.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

static void
mergeSort(Object** array, Object** buf, size_t begin, size_t end, const Ordering* ordering);

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
binarySearch(const RandIt& begin,
             const RandIt& end,
             const Object& key,
             const Ordering* ordering,
             uint_t findType)
{
    size_t low, high, mid, res;
    int cmp;

    low = begin.offset();
    high = end.offset();

    // nothing to do -- return immediately
    if ((high-- - low) == 0)
    {
        return (findType == find_ip) ? low : size_t_max;
    }

    // while low and high make sense
    while ((low <= high) && (high != size_t_max))
    {
        // mid is half-way between low and high
        res = mid = (low + high) / 2;

        // see how the key compares against the object at mid
        cmp = utl::compare(begin[mid], key, ordering);
        // if key < begin[mid], search the lower half
        if (cmp > 0)
        {
            high = mid - 1;
        }
        // else if key > begin[mid], search the upper half
        else if (cmp < 0)
        {
            // new low is mid + 1
            low = mid + 1;
        }
        else
        {
            break;
        }
    }

    // we're done if this is an insertion point search
    if (findType == find_ip)
    {
        return (cmp != 0) ? low : res;
    }

    // didn't find a match?
    if (cmp != 0)
    {
        return size_t_max;
    }

    // there could be multiple matches for the same key
    if (findType == find_first)
    {
        if ((res > low) && (utl::compare(&key, begin[res - 1], ordering) == 0))
        {
            AutoPtr<RandIt> begin2 = begin.clone();
            begin2->seek(low);
            AutoPtr<RandIt> end2 = end.clone();
            end2->seek(res);
            res = binarySearch(*begin2, *end2, key, ordering, findType);
        }
    }
    else if (findType == find_last)
    {
        if ((res < high) && (utl::compare(&key, begin[res + 1], ordering) == 0))
        {
            AutoPtr<RandIt> begin2 = begin.clone();
            begin2->seek(res + 1);
            AutoPtr<RandIt> end2 = end.clone();
            end2->seek(high + 1);
            res = binarySearch(*begin2, *end2, key, ordering, findType);
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
clobber(const FwdIt& lhsBegin,
        const FwdIt& lhsEnd,
        const FwdIt& rhsBegin,
        const FwdIt& rhsEnd,
        const Ordering* ordering)
{
    // Create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // While there are objects left in both sets
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // Compare lhs's object with rhs's object
        Object* lhsObject = *lhs;
        Object* rhsObject = *rhs;
        if ((lhsObject == nullptr) && (rhsObject == nullptr))
        {
            ++lhs;
            ++rhs;
            continue;
        }
        int cmp = compare(lhsObject->getKey(), rhsObject->getKey(), ordering);
        // If lhs < rhs, skip lhs object
        if (cmp < 0)
        {
            ++lhs;
        }
        // If lhs > rhs, skip rhs object
        else if (cmp > 0)
        {
            ++rhs;
        }
        // Else objects are equal, so object belongs to intersection
        else
        {
            // Clobber first set's object with second set's object
            *lhsObject = *rhsObject;
            // Increment iterators
            ++lhs;
            ++rhs;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
compare(const FwdIt& lhsBegin,
        const FwdIt& lhsEnd,
        const FwdIt& rhsBegin,
        const FwdIt& rhsEnd,
        const Ordering* ordering)
{
    // create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;
    int res = 0;

    for (;; ++lhs, ++rhs)
    {
        bool lhsDone = (lhs == lhsEnd);
        bool rhsDone = (rhs == rhsEnd);
        // if we reached the end of both sequences, they're equal
        if (lhsDone && rhsDone)
        {
            break;
        }
        // end of lhs sequence but not rhs, lhs < rhs
        if (lhsDone)
        {
            res = -1;
            break;
        }
        // end of rhs sequence but not lhs, lhs > rhs
        if (rhsDone)
        {
            res = 1;
            break;
        }
        // both objects exist, so compare them
        res = compare(*lhs, *rhs, ordering);
        // if objects are unequal, we're done
        if (res != 0)
        {
            break;
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
copy(FwdIt& out,
     const FwdIt& begin,
     const FwdIt& end,
     bool cloning,
     const Predicate* pred,
     bool predVal)
{
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;
    while (it != end)
    {
        AutoPtr<> object(*it, false);
        // Ensure the predicate is satisfied (if it exists)
        if ((pred == nullptr) || (pred->eval(object) == predVal))
        {
            // If cloning enabled, insert the cloned object
            if (cloning)
            {
                object = object->clone();
            }
            out.set(object.release());
            ++out;
        }
        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
copy(Object** out,
     const FwdIt& begin,
     const FwdIt& end,
     bool cloning,
     const Predicate* pred,
     bool predVal)
{
    Object** ptr = out;
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;
    size_t idx = 0;
    while (it != end)
    {
        AutoPtr<> object(*it, false);
        // ensure the predicate is satisfied (if it exists)
        if ((pred == nullptr) || (pred->eval(object) == predVal))
        {
            // if cloning enabled, add the cloned object
            if (cloning)
            {
                object = object->clone();
            }
            ptr[idx++] = object.release();
        }
        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
copy(FwdIt& out, const Vector<Object*>& in, bool cloning, const Predicate* pred, bool predVal)
{
    Object* const* ptr = in.get();
    size_t size = in.size();
    size_t idx = 0;
    while (idx != size)
    {
        AutoPtr<> object(ptr[idx], false);
        // ensure the predicate is satisfied (if it exists)
        if ((pred == nullptr) || (pred->eval(object) == predVal))
        {
            // if cloning enabled, insert the cloned object
            if (cloning)
            {
                object = object->clone();
            }
            out.set(object.release());
            ++out;
        }
        ++idx;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
count(const FwdIt& begin, const FwdIt& end, const Predicate* pred, bool predVal)
{
    if ((pred == nullptr) && (begin.isA(RandIt)))
    {
        return utl::cast<RandIt>(end).subtract(utl::cast<RandIt>(begin));
    }

    size_t res = 0;
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;
    while (it != end)
    {
        if ((pred == nullptr) || (pred->eval(*it) == predVal))
        {
            ++res;
        }
        ++it;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
difference(const FwdIt& lhsBegin,
           const FwdIt& lhsEnd,
           const FwdIt& rhsBegin,
           const FwdIt& rhsEnd,
           FwdIt& out,
           const Ordering* ordering,
           bool cloning,
           bool outIsLHS)
{
    // create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // while objects remain in both sets
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // compare lhs's object with rhs's object
        int cmp = compare(*lhs, *rhs, ordering);
        // If lhs < rhs, lhs's object doesn't appear in rhs so
        // it's part of our result.
        if (cmp < 0)
        {
            AutoPtr<> object(*lhs, false);
            // if cloning, output a clone of the object
            if (cloning)
            {
                object = object->clone();
            }
            out.set(object.release());
            // increment iterators
            ++out;
            ++lhs;
        }
        // else if lhs > rhs, rhs's object doesn't appear in rhs so
        // skip past it.
        else if (cmp > 0)
        {
            ++rhs;
        }
        // else the objects are equal so skip them both
        else
        {
            ++lhs;
            ++rhs;
        }
    }

    // left-overs in lhs are part of the result also
    copy(out, lhs, lhsEnd, cloning);

    // if output is lhs, remove objects not part of the result
    if (outIsLHS)
    {
        remove(out, lhsEnd);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
dump(const FwdIt& begin,
     const FwdIt& end,
     Stream& os,
     uint_t level,
     bool key,
     bool printClassName,
     uint_t indent,
     const char* separator)
{
    // create iterator
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;

    // for each object
    bool firstObject = true;
    while (it != end)
    {
        Object* object = *it;
        ASSERTD(object != nullptr);

        if (!firstObject && (separator != nullptr))
        {
            os << separator << endl;
            firstObject = false;
        }

        // dump the object
        const Object* dumpObject;
        if (key)
        {
            dumpObject = object->getKey();
        }
        else
        {
            dumpObject = object;
        }
        if (printClassName)
        {
            if (indent == 0)
            {
                os << "=== ";
            }
            os << dumpObject->getClassName() << endl;
        }
        os.indent(indent);
        dumpObject->dump(os, level);
        os.unindent(indent);

        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
heapSort(const BidIt& begin, const BidIt& end, const Ordering* ordering, size_t size)
{
    // create a heap
    if (size == size_t_max)
        size = count(begin, end);
    Heap heap(size);
    heap.setOrdering(clone(ordering));

    // add objects to the heap
    AutoPtr<BidIt> itPtr = begin.clone();
    BidIt& it = *itPtr;
    while (it != end)
    {
        heap.add(*it);
        ++it;
    }

    // remove objects from the heap
    while (it != begin)
    {
        --it;
        it.set(heap.pop());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
heapSort(Object** array, size_t begin, size_t end, const Ordering* ordering)
{
    Object** ptr = array;

    // count items
    size_t items = end - begin;

    // create a heap
    Heap heap(items);
    heap.setOrdering(clone(ordering));

    // add objects to the heap
    size_t i;
    for (i = begin; i != end; i++)
    {
        heap.add(ptr[i]);
    }

    // remove objects from the heap
    i = end;
    do
        ptr[--i] = heap.pop();
    while (i != begin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
insertionSort(const FwdIt& begin, const FwdIt& end, const Ordering* ordering, size_t size)
{
    if (size == size_t_max)
        size = count(begin, end);
    Vector<Object*> vect(size, 1);
    copy(vect, begin, end);
    insertionSort(vect, 0, size, ordering);
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;
    copy(it, vect);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
insertionSort(Object** array, size_t begin_, size_t end_, const Ordering* ordering)
{
    Object* object;
    auto begin = array + begin_;
    auto end = array + end_;

    // nothing to do -> return immediately
    if ((end - begin) <= 1)
    {
        return;
    }

    // while un-inserted elements remain
    auto i = begin;
    while (++i != end)
    {
        // object is the element we are about to insert into the sorted sequence
        object = *i;
        // j is the pointer we are trying to insert object at
        // we decrement j until we find a valid insertion-point for object
        auto j = i;
        for (;;)
        {
            // compare object with the previous object
            auto cmp = compare(object, *(j - 1), ordering);

            // if object is >= the previous object, it's successfully inserted
            if (cmp >= 0)
            {
                break;
            }

            // object is < the previous object, so set array[j] = array[j - 1]
            *j = *(j - 1);

            // if j reaches begin, object is successfully inserted
            if (--j == begin)
            {
                break;
            }
        }
        // j is the insertion-point that we found for object, so array[j] = object
        *j = object;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
intersect(const FwdIt& lhsBegin,
          const FwdIt& lhsEnd,
          const FwdIt& rhsBegin,
          const FwdIt& rhsEnd,
          FwdIt& out,
          const Ordering* ordering,
          bool cloning,
          bool multiSet,
          bool outIsLHS)
{
    // Create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // While there are objects left in both sets
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // Compare lhs's object with rhs's object
        int cmp = compare(*lhs, *rhs, ordering);
        // If lhs < rhs, skip lhs object
        if (cmp < 0)
        {
            ++lhs;
        }
        // If lhs > rhs, skip rhs object
        else if (cmp > 0)
        {
            ++rhs;
        }
        // Else objects are equal, so object belongs to intersection
        else
        {
            AutoPtr<> object(*lhs, false);
            // If we're cloning, output a clone of the object
            if (cloning)
            {
                object = object->clone();
            }
            out.set(object.release());
            // Increment iterators
            ++out;
            ++lhs;
            if (!multiSet)
            {
                ++rhs;
            }
        }
    }

    if (outIsLHS)
    {
        remove(out, lhsEnd);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
intersectCard(const FwdIt& lhsBegin,
              const FwdIt& lhsEnd,
              const FwdIt& rhsBegin,
              const FwdIt& rhsEnd,
              const Ordering* ordering,
              bool multiSet)
{
    // Initialize counter
    size_t res = 0;

    // Create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // While both sets have objects left
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // Compare lhs's object with rhs's object
        int cmp = compare(*lhs, *rhs, ordering);
        // If lhs < rhs, skip lhs object
        if (cmp < 0)
        {
            ++lhs;
        }
        // If lhs > rhs, skip rhs object
        else if (cmp > 0)
        {
            ++rhs;
        }
        // Else object belongs to intersection
        else
        {
            // Increment count, increment iterators
            ++res;
            ++lhs;
            if (!multiSet)
            {
                ++rhs;
            }
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
intersects(const FwdIt& lhsBegin,
           const FwdIt& lhsEnd,
           const FwdIt& rhsBegin,
           const FwdIt& rhsEnd,
           const Ordering* ordering)
{
    // result is initially false
    bool res = false;

    // create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // while both sets have objects left
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // compare lhs's object with rhs's object
        int cmp = compare(*lhs, *rhs, ordering);
        // if lhs < rhs, skip lhs object
        if (cmp < 0)
        {
            ++lhs;
        }
        // if lhs > rhs, skip rhs object
        else if (cmp > 0)
        {
            ++rhs;
        }
        // else there is an intersection
        else
        {
            res = true;
            break;
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
isSorted(const FwdIt& begin, const FwdIt& end, const Ordering* ordering)
{
    AutoPtr<FwdIt> itPtr = begin.clone();
    auto& it = *itPtr;
    auto prev = *it;
    for (++it; it != end; ++it)
    {
        auto cur = *it;
        if (compare(prev, cur, ordering) > 0)
        {
            return false;
        }
        prev = cur;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
isSubSet(const FwdIt& lhsBegin,
         const FwdIt& lhsEnd,
         const FwdIt& rhsBegin,
         const FwdIt& rhsEnd,
         const Ordering* ordering)
{
    bool res = true;

    // Create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // While objects remain in both sets
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // Compare lhs object with rhs object
        int cmp = compare(*lhs, *rhs, ordering);
        // If lhs < rhs, lhs has an object rhs doesn't so lhs not a subset
        if (cmp < 0)
        {
            res = false;
            break;
        }
        // Else if lhs > rhs, rhs has an object lhs doesn't so skip it
        else if (cmp > 0)
        {
            ++rhs;
        }
        // Else objects are equal so skip over them
        else
        {
            ++lhs;
            ++rhs;
        }
    }

    if (lhs != lhsEnd)
    {
        res = false;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
isSuperSet(const FwdIt& lhsBegin,
           const FwdIt& lhsEnd,
           const FwdIt& rhsBegin,
           const FwdIt& rhsEnd,
           const Ordering* ordering)
{
    // Simply use isSubSet with sets reversed
    return isSubSet(rhsBegin, rhsEnd, lhsBegin, lhsEnd, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
linearSearch(const FwdIt& begin,
             const FwdIt& end,
             const Object& key,
             FwdIt& out,
             const Ordering* ordering,
             uint_t findType)
{
    int cmp;
    bool found = false;
    AutoPtr<FwdIt> lastMatch;
    for (out = begin; out != end; ++out)
    {
        // compare key with object
        cmp = utl::compare(*out, &key, ordering);
        // if they're equal, we found a matching object
        if (cmp == 0)
        {
            found = true;
            if (findType == find_last)
            {
                lastMatch = out.clone();
            }
            else
            {
                break;
            }
        }
    }

    if (found && (findType == find_last))
    {
        out = *lastMatch;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
linearSearchSorted(const FwdIt& begin,
                   const FwdIt& end,
                   const Object& key,
                   FwdIt& out,
                   const Ordering* ordering,
                   uint_t findType)
{
    int cmp;
    bool found = false;
    AutoPtr<FwdIt> lastMatch;
    for (out = begin; out != end; ++out)
    {
        // compare key with the current object
        cmp = utl::compare(&key, *out, ordering);

        // if key < the current object, we're done
        if (cmp < 0)
        {
            if (findType != find_ip)
            {
                out = end;
            }
            break;
        }

        // note: key >= current object

        if (cmp == 0)
        {
            found = true;
            if (findType == find_last)
            {
                lastMatch = out.clone();
            }
            else
            {
                break;
            }
        }
    }

    if (found && (findType == find_last))
    {
        out = *lastMatch;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
merge(const FwdIt& lhsBegin,
      const FwdIt& lhsEnd,
      const FwdIt& rhsBegin,
      const FwdIt& rhsEnd,
      FwdIt& out,
      const Ordering* ordering,
      bool cloning)
{
    // create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // while objects remain in both sequences
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // compare lhs's object with rhs's object
        int cmp = compare(*lhs, *rhs, ordering);
        AutoPtr<> object;
        // if lhs <= rhs, output lhs
        if (cmp <= 0)
        {
            object.set(*lhs, false);
            ++lhs;
            if (cmp == 0)
                ++rhs;
        }
        else
        {
            object.set(*rhs, false);
            ++rhs;
        }
        // if cloning, output a clone of the object
        if (cloning)
        {
            object = object->clone();
        }
        out.set(object.release());

        // increment output iterator
        ++out;
    }

    // left-overs are part of the result also
    copy(out, lhs, lhsEnd, cloning);
    copy(out, rhs, rhsEnd, cloning);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
mergeSort(const FwdIt& begin, const FwdIt& end, const Ordering* ordering, size_t size)
{
    if (size == size_t_max)
        size = count(begin, end);
    Vector<Object*> vect(size, 1);
    copy(vect, begin, end);
    mergeSort(vect, 0, size, ordering);
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;
    copy(it, vect);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
mergeSort(Object** array, size_t begin, size_t end, const Ordering* ordering)
{
    Vector<Object*> buf(end - begin);
    mergeSort(array, buf.get(), begin, end, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
mergeSort(Object** array, Object** buf, size_t begin, size_t end, const Ordering* ordering)
{
    Object** ptr = array;

    // use a different algorithm for smaller-sized sequences
    if ((end - begin) <= 16)
    {
        insertionSort(array, begin, end);
        return;
    }

    // merge sort each half
    size_t mid = (begin + end) / 2;
    mergeSort(array, buf, begin, mid, ordering);
    mergeSort(array, buf, mid, end, ordering);

    // merge halves together
    size_t lhsIdx = begin;
    size_t rhsIdx = mid;
    Object** out = buf;
    while ((lhsIdx < mid) && (rhsIdx < end))
    {
        // compare lhs's object with rhs's object
        int cmp = compare(ptr[lhsIdx], ptr[rhsIdx], ordering);
        Object* object;
        // if lhs <= rhs, output lhs
        if (cmp <= 0)
        {
            object = ptr[lhsIdx];
            ++lhsIdx;
        }
        else
        {
            object = ptr[rhsIdx];
            ++rhsIdx;
        }
        *(out++) = object;
    }

    // handle left-overs in left or right half
    if (lhsIdx < mid)
    {
        memmove(out, array + lhsIdx, (mid - lhsIdx) * sizeof(void*));
        // bufVect.copy(array, idx, lhsIdx, mid);
    }
    else
    {
        memmove(out, array + rhsIdx, (end - rhsIdx) * sizeof(void*));
        // bufVect.copy(array, idx, rhsIdx, end);
    }

    // copy the result back into vect[]
    memmove(array, buf, (end - begin) * sizeof(void*));
    // arrayVect.copy(buf, begin, 0, end - begin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
multiKeyQuickSort(const FwdIt& begin, const FwdIt& end, bool key, bool reverse, size_t size)
{
    // prepare to sort
    if (size == size_t_max)
        size = count(begin, end);
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;
    Vector<MultiKeyObject> objects(size, 1);
    size_t i;
    for (i = 0; it != end; ++it, ++i)
    {
        Object* object = *it;
        const String* str;
        if (key)
        {
            str = utl::cast<String>(object->getKey());
        }
        else
        {
            str = utl::cast<String>(object);
        }
        objects[i] = MultiKeyObject(object, str->get());
    }

    // perform the sort
    if (reverse)
    {
        multiKeyQuickSort<char>(objects.get(), subtractReverse<char>(), 0, objects.size());
    }
    else
    {
        multiKeyQuickSort<char>(objects.get(), subtract<char>(), 0, objects.size());
    }

    // put results back into sequence
    for (i = 0, it = begin; it != end; ++it, ++i)
    {
        it.set(objects[i].getObject());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
quickSort(const FwdIt& begin, const FwdIt& end, const Ordering* ordering, size_t size)
{
    if (size == size_t_max)
        size = count(begin, end);
    Vector<Object*> vect(size, 1);
    copy(vect, begin, end);
    quickSort(vect, 0, size, ordering);
    AutoPtr<FwdIt> it = begin.clone();
    copy(*it, vect);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
quickSort(Object** array, size_t begin, size_t end, const Ordering* ordering)
{
    Object** ptr = array;
    size_t i, j, low, mid, high;
    size_t stack[64];
    size_t* sp = stack;
    int cmp;

    // initialize
    low = begin;
    high = end;

    // nothing to do -- return immediately
    if ((high-- - low) <= 1)
    {
        return;
    }

    // quickSort with median-of-three partitioning and no recursion
    for (;;)
    {
        mid = (low + high) / 2;
        if (high - low > 10)
        {
            // perform median-of-three partitioning
            cmp = compare(ptr[low], ptr[mid], ordering);
            if (cmp > 0)
            {
                utl::swap(array, low, mid);
            }
            cmp = compare(ptr[mid], ptr[high], ordering);
            if (cmp > 0)
            {
                utl::swap(array, mid, high);
            }
            cmp = compare(ptr[low], ptr[mid], ordering);
            if (cmp > 0)
            {
                utl::swap(array, low, mid);
            }
            i = low;
            j = high - 1;
            utl::swap(array, mid, high - 1);
            for (;;)
            {
                // seek until it[i] >= mid
                for (;;)
                {
                    ++i;
                    cmp = compare(ptr[i], ptr[high - 1], ordering);
                    if (cmp >= 0)
                    {
                        break;
                    }
                }
                // seek until it[j] <= mid
                for (;;)
                {
                    --j;
                    cmp = compare(ptr[j], ptr[high - 1], ordering);
                    if (cmp <= 0)
                    {
                        break;
                    }
                }
                // break the loop when i and j meet
                if (i >= j)
                {
                    break;
                }
                // swap it[i], it[j]
                utl::swap(array, i, j);
            }
            utl::swap(array, i, high - 1);
            // push the larger segment onto the stack,
            // prepare to sort the smaller segment.
            if ((i - low) > (high - i))
            {
                *sp++ = low;
                *sp++ = i - 1;
                low = i + 1;
            }
            else
            {
                *sp++ = i + 1;
                *sp++ = high;
                high = i - 1;
            }
        }
        else
        {
            // insertion-sort this segment
            insertionSort(array, low, high + 1, ordering);
            // break out if there's nothing on the stack
            if (sp == stack)
            {
                break;
            }
            // pop a segment off of the stack
            high = *(--sp);
            low = *(--sp);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
remove(FwdIt& begin, const FwdIt& end, bool cmp, const Predicate* pred, bool predVal)
{
    const Object* endObject = *end;
    if (endObject == nullptr)
    {
        cmp = false;
    }
    for (;;)
    {
        Object* object = *begin;
        if (cmp)
        {
            if (utl::compare(object, endObject, nullptr) == 0)
                break;
        }
        else
        {
            if (object == endObject)
                break;
        }
        if ((pred == nullptr) || (pred->eval(object) == predVal))
        {
            begin.set(nullptr);
        }
        else
        {
            ++begin;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
reverse(const BidIt& begin, const BidIt& end)
{
    AutoPtr<BidIt> lhsPtr = begin.clone();
    AutoPtr<BidIt> rhsPtr = end.clone();
    BidIt& lhs = *lhsPtr;
    BidIt& rhs = *rhsPtr;

    // nothing to do?
    if (lhs == rhs)
    {
        return;
    }

    --rhs;
    while (lhs != rhs)
    {
        swap(lhs, rhs);
        ++lhs;
        --rhs;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serializeIn(FwdIt& out, size_t numItems, Stream& is, uint_t mode, const RunTimeClass* rtc)
{
    // while there are objects left to serialize
    while (numItems-- > 0)
    {
        // serialize an object
        AutoPtr<> object;
        if (rtc == nullptr)
        {
            object = Object::serializeInBoxed(is, mode);
            if (object == nullptr)
            {
                throw StreamSerializeEx(utl::clone(is.getNamePtr()));
            }
        }
        else
        {
            object = rtc->create();
            object->serializeIn(is, mode);
        }
        // output the object
        out.set(object.release());
        ++out;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serializeIn(FwdIt& out, size_t numItems, Stream& is, uint_t mode)
{
    // while there are objects left to serialize
    while (numItems-- > 0)
    {
        // serialize an object
        Object* object = Object::serializeInBoxed(is, mode);
        if (object == nullptr)
        {
            throw StreamSerializeEx(utl::clone(is.getNamePtr()));
        }
        // output the object
        out.set(object);
        ++out;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serializeOut(const FwdIt& begin, const FwdIt& end, Stream& os, uint_t mode, bool boxed)
{
    // create iterator
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;

    // while there are objects left to serialize
    while (it != end)
    {
        // get an object
        Object* object = *it;
        // if the object exists, serialize it
        if (object != nullptr)
        {
            if (boxed)
            {
                object->serializeOutBoxed(os, mode);
            }
            else
            {
                object->serializeOut(os, mode);
            }
        }
        ++it;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
sort(const FwdIt& begin, const FwdIt& end, const Ordering* ordering, uint_t algorithm, size_t size)
{
    switch (algorithm)
    {
    case sort_heapSort:
        heapSort(utl::cast<BidIt>(begin), utl::cast<BidIt>(end), ordering, size);
        break;
    case sort_insertionSort:
        insertionSort(begin, end, ordering, size);
        break;
    case sort_mergeSort:
        mergeSort(begin, end, ordering, size);
        break;
    case sort_quickSort:
        quickSort(begin, end, ordering, size);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
sort(Object** array, size_t begin, size_t end, const Ordering* ordering, uint_t algorithm)
{
    switch (algorithm)
    {
    case sort_heapSort:
        heapSort(array, begin, end, ordering);
        break;
    case sort_insertionSort:
        insertionSort(array, begin, end, ordering);
        break;
    case sort_mergeSort:
        mergeSort(array, begin, end, ordering);
        break;
    case sort_quickSort:
        quickSort(array, begin, end, ordering);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
symmetricDifference(const FwdIt& lhsBegin,
                    const FwdIt& lhsEnd,
                    const FwdIt& rhsBegin,
                    const FwdIt& rhsEnd,
                    FwdIt& out,
                    const Ordering* ordering,
                    bool cloning)
{
    // create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // while both iterators are valid
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // compare objects
        int cmp = compare(*lhs, *rhs, ordering);
        // if lhs < rhs, the lhs object is in the difference
        if (cmp < 0)
        {
            AutoPtr<> object(*lhs, false);
            // if cloning, output a clone of the object
            if (cloning)
            {
                object = object->clone();
            }
            out.set(object.release());
            // increment iterators
            ++out;
            ++lhs;
        }
        // else if lhs > rhs, output the rhs object
        else if (cmp > 0)
        {
            AutoPtr<> object(*rhs, false);
            // if cloning, output a clone of the object
            if (cloning)
            {
                object = object->clone();
            }
            out.set(object.release());
            // increment iterators
            ++out;
            ++rhs;
        }
        // else objects are equal so skip them
        else
        {
            ++lhs;
            ++rhs;
        }
    }

    // once we get here, lhs and/or rhs ran out of objects

    // output any left-overs
    copy(out, lhs, lhsEnd, cloning);
    copy(out, rhs, rhsEnd, cloning);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
symmetricDifference(const FwdIt& lhsBegin,
                    const FwdIt& lhsEnd,
                    const FwdIt& rhsBegin,
                    const FwdIt& rhsEnd,
                    FwdIt& lhsOut,
                    FwdIt& rhsOut,
                    const Ordering* ordering,
                    bool lhsCloning,
                    bool rhsCloning)
{
    // create iterators
    AutoPtr<FwdIt> lhsPtr = lhsBegin.clone();
    AutoPtr<FwdIt> rhsPtr = rhsBegin.clone();
    FwdIt& lhs = *lhsPtr;
    FwdIt& rhs = *rhsPtr;

    // while both iterators are valid
    while ((lhs != lhsEnd) && (rhs != rhsEnd))
    {
        // compare objects
        int cmp = compare(*lhs, *rhs, ordering);
        // if lhs < rhs, the lhs object is in the difference
        if (cmp < 0)
        {
            AutoPtr<> object(*lhs, false);
            // if cloning, output a clone of the object
            if (lhsCloning)
            {
                object = object->clone();
            }
            lhsOut.set(object.release());
            // increment iterators
            ++lhsOut;
            ++lhs;
        }
        // else if lhs > rhs, output the rhs object
        else if (cmp > 0)
        {
            AutoPtr<> object(*rhs, false);
            // if cloning, output a clone of the object
            if (rhsCloning)
            {
                object = object->clone();
            }
            rhsOut.set(object.release());
            // increment iterators
            ++rhsOut;
            ++rhs;
        }
        // else objects are equal so skip them
        else
        {
            ++lhs;
            ++rhs;
        }
    }

    // once we get here, lhs and/or rhs ran out of objects.

    // output any left-overs
    copy(lhsOut, lhs, lhsEnd, lhsCloning);
    copy(rhsOut, rhs, rhsEnd, rhsCloning);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
toString(const FwdIt& begin, const FwdIt& end, const String& sep, bool key)
{
    String res;

    // create iterator
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;

    // for each object
    bool first = true;
    while (it != end)
    {
        Object* object = *it;
        ASSERTD(object != nullptr);

        // add the separator if this isn't the first string
        if (!first)
        {
            res += sep;
        }
        // add the object's string
        if (key)
        {
            res += object->getKey().toString();
        }
        else
        {
            res += object->toString();
        }
        first = false;

        ++it;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
unique(const FwdIt& begin,
       const FwdIt& end,
       FwdIt& out,
       const Ordering* ordering,
       bool cloning,
       bool outIsSrc)
{
    // create iterator
    AutoPtr<FwdIt> itPtr = begin.clone();
    FwdIt& it = *itPtr;

    AutoPtr<> object(*it, false);
    if (object.isNull())
    {
        return;
    }
    if (cloning)
    {
        object = object->clone();
    }
    ++it;
    out.set(object.release());
    ++out;

    // while there are objects left
    while (it != end)
    {
        Object* curObject = *it;
        if (compare(*curObject, *object, ordering) == 0)
        {
            ++it;
        }
        else
        {
            object = curObject;
            if (cloning)
            {
                object = object->clone();
            }
            ++it;
            out.set(object.release());
            ++out;
        }
    }

    if (outIsSrc)
    {
        remove(out, end);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
