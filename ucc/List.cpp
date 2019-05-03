#include <libutl/libutl.h>
#include <libutl/List.h>
#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::List);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

List::List(bool owner, bool multiSet, bool keepSorted, Ordering* ordering)
{
    init(owner, multiSet, keepSorted, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::steal(Object& rhs_)
{
    auto& rhs = utl::cast<List>(rhs_);
    deInit();
    super::steal(rhs);
    _front = rhs._front;
    _back = rhs._back;
    rhs._front = rhs._back = new ListNode(&maxObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
List::innerAllocatedSize() const
{
    size_t sz = super::innerAllocatedSize();
    sz += (this->size() + 1) * sizeof(ListNode);
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::setOrdering(Ordering* ordering, uint_t algorithm)
{
    if (!isKeptSorted())
    {
        algorithm = sort_none;
    }
    super::setOrdering(ordering, algorithm);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::sort(uint_t algorithm)
{
    if (algorithm == sort_mergeSort)
    {
        _front = mergeSort(_front);
        _front->setPrev(nullptr);
    }
    else
    {
        super::sort(algorithm);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::moveBefore(const iterator& lhs, const iterator& rhs)
{
    // sanity-checking
    ASSERTD(lhs.isValid(this));
    ASSERTD(rhs.isValid(this));
    ASSERTD(lhs.hasSameOwner(rhs));

    ListNode* lhsNode = lhs.getNode();
    ListNode* rhsNode = rhs.getNode();

    // make sure this is sane
    ASSERTD(lhsNode != nullptr);
    ASSERTD(rhsNode != nullptr);
    ASSERTD(lhsNode != rhsNode);
    ASSERTD(lhsNode != _back);

    // nothing to do?
    if (lhsNode->next() == rhsNode)
    {
        return;
    }

    // unlink lhsNode
    lhsNode->remove();
    lhsNode->addBefore(rhsNode);
    if (rhsNode == _front)
    {
        _front = lhsNode;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
List::find(const Object& key) const
{
    ListNode* node = findNode(key);
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

List::iterator
List::findIt(const Object& key) const
{
    iterator it = const_cast_this->findIt(key);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findIt(const Object& key)
{
    ListNode* node = findNode(key);
    if (node == nullptr)
    {
        node = _back;
    }
    return iterator(this, node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::findIt(const Object& key, BidIt& it)
{
    iterator res = findIt(key);
    it = res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::findEqualRange(const Object& key, BidIt& begin, BidIt& end)
{
    findFirstIt(key, begin);
    if (begin.isEnd())
    {
        end = begin;
    }
    else
    {
        linearSearchSorted(begin, this->end(), key, end, ordering(), find_last);
        ++end;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::findFirstIt(const Object& key, BidIt& it, bool insert)
{
    it = findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findFirstIt(const Object& key, bool insert) const
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findFirstIt(const Object& key, bool insert)
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::findLastIt(const Object& key, BidIt& it)
{
    it = findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findLastIt(const Object& key) const
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findLastIt(const Object& key)
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
List::add(const Object* object)
{
    if (isKeptSorted())
    {
        ListNode* node = findInsertionPoint(*object);
        if (!isMultiSet() && (compareObjects(node->get(), object) == 0))
        {
            if (isOwner())
                delete object;
            return false;
        }
        ListNode* ln = new ListNode(object);
        ln->addBefore(node);
        if (node == _front)
        {
            _front = ln;
        }
        ++_items;
    }
    else
    {
        if (!isMultiSet() && (find(*object) != nullptr))
        {
            if (isOwner())
                delete object;
            return false;
        }
        pushBack(object);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::addBefore(const Object* object, const iterator& it)
{
    ASSERTD(it.isValid(this));
    ListNode* node = it.getNode();
    ListNode* ln = new ListNode(object);
    ln->addBefore(node);
    if (node == _front)
    {
        _front = node;
    }
    ++_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::pushFront(const Object* object)
{
    ListNode* ln = new ListNode(object);
    ln->addBefore(_front);
    _front = ln;
    ++_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::pushBack(const Object* object)
{
    ListNode* ln = new ListNode(object);
    ln->addBefore(_back);
    if (_front == _back)
    {
        _front = ln;
    }
    ++_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::insert(const Object* object, const BidIt& it_)
{
    auto& it = utl::cast<iterator>(it_.getProxiedObject());
    IFDEBUG(checkInsert(object, it, isSorted()));
    addBefore(object, it);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::clear()
{
    ListNode* curNode = _front;
    while (curNode != _back)
    {
        ListNode* next = curNode->next();
        if (isOwner())
        {
            delete curNode->get();
        }
        delete curNode;
        curNode = next;
    }

    _front = _back;
    _back->setPrev(nullptr);
    _items = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
List::remove(const Object& key)
{
    ListNode* node = findNode(key);
    if (node == nullptr)
    {
        return false;
    }
    else
    {
        removeNode(node);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::removeIt(BidIt& it)
{
    ASSERTD(*it != nullptr);
    auto& lit = utl::cast<iterator>(it.getProxiedObject());
    ASSERTD(lit.isValid(this));
    ListNode* node = lit.getNode();
    lit.setNode(node->next());
    removeNode(node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
List::removeFront()
{
    if (_items == 0)
    {
        return false;
    }
    else
    {
        removeNode(_front);
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
List::removeBack()
{
    if (_items == 0)
    {
        return false;
    }
    else
    {
        removeNode(_back->prev());
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
List::popFront()
{
    if (_front == _back)
        return nullptr;
    Object* res = _front->get();
    _front->set(nullptr);
    removeNode(_front);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
List::popBack()
{
    if (_front == _back)
        return nullptr;
    ListNode* node = _back->prev();
    Object* res = node->get();
    node->set(nullptr);
    removeNode(node);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::init(bool owner, bool multiSet, bool keepSorted, Ordering* ordering)
{
    setOwner(owner);
    setMultiSet(multiSet);
    setKeepSorted(keepSorted);
    if (ordering != nullptr)
        setOrdering(ordering, sort_none);
    _front = _back = new ListNode(&maxObject);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::deInit()
{
    clear();
    delete _front;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
List::findInsertionPoint(const Object& key) const
{
    iterator it = findIt(key, find_ip);
    return it.getNode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findIt(const Object& key, uint_t findType) const
{
    iterator it = const_cast_this->findIt(key, findType);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

List::iterator
List::findIt(const Object& key, uint_t findType)
{
    iterator it;
    if (isSorted())
    {
        linearSearchSorted(begin(), end(), key, it, ordering(), findType);
    }
    else
    {
        linearSearch(begin(), end(), key, it, ordering(), findType);
    }
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
List::findNode(const Object& key) const
{
    iterator it = findIt(key, find_first);
    if (it.isEnd())
    {
        return nullptr;
    }
    return it.getNode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
List::insertionSort(ListNode* node)
{
    auto nodePrev = node->prev();
    auto cur = node->next();

    // while un-inserted nodes remain
    while (cur != _back)
    {
        // cur's successor will be its next value
        auto curNext = cur->next();

        // unlink cur and find its new successor
        auto succ = cur->prev();
        cur->remove();
        for (;;)
        {
            // cur's object >= succ's object -> successor found
            if (compareObjects(cur->get(), succ->get()) >= 0)
            {
                // advance succ to find cur's successor
                succ = succ->next();
                break;
            }
            // cur's object is < succ's

            // don't insert before any node before the node given to this function
            if (succ->prev() == nodePrev)
            {
                node = cur;
                break;
            }
            succ = succ->prev();
        }

        // insert cur before succ and advance cur
        cur->addBefore(succ);
        cur = curNext;
    }
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
List::merge(ListNode* lhs, ListNode* rhs)
{
    ListNode dummy;
    ListNode* out = &dummy;

    auto lhsAtEnd = (lhs == _back);
    auto rhsAtEnd = (rhs == _back);
    while (!(lhsAtEnd && rhsAtEnd))
    {
        // lhs object <= rhs object?
        if (rhsAtEnd || (compareObjects(lhs->get(), rhs->get()) <= 0))
        {
            // out <--> lhs, out = lhs
            out->setNext(lhs);
            lhs->setPrev(out);
            out = lhs;
            // advance lhs
            lhs = lhs->next();
            lhsAtEnd = (lhs == _back);
        }
        else
        {
            // out <--> rhs, out = rhs
            out->setNext(rhs);
            rhs->setPrev(out);
            out = rhs;
            // advance rhs
            rhs = rhs->next();
            rhsAtEnd = (rhs == _back);
        }
    }

    auto head = dummy.next();
    head->setPrev(nullptr);
    return head;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ListNode*
List::mergeSort(ListNode* node)
{
    if (node->next() == _back)
        return node;

    ListNode* lhsBegin = node;
    ListNode* lhsEnd = node;
    ListNode* rhsBegin = _back;
    ListNode* rhsEnd = _back->prev();

    // find lhsEnd and rhsBegin
    auto lhsEndNext = lhsEnd->next();
    auto rhsBeginPrev = rhsBegin->prev();

    size_t count = 1;
    while (lhsEndNext != rhsBegin)
    {
        // advance lhsEnd
        lhsEnd = lhsEndNext;
        lhsEndNext = lhsEnd->next();
        if (lhsEndNext == rhsBegin)
        {
            ++count;
            break;
        }
        else
        {
            // retreat rhsBegin
            rhsBegin = rhsBeginPrev;
            rhsBeginPrev = rhsBegin->prev();
            count += 2;
        }
    }

    // insertion-sort a small segment
    if (count <= 16)
    {
        return insertionSort(lhsBegin);
    }
    else
    {
        // recursively merge-sort [lhsBegin, .., lhsEnd]
        lhsEnd->setNext(_back);
        _back->setPrev(lhsEnd);
        auto aSeg = mergeSort(lhsBegin);

        // resursively merge-sort [rhsBegin, .., rhsEnd]
        rhsBegin->setPrev(nullptr);
        _back->setPrev(rhsEnd);
        auto bSeg = mergeSort(rhsBegin);

        // merge the two sorted halves together
        return merge(aSeg, bSeg);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
List::removeNode(ListNode* node)
{
    // remove the node
    ListNode* next = node->next();
    node->remove();
    Object* object = node->get();
    if (isOwner())
        delete object;
    --_items;
    delete node;

    // might have changed the front of the list
    // (can't ever change the back b/c it's a sentinel)
    if (next->prev() == nullptr)
    {
        _front = next;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
