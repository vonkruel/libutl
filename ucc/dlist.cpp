#include <libutl/libutl.h>
#include <libutl/dlist.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
list_compare(const ListNode* lhs, const ListNode* rhs, const Ordering* ordering)
{
    if (lhs == rhs)
        return 0;

    for (;;)
    {
        bool lhsEnd = (lhs == nullptr) || lhs->isSentinelTail();
        bool rhsEnd = (rhs == nullptr) || rhs->isSentinelTail();

        if (lhsEnd && rhsEnd)
        {
            break;
        }
        if (lhsEnd)
        {
            return -1;
        }
        if (rhsEnd)
        {
            return 1;
        }

        int res = utl::compare(lhs->get(), rhs->get(), ordering);
        if (res != 0)
        {
            return res;
        }

        lhs = lhs->next();
        rhs = rhs->next();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
list_clone(ListNode*& outList, ListNode** outTail, const ListNode* inList, bool owner)
{
    const ListNode* srcNode = inList;
    ListNode* dstNode = nullptr;
    ListNode* dstHead = nullptr;

    while (srcNode != nullptr)
    {
        Object* object = srcNode->get();
        if (owner && (object != maxObject))
        {
            object = object->clone();
        }
        ListNode* newNode = new ListNode(object);
        if (dstNode == nullptr)
        {
            dstHead = dstNode = newNode;
        }
        else
        {
            newNode->addAfter(dstNode);
            dstNode = newNode;
        }
        srcNode = srcNode->next();
    }

    outList = dstHead;
    if (outTail != nullptr)
        *outTail = dstNode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
list_serialize(ListNode*& list, ListNode** tail, bool owner, Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        list_clear(list, tail, owner);

        // determine # of items
        size_t items;
        utl::serialize(items, stream, io, mode);

        // serialize objects
        ListNode* node = nullptr;

        size_t i;
        for (i = 0; i < items; i++)
        {
            Object* object = Object::serializeInBoxed(stream, mode);
            ListNode* newNode = new ListNode(object);
            if (tail != nullptr)
                *tail = newNode;
            if (node == nullptr)
            {
                list = node = new ListNode(object);
            }
            else
            {
                newNode->addAfter(node);
                node = newNode;
            }
        }

        bool hasSentinelTail;
        utl::serialize(hasSentinelTail, stream, io, mode);
        if (hasSentinelTail)
        {
            ListNode* newNode = new ListNode(maxObject);
            if (tail != nullptr)
                *tail = newNode;
            newNode->addAfter(node);
        }
    }
    else
    {
        // serialize # of items
        size_t numItems = list_items(list);
        utl::serialize(numItems, stream, io, mode);

        // serialize contained objects
        ListNode* node = list;
        bool hasSentinelTail = false;
        while (node != nullptr)
        {
            if (node->isSentinelTail())
            {
                hasSentinelTail = true;
                break;
            }
            Object* object = node->get();
            object->serializeOutBoxed(stream, mode);
            node = node->next();
        }

        // serialize hasSentinelTail flag
        utl::serialize(hasSentinelTail, stream, io, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
list_clear(ListNode*& list, ListNode** tail, bool owner)
{
    ListNode* node = list;
    while (node != nullptr)
    {
        ListNode* nextNode = node->next();
        Object* object = node->get();
        if (owner && (object != maxObject))
            delete object;
        delete node;
        node = nextNode;
    }
    list = nullptr;
    if (tail != nullptr)
        *tail = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
list_items(const ListNode* list)
{
    size_t count = 0;
    const ListNode* node = list;
    while ((node != nullptr) && !node->isSentinelTail())
    {
        count++;
        node = node->next();
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
list_tail(ListNode* list)
{
    ListNode* tail = list;
    if (tail == nullptr)
        return nullptr;
    while (!tail->isTail())
        tail = tail->next();
    return tail;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const ListNode*
list_tail(const ListNode* list)
{
    const ListNode* tail = list;
    if (tail == nullptr)
        return nullptr;
    while (!tail->isTail())
        tail = tail->next();
    return tail;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
list_add(
    ListNode*& list, ListNode** tail, const Object* object, bool sorted, const Ordering* ordering)
{
    ListNode* newNode = new ListNode(object);

    // empty list ?
    if (list == nullptr)
    {
        list = newNode;
        if (tail != nullptr)
            *tail = newNode;
        return;
    }

    ListNode* ip;
    int cmpRes;
    if (sorted)
    {
        ip = list_findInsertionPoint(list, *object, sorted, ordering, &cmpRes);
    }
    else
    {
        if (tail == nullptr)
        {
            ip = list;
            cmpRes = 1;
        }
        else
        {
            ASSERTD(*tail != nullptr);
            ip = *tail;
            cmpRes = ip->isSentinelTail() ? 1 : -1;
        }
    }
    if (cmpRes < 0)
    {
        newNode->addAfter(ip);
        if ((tail != nullptr) && (ip == *tail))
        {
            *tail = newNode;
        }
    }
    else
    {
        newNode->addBefore(ip);
        if (ip == list)
        {
            list = newNode;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
list_remove(ListNode*& list,
            ListNode** tail,
            const Object& key,
            bool owner,
            bool sorted,
            const Ordering* ordering)
{
    auto node = list_findNode(list, key, sorted, ordering);
    if (node == nullptr)
    {
        return false;
    }
    else
    {
        list_removeNode(list, tail, node, owner);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
list_removeNode(ListNode*& list, ListNode** tail, ListNode* node, bool owner)
{
    // removing head node?
    if (node == list)
    {
        list = node->next();
    }

    // delete the object if we own it
    if (owner)
    {
        delete node->get();
    }

    // fix tail
    if ((tail != nullptr) && (*tail == node))
    {
        *tail = node->prev();
    }

    // unlink a node and delete it
    node->remove();
    delete node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
list_find(ListNode* list, const Object& key, bool sorted, const Ordering* ordering)
{
    auto node = list_findNode(list, key, sorted, ordering);
    if (node == nullptr)
    {
        return nullptr;
    }
    else
    {
        return node->get();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
list_findInsertionPoint(
    ListNode* list, const Object& key, bool sorted, const Ordering* ordering, int* cmpRes)
{
    auto node = list;
    int cmp;
    for (;;)
    {
        auto object = node->get();
        cmp = utl::compare(object, &key, ordering);
        if ((cmp == 0) || (sorted && (cmp > 0)))
        {
            break;
        }
        if (node->next() == nullptr)
        {
            break;
        }
        node = node->next();
    }
    if (cmpRes != nullptr)
        *cmpRes = cmp;
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
list_findNode(ListNode* list, const Object& key, bool sorted, const Ordering* ordering)
{
    auto node = list_findInsertionPoint(list, key, sorted, ordering);
    return (utl::compare(node->get(), key, ordering) == 0) ? node : nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
