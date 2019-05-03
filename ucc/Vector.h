#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FlagsMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A sequence of same-type objects.

   <b>Attributes</b>

   \arg \b array : The array of objects.

   \arg \b size : The size of the sequence (\b size <= \b allocSize).

   \arg \b allocSize : The number of objects that can be stored in the present allocation.

   \arg \b increment : When the allocation is to be grown, the growth increment determines the
   amount of growth.  For example, if increment = 16, the allocation will always be grown by a
   multiple of 16.  As a special case, if increment = size_t_max, the allocation will be doubled in
   size until it is >= the required size.  The default is to allocate exactly the amount of memory
   that's immediately required (increment = 1).

   \arg <b><i>owner</i> flag</b> : If a Vector owns its associated array, it is responsible for
   deleting the array when appropriate (e.g. upon its destruction).

   \arg <b><i>autoInit</i> flag</b> : When the sequence grows, newly created objects may optionally
   be initialized with the default value for the contained type.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> class Vector : public Object, protected FlagsMI
{
    UTL_CLASS_DECL_TPL(Vector, T, Object);

public:
    typedef T type;
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;

public:
    /**
       Constructor.
       \param list initializer list
    */
    Vector(std::initializer_list<T> list);

    /**
       Constructor
       \param size sequence size
       \param increment (optional : 1) growth increment
    */
    Vector(size_t size, size_t increment = 1);

    /**
       Constructor
       \param array associated array
       \param size sequence size
       \param owner (optional : true) \b owner flag
       \param increment (optional : 1) growth increment
    */
    Vector(T* array, size_t size, bool owner = true, size_t increment = 1);

    /// \name Various Read-Only Operations
    //@{
    virtual int compare(const Object& rhs) const;

    virtual size_t
    innerAllocatedSize() const
    {
        if (!isOwner())
            return 0;
        return (_allocSize * sizeof(T));
    }

    /** Get the \b owner flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Get the \b autoInit flag. */
    bool
    autoInit() const
    {
        return getFlag(flg_autoInit);
    }

    /** Get the associated array. */
    const T*
    get() const
    {
        return _array;
    }

    /** Get the associated array. */
    T*
    get()
    {
        return _array;
    }

    /** Get a const reference to the object at the given index. */
    const T&
    get(size_t idx) const
    {
        ASSERTD(idx < _size);
        return _array[idx];
    }

    /** get() with \b int parameter. */
    const T&
    get(int idx) const
    {
        return get((size_t)idx);
    }

    /** get() with \b uint_t parameter. */
    const T&
    get(uint_t idx) const
    {
        return get((size_t)idx);
    }

    /** Is the sequence empty? */
    bool
    empty() const
    {
        return (_size == 0);
    }

    /** Get the size of the sequence. */
    size_t
    length() const
    {
        return _size;
    }

    /** Get the size of the sequence. */
    size_t
    size() const
    {
        return _size;
    }

    /** Get the growth increment. */
    size_t
    increment() const
    {
        return _increment;
    }
    //@}

    /// \name Iterators
    //@{
    /** Get begin iterator (const version). */
    const_iterator
    begin() const
    {
        return _array;
    }

    /** Get end iterator (const version). */
    const_iterator
    end() const
    {
        return _array + _size;
    }

    /** Get begin iterator (const version). */
    const_iterator
    cbegin()
    {
        return _array;
    }

    /** Get end iterator (const version). */
    const_iterator
    cend()
    {
        return _array + _size;
    }

    /** Get begin iterator. */
    iterator
    begin()
    {
        return _array;
    }

    /** Get end iterator. */
    iterator
    end()
    {
        return _array + _size;
    }
    //@}

    /// \name Iterator -> Index Conversions
    //@{
    /** Convert iterator to index. */
    size_t
    indexOf(const_iterator it) const
    {
        ASSERTD((it >= this->begin()) && (it <= this->end()));
        return (it - this->begin());
    }

    /** Convert iterator to index. */
    size_t
    indexOf(iterator it)
    {
        ASSERTD((it >= this->begin()) && (it <= this->end()));
        return (it - this->begin());
    }
    //@}

    /// \name Modification
    //@{
    /** Empty the sequence. */
    void
    clear()
    {
        _size = 0;
    }

    /** Empty the sequence, delete the allocated array (if isOwner()) and set it to nullptr. */
    void excise();

    /** Make the allocation exactly large enough to contain the sequence. */
    void economize();

    virtual void steal(Object& rhs);

    virtual void copy(const Object&);

    /**
       Copy (part of) another vector.  The portion of the source vector [srcBegin,srcEnd)
       is copied into this vector starting at position idx.
       \param idx destination index
       \param src vector to copy objects from
       \param srcBegin (optional : 0) source begin index
       \param srcEnd (optional : size_t_max) source end index
    */
    void
    copy(size_t idx, const Vector<T>& src, size_t srcBegin = 0, size_t srcEnd = size_t_max)
    {
        copy(idx, src._array, srcBegin, min(srcEnd, src._size));
    }

    /**
       Copy (part of) the given sequence.  The part of the source sequence [srcBegin,srcEnd)
       is copied into this vector starting at position idx.
       \param idx destination index
       \param src sequence to copy objects from
       \param srcBegin (optional : 0) source begin index
       \param srcEnd (optional : size_t_max) source end index
    */
    void copy(size_t idx, const T* src, size_t srcBegin, size_t srcEnd);

    /**
       Copy an initializer_list into this sequence.
       \param idx index to start copying into
       \param list initializer list
    */
    void copy(size_t idx, std::initializer_list<T> list);

    /**
       Move (part of) another vector.  The portion of the source vector [srcBegin,srcEnd)
       is moved into this vector starting at position idx.
       \param idx destination index
       \param src vector to copy objects from
       \param srcBegin (optional : 0) source begin index
       \param srcEnd (optional : size_t_max) source end index
    */
    void
    move(size_t idx, const Vector<T>& src, size_t srcBegin = 0, size_t srcEnd = size_t_max)
    {
        move(idx, src._array, srcBegin, min(srcEnd, src._size));
    }

    /**
       Move (part of) the given sequence.  The part of the source sequence [srcBegin,srcEnd)
       is moved into this vector starting at position idx.
       \param idx destination index
       \param src sequence to copy objects from
       \param srcBegin (optional : 0) source begin index
       \param srcEnd (optional : size_t_max) source end index
    */
    void move(size_t idx, T* src, size_t srcBegin, size_t srcEnd);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Set the ownership flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /** Ensure that self owns the allocation. */
    void assertOwner();

    /**
       Ensure that self owns the allocation.
       \param allocSize new size of sequence
    */
    void assertOwner(size_t allocSize);

    /**
       Ensure that self owns the allocation.
       \param allocSize new size of sequence
       \param increment growth increment
    */
    void
    assertOwner(size_t allocSize, size_t increment)
    {
        setIncrement(increment);
        assertOwner(allocSize);
    }

    /** Get a reference to the object at the given index. */
    T&
    get(size_t idx)
    {
        ASSERTD(idx < _size);
        return _array[idx];
    }

    /** get() with \b int parameter. */
    T&
    get(int idx)
    {
        return get((size_t)idx);
    }

    /** get() with \b uint_t parameter. */
    T&
    get(uint_t idx)
    {
        return get((size_t)idx);
    }

    /** Set the size of the sequence. */
    void setSize(size_t size);

    /**
       Ensure the sequence is no smaller than the given size.
       \param size required minimum size
    */
    void grow(size_t size);

    /**
       Ensure the sequence is no smaller than the given size.
       \param size required minimum size
       \param increment growth increment
    */
    void
    grow(size_t size, size_t increment)
    {
        setIncrement(increment);
        grow(size);
    }

    /**
       Reserve allocation space.
       \param allocSize minimum allocation size
    */
    void
    reserve(size_t allocSize)
    {
        if (allocSize > _allocSize)
            assertOwner(allocSize);
    }

    /**
       Reserve allocation space.
       \param allocSize minimum allocation size
       \param increment growth increment
    */
    void
    reserve(size_t allocSize, size_t increment)
    {
        if (allocSize > _allocSize)
            assertOwner(allocSize, increment);
    }

    /** Set the \b autoInit flag. */
    void
    setAutoInit(bool autoInit)
    {
        setFlag(flg_autoInit, autoInit);
    }

    /** Append an object to the sequence. */
    void
    append(const T& val)
    {
        append(&val, 1);
    }

    /** Append another sequence to this one. */
    void
    append(const Vector<T>& v)
    {
        append(v._array, v._size);
    }

    /** Append another sequence to this one. */
    void append(const T* array, size_t arraySize);

    /** Append another sequence to this one. */
    void append(std::initializer_list<T> list);

    /** Insert uninitialized objects in the range [idx,idx+num). */
    void insert(size_t idx, size_t num);

    /**
       Insert objects into the sequence.
       \param idx index where objects are to be inserted
       \param num number of objects to insert
       \param val initial value for each new object
    */
    void
    insert(size_t idx, size_t num, const T& val)
    {
        insert(idx, num);
        set(idx, num, val);
    }

    /**
       Insert objects into the sequence.
       \param idx index where objects are to be added
       \param num number of objects to insert
       \param val initial value for inserted objects
    */
    void
    insert(size_t idx, size_t num, const T* val)
    {
        insert(idx, num);
        set(idx, num, *val);
    }

    /**
       Insert another sequence into this one.
       \param idx index where the sequence will be inserted
       \param array pointer to the first object to be inserted
       \param arraySize number of objects to be inserted
    */
    void
    insert(size_t idx, const T* array, size_t arraySize)
    {
        insert(idx, arraySize);
        copy(idx, array, 0, arraySize);
    }

    /**
       Insert another sequence into this one.
       \param idx index where the sequence will be inserted
       \param list initializer list
    */
    void
    insert(size_t idx, std::initializer_list<T> list)
    {
        insert(idx, list.size());
        copy(idx, list);
    }

    /**
       Remove objects from the sequence.
       \param idx index of first object to be removed
       \param num number of objects to remove
    */
    void remove(size_t idx, size_t num);

    /**
       Relocate one object within the sequence.
       \param destIdx index of object that will immediately follow the relocated object
       \param srcIdx index of object to be relocated
    */
    void relocate(size_t destIdx, size_t srcIdx);

    /**
       Reverse the order of the objects in the sequence.
       \param idx index of to-be-reversed section
       \param num length of to-be-reversed section
    */
    void reverse(size_t idx, size_t num);

    /**
       clear(), then set the given array/size.  The \b owner flag is preserved.
       \param array new allocation
       \param size size of new sequence
    */
    void
    set(T* array, size_t size)
    {
        set(array, size, isOwner(), _increment);
    }

    /**
       clear(), then set the given array/size and \b owner flag.
       \param array new array
       \param size size of new array
       \param owner new \b owner flag
       \param increment growth increment
    */
    void set(T* array, size_t size, bool owner, size_t increment);

    /** Assign val to all objects in the sequence. */
    void
    set(const T& val)
    {
        set(0, _size, val);
    }

    /**
       Assign a value to objects in the sequence.
       \param idx index of first object to set = \b val
       \param num number of consecutive objects to set
       \param val value to assign
    */
    void set(size_t idx, size_t num, const T& val);

    /**
       Set the value of the object at the given index.
       \param idx index of object
       \param val new value for object at \b idx
    */
    void
    set(size_t idx, const T& val)
    {
        ASSERTD(idx < _size);
        _array[idx] = val;
    }

    /** Set the growth increment. */
    void
    setIncrement(size_t increment)
    {
        ASSERTD((increment == size_t_max) || (increment == nextPow2(increment)));
        _increment = increment;
    }

    /** Swap the objects at the given indexes. */
    inline void swap(size_t i, size_t j);

    /**
       Swap the sub-arrays described by the index ranges
       [i, i + n) and [j, j + n).
    */
    inline void swap(size_t i, size_t j, size_t n);
    //@}

    /// \name Operators
    //@{
    /**
       Array access operator.
       \return reference to object at given index
       \param idx index of requested object
    */
    T& operator[](size_t idx)
    {
        ASSERTD(idx < _size);
        return _array[idx];
    }

    /**
       Array access operator.
       \return reference to object at given index
       \param idx index of requested object
    */
    const T& operator[](size_t idx) const
    {
        ASSERTD(idx < _size);
        return _array[idx];
    }

    /** Same as above, but takes an \b int parameter. */
    T& operator[](int idx)
    {
        return operator[]((size_t)idx);
    }

    /** Same as above, but takes an \b int parameter. */
    const T& operator[](int idx) const
    {
        return operator[]((size_t)idx);
    }

    /** Same as above, but takes a \b uint_t parameter. */
    T& operator[](uint_t idx)
    {
        return operator[]((size_t)idx);
    }

    /** Same as above, but takes a \b uint_t parameter. */
    const T& operator[](uint_t idx) const
    {
        return operator[]((size_t)idx);
    }

    /** Conversion to const T*. */
    operator const T*() const
    {
        return _array;
    }

    /** Conversion to const T*. */
    operator const T*()
    {
        return _array;
    }

    /** Conversion to T*. */
    operator T*()
    {
        return _array;
    }

    /** Conversion to const void*. */
    operator const void*() const
    {
        return _array;
    }

    /** Conversion to const void*. */
    operator const void*()
    {
        return _array;
    }

    /** Conversion to void*. */
    operator void*()
    {
        return _array;
    }

    /** Pointer addition (ssize_t argument). */
    T*
    operator+(ssize_t n)
    {
        ASSERTD(n >= 0);
        ASSERTD((size_t)n <= _size);
        return _array + n;
    }

    /** Pointer addition (ssize_t argument). */
    const T*
    operator+(ssize_t n) const
    {
        ASSERTD(n >= 0);
        ASSERTD((size_t)n <= _size);
        return _array + n;
    }

    /** Pointer addition (size_t argument). */
    T*
    operator+(size_t n)
    {
        ASSERTD(n <= _size);
        return _array + n;
    }

    /** Pointer addition (size_t argument). */
    const T*
    operator+(size_t n) const
    {
        ASSERTD(n <= _size);
        return _array + n;
    }
    //@}
protected:
    T* _array;
    size_t _size;
    size_t _allocSize;
    size_t _increment;

private:
    inline void
    init()
    {
        _array = nullptr;
        _size = _allocSize = 0;
        _increment = 1;
        setOwner(true);
    }

    inline void init(T* array, size_t size = 0, bool owner = true, size_t increment = 1);

    void
    deInit()
    {
        excise();
    }

    void _setAllocSize(size_t allocSize);

    void _autoInit(size_t begin, size_t end);

private:
    enum flg_t
    {
        flg_autoInit,
        flg_owner
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::Vector, T);

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Stream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> Vector<T>::Vector(std::initializer_list<T> list)
{
    init();
    _setAllocSize(list.size());
    for (auto obj : list)
        append(obj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> Vector<T>::Vector(size_t size, size_t increment)
{
    init(nullptr, size, true, increment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> Vector<T>::Vector(T* array, size_t size, bool owner, size_t increment)
{
    init(array, size, owner, increment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::economize()
{
    ASSERTD(isOwner());
    if (_allocSize > _size)
    {
        auto saveIncrement = _increment;
        _increment = 1;
        _setAllocSize(_size);
        _increment = saveIncrement;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::excise()
{
    if (isOwner())
        delete[] _array;
    _array = nullptr;
    _size = 0;
    _allocSize = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
int
Vector<T>::compare(const Object& rhs) const
{
    auto& v = utl::cast<const Vector<T>>(rhs);
    if (&v == this)
        return 0;
    return utl::compare(_array, _size, v._array, v._size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::steal(Object& rhs)
{
    auto& v = utl::cast<Vector<T>>(rhs);
    if (_array != nullptr)
        excise();
    FlagsMI::copyFlags(v);
    _array = v._array;
    _size = v._size;
    _allocSize = v._allocSize;
    _increment = v._increment;
    v._array = nullptr;
    v._size = 0;
    v._allocSize = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::copy(const Object& rhs)
{
    auto& v = utl::cast<const Vector<T>>(rhs);
    if (&v == this)
        return;
    bool lhsOwner = this->isOwner();
    bool rhsOwner = v.isOwner();
    if (lhsOwner && rhsOwner)
    {
        clear();
    }
    else
    {
        excise();
    }
    FlagsMI::copyFlags(v);
    _increment = v._increment;
    if (rhsOwner)
    {
        copy(0, v);
    }
    else
    {
        _array = v._array;
        _size = v._size;
        _allocSize = v._size;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::copy(size_t idx, const T* src, size_t srcBegin, size_t srcEnd)
{
    // grow to required size
    grow(idx + (srcEnd - srcBegin));

    // perform the copy
    auto dstPtr = _array + idx;
    auto srcPtr = src + srcBegin;
    auto srcLim = src + srcEnd;
    if (std::is_trivially_copyable<T>())
    {
        memcpy(dstPtr, srcPtr, (srcLim - srcPtr) * sizeof(T));
    }
    else
    {
        while (srcPtr != srcLim)
            *dstPtr++ = *srcPtr++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::copy(size_t idx, std::initializer_list<T> list)
{
    grow(idx + list.size());
    T* ptr = _array + idx;
    for (auto obj : list)
        *ptr++ = obj;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::move(size_t idx, T* src, size_t srcBegin, size_t srcEnd)
{
    // grow to required size
    grow(idx + (srcEnd - srcBegin));

    // move objects
    auto dstPtr = _array + idx;
    auto srcPtr = src + srcBegin;
    auto srcLim = src + srcEnd;
    if (std::is_trivially_copyable<T>())
    {
        memcpy(dstPtr, srcPtr, (srcLim - srcPtr) * sizeof(T));
    }
    else
    {
        while (srcPtr != srcLim)
            *dstPtr++ = std::move(*srcPtr++);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::serialize(Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        clear();
        setOwner(true);
        // increment
        utl::serialize(_increment, stream, io, mode);
        if (_increment == 0)
            _increment = 1;
        // size
        size_t size;
        utl::serialize(size, stream, io, mode);
        setSize(size);
    }
    else
    {
        // increment
        utl::serialize(_increment, stream, io, mode);
        // size
        utl::serialize(_size, stream, io, mode);
    }

    for (size_t i = 0; i < _size; i++)
    {
        utl::serialize(_array[i], stream, io, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::assertOwner()
{
    if (isOwner())
        return;

    // set the flag
    setOwner(true);

    // remember _array, _size
    T* oldArray = _array;
    size_t oldSize = _size;

    // _array = null
    _array = nullptr;
    _size = 0;
    _allocSize = 0;

    // grow allocation to the old size, re-set _size
    _setAllocSize(oldSize);
    _size = oldSize;

    // copy from the old array to the new one
    copy(0, oldArray, 0, oldSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::assertOwner(size_t allocSize)
{
    // we already own the array?
    if (isOwner())
    {
        if (allocSize > _allocSize)
        {
            _setAllocSize(allocSize);
        }
    }
    else
    {
        // set the flag
        setOwner(true);

        // empty sequence?
        if (allocSize == 0)
        {
            // just nuke the array
            _size = 0;
            _allocSize = 0;
            _array = nullptr;
        }
        else
        {
            // remember _array, _size
            T* oldArray = _array;
            size_t oldSize = _size;

            // _array = null
            _array = nullptr;
            _size = 0;
            _allocSize = 0;

            // grow allocation to the new size
            _setAllocSize(allocSize);

            // move as much as we can from the old allocation to the new one
            _size = min(oldSize, allocSize);
            move(0, oldArray, 0, _size);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::setSize(size_t size)
{
    if (isOwner() && (size > _allocSize))
        _setAllocSize(size);
    if ((size > _size) && autoInit())
        _autoInit(_size, size);
    _size = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::grow(size_t size)
{
    if (size <= _size)
        return;
    if (size > _allocSize)
        assertOwner(size);
    if (autoInit())
        _autoInit(_size, size);
    _size = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::append(const T* array, size_t arraySize)
{
    size_t oldSize = _size;
    grow(_size + arraySize);
    if (std::is_trivially_copyable<T>())
    {
        memcpy(_array + oldSize, array, arraySize * sizeof(T));
    }
    else
    {
        T* op = _array + oldSize;
        const T* ip = array;
        T* lim = _array + _size;
        for (; op < lim; ++ip, ++op)
            *op = *ip;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::append(std::initializer_list<T> list)
{
    reserve(_size + list.size());
    for (auto obj : list)
        append(obj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::insert(size_t idx, size_t num)
{
    // the sequence grows in size by num
    grow(_size + num);

    // shift elements over
    if (std::is_trivially_copyable<T>())
    {
        memmove(_array + idx + num, _array + idx, (_size - idx - num) * sizeof(T));
    }
    else
    {
        T* op = _array + _size - 1;
        T* opLim = _array + idx + num;
        T* ip = op - num;
        for (; op >= opLim; --ip, --op)
            *op = std::move(*ip);
    }
    if (autoInit())
        _autoInit(idx, idx + num);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::remove(size_t idx, size_t num)
{
    ASSERTD(_size >= (idx + num));

    // shift elements over
    if (std::is_trivially_copyable<T>())
    {
        memmove(_array + idx, _array + idx + num, (_size - idx - num) * sizeof(T));
    }
    else
    {
        T* op = _array + idx;
        T* opLim = _array + _size - num;
        T* ip = op + num;
        for (; op != opLim; ++ip, ++op)
            *op = std::move(*ip);
    }

    _size -= num;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::relocate(size_t destIdx, size_t srcIdx)
{
    ASSERTD(destIdx <= _size);
    ASSERTD(srcIdx < _size);
    if ((destIdx == srcIdx) || (destIdx == (srcIdx + 1)))
        return;
    T tmp = _array[srcIdx];
    if (std::is_trivially_copyable<T>())
    {
        if (srcIdx < destIdx)
        {
            // objects in (srcIdx,destIdx) move to the left
            memmove(_array + srcIdx, _array + srcIdx + 1, (destIdx - srcIdx - 1) * sizeof(T));
            _array[destIdx - 1] = tmp;
        }
        else
        {
            // objects in [destIdx,srcIdx) move to the right
            memmove(_array + destIdx + 1, _array + destIdx, (srcIdx - destIdx) * sizeof(T));
            _array[destIdx] = tmp;
        }
    }
    else
    {
        T* destPtr;
        T* srcPtr;
        T* srcLim;
        if (srcIdx < destIdx)
        {
            // objects in (srcIdx,destIdx) move to the left
            destPtr = _array + srcIdx;
            srcPtr = _array + srcIdx + 1;
            srcLim = srcPtr + (destIdx - srcIdx - 1);
            for (; srcPtr != srcLim; ++srcPtr, ++destPtr)
                *destPtr = std::move(*srcPtr);
            _array[destIdx - 1] = tmp;
        }
        else
        {
            // objects in [destIdx,srcIdx) move to the right
            destPtr = _array + destIdx + 1;
            srcPtr = _array + destIdx;
            srcLim = srcPtr + (srcIdx - destIdx);
            for (; srcPtr != srcLim; ++srcPtr, ++destPtr)
                *destPtr = std::move(*srcPtr);
            _array[destIdx] = tmp;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::reverse(size_t idx, size_t num)
{
    ASSERTD((idx + num) <= _size);
    T* lhs = _array + idx;
    T* rhs = _array + idx + num - 1;
    for (; lhs < rhs; ++lhs, --rhs)
    {
        std::swap(*lhs, *rhs);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::set(T* array, size_t size, bool owner, size_t increment)
{
    if (_array != nullptr)
        clear();
    setOwner(owner);
    _array = array;
    _size = size;
    _allocSize = size;
    _increment = increment;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::set(size_t idx, size_t num, const T& val)
{
    T* lim = _array + idx + num;
    for (T* ptr = _array + idx; ptr != lim; ++ptr)
    {
        *ptr = val;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::swap(size_t i, size_t j)
{
    ASSERTD(i < _size);
    ASSERTD(j < _size);
    std::swap(_array[i], _array[j]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::swap(size_t i, size_t j, size_t n)
{
    ASSERTD((i + n) <= _size);
    ASSERTD((j + n) <= _size);

    T* lhs = _array + i;
    T* rhs = _array + j;
    T* lhsLim = lhs + n;
    for (; lhs != lhsLim; ++lhs, ++rhs)
    {
        std::swap(*lhs, *rhs);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::init(T* array, size_t size, bool owner, size_t increment)
{
    setOwner(owner);
    if ((array == nullptr) && (size > 0))
    {
        ASSERTD(owner);
        _array = nullptr;
        _size = 0;
        _allocSize = 0;
        grow(size, increment);
    }
    else
    {
        _array = array;
        _size = size;
        _allocSize = _size;
        setIncrement(increment);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::_setAllocSize(size_t reqAllocSize)
{
    ASSERTD(isOwner());

    // figure out the new allocation size
    size_t allocSize;
    if (_increment == size_t_max)
    {
        // find nearest power of 2
        allocSize = nextPow2(reqAllocSize);
    }
    else
    {
        // find nearest multiple of _increment
        allocSize = nextMultipleOfPow2(_increment, reqAllocSize);
    }
    ASSERTD(allocSize >= reqAllocSize);

    // ensure _size <= requested allocSize
    _size = min(_size, reqAllocSize);

    // create new array
    if (std::is_trivially_copyable<T>())
    {
        _array =
            static_cast<T*>(utl::realloc(_array, _allocSize * sizeof(T), allocSize * sizeof(T)));
    }
    else
    {
        // remember the old allocation, make a new one
        T* oldArray = _array;
        _array = new T[allocSize];

        // move objects to the new array
        T* lhs = _array;
        T* rhs = oldArray;
        T* lhsLim = lhs + _size;
        for (; lhs != lhsLim; ++lhs, ++rhs)
            *lhs = std::move(*rhs);

        // nuke the old allocation
        delete[] oldArray;
    }

    // re-set allocSize
    _allocSize = allocSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
Vector<T>::_autoInit(size_t begin, size_t end)
{
    auto ptr = _array + begin;
    auto lim = _array + end;
    for (; ptr != lim; ++ptr)
        *ptr = T();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
