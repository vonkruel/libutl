#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Mutex.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   utl::ConcurrentQueue that uses mutexes for the producer & consumer locks.

   This class was written for testing, and it was left in.

   \author Adam McKee
   \ingroup collection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class ConcurrentQueue_mutex
{
public:
    /** Constructor */
    ConcurrentQueue_mutex();

    /** Destructor. */
    ~ConcurrentQueue_mutex();

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
    Mutex _consumerLock;
    Node* _head;
    char pad1[UTL_ARCH_CACHE_LINE_SIZE - sizeof(Node*)];
    Mutex _producerLock;
    Node* _tail;
    char pad2[UTL_ARCH_CACHE_LINE_SIZE - sizeof(Node*)];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ConcurrentQueue_mutex<T>::ConcurrentQueue_mutex()
{
    _head = _tail = new Node();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ConcurrentQueue_mutex<T>::~ConcurrentQueue_mutex()
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
ConcurrentQueue_mutex<T>::enQ(T value)
{
    Node* node = new Node(value);

    // acquire producer lock
    _producerLock.lock();

    // tail->node, and tail=node
    _tail->next = node;
    _tail = node;

    // release producer lock
    _producerLock.unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
bool
ConcurrentQueue_mutex<T>::deQ(T& value)
{
    _consumerLock.lock();

    // remember head and its next
    Node* head = _head;
    Node* next = _head->next;

    // empty?
    if (next == nullptr)
    {
        _consumerLock.unlock();
        return false;
    }

    // copy value, and advance _head
    value = next->value;
    _head = next;

    // release consumer lock
    _consumerLock.unlock();

    // clean up the old head
    delete head;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
ConcurrentQueue_mutex<T>::forEach(std::function<void(T)> func) const
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
