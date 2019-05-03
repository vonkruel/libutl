#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Object.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Boyer-Moore string search.

   The Boyer-Moore string search algorithm is extremely fast, but requires some initial setup.
   Due to the cost associated with the initial setup, you should use BoyerMooreSearch in the
   following situations:

   \arg the string to be searched is long
   \arg many strings are to be searched for a single string

   \author Adam McKee
   \ingroup string
   \see String::findBM
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = char> class BoyerMooreSearch : public Object, protected FlagsMI
{
    UTL_CLASS_DECL_TPL(BoyerMooreSearch, T, Object);

public:
    /**
       Constructor.
       \param owner pattern ownership flag
       \param pat search pattern
       \param patLen pattern length
       \param A (optional) alphabet size
    */
    BoyerMooreSearch(bool owner, const T* pat, size_t patLen, uint_t A = uint_t_max)
    {
        init(A);
        set(owner, pat, patLen);
    }

    /**
       Find the first occurrence of the search pattern in the given string.
       \return pointer to first occurrence (nullptr if none)
       \param str search string
       \param strLen search string length
    */
    const T* find(const T* str, size_t strLen) const;

    /**
       Set a new pattern.
       \param owner pattern ownership flag
       \param pat search pattern
       \param patLen pattern length
    */
    void set(bool owner, const T* pat, size_t patLen);

    /** Get the length of the pattern. */
    size_t
    length() const
    {
        return _patLen;
    }

    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Get the \b owner flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

private:
    enum flg_t
    {
        flg_owner
    };

private:
    void init(uint_t A = uint_t_max);
    void
    deInit()
    {
        if (isOwner())
            delete[] _pat;
        delete[] _skip;
    }

private:
    const T* _pat;
    size_t _patLen;
    ushort_t* _skip;
    uint_t _A;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const T*
BoyerMooreSearch<T>::find(const T* str, size_t strLen) const
{
    // minimize this-> overhead
    const T* pat = _pat;
    size_t patLen = _patLen;
    const ushort_t* skip = _skip;

    // the actual search
    const T* strPtr = str + patLen - 1;
    const T* strEnd = str + strLen;
    while (strPtr < strEnd)
    {
        const T* patPtr = pat + patLen - 1;
        while (*patPtr == *strPtr)
        {
            if (patPtr == pat)
                return strPtr;
            --patPtr;
            --strPtr;
        }

        strPtr += max(skip[(uint64_t)*strPtr], (ushort_t)(patLen - (patPtr - pat)));
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
BoyerMooreSearch<T>::set(bool owner, const T* pat, size_t patLen)
{
    if (isOwner())
        delete[] _pat;
    setOwner(owner);
    _pat = pat;
    _patLen = patLen;

    // set up the skip table
    size_t i;
    uint_t A = _A;
    ushort_t* skip = _skip;
    for (i = 0; i < A; i++)
    {
        skip[i] = patLen;
    }
    for (i = 0; i < patLen; i++)
    {
        skip[(uint64_t)pat[i]] = patLen - 1 - i;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
BoyerMooreSearch<T>::init(uint_t A)
{
    if (A == uint_t_max)
    {
        A = (uint_t)std::numeric_limits<T>::max() - (uint_t)std::numeric_limits<T>::min() + 1U;
    }
    _A = A;
    _pat = nullptr;
    _skip = new ushort_t[A];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const T*
boyerMooreSearch(const T* str, size_t strLen, const T* pat, size_t patLen)
{
    return BoyerMooreSearch<T>(false, pat, patLen).find(str, strLen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::BoyerMooreSearch, T);
