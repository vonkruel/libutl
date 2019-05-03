#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Predicate.h>
#include <libutl/RandIt.h>
#include <libutl/Stream.h>
#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Specifies a sort algorithm.
   \ingroup algorithm
*/
enum sort_t
{
    sort_heapSort,      /**< heap sort */
    sort_insertionSort, /**< insertion sort */
    sort_mergeSort,     /**< merge sort */
    sort_quickSort,     /**< quick sort */
    sort_none           /**< do not sort */
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Binary search an ordered sequence for a key.

   \ingroup algorithm
   \return index of matching object (size_t_max if none found)
   \param begin begin iterator
   \param end end iterator
   \param key key to search for
   \param ordering (optional) ordering
   \param findType (optional : find_first) search type (see utl::find_t)
*/
size_t binarySearch(const RandIt& begin,
                    const RandIt& end,
                    const Object& key,
                    const Ordering* ordering = nullptr,
                    uint_t findType = find_first);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Where lhs and rhs intersect, make the lhs version of the object equal to the rhs version
   ("clobber" it with the rhs version).

   \ingroup algorithm
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param ordering (optional) ordering
*/
void clobber(const FwdIt& lhsBegin,
             const FwdIt& lhsEnd,
             const FwdIt& rhsBegin,
             const FwdIt& rhsEnd,
             const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two sorted sequences.  Objects in each sequence are compared until they are unequal or
   the end of one or both sequences is reached.

   \ingroup algorithm
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param ordering (optional) ordering
*/
int compare(const FwdIt& lhsBegin,
            const FwdIt& lhsEnd,
            const FwdIt& rhsBegin,
            const FwdIt& rhsEnd,
            const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Copy objects from one sequence into another.

   \ingroup algorithm
   \param out output iterator
   \param begin begin of source sequence
   \param end end of source sequence
   \param cloning (optional : false) clone source objects?
   \param pred (optional) predicate : require (pred(sourceObject) == predVal)
   \param predVal (optional : true) predicate value
*/
void copy(FwdIt& out,
          const FwdIt& begin,
          const FwdIt& end,
          bool cloning = false,
          const Predicate* pred = nullptr,
          bool predVal = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Copy objects from one sequence to another.

   \ingroup algorithm
   \param out output vector
   \param begin begin of source sequence
   \param end end of source sequence
   \param cloning (optional : false) clone source objects?
   \param pred (optional) predicate : require (pred(sourceObject) == predVal)
   \param predVal (optional : true) predicate value
*/
void copy(Object** out,
          const FwdIt& begin,
          const FwdIt& end,
          bool cloning = false,
          const Predicate* pred = nullptr,
          bool predVal = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Copy objects from a Vector to a sequence.

   \ingroup algorithm
   \param out output sequence
   \param in source vector
   \param cloning (optional : false) clone source objects?
   \param pred (optional) predicate : require (pred(sourceObject) == predVal)
   \param predVal (optional : true) predicate value
*/
void copy(FwdIt& out,
          const Vector<Object*>& in,
          bool cloning = false,
          const Predicate* pred = nullptr,
          bool predVal = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Count objects that satisfy a Predicate.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param pred (optional) predicate : require (pred(object) == predVal)
   \param predVal (optional : true) predicate value
*/
size_t
count(const FwdIt& begin, const FwdIt& end, const Predicate* pred = nullptr, bool predVal = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the difference between two sorted sequences.  The difference consists of objects that
   exist in the lhs sequence but not the rhs sequence.

   \see symmetricDifference
   \ingroup algorithm
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param out output sequence
   \param ordering (optional) ordering
   \param cloning (optional : false) clone source objects?
   \param outIsLHS (optional : false) output sequence == lhs sequence?
*/
void difference(const FwdIt& lhsBegin,
                const FwdIt& lhsEnd,
                const FwdIt& rhsBegin,
                const FwdIt& rhsEnd,
                FwdIt& out,
                const Ordering* ordering = nullptr,
                bool cloning = false,
                bool outIsLHS = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Dump objects to the given stream (with Object::dump()).

   \see Object::getKey
   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param os output stream
   \param level (optional) verbosity
   \param key (optional : false) dump keys instead of objects?
   \param printClassName (optional : false) print each object's class name?
   \param indent (optional : 0) indent each object?
   \param separator (optional) string to visually separate objects
*/
void dump(const FwdIt& begin,
          const FwdIt& end,
          Stream& os,
          uint_t level = uint_t_max,
          bool key = false,
          bool printClassName = false,
          uint_t indent = 0,
          const char* separator = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Heap-sort a sequence.

   \see Heap
   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
   \param size (optional) size of sequence
*/
void heapSort(const BidIt& begin,
              const BidIt& end,
              const Ordering* ordering = nullptr,
              size_t size = size_t_max);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Heap-sort a sequence.

   \see Heap
   \ingroup algorithm
   \param array sequence to be sorted
   \param begin index of first object
   \param end index of last object + 1
   \param ordering (optional) ordering
*/
void heapSort(Object** array, size_t begin, size_t end, const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Insertion-sort a sequence.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
   \param size (optional) size of sequence
*/
void insertionSort(const FwdIt& begin,
                   const FwdIt& end,
                   const Ordering* ordering = nullptr,
                   size_t size = size_t_max);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Insertion-sort (part of) an array.

   \ingroup algorithm
   \param array sequence to be sorted
   \param begin index of first object
   \param end index of last object + 1
   \param ordering (optional) ordering
*/
void insertionSort(Object** array, size_t begin, size_t end, const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the intersection of two sorted sequences.  The intersection consists of objects that
   exist in both of the sorted sequences.

   \ingroup algorithm
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param out output sequence
   \param ordering (optional) ordering
   \param cloning (optional : false) clone source objects?
   \param multiSet (optional : false) can an rhs object have multiple
                   matching lhs objects?
   \param outIsLHS (optional : false) output sequence == lhs sequence?
*/
void intersect(const FwdIt& lhsBegin,
               const FwdIt& lhsEnd,
               const FwdIt& rhsBegin,
               const FwdIt& rhsEnd,
               FwdIt& out,
               const Ordering* ordering = nullptr,
               bool cloning = false,
               bool multiSet = false,
               bool outIsLHS = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the cardinality of the intersection of two sorted sequences.

   \ingroup algorithm
   \return number of objects that exist in both sequences
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param ordering (optional) ordering
   \param multiSet (optional : false) can an rhs object have multiple
                   matching lhs objects?
*/
size_t intersectCard(const FwdIt& lhsBegin,
                     const FwdIt& lhsEnd,
                     const FwdIt& rhsBegin,
                     const FwdIt& rhsEnd,
                     const Ordering* ordering = nullptr,
                     bool multiSet = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the intersection of two sorted sequences is non-empty.

   \ingroup algorithm
   \return true if intersection exists, false otherwise
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param ordering (optional) ordering
*/
bool intersects(const FwdIt& lhsBegin,
                const FwdIt& lhsEnd,
                const FwdIt& rhsBegin,
                const FwdIt& rhsEnd,
                const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the given sequence is sorted.

   \ingroup algorithm
   \return true if the sequence is sorted, false otherwise
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
*/
bool isSorted(const FwdIt& begin, const FwdIt& end, const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the lhs sorted sequence is a subset of the rhs sorted sequence.

   \ingroup algorithm
   \return true if lhs is a subset of rhs, false otherwise
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param ordering (optional) ordering
*/
bool isSubSet(const FwdIt& lhsBegin,
              const FwdIt& lhsEnd,
              const FwdIt& rhsBegin,
              const FwdIt& rhsEnd,
              const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the lhs sorted sequence is a superset of the rhs sorted sequence.

   \ingroup algorithm
   \return true if lhs is a superset of rhs, false otherwise
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param ordering (optional) ordering
*/
bool isSuperSet(const FwdIt& lhsBegin,
                const FwdIt& lhsEnd,
                const FwdIt& rhsBegin,
                const FwdIt& rhsEnd,
                const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Linear search a sequence for a key.

   \ingroup algorithm
   \param begin begin iterator
   \param end end iterator
   \param key key to search for
   \param out output iterator
   \param ordering (optional) ordering
   \param findType (optional : find_first) search type (see utl::find_t)
*/
void linearSearch(const FwdIt& begin,
                  const FwdIt& end,
                  const Object& key,
                  FwdIt& out,
                  const Ordering* ordering = nullptr,
                  uint_t findType = find_first);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Linear search a sorted sequence for a key.  Since the sequence is known to be sorted, an
   unsuccessful search can terminate more quickly.

   \ingroup algorithm
   \param begin begin iterator
   \param end end iterator
   \param key key to search for
   \param out output iterator
   \param ordering (optional) ordering
   \param findType (optional : find_first) search type (see utl::find_t)
*/
void linearSearchSorted(const FwdIt& begin,
                        const FwdIt& end,
                        const Object& key,
                        FwdIt& out,
                        const Ordering* ordering = nullptr,
                        uint_t findType = find_first);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Merge two sorted sequences into a single sorted sequence.

   \ingroup algorithm
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param out output iterator
   \param ordering (optional) ordering
   \param cloning (optional : false) clone source objects?
*/
void merge(const FwdIt& lhsBegin,
           const FwdIt& lhsEnd,
           const FwdIt& rhsBegin,
           const FwdIt& rhsEnd,
           FwdIt& out,
           const Ordering* ordering = nullptr,
           bool cloning = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Merge-sort a sequence.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
   \param size (optional) size of sequence
*/
void mergeSort(const FwdIt& begin,
               const FwdIt& end,
               const Ordering* ordering = nullptr,
               size_t size = size_t_max);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Merge-sort (part of) an array.

   \ingroup algorithm
   \param array sequence to be sorted
   \param begin index of first object
   \param end index of last object + 1
   \param ordering (optional) ordering
*/
void mergeSort(Object** array, size_t begin, size_t end, const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Multi-key quick-sort a sequence.  Every object in the sequence must be a String (\b key = false)
   or have a String as its key (\b key = true).

   \see Object::getKey
   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param key (optional : true) objects' strings are their keys?
   \param reverse (optional : false) reverse sort?
   \param size (optional) size of sequence
*/
void multiKeyQuickSort(const FwdIt& begin,
                       const FwdIt& end,
                       bool key = true,
                       bool reverse = false,
                       size_t size = size_t_max);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Quick-sort a sequence.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
   \param size (optional) size of sequence
*/
void quickSort(const FwdIt& begin,
               const FwdIt& end,
               const Ordering* ordering = nullptr,
               size_t size = size_t_max);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Quick-sort (part of) a sequence.

   \ingroup algorithm
   \param array sequence to be sorted
   \param begin index of first object
   \param end index of last object + 1
   \param ordering (optional) ordering
*/
void quickSort(Object** array, size_t begin, size_t end, const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Remove objects from a sequence.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param cmp (optional : false) compare objects (via Object::compare())?
   \param pred (optional) predicate : require (pred(object) == predVal)
   \param predVal (optional : true) predicate value
*/
void remove(FwdIt& begin,
            const FwdIt& end,
            bool cmp = false,
            const Predicate* pred = nullptr,
            bool predVal = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reverse a sequence.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
*/
void reverse(const BidIt& begin, const BidIt& end);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize objects from an input stream.  If a RunTimeClass is specified, it is assumed that the
   objects were serialized "unboxed" (without their type information), and all objects are assumed
   to be of the given class.

   \see Object::serialize
   \ingroup algorithm
   \param out output iterator
   \param numItems number of objects to serialize
   \param is input stream
   \param mode (optional : ser_default) serialization mode
   \param rtc (optional) run-time class for type being serialized
*/
void serializeIn(FwdIt& out,
                 size_t numItems,
                 Stream& is,
                 uint_t mode = ser_default,
                 const RunTimeClass* rtc = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize objects to an output stream.

   \see Object::serialize
   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param os output stream
   \param mode (optional : ser_default) serialization mode
   \param boxed (optional : true) serialize with type information?
*/
void serializeOut(
    const FwdIt& begin, const FwdIt& end, Stream& os, uint_t mode = ser_default, bool boxed = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Sort a sequence using a given algorithm.

   \ingroup algorithm
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
   \param algorithm (optional : sort_quickSort) see utl::sort_t
   \param size (optional) size of sequence
*/
void sort(const FwdIt& begin,
          const FwdIt& end,
          const Ordering* ordering = nullptr,
          uint_t algorithm = sort_quickSort,
          size_t size = size_t_max);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Sort a sequence using a given algorithm.

   \ingroup algorithm
   \param array sequence to be sorted
   \param begin begin of sequence
   \param end end of sequence
   \param ordering (optional) ordering
   \param algorithm (optional : sort_quickSort) see utl::sort_t
*/
void sort(Object** array,
          size_t begin,
          size_t end,
          const Ordering* ordering = nullptr,
          uint_t algorithm = sort_quickSort);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Swap two objects.

   \ingroup algorithm
   \param lhs iterator for lhs object
   \param rhs iterator for rhs object
*/
inline void
swap(FwdIt& lhs, FwdIt& rhs)
{
    Object* tmp = *lhs;
    lhs.set(*rhs);
    rhs.set(tmp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Swap two objects in a random-access sequence.

   \ingroup algorithm
   \param it iterator for sequence
   \param lhsIdx index of lhs object
   \param rhsIdx index of rhs object
*/
inline void
swap(RandIt& it, size_t lhsIdx, size_t rhsIdx)
{
    Object* tmp = it[lhsIdx];
    it.set(lhsIdx, it.get(rhsIdx));
    it.set(rhsIdx, tmp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the symmetric difference of two sorted sequences.  The symmetric difference consists
   of objects that exist in exactly one of the sorted sequences.

   \ingroup algorithm
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param out output iterator
   \param ordering (optional) ordering
   \param cloning (optional : false) clone source objects?
*/
void symmetricDifference(const FwdIt& lhsBegin,
                         const FwdIt& lhsEnd,
                         const FwdIt& rhsBegin,
                         const FwdIt& rhsEnd,
                         FwdIt& out,
                         const Ordering* ordering = nullptr,
                         bool cloning = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the symmetric difference of two sorted sequences.  The symmetric difference consists
   of objects that exist in exactly one of the sorted sequences.

   \ingroup algorithm
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param lhsOut output iterator for lhs objects
   \param rhsOut output iterator for rhs objects
   \param ordering (optional) ordering
   \param lhsCloning (optional : false) clone lhs objects?
   \param rhsCloning (optional : false) clone rhs objects?
*/
void symmetricDifference(const FwdIt& lhsBegin,
                         const FwdIt& lhsEnd,
                         const FwdIt& rhsBegin,
                         const FwdIt& rhsEnd,
                         FwdIt& lhsOut,
                         FwdIt& rhsOut,
                         const Ordering* ordering = nullptr,
                         bool lhsCloning = false,
                         bool rhsCloning = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Obtain a string representation of a sequence (via Object::toString()).

   \see Object::getKey
   \ingroup algorithm
   \param begin begin iterator
   \param end end iterator
   \param sep separator (e.g. ", ")
   \param key (optional : false) invoke Object::toString() on object keys?
*/
String toString(const FwdIt& begin, const FwdIt& end, const String& sep, bool key = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Remove duplicate objects from a sorted sequence.

   \ingroup algorithm
   \param begin begin iterator
   \param end end iterator
   \param out output iterator
   \param ordering (optional) ordering
   \param cloning (optional : false) clone source objects?
   \param outIsSrc (optional : false) output sequence == source sequence?
*/
void unique(const FwdIt& begin,
            const FwdIt& end,
            FwdIt& out,
            const Ordering* ordering = nullptr,
            bool cloning = false,
            bool outIsSrc = false);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/algorithms_inl.h>
