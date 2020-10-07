#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Thread-safe queue structure.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ConcurrentQueue
{
public:
    /** Constructor */
    ConcurrentQueue();

    /** Destructor. */
    ~ConcurrentQueue();

    /** Queue an object. */
    void enQ(T value);

    /** Dequeue an object. */
    bool deQ(T& value);

    /** Execute the given function on each contained item (not thread-safe!). */
    void forEach(std::function<void(T)> f) const;

private:
    struct Node
    {
        Node()
            : next(nullptr)
        {
        }
        Node(T val)
            : value(val)
            , next(nullptr)
        {
        }

    public:
        T value;
        Node* next;
    };

private:
    char pad0[UTL_ARCH_CACHE_LINE_SIZE];
    Node* _head;
    char pad1[UTL_ARCH_CACHE_LINE_SIZE - sizeof(Node*)];
    std::atomic_bool _consumerLock;
    char pad2[UTL_ARCH_CACHE_LINE_SIZE - sizeof(bool)];
    Node* _tail;
    char pad3[UTL_ARCH_CACHE_LINE_SIZE - sizeof(Node*)];
    std::atomic_bool _producerLock;
    char pad4[UTL_ARCH_CACHE_LINE_SIZE - sizeof(bool)];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ConcurrentQueue<T>::ConcurrentQueue()
    : _consumerLock(false)
    , _producerLock(false)
{
    _head = _tail = new Node();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
    while (_head != nullptr)
    {
        Node* node = _head;
        _head = node->next;
        delete node;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
ConcurrentQueue<T>::enQ(T value)
{
    Node* node = new Node(value);

    // acquire producer lock
    bool cur = false;
    while (!_producerLock.compare_exchange_weak(cur, true, std::memory_order_acquire,
                                                std::memory_order_relaxed))
    {
        cur = false;
    }

    // link from tail, become new tail
    _tail->next = node;
    _tail = node;

    // release producer lock
    _producerLock.store(false, std::memory_order_release);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
bool
ConcurrentQueue<T>::deQ(T& value)
{
    // acquire consumer lock
    bool cur = false;
    while (!_consumerLock.compare_exchange_weak(cur, true, std::memory_order_acquire,
                                                std::memory_order_relaxed))
    {
        cur = false;
    }

    // remember head and its next
    Node* head = _head;
    Node* next = _head->next;

    // empty?
    if (next == nullptr)
    {
        _consumerLock.store(false, std::memory_order_relaxed);
        return false;
    }

    // copy value, and advance _head
    value = next->value;
    _head = next;

    // release consumer lock
    _consumerLock.store(false, std::memory_order_release);

    // clean up the old head
    delete head;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
ConcurrentQueue<T>::forEach(std::function<void(T)> func) const
{
    Node* node = _head;
    while (true)
    {
        node = node->next;
        if (node == nullptr)
            break;
        func(node->value);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
