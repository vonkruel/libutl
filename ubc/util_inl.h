#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Ordering.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Toggle a boolean value RAII-style.

   \ingroup utility
*/
class BoolToggle
{
public:
    /** Constructor. */
    BoolToggle(bool& b)
        : _b(b)
    {
        _b = !_b;
    }

    /** Destructor. */
    ~BoolToggle()
    {
        _b = !_b;
    }

private:
    bool& _b;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base case for utl::min() variadic template. */
template <typename T>
const T&
min(const T& v)
{
    return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the smallest value among two or more provided values of the same type.

   \ingroup utility
   \return the smallest of the provided values
   \param v0 first object
   \param v1 second object
   \param args additional objects (if any)
*/
template <typename T, typename... R>
const T&
min(const T& v0, const T& v1, const R&... args)
{
    return utl::min((v1 < v0) ? v1 : v0, args...);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base case for utl::max() variadic template. */
template <typename T>
const T&
max(const T& v)
{
    return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the largest value among two or more provided values of the same type.

   \ingroup utility
   \return the largest of the provided values
   \param v0 first object
   \param v1 second object
   \param args additional objects (if any)
*/
template <typename T, typename... R>
const T&
max(const T& v0, const T& v1, const R&... args)
{
    return utl::max((v1 < v0) ? v0 : v1, args...);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the smallest power of 2 which is >= \b n.

   \ingroup math
*/
inline uint32_t
nextPow2(uint32_t n)
{
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return n + 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the smallest power of 2 which is >= \b n.

   \ingroup math
*/
inline uint64_t
nextPow2(uint64_t n)
{
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    n |= (n >> 32);
    return n + 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the smallest number \b n s.t. \b n is a multiple of \b x, and \b n >= \b target.

   \ingroup math
*/
template <class T>
inline T
nextMultiple(const T& x, const T& target)
{
    ASSERTD(x != 0);
    T rem = target % x;
    return (rem == 0) ? target : target + (x - rem);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the smallest number \b n s.t. \b n is a multiple of \b x (a power of 2), and
   \b n >= \b target.

   \ingroup math
*/
inline uint32_t
nextMultipleOfPow2(uint32_t x, uint32_t target)
{
    ASSERTD(x != 0);
    ASSERTD(x == nextPow2(x));
    return (target + x - 1) & ~(x - 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Return the smallest number \b n s.t. \b n is a multiple of \b x (a power of 2), and
   \b n >= \b target.

   \ingroup math
*/
inline uint64_t
nextMultipleOfPow2(uint64_t x, uint64_t target)
{
    ASSERTD(x != 0);
    ASSERTD(x == nextPow2(x));
    return (target + x - 1) & ~(x - 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compute the allocated size of an object.

   \ingroup utility
   \return allocated size of object (0 if object is null)
   \param object object to invoke utl::Object::allocatedSize() on
*/
inline size_t
allocatedSize(const Object* object)
{
    if (object == nullptr)
        return 0;
    return object->allocatedSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Grow an array.

   \ingroup utility
   \param array array to grow
   \param arraySize array size
   \param minSize grow to at least this size
   \param growthIncrement (optional) growth increment
   \param defVal (optional) default value for new objects
*/
template <typename T>
void
arrayGrow(T*& array,
          size_t& arraySize,
          size_t minSize = size_t_max,
          size_t growthIncrement = size_t_max,
          const T* defVal = nullptr)
{
    // by default, just make room for one more object
    if (minSize == size_t_max)
        minSize = (arraySize + 1);

    // allocation is already big enough -> do nothing
    if (arraySize >= minSize)
        return;

    // compute new allocation size
    size_t newSize;
    if (growthIncrement == size_t_max)
    {
        newSize = nextPow2(minSize);
    }
    else
    {
        ASSERTD(growthIncrement == nextPow2(growthIncrement));
        newSize = nextMultipleOfPow2(growthIncrement, minSize);
    }
    ASSERTD(newSize >= minSize);

    // copy objects from old allocation to new
    T* newArray = new T[newSize];
    if (std::is_trivially_copyable<T>())
    {
        memcpy(newArray, array, arraySize * sizeof(T));
    }
    else
    {
        T* lhsPtr = newArray;
        T* rhsPtr = array;
        T* rhsLim = array + arraySize;
        for (; rhsPtr != rhsLim; ++lhsPtr, ++rhsPtr)
        {
            *lhsPtr = std::move(*rhsPtr);
        }
    }

    // if caller wants to initialize the objects we added on, do that
    if (defVal != nullptr)
    {
        T* lhsPtr = newArray + arraySize;
        T* lhsLim = newArray + newSize;
        for (; lhsPtr != lhsLim; ++lhsPtr)
        {
            *lhsPtr = *defVal;
        }
    }

    // clean up the old allocation, set array,arraySize for caller
    delete[] array;
    array = newArray;
    arraySize = newSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Grow an array.

   \ingroup utility
   \param array array to grow
   \param arrayPtr index into array
   \param arrayLim end of array
   \param minSize minimum size
   \param growthIncrement limit growth
*/
template <typename T>
void
arrayGrow(T*& array,
          T*& arrayPtr,
          T*& arrayLim,
          size_t minSize = size_t_max,
          size_t growthIncrement = size_t_max)
{
    size_t arraySize = arrayLim - array;
    size_t arrayIdx = arrayPtr - array;
    arrayGrow(array, arraySize, minSize, growthIncrement, static_cast<T*>(nullptr));
    arrayPtr = array + arrayIdx;
    arrayLim = array + arraySize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Safely cast an object to the desired type.
   In a DEBUG build, trigger an assertion failure if the cast cannot be done.

   \ingroup utility
*/
template <typename T>
inline const T&
cast(const utl::Object& object)
{
    ASSERTD(object.isA(T));
    return static_cast<const T&>(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Safely cast an object to the desired type.
   In a DEBUG build, trigger an assertion failure if the cast cannot be done.

   \ingroup utility
*/
template <typename T>
inline T&
cast(utl::Object& object)
{
    ASSERTD(object.isA(T));
    return static_cast<T&>(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Safely cast an object pointer to the desired type.
   In a DEBUG build, trigger an assertion failure if the cast cannot be done.

   \ingroup utility
*/
template <typename T>
inline const T*
cast(const utl::Object* object)
{
    ASSERTD((object == nullptr) || object->isA(T));
    return static_cast<const T*>(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Safely cast an object pointer to the desired type.
   In a DEBUG build, trigger an assertion failure if the cast cannot be done.

   \ingroup utility
*/
template <typename T>
inline T*
cast(utl::Object* object)
{
    ASSERTD((object == nullptr) || object->isA(T));
    return static_cast<T*>(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Safely cast an object reference to the desired type (remove const-qualifier).
   In a DEBUG build, trigger an assertion failure if the cast cannot be done.

   \ingroup utility
*/
template <typename T>
inline T&
cast_const(const utl::Object& object)
{
    ASSERTD(object.isA(T));
    return static_cast<T&>(const_cast<utl::Object&>(object));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Safely cast an object pointer to the desired type (remove const-qualifier).
   In a DEBUG build, trigger an assertion failure if the cast cannot be done.

   \ingroup utility
*/
template <typename T>
inline T*
cast_const(const utl::Object* object)
{
    ASSERTD((object == nullptr) || object->isA(T));
    return static_cast<T*>(const_cast<utl::Object*>(object));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a clone of the given object.

   \ingroup utility
   \return newly created copy (nullptr if object == nullptr)
   \param object object to clone
*/
template <typename T>
T*
clone(const T* object)
{
    if (object == nullptr)
        return nullptr;
    return object->clone();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two objects of the same type.

   \ingroup utility
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhs left-hand-side object
   \param rhs right-hand-side object
*/
template <typename T>
inline int
compare(const T& lhs, const T& rhs)
{
    if (lhs < rhs)
        return -1;
    if (lhs == rhs)
        return 0;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two objects.  If an Ordering is given it will be used, otherwise Object::compare() will
   perform the comparison.

   \ingroup utility
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhs left-hand-side object
   \param rhs right-hand-side object
   \param ordering (optional) ordering
*/
inline int
compare(const Object* lhs, const Object* rhs, const Ordering* ordering = nullptr)
{
    ASSERTD(lhs != nullptr);
    ASSERTD(rhs != nullptr);
    if (lhs == rhs)
        return 0;
    if (ordering == nullptr)
        return lhs->compare(*rhs);
    return ordering->cmp(lhs, rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two objects.  This function is just like compare(), but it can deal with one or both
   of the object pointers being null.  If exactly one object pointer is null, the object
   referenced by the other pointer is considered greater.  If both object pointers are null,
   this function returns 0.  Otherwise, its behavior is identical to compare().

   \ingroup utility
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhs left-hand-side object
   \param rhs right-hand-side object
   \param ordering (optional) ordering to use
*/
inline int
compareNullable(const Object* lhs, const Object* rhs, const Ordering* ordering = nullptr)
{
    if (lhs == rhs)
        return 0;
    if ((lhs != nullptr) && (rhs != nullptr))
    {
        if (ordering == nullptr)
            return lhs->compare(*rhs);
        return ordering->cmp(lhs, rhs);
    }
    if (lhs == nullptr)
        return -1;
    // rhs must be nullptr
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two same-length arrays of objects of the same type.

   \ingroup utility
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhs left-hand-side sequence
   \param rhs right-hand-side sequence
   \param len lhs and rhs sequence length
*/
template <typename T>
int
compare(const T* lhs, const T* rhs, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++)
    {
        int res = utl::compare(lhs[i], rhs[i]);
        if (res != 0)
        {
            return res;
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two arrays of objects of the same type.

   \ingroup utility
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
   \param lhs left-hand-side sequence
   \param lhsLen lhs sequence length
   \param rhs right-hand-side sequence
   \param rhsLen rhs sequence length
*/
template <typename T>
int
compare(const T* lhs, size_t lhsLen, const T* rhs, size_t rhsLen)
{
    size_t i;
    size_t minLen = utl::min(lhsLen, rhsLen);
    for (i = 0; i < minLen; i++)
    {
        int res = utl::compare(lhs[i], rhs[i]);
        if (res != 0)
        {
            return res;
        }
    }
    if (lhsLen < minLen)
        return -1;
    else if (rhsLen < minLen)
        return 1;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two sequences.

   \ingroup utility
   \param lhsBegin begin of lhs sequence
   \param lhsEnd end of lhs sequence
   \param rhsBegin begin of rhs sequence
   \param rhsEnd end of rhs sequence
   \param comp comparison function
   \return < 0 if lhs < rhs, 0 if lhs = rhs, > 0 if lhs > rhs
*/
template <typename InputCont1, typename InputCont2>
int
compare(typename InputCont1::const_iterator lhsBegin,
        typename InputCont1::const_iterator lhsEnd,
        typename InputCont2::const_iterator rhsBegin,
        typename InputCont2::const_iterator rhsEnd,
        std::function<int(typename InputCont1::value_type, typename InputCont2::value_type)> comp)
{
    auto lhsIt = lhsBegin;
    auto rhsIt = rhsBegin;
    for (;; ++lhsIt, ++rhsIt)
    {
        bool lhsDone = (lhsIt == lhsEnd);
        bool rhsDone = (rhsIt == rhsEnd);
        if (lhsDone && rhsDone)
            return 0;
        if (lhsDone)
            return -1;
        if (rhsDone)
            return 1;
        int res = comp(*lhsIt, *rhsIt);
        if (res != 0)
            return res;
    }
    ABORT();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the given comparison result is consistent with the given comparison operator.

   \ingroup utility
   \return true if cmpRes is consistent with cmpOp, false otherwise
   \param cmpRes comparison result (see compare(), cmp(), etc.)
   \param cmpOp comparison operator (see utl::cmp_t)
*/
inline bool
comparisonResultMatchesOp(int cmpRes, uint_t cmpOp)
{
    if (cmpRes < 0)
    {
        return (cmpOp == cmp_lessThan);
    }
    if (cmpRes == 0)
    {
        return ((cmpOp != cmp_lessThan) && (cmpOp != cmp_greaterThan));
    }
    return (cmpOp == cmp_greaterThan);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Object comparison evaluator.

   \ingroup utility
   \return newly created copy (nullptr if object == nullptr)
   \param ordering (optional) ordering (\em not cloned)
*/
template <typename T> struct comparisonFunctor
{
    comparisonFunctor(const Ordering* ordering = nullptr)
        : _ordering(ordering)
    {
    }

    int
    operator()(const Object* lhs, const Object* rhs) const
    {
        return utl::compare(lhs, rhs, _ordering);
    }

private:
    const Ordering* _ordering;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Object comparison predicate evaluator that returns the value of (lhs == rhs).

   \ingroup utility
   \return newly created copy (nullptr if object == nullptr)
   \param ordering (optional) ordering (\em not cloned)
*/
template <typename T = utl::Object> struct equals
{
    equals(const Ordering* ordering = nullptr)
        : _ordering(ordering)
    {
    }

    bool
    operator()(const T* lhs, const T* rhs) const
    {
        return (utl::compare(lhs, rhs, _ordering) == 0);
    }

private:
    const Ordering* _ordering;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Object comparison predicate evaluator that returns the value of (lhs < rhs).

   \ingroup utility
   \return newly created copy (nullptr if object == nullptr)
   \param ordering (optional) ordering (\em not cloned)
*/
template <typename T = utl::Object> struct lessThan
{
    lessThan(const Ordering* ordering = nullptr)
        : _ordering(ordering)
    {
    }

    bool
    operator()(const T* lhs, const T* rhs) const
    {
        return (utl::compare(lhs, rhs, _ordering) < 0);
    }

private:
    const Ordering* _ordering;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Copy one array of objects to another.

   \ingroup utility
   \param dest destination sequence
   \param src source sequence
   \param len source sequence length
*/
template <typename T>
void
copy(T* dest, const T* src, size_t len)
{
    T* destPtr = dest;
    const T* srcPtr = src;
    const T* srcLim = src + len;
    for (; srcPtr != srcLim; ++srcPtr, ++destPtr)
    {
        *destPtr = *srcPtr;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether \b x is evenly divisible by \b n.

   \ingroup math
   \return true if \b x divisible by \b n, false otherwise
*/
template <class T>
inline bool
isDivisibleBy(const T& x, const T& n)
{
    return (((x / n) * n) == x);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Round the given value \b v up to the nearest multiple of \b m.

   \ingroup utility
   \return if v is already a multiple of m, then return v, otherwise return v + (v \% m)
   \param v value to round up
   \param m rounding factor
*/
template <typename T>
inline T
roundUp(const T& v, const T& m)
{
    T vModm = v % m;
    if (vModm == 0)
        return v;
    return (v - vModm + m);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Round the given value \b v down to the nearest multiple of \b m.

   \ingroup utility
   \return if v is already a multiple of m, then return v, otherwise return v - (v \% m)
   \param v value to round down
   \param m rounding factor
*/
template <typename T>
inline T
roundDown(const T& v, const T& m)
{
    return v - (v % m);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an array.

   \ingroup utility
   \param array array to serialize to/from stream
   \param len array length
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
template <typename T>
void
serialize(T* array, size_t len, Stream& stream, uint_t io, uint_t mode = ser_default)
{
    if (mode & ser_default)
        mode = getSerializeMode();
    size_t i;
    for (i = 0; i < len; i++)
    {
        serialize(array[i], stream, io, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an object (boxed).

   \ingroup utility
   \param object object to serialize
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
template <typename T>
inline void
serialize(T*& object, Stream& stream, uint_t io, uint_t mode = ser_default)
{
    serializeBoxed(object, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize an object (non-boxed).

   \ingroup utility
   \param object object to serialize
   \param stream stream to serialize to/from
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
template <typename T>
inline void
serialize(T& object, Stream& stream, uint_t io, uint_t mode = ser_default)
{
    object.serialize(stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a nullable object.

   \ingroup utility
   \param object object pointer (by reference)
   \param stream stream to serialize to/from
   \param io io_rd or io_wr
   \param mode (optional : ser_default) serialization mode
*/
template <typename T>
void
serializeNullable(T*& object, Stream& stream, uint_t io, uint_t mode = ser_default)
{
    if (io == io_rd)
    {
        delete object;
        object = nullptr;
        Object* _object = Object::serializeInNullable(stream, mode);
        object = utl::cast<T>(_object);
    }
    else
    {
        Object::serializeOutNullable(object, stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a boxed object.

   \ingroup utility
   \param object object pointer (by reference)
   \param stream stream to serialize to/from
   \param io io_rd or io_wr
   \param mode (optional : ser_default) serialization mode
*/
template <typename T>
void
serializeBoxed(T*& object, Stream& stream, uint_t io, uint_t mode = ser_default)
{
    if (io == io_rd)
    {
        delete object;
        object = nullptr;
        Object* _object = Object::serializeInBoxed(stream, mode);
        object = utl::cast<T>(_object);
    }
    else
    {
        object->serializeOutBoxed(stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reverse the byte ordering of a 16-bit unsigned integer.

   \ingroup utility
   \return 16-bit integer with bytes reversed
   \param n the integer to be byte-reversed
*/
inline uint16_t
reverseBytes(uint16_t n)
{
    return ((n & 0xff00) >> 8) | ((n & 0x00ff) << 8);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reverse the byte ordering of a 32-bit unsigned integer.

   \ingroup utility
   \return 32-bit integer with bytes reversed
   \param n the integer to be byte-reversed
*/
inline uint32_t
reverseBytes(uint32_t n)
{
    n = ((n & 0xffff0000) >> 16) | ((n & 0x0000ffff) << 16);
    n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);
    return n;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reverse the byte ordering of a 64-bit unsigned integer.

   \ingroup utility
   \return 64-bit integer with bytes reversed
   \param n the integer to be byte-reversed
*/
inline uint64_t
reverseBytes(uint64_t n)
{
    n = ((n & 0xffffffff00000000) >> 32) | ((n & 0x00000000ffffffff) << 32);
    n = ((n & 0xffff0000ffff0000) >> 16) | ((n & 0x0000ffff0000ffff) << 16);
    n = ((n & 0xff00ff00ff00ff00) >> 8) | ((n & 0x00ff00ff00ff00ff) << 8);
    return n;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Swap two elements of the given array.

   \ingroup utility
   \param array given array
   \param lhsIdx index of lhs element
   \param rhsIdx index of rhs element
*/
template <typename T>
inline void
swap(T* array, size_t lhsIdx, size_t rhsIdx)
{
    T tmp = std::move(array[lhsIdx]);
    array[lhsIdx] = std::move(array[rhsIdx]);
    array[rhsIdx] = std::move(tmp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Swap two sections of the given array.

   \ingroup utility
   \param array given array
   \param lhsIdx index of lhs segment
   \param rhsIdx index of rhs segment
   \param size size of segments to be swapped
*/
template <typename T>
void
swap(T* array, size_t lhsIdx, size_t rhsIdx, size_t size)
{
    T* pi = array + lhsIdx;
    T* pj = array + rhsIdx;
    T* piLim = array + lhsIdx + size;
    T tmp;
    for (; pi != piLim; ++pi, ++pj)
    {
        tmp = std::move(*pi);
        *pi = std::move(*pj);
        *pj = std::move(tmp);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Fake read/write access to all memory.
   You can invoke clobber() to force the compiler to assume prior writes will be observed.

   \ingroup utility
*/
inline void
clobber()
{
    // volatile -> instruct the optimizer to leave this assembly code alone & not analyze it
    // memory   -> assume that global memory is "clobbered" (affected)
#if UTL_CC == UTL_CC_MSVC
    utl::abort("utl::clobber() is not implemented for MSVC.");
#else
    asm volatile("" : : : "memory");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Fake read/write access to all memory, and require the object at address ptr to exist.
   The compiler can't optimize away *ptr (regardless of size or type) if its address will be used.

   \ingroup utility
   \param ptr the address of an object we want to not be optimized away
*/
inline void
escape(void* ptr)
{
    // volatile -> instruct the optimizer to leave this assembly code alone & not analyze it
    // "g"(ptr) -> the (empty) assembly code uses ptr as an input
    // memory   -> assume that global memory is "clobbered" (affected)
#if UTL_CC == UTL_CC_MSVC
    utl::abort("utl::escape() is not implemented for MSVC.");
#else
    asm volatile("" : : "g"(ptr) : "memory");
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Convert size in kilobytes to size in bytes.

   \ingroup utility
   \return number of bytes
   \param n number of kilobytes
*/
constexpr size_t
KB(size_t n)
{
    return n * (size_t)1024;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Convert size in megabytes to size in bytes.

   \ingroup utility
   \return number of bytes
   \param n number of kilobytes
*/
constexpr size_t
MB(size_t n)
{
    return n * (size_t)1024 * (size_t)1024;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Convert size in gigabytes to size in bytes.

   \ingroup utility
   \return number of bytes
   \param n number of kilobytes
*/
constexpr size_t
GB(size_t n)
{
    return n * (size_t)1024 * (size_t)1024 * (size_t)1024;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
