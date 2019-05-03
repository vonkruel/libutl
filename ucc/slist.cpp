#include <libutl/libutl.h>
#include <libutl/slist.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
slist_compare(const SlistNode* lhs, const SlistNode* rhs, const Ordering* ordering)
{
    if (lhs == rhs)
    {
        return 0;
    }

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
slist_clone(SlistNode*& outList, SlistNode** outTail, const SlistNode* inList, bool owner)
{
    const SlistNode* srcNode = inList;
    SlistNode* dstNode = nullptr;
    SlistNode* dstHead = nullptr;

    while (srcNode != nullptr)
    {
        Object* object = srcNode->get();
        if (owner && (object != maxObject))
        {
            object = object->clone();
        }
        SlistNode* newNode = new SlistNode(object);
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
slist_serialize(
    SlistNode*& list, SlistNode** tail, bool owner, Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        slist_clear(list, tail, owner);

        // determine # of items
        size_t items;
        utl::serialize(items, stream, io, mode);

        // serialize objects
        SlistNode* node = nullptr;

        size_t i;
        for (i = 0; i < items; i++)
        {
            Object* object = Object::serializeInBoxed(stream, mode);
            SlistNode* newNode = new SlistNode(object);
            if (tail != nullptr)
                *tail = newNode;
            if (node == nullptr)
            {
                list = node = new SlistNode(object);
            }
            else
            {
                newNode->addAfter(node);
                node = newNode;
            }
        }

        bool hasSentinalTail;
        utl::serialize(hasSentinalTail, stream, io, mode);
        if (hasSentinalTail)
        {
            SlistNode* newNode = new SlistNode(maxObject);
            if (tail != nullptr)
                *tail = newNode;
            newNode->addAfter(node);
        }
    }
    else
    {
        // serialize # of items
        size_t numItems = slist_items(list);
        utl::serialize(numItems, stream, io, mode);

        // serialize contained objects
        SlistNode* node = list;
        bool hasSentinalTail = false;
        while (node != nullptr)
        {
            if (node->isSentinelTail())
            {
                hasSentinalTail = true;
                break;
            }
            Object* object = node->get();
            object->serializeOutBoxed(stream, mode);
            node = node->next();
        }
        utl::serialize(hasSentinalTail, stream, io, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
slist_clear(SlistNode*& list, SlistNode** tail, bool owner)
{
    SlistNode* node = list;
    while (node != nullptr)
    {
        SlistNode* nextNode = node->next();
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
slist_items(const SlistNode* list)
{
    size_t count = 0;
    const SlistNode* node = list;
    while ((node != nullptr) && !node->isSentinelTail())
    {
        count++;
        node = node->next();
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SlistNode*
slist_tail(SlistNode* list)
{
    SlistNode* tail = list;
    if (tail == nullptr)
        return nullptr;
    while (!tail->isTail())
        tail = tail->next();
    return tail;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const SlistNode*
slist_tail(const SlistNode* list)
{
    const SlistNode* tail = list;
    if (tail == nullptr)
        return nullptr;
    while (!tail->isTail())
        tail = tail->next();
    return tail;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
slist_add(
    SlistNode*& list, SlistNode** tail, const Object* object, bool sorted, const Ordering* ordering)
{
    SlistNode* newNode = new SlistNode(object);

    // empty list ?
    if (list == nullptr)
    {
        list = newNode;
        if (tail != nullptr)
            *tail = newNode;
        return;
    }

    SlistNode* ip;
    int cmpRes;
    if (sorted)
    {
        ip = slist_findInsertionPoint(list, *object, sorted, ordering, &cmpRes);
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
    }
    else
    {
        newNode->addBefore(ip);
        // even if adding before the head node, the head node
        // is unchanged (see SlistNode::addBefore())
    }

    // fix tail ?
    if ((tail != nullptr) && ((*tail)->next() != nullptr))
    {
        *tail = (*tail)->next();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
slist_remove(SlistNode*& list,
             SlistNode** tail,
             const Object& key,
             bool owner,
             bool sorted,
             const Ordering* ordering)
{
    SlistNode* prev;
    auto node = slist_findNode(list, key, sorted, ordering, &prev);
    if (node == nullptr)
    {
        return false;
    }
    else
    {
        slist_removeNode(list, tail, node, prev, owner);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
slist_removeNode(SlistNode*& list, SlistNode** tail, SlistNode* node, SlistNode* prev, bool owner)
{
    // removing head node?
    if (node == list)
    {
        list = list->next();
    }

    // delete the object if we own it
    if (owner)
    {
        delete node->get();
    }

    // unlink a node and delete it
    SlistNode* removedNode = node->remove(prev);
    if ((*tail != nullptr) && (*tail == removedNode))
    {
        ASSERTD(prev != nullptr);
        *tail = prev;
    }
    delete node->remove(prev);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
slist_find(SlistNode* list, const Object& key, bool sorted, const Ordering* ordering)
{
    auto node = slist_findNode(list, key, sorted, ordering);
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

SlistNode*
slist_findInsertionPoint(SlistNode* list,
                         const Object& key,
                         bool sorted,
                         const Ordering* ordering,
                         int* cmpRes,
                         SlistNode** prev)
{
    if (prev != nullptr)
        *prev = nullptr;
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
        if (prev != nullptr)
            *prev = node;
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

SlistNode*
slist_findNode(
    SlistNode* list, const Object& key, bool sorted, const Ordering* ordering, SlistNode** prev)
{
    auto node = slist_findInsertionPoint(list, key, sorted, ordering, nullptr, prev);
    return (utl::compare(node->get(), key, ordering) == 0) ? node : nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
