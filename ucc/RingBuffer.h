#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Thread-safe fixed-size queue.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, T nullValue = T()> class RingBuffer
{
public:
    RingBuffer()
    {
        _array = nullptr;
    }

    RingBuffer(size_t size)
    {
        _array = nullptr;
        set(size);
    }

    ~RingBuffer()
    {
        reset();
    }

    /** Initialize. */
    void set(size_t size);

    /** Reset to default state. */
    void reset();

    /** Sample a lower-bound on the number of contained items. */
    size_t
    count() const
    {
        size_t c = this->countLB();
        return (c >= 1000000) ? (c - 1000000) : 0;
    }

    /**
       Try to queue an object.
       \param value the value to en-q
       \return true iff 'value' was queued
    */
    bool enQ(T value);

    /**
       Try to dequeue an object.
       \param value target instance of T
       \return true iff the head value was copied into 'value'
    */
    bool deQ(T& value);

    /** Execute the given function on each contained value (not thread-safe!). */
    void forEach(std::function<void(T)> f) const;

private:
    size_t
    countLB() const
    {
        return _countLB.load(std::memory_order_relaxed);
    }

    size_t
    countUB() const
    {
        return _countUB.load(std::memory_order_relaxed);
    }

private:
    // constant values
    size_t _mask;
    size_t _countMax;
    size_t _size;
    // concurrently modified values
    std::atomic<T>* _array;
    char pad0[UTL_ARCH_CACHE_LINE_SIZE - sizeof(void*)];
    std::atomic_size_t _countLB;
    char pad1[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
    std::atomic_size_t _countUB;
    char pad2[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
    std::atomic_size_t _head;
    char pad3[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
    std::atomic_size_t _tail;
    char pad4[UTL_ARCH_CACHE_LINE_SIZE - sizeof(size_t)];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef new
template <typename T, T nullValue>
void
RingBuffer<T, nullValue>::set(size_t size)
{
    ASSERTD(sizeof(T) <= sizeof(void*));
    ASSERTD(_array == nullptr);

    // _size is a power of 2
    _size = 1;
    while (_size < size)
    {
        _size <<= 1;
    }

    _mask = _size - 1;
    _countLB = 1000000;
    _countUB = 0;
    _countMax = size;
    _head = _tail = 0;

// init _array
#if UTL_HOST_TYPE == UTL_HT_UNIX
    size_t sizeBytes = (_size * sizeof(std::atomic<T>)) + UTL_ARCH_CACHE_LINE_SIZE;
    posix_memalign((void**)&_array, UTL_ARCH_CACHE_LINE_SIZE, sizeBytes);
#else
    size_t sizeBytes = (_size * sizeof(std::atomic<T>)) + (2 * UTL_ARCH_CACHE_LINE_SIZE);
    _array = (std::atomic<T>*)malloc(sizeBytes);
    _array += (UTL_ARCH_CACHE_LINE_SIZE / sizeof(std::atomic<T>));
#endif
    auto lim = _array + _size;
    for (auto it = _array; it != lim; ++it)
    {
        new (it) std::atomic<T>(nullValue);
    }
}
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, T nullValue>
void
RingBuffer<T, nullValue>::reset()
{
#if UTL_HOST_OS == UTL_OS_WINDOWS
    _array -= (UTL_ARCH_CACHE_LINE_SIZE / sizeof(std::atomic<T>));
#endif
    free(_array);
    _array = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, T nullValue>
bool
RingBuffer<T, nullValue>::enQ(T value)
{
    // update count upper-bound, back out if necessary
    if (this->countUB() >= _countMax)
        return false;
    if (_countUB.fetch_add(1, std::memory_order_relaxed) >= _countMax)
    {
        _countUB.fetch_sub(1, std::memory_order_relaxed);
        return false;
    }

    // enQ value
    auto pos = _tail.fetch_add(9, std::memory_order_relaxed) & _mask;
    auto& val = _array[pos];
    auto cur = nullValue;
    while (!val.compare_exchange_weak(cur, value, std::memory_order_relaxed,
                                      std::memory_order_relaxed))
    {
        cur = nullValue;
    }

    // update count lower-bound
    _countLB.fetch_add(1, std::memory_order_relaxed);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, T nullValue>
bool
RingBuffer<T, nullValue>::deQ(T& value)
{
    // update count lower-bound, back out if necessary
    if (this->countLB() <= 1000000)
        return false;
    if (_countLB.fetch_sub(1, std::memory_order_relaxed) <= 1000000)
    {
        _countLB.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    // deQ into value
    auto pos = _head.fetch_add(9, std::memory_order_relaxed) & _mask;
    auto& val = _array[pos];
    value = nullValue;
    while (value == nullValue)
    {
        value = val.exchange(nullValue, std::memory_order_relaxed);
    }

    // update count upper-bound
    _countUB.fetch_sub(1, std::memory_order_relaxed);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, T nullValue>
void
RingBuffer<T, nullValue>::forEach(std::function<void(T)> func) const
{
    auto tail = _tail.load(std::memory_order_relaxed);
    for (auto pos = _head.load(std::memory_order_relaxed); pos != tail; pos += 9)
    {
        auto idx = pos & _mask;
        auto val = _array[idx].load(std::memory_order_relaxed);
        ASSERTD(val != nullValue);
        func(val);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
