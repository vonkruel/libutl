#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for binary functions.
   \ingroup algorithm
*/
template <class Arg1, class Arg2, class Result> struct binaryFunction
{
    typedef Arg1 arg1_t;
    typedef Arg2 arg2_t;
    typedef Result result_t;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Subtraction function.
   \ingroup algorithm
*/
template <class T> struct subtract : public binaryFunction<T, T, T>
{
    /**
       Return the result of (lhs - rhs).
       \return lhs - rhs
       \param lhs left-hand-side parameter
       \param rhs right-hand-side parameter
    */
    T
    operator()(const T& lhs, const T& rhs) const
    {
        return (lhs - rhs);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reverse subtraction function.
   \ingroup algorithm
*/
template <class T> struct subtractReverse : public binaryFunction<T, T, T>
{
    /**
       Return the result of (rhs - lhs).
       \return rhs - lhs
       \param lhs left-hand-side parameter
       \param rhs right-hand-side parameter
    */
    T
    operator()(const T& lhs, const T& rhs) const
    {
        return (rhs - lhs);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Allows multi-key sort algorithms to be used with objects.
   \ingroup algorithm
*/
class MultiKeyObject
{
public:
    MultiKeyObject()
    {
        _object = nullptr;
        _str = nullptr;
    }

    MultiKeyObject(const Object* object, const char* str)
    {
        _object = object;
        _str = str;
    }

    /** Return the object. */
    const Object*
    getObject() const
    {
        return _object;
    }

    /** Array element access operator. */
    const char& operator[](size_t idx) const
    {
        return _str[idx];
    }

    /** Addition operator. */
    const char*
    operator+(size_t num) const
    {
        return _str + num;
    }

    // required by Vector
    bool
    operator==(const MultiKeyObject& rhs) const
    {
        ABORT();
        return 0;
    }

    // required by Vector
    int
    operator<(const MultiKeyObject& rhs) const
    {
        ABORT();
        return 0;
    }

    // required by Vector
    int
    operator>(const MultiKeyObject& rhs) const
    {
        ABORT();
        return 0;
    }

private:
    const Object* _object;
    const char* _str;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_TYPE_NO_SERIALIZE(MultiKeyObject);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/algorithms.h>
#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Multi-key insertion-sort (part of) the given sequence.  The portion to be sorted is described
   by the index range [begin,end).
   \see multiKeyQuickSort
   \ingroup algorithm
   \param array sequence of multi-key objects to be sorted
   \param cmp binary key comparison function
   \param begin begin index
   \param end end index
   \param depth current depth
*/
template <class T, class P, class C>
void
multiKeyInsertionSort(P* array, C cmp, size_t begin, size_t end, size_t depth = 0)
{
    array += begin;
    P* pi;
    P* pj;
    const T* s;
    const T* t;
    P tmp;
    const T defT = T();

    size_t n = end - begin;
    if (n <= 1)
        return;

    for (pi = array + 1; --n > 0; pi++)
    {
        for (pj = pi; pj > array; pj--)
        {
            for (s = *(pj - 1) + depth, t = *pj + depth; (cmp(*s, *t) == 0) && (cmp(*s, defT) != 0);
                 s++, t++)
                ;
            if (cmp(*s, *t) <= 0)
                break;
            tmp = *pj;
            *pj = *(pj - 1);
            *(pj - 1) = tmp;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Perform median-of-three partitioning.
   \ingroup algorithm
   \param array sequence to partition
   \param cmp binary comparison function
   \param l low index
   \param m mid index
   \param h high index
   \param depth current depth
*/
template <class T, class P, class C>
size_t
multiKeyMedianOfThree(P* array, C cmp, size_t l, size_t m, size_t h, size_t depth = 0)
{
    T vl, vm, vh;
    if (cmp(vl = array[l][depth], vm = array[m][depth]) == 0)
    {
        return l;
    }
    if ((cmp(vh = array[h][depth], vl) == 0) || (cmp(vh, vm) == 0))
    {
        return h;
    }
    if (cmp(vl, vm) < 0)
    {
        return (cmp(vm, vh) < 0) ? m : ((cmp(vl, vh) < 0) ? h : l);
    }
    else
    {
        return (cmp(vm, vh) > 0) ? m : ((cmp(vl, vh) < 0) ? l : h);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Multi-key quick-sort (part of) the given sequence.  The portion
   to be sorted is described by the index range [begin,end).
   \ingroup algorithm
   \param array sequence of multi-key objects to be sorted
   \param cmp binary key comparison function
   \param begin begin index
   \param end end index
   \param depth current depth
   \see multiKeyInsertionSort
*/
template <class T, class P, class C>
void
multiKeyQuickSort(P* array, C cmp, size_t begin, size_t end, size_t depth = 0)
{
    int r;
    size_t i, l, m, h, a, b, c, d, n;
    size_t stack[96];
    size_t* sp = stack;
    size_t bed[4][4];

    for (;;)
    {
        n = end - begin;
        while (n <= 10)
        {
            // insertion-sort the block
            if (n > 1)
            {
                multiKeyInsertionSort<T>(array, cmp, begin, end, depth);
            }

            // pop a segment off the stack -- we're done if the stack is empty
            if (sp == stack)
            {
                return;
            }
            depth = *(--sp);
            end = *(--sp);
            begin = *(--sp);
            n = end - begin;
        }

        // median-of-three (or pseudo-median of 9) partitioning
        l = begin;
        m = (begin + end) / 2;
        h = end - 1;
        if (n > 30)
        {
            size_t n8 = n / 8;
            l = multiKeyMedianOfThree<T>(array, cmp, l, l + n8, l + 2 * n8, depth);
            m = multiKeyMedianOfThree<T>(array, cmp, m - n8, m, m + n8, depth);
            h = multiKeyMedianOfThree<T>(array, cmp, h - 2 * n8, h - n8, h, depth);
        }
        m = multiKeyMedianOfThree<T>(array, cmp, l, m, h, depth);
        utl::swap(array, begin, m);
        T v = array[begin][depth];

        a = b = begin + 1;
        c = d = end - 1;

        for (;;)
        {
            while (b <= c)
            {
                r = cmp(array[b][depth], v);
                if (r > 0)
                {
                    break;
                }
                if (r == 0)
                {
                    utl::swap(array, a, b);
                    a++;
                }
                b++;
            }
            // [begin,a) =  v
            // [a, b)    <  v
            // [begin,b) <= v
            // (b > c) || ([b] > v)

            while (b <= c)
            {
                r = cmp(array[c][depth], v);
                if (r < 0)
                {
                    break;
                }
                if (r == 0)
                {
                    utl::swap(array, c, d);
                    d--;
                }
                c--;
            }
            // [c + 1,d + 1)   >  v
            // [d + 1,end)     =  v
            // [c + 1,end)     >= v
            // (b > c) || ([c] < v)

            if (b > c)
            {
                break;
            }
            // ([b] > v) && ([c] < v)
            utl::swap(array, b, c);
            // ([b] < v) && ([c] > v)
            b++;
            c--;
        }

        r = min(a - begin, b - a);
        utl::swap(array, begin, b - r, r);
        r = min(d - c, end - d - 1);
        utl::swap(array, b, end - r, r);

        // identify segments to be sorted
        r = b - a;
        bed[0][0] = begin;
        bed[0][1] = begin + r;
        bed[0][2] = depth;
        bed[0][3] = r;
        bed[1][0] = begin + r;
        bed[1][1] = end - (d - c);
        bed[1][2] = depth + 1;
        if (cmp(array[begin + r][depth], T()) == 0)
        {
            bed[1][1] = bed[1][0];
            bed[1][3] = 0;
        }
        else
        {
            bed[1][3] = bed[1][1] - bed[1][0];
        }
        r = d - c;
        bed[2][0] = end - r;
        bed[2][1] = end;
        bed[2][2] = depth;
        bed[2][3] = r;

        // bubble sort in order of increasing size
        if (bed[2][3] < bed[1][3])
        {
            memmove(bed[3], bed[1], 4 * sizeof(size_t));
            memmove(bed[1], bed[2], 4 * sizeof(size_t));
            memmove(bed[2], bed[3], 4 * sizeof(size_t));
        }
        if (bed[1][3] < bed[0][3])
        {
            memmove(bed[3], bed[0], 4 * sizeof(size_t));
            memmove(bed[0], bed[1], 4 * sizeof(size_t));
            memmove(bed[1], bed[3], 4 * sizeof(size_t));
        }
        if (bed[2][3] < bed[1][3])
        {
            memmove(bed[3], bed[1], 4 * sizeof(size_t));
            memmove(bed[1], bed[2], 4 * sizeof(size_t));
            memmove(bed[2], bed[3], 4 * sizeof(size_t));
        }

        // push larger segments onto the stack
        for (i = 2; i >= 1; i--)
        {
            if (bed[i][3] > 1)
            {
                ASSERTD(sp != (stack + 96));
                *sp++ = bed[i][0];
                *sp++ = bed[i][1];
                *sp++ = bed[i][2];
            }
        }

        // prepare to sort the smallest segment
        begin = bed[0][0];
        end = bed[0][1];
        depth = bed[0][2];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Binary search an ordered sequence for a key.
   \ingroup algorithm
   \return index of matching object (size_t_max if none found)
   \param array sequence to be searched
   \param begin begin index
   \param end end index
   \param key key to search for
   \param cmpFn comparison function
   \param findType (optional : find_first) search type (see utl::find_t)
*/
template <class T, class C>
size_t
binarySearch(
    const T* array, size_t begin, size_t end, const T& key, C cmpFn, uint_t findType = find_first)
{
    size_t low, high, mid, res;
    int cmpRes;

    low = begin;
    high = end;

    // nothing to do -- return immediately
    if ((high-- - low) == 0)
        return low;

    // while low and high make sense
    while ((low <= high) && (high != size_t_max))
    {
        // mid is half-way between low and high
        res = mid = (low + high) / 2;

        // see how the key compares against the object at mid
        cmpRes = cmpFn(key, array[mid]);
        // if key < array[mid], search the lower half
        if (cmpRes < 0)
        {
            high = mid - 1;
        }
        // else if key > begin[mid], search the upper half
        else if (cmpRes > 0)
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
        return (cmpRes != 0) ? low : res;

    // didn't find a match?
    if (cmpRes != 0)
        return size_t_max;

    // there could be multiple matches for the same key
    if (findType == find_first)
    {
        if ((res > low) && (cmpFn(key, array[res - 1]) == 0))
        {
            res = binarySearch(array, low, res, key, cmpFn, findType);
        }
    }
    else if (findType == find_last)
    {
        if ((res < high) && (cmpFn(key, array[res + 1]) == 0))
        {
            res = binarySearch(array, res + 1, high + 1, key, cmpFn, findType);
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
