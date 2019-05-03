#include <libutl/libutl.h>
#include <libutl/Hashtable.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Hashtable //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::steal(Object& rhs_)
{
    auto& rhs = utl::cast<Hashtable>(rhs_);
    deInit();
    super::steal(rhs);
    _hashfn = rhs._hashfn;
    _limit = rhs._limit;
    _maxLF = rhs._maxLF;
    _array.steal(rhs._array);
    rhs._hashfn = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::vclone(const Object& rhs)
{
    const Hashtable& ht = cast<Hashtable>(rhs);
    ASSERTD(_hashfn == nullptr);
    if (ht._hashfn != nullptr)
    {
        setHashFunction(ht._hashfn->clone());
    }
    super::vclone(rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Hashtable::innerAllocatedSize() const
{
    auto sz = super::innerAllocatedSize();
    sz += _array.innerAllocatedSize();

    // add space for ListNodes
    size_t i;
    for (i = 0; i != _array.size(); ++i)
    {
        if (!isHead(i))
            continue;
        for (auto ln = getHead(i); ln != nullptr; ln = ln->next())
        {
            sz += sizeof(ListNode);
        }
    }

    // assume _hashfn is same size as utl::HashFunction
    if (_hashfn != nullptr)
        sz += sizeof(HashFunction);

    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::reserve(size_t reqSize)
{
    if (reqSize <= _limit)
        return;

    // compute a new size where the table is maxLF % full when containing reqSize objects
    //     e.g. if maxLF = 90 and reqSize = 90, then newSize = 100
    //
    // reqSize / newSize = maxLF / 100
    //           newSize = reqSize * (100 / maxLF)
    size_t newSize = (double)reqSize * (100.0 / (double)_maxLF);

    // grow to a prime size >= newSize
    grow(newSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Hashtable::find(const Object& key) const
{
    size_t idx;
    ListNode* node;
    find(key, idx, node);
    if (idx == size_t_max)
    {
        return nullptr;
    }
    if (likely(node == nullptr))
    {
        return getObject(idx);
    }
    else
    {
        return node->get();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Hashtable::iterator
Hashtable::findIt(const Object& key) const
{
    iterator it = const_cast_this->findIt(key);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Hashtable::iterator
Hashtable::findIt(const Object& key)
{
    size_t idx;
    ListNode* node;
    find(key, idx, node);
    if (idx == size_t_max)
        return end();
    return iterator(this, idx, node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::findIt(const Object& key, BidIt& it)
{
    it = findIt(key);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Hashtable::add(const Object* object)
{
    // grow if necessary
    auto size = _array.size();
    if (_items >= _limit)
        grow(size + 1);

    // calculate the hash
    auto idx = hash(object);

    // Object* at this index?
    if (likely(isObject(idx)))
    {
        auto tableObj = getObject(idx);
        if (tableObj == nullptr)
        {
            setObject(idx, const_cast<Object*>(object));
        }
        else
        {
            if (!isMultiSet() && (compareObjects(tableObj, object) == 0))
            {
                if (isOwner())
                    delete object;
                return false;
            }
            // single object -> list
            auto head = new ListNode(tableObj);
            list_add(head, nullptr, object, true, ordering());
            setHead(idx, head);
        }
    }
    else
    {
        // grow the list
        auto head = getHead(idx);
        int cmpRes;
        auto node = list_findInsertionPoint(head, *object, true, ordering(), &cmpRes);
        if (!isMultiSet() && (cmpRes == 0))
        {
            if (isOwner())
                delete object;
            return false;
        }
        auto newNode = new ListNode(object);
        if (cmpRes < 0)
        {
            newNode->addAfter(node);
        }
        else
        {
            ASSERTD(cmpRes >= 0);
            newNode->addBefore(node);
            if (node == head)
                setHead(idx, newNode);
        }
    }

    ++_items;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Hashtable::addOrFind(const Object* object)
{
    // grow if necessary
    auto size = _array.size();
    if (_items >= _limit)
        grow(size + 1);

    // calculate the hash
    auto idx = hash(object);
    Object* res;

    // Object* at this index?
    if (likely(isObject(idx)))
    {
        auto tableObj = getObject(idx);
        if (tableObj == nullptr)
        {
            res = setObject(idx, const_cast<Object*>(object));
        }
        else
        {
            if (!isMultiSet() && (compareObjects(tableObj, object) == 0))
            {
                if (isOwner())
                    delete object;
                return tableObj;
            }
            // single object -> list
            auto head = new ListNode(tableObj);
            list_add(head, nullptr, object, true, ordering());
            setHead(idx, head);
        }
    }
    else
    {
        // grow the list
        auto head = getHead(idx);
        int cmpRes;
        auto node = list_findInsertionPoint(head, *object, true, ordering(), &cmpRes);
        if (!isMultiSet() && (cmpRes == 0))
        {
            if (isOwner())
                delete object;
            return node->get();
        }
        auto newNode = new ListNode(object);
        if (cmpRes < 0)
        {
            newNode->addAfter(node);
        }
        else
        {
            ASSERTD(cmpRes >= 0);
            newNode->addBefore(node);
            if (node == head)
                setHead(idx, newNode);
        }
    }

    ++_items;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Hashtable::addOrUpdate(const Object* object)
{
    // grow if necessary
    auto size = _array.size();
    if (_items >= _limit)
        grow(size + 1);

    // calculate the hash
    auto idx = hash(object);

    // Object* at this index?
    if (likely(isObject(idx)))
    {
        auto tableObj = getObject(idx);
        if (tableObj == nullptr)
        {
            setObject(idx, const_cast<Object*>(object));
        }
        else
        {
            if (!isMultiSet() && (compareObjects(tableObj, object) == 0))
            {
                if (isOwner())
                    delete tableObj;
                setObject(idx, const_cast<Object*>(object));
                return false;
            }
            // single object -> list
            auto head = new ListNode(tableObj);
            list_add(head, nullptr, object, true, ordering());
            setHead(idx, head);
        }
    }
    else
    {
        // grow the list
        auto head = getHead(idx);
        int cmpRes;
        auto node = list_findInsertionPoint(head, *object, true, ordering(), &cmpRes);
        if (!isMultiSet() && (cmpRes == 0))
        {
            if (isOwner())
                delete node->get();
            node->set(object);
            return false;
        }
        auto newNode = new ListNode(object);
        if (cmpRes < 0)
        {
            newNode->addAfter(node);
        }
        else
        {
            ASSERTD(cmpRes >= 0);
            newNode->addBefore(node);
            if (node == head)
                setHead(idx, newNode);
        }
    }

    ++_items;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::clear()
{
    size_t i, lim = _array.size();
    for (i = 0; i != lim; ++i)
    {
        if (likely(isObject(i)))
        {
            auto tableObj = getObject(i);
            if (tableObj == nullptr)
                continue;
            if (isOwner())
                delete tableObj;
        }
        else
        {
            auto head = getHead(i);
            list_clear(head, nullptr, isOwner());
        }
        setObject(i, nullptr);
    }
    _items = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::excise()
{
    _limit = 0;
    clear();
    _array.excise();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Hashtable::remove(const Object& key)
{
    size_t idx;
    ListNode* node;
    find(key, idx, node);
    if (idx == size_t_max)
        return false;
    remove(idx, node);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::removeIt(BidIt& it)
{
    ASSERTD(*it != nullptr);
    auto& hit = utl::cast<iterator>(it.getProxiedObject());
    ASSERTD(hit.isValid(this));
    auto idx = hit.getIdx();
    auto node = const_cast<ListNode*>(hit.getNode());
    ++hit;
    remove(idx, node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Hashtable::iterator
Hashtable::begin() const
{
    if (_items == 0)
        return end();
    ListNode* node;
    bool advance;
    if (likely(isObject(0)))
    {
        node = nullptr;
        advance = (getObject(0) == nullptr);
    }
    else
    {
        node = getHead(0);
        advance = false;
    }
    iterator it(this, 0, node);
    IFDEBUG(it.setConst(true));
    if (advance)
        ++it;
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Hashtable::iterator
Hashtable::begin()
{
    if (_items == 0)
        return end();
    ListNode* node;
    bool advance;
    if (likely(isObject(0)))
    {
        node = nullptr;
        advance = (getObject(0) == nullptr);
    }
    else
    {
        node = getHead(0);
        advance = false;
    }
    iterator it(this, 0, node);
    if (advance)
        ++it;
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Hashtable::beginNew() const
{
    if (_items == 0)
        return endNew();
    ListNode* node;
    bool advance;
    if (likely(isObject(0)))
    {
        node = nullptr;
        advance = (getObject(0) == nullptr);
    }
    else
    {
        node = getHead(0);
        advance = false;
    }
    auto it = new iterator(this, 0, node);
    IFDEBUG(it->setConst(true));
    if (advance)
        it->forward();
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Hashtable::beginNew()
{
    if (_items == 0)
        return endNew();
    ListNode* node;
    bool advance;
    if (likely(isObject(0)))
    {
        node = nullptr;
        advance = (getObject(0) == nullptr);
    }
    else
    {
        node = getHead(0);
        advance = false;
    }
    auto it = new iterator(this, 0, node);
    if (advance)
        it->forward();
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Hashtable::endNew() const
{
    auto it = new iterator(this, size_t_max, nullptr);
    IFDEBUG(it->setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BidIt*
Hashtable::endNew()
{
    return new iterator(this, size_t_max, nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::init(size_t size, uint_t maxLF, bool owner, bool multiSet, const HashFunction* hashfn)
{
    _hashfn = hashfn;
    _limit = 0;
    _maxLF = maxLF;
    setOwner(owner);
    setMultiSet(multiSet);

    // Set up the pip array
    _array.setAutoInit(true);
    _array.setIncrement(1);
    ASSERTD(_array.size() == 0);
    reserve(size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::deInit()
{
    clear();
    delete _hashfn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::grow(size_t newSize)
{
    // do nothing if newSize <= size
    size_t size = _array.size();
    if (newSize <= size)
        return;

    // fix newSize
    size_t i;
    for (i = 0; primes[i] < newSize; ++i)
        ;
    newSize = primes[i];
    ASSERTD(newSize != size_t_max);

    // remember old _array
    auto oldArray = _array.get();

    // re-initialize _array
    _array.setOwner(false);
    _array.excise();
    _array.setOwner(true);
    _array.setSize(newSize);

    // re-add objects from oldArray
    _items = 0;
    _limit = size_t_max;
    for (i = 0; i != size; i++)
    {
        auto pip = oldArray[i];
        if (likely(pip.getInt() == 0))
        {
            auto tableObj = pip.getPointer();
            if (tableObj == nullptr)
                continue;
            add(tableObj);
        }
        else
        {
            auto head = reinterpret_cast<ListNode*>(pip.getPointer());
            for_each_ln(head, Object, listObject);
            add(&listObject);
            for_each_end;
            list_clear(head, nullptr, false);
        }
    }

    // set _limit based on newSize & max-percentage
    _limit = (double)newSize * ((double)_maxLF / 100.0);

    delete[] oldArray;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::find(const Object& key, size_t& idx, ListNode*& node) const
{
    if (empty())
    {
        idx = size_t_max;
        return;
    }

    idx = hash(key);
    if (isObject(idx))
    {
        auto tableObj = getObject(idx);
        if (tableObj == nullptr)
        {
            node = nullptr;
            idx = size_t_max;
        }
        else
        {
            if (compareObjects(tableObj, key) == 0)
            {
                node = nullptr;
            }
            else
            {
                idx = size_t_max;
            }
        }
    }
    else
    {
        auto head = getHead(idx);
        node = list_findNode(head, key, true, ordering());
        if (node == nullptr)
            idx = size_t_max;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Hashtable::remove(size_t idx, ListNode* node)
{
    if (node == nullptr)
    {
        if (isOwner())
            delete getObject(idx);
        setObject(idx, nullptr);
    }
    else
    {
        auto head = getHead(idx);
        list_removeNode(head, nullptr, node, isOwner());
        setHead(idx, head);

        // if only one item left, get rid of list
        ASSERTD(head != nullptr);
        if (head->next() == nullptr)
        {
            auto object = head->get();
            list_clear(head, nullptr, false);
            setObject(idx, object);
        }
    }
    --_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// How these primes were selected:
//
// I used this page: http://www.numberempire.com/primenumbers.php
//
// Lower primes are chosen to be near the half-way point between powers of 2.
//     next prime after: 2^n + (2^n * 0.50)
//
// When we reach very large sizes (about 100 million), we start including primes near the 1/4 and
// 3/4 point between powers of 2.  This is to help prevent a severely oversized table.
//     next prime after: 2^n + (2^n * 0.25)
//                       2^n + (2^n * 0.50)
//                       2^n + (2^n * 0.75)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Hashtable::primes[] = {
    7,          13,         23,         53,         97,         193,        389,        769,
    1543,       3079,       6151,       12289,      24593,      49157,      98317,      196613,
    393241,     786433,     1572869,    3145739,    6291469,    12582917,   25165843,   50331653,
    100663319,  167772161,  201326611,  234881033,  335544323,  402653189,  469762049,  671088667,
    805306457,  939524129,  1342177283, 1610612741, 1879048201, 2684354591, 3221225473, 3758096411,
    5368709131, 6442450967, 7516192771, size_t_max};

////////////////////////////////////////////////////////////////////////////////////////////////////
// HashtableIt /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int
HashtableIt::compare(const Object& rhs) const
{
    auto& hit = utl::cast<HashtableIt>(rhs);
    ASSERTD(hasSameOwner(hit));
    int res = utl::compare(_idx, hit._idx);
    if (res != 0)
        return res;
    res = utl::compare(_node, hit._node);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HashtableIt::copy(const Object& rhs)
{
    auto& hit = utl::cast<HashtableIt>(rhs);
    super::copy(hit);
    _ht = hit._ht;
    _idx = hit._idx;
    _node = hit._node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HashtableIt::forward(size_t dist)
{
    ASSERTD(isValid(_ht));
    auto ht = _ht;
    auto& array = ht->_array;
    auto size = array.size();

    // can't advance any more?
    if (_idx >= size)
    {
        _idx = size_t_max;
        _node = nullptr;
        return;
    }

    auto idx = _idx;
    auto node = _node;
    while (dist-- > 0)
    {
        if ((node == nullptr) || (node->next() == nullptr))
        {
            while ((++idx != size) && (array[idx].get() == 0))
                ;
            if (idx == size)
            {
                _idx = size_t_max;
                _node = nullptr;
                return;
            }
            if (likely(ht->isObject(idx)))
            {
                node = nullptr;
            }
            else
            {
                node = ht->getHead(idx);
            }
        }
        else
        {
            node = node->next();
        }
    }
    _idx = idx;
    _node = node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
HashtableIt::get() const
{
    ASSERTD(isValid(_ht));
    auto& array = _ht->_array;
    auto size = array.size();
    if (_idx >= size)
        return nullptr;
    if (_node == nullptr)
        return _ht->getObject(_idx);
    return _node->get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HashtableIt::reverse(size_t dist)
{
    ASSERTD(isValid(_ht));
    auto ht = _ht;
    auto& array = ht->_array;
    auto size = array.size();

    if (_idx >= size)
    {
        _idx = size;
        ASSERTD(_node == nullptr);
    }

    auto idx = _idx;
    auto node = _node;
    while (dist-- > 0)
    {
        if ((node == nullptr) || (node->prev() == nullptr))
        {
            while ((--idx != size_t_max) && (array[idx].get() == 0))
                ;
            if (idx == size_t_max)
            {
                _idx = size_t_max;
                _node = nullptr;
                return;
            }
            if (likely(ht->isObject(idx)))
            {
                node = nullptr;
            }
            else
            {
                node = list_tail(ht->getHead(idx));
            }
        }
        else
        {
            node = node->prev();
        }
    }
    _idx = idx;
    _node = node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
HashtableIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_ht));

    // adding the object
    if (get() == nullptr)
    {
        ASSERTD(object != nullptr);
        _ht->add(object);
        return;
    }

    // removing the object
    if (object == nullptr)
    {
        _ht->removeIt((HashtableIt&)*this);
        return;
    }

    // replacing an object?
    ASSERTD(_ht->hash(object) == _idx);
    if (likely(_node == nullptr))
    {
        auto tableObj = _ht->getObject(_idx);
        if (object == tableObj)
            return;
        if (_ht->isOwner())
            delete tableObj;
        _ht->setObject(_idx, const_cast<Object*>(object));
    }
    else
    {
        auto tableObj = _node->get();
        if (object == tableObj)
            return;
        if (_ht->isOwner())
            delete tableObj;
        _node->set(object);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Hashtable);
UTL_CLASS_IMPL(utl::HashtableIt);
