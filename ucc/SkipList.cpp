#include <libutl/libutl.h>
#include <libutl/SkipList.h>
#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::SkipList);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::steal(Object& rhs_)
{
    auto& rhs = utl::cast<SkipList>(rhs_);
    deInit();
    super::steal(rhs);
    _head = rhs._head;
    _tail = rhs._tail;
    _level = rhs._level;
    _maxLevel = rhs._maxLevel;
#ifdef UTL_SKIPLIST_RNG
    _rng = rhs._rng;
#else
    _counter = rhs._counter;
    _counterLim = rhs._counterLim;
#endif
    rhs.init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
SkipList::innerAllocatedSize() const
{
    size_t sz = super::innerAllocatedSize();
    sz += sizeof(SkipListNode); // tail

    // initially count the size of nodes at level 0
    SkipListNode* node;
    for (node = _head; node != _tail; node = node->next(0))
    {
        sz += sizeof(SkipListNode);
    }

    // add sizeof(SkipListNode*) for each higher-level node pointer
    for (uint_t lvl = 1; lvl <= _level; ++lvl)
    {
        for (node = _head; node != _tail; node = node->next(lvl))
        {
            sz += sizeof(SkipListNode*);
        }
    }

    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
SkipList::find(const Object& key) const
{
    SkipListNode* node = findNode(key);
    if (node == nullptr)
        return nullptr;
    return node->get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findIt(const Object& key) const
{
    SkipListIt it = const_cast_this->findIt(key);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findIt(const Object& key)
{
    SkipListNode* node = findNode(key);
    if (node == nullptr)
    {
        node = _tail;
    }
    return SkipListIt(this, node);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::findIt(const Object& key, BidIt& it)
{
    SkipListIt res = findIt(key);
    it = res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::findFirstIt(const Object& key, BidIt& it, bool insert)
{
    it = findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findFirstIt(const Object& key, bool insert) const
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findFirstIt(const Object& key, bool insert)
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::findLastIt(const Object& key, BidIt& it)
{
    it = findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findLastIt(const Object& key) const
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findLastIt(const Object& key)
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SkipList::add(const Object* object)
{
// determine level of new node
#ifdef UTL_SKIPLIST_RNG
    uint_t lvl;
    for (lvl = 0; (_rng->uniform(0.0, 1.0) >= 0.5) && (lvl < _maxLevel); ++lvl)
        ;
#else
    uint32_t c = ++_counter;
    if (_counter == _counterLim)
        _counter = 0;
    uint_t lvl = 0;
    while (!(c & 1))
    {
        c >>= 1;
        ++lvl;
    }
#endif
    ASSERTD(lvl <= _maxLevel);

    // make new node
    uint_t newNodeSize = sizeof(SkipListNode) + (lvl * sizeof(SkipListNode*));
    SkipListNode* newNode = (SkipListNode*)(new byte_t[newNodeSize]);
    newNode->set(object);

    // integrate the new node into the structure
    SkipListNode* node = _head;
    for (uint_t i = _level; i != uint_t_max; --i)
    {
        SkipListNode* next = node->next(i);
        int cmpRes;
        while ((cmpRes = compareObjects(next->get(), *object)) < 0)
        {
            node = next;
            next = node->next(i);
        }
        if (i > lvl)
            continue;
        if (i == 0)
        {
            // duplicates only allowed in multi-set
            if ((cmpRes == 0) && !isMultiSet())
            {
                if (isOwner())
                    delete object;
                delete[](byte_t*) newNode;
                return false;
            }
            newNode->setPrev(node);
            next->setPrev(newNode);
        }
        newNode->setNext(i, next);
        node->setNext(i, newNode);
    }
    // increasing _level -> link between head & tail at levels > _level
    if (lvl > _level)
    {
        for (uint_t i = _level + 1; i <= lvl; ++i)
        {
            _head->setNext(i, newNode);
            newNode->setNext(i, _tail);
        }
        _level = lvl;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::insert(const Object* object, const BidIt& it_)
{
    auto& it = utl::cast<SkipListIt>(it_.getProxiedObject());
    ASSERTD(it.update() != nullptr);
    IFDEBUG(checkInsert(object, it, true));
    SkipListNode* node;
    SkipListNode** update = it.update();

// determine level of new node
#ifdef UTL_SKIPLIST_RNG
    uint_t lvl;
    for (lvl = 0; (_rng->uniform(0.0, 1.0) >= 0.5) && (lvl < _maxLevel); ++lvl)
        ;
#else
    uint32_t c = ++_counter;
    if (_counter == _counterLim)
        _counter = 0;
    uint_t lvl = 0;
    while (!(c & 1))
    {
        c >>= 1;
        ++lvl;
    }
#endif
    ASSERTD(lvl <= _maxLevel);

    // increase global maximum level?
    if (lvl > _level)
    {
        for (uint_t i = _level + 1; i <= lvl; i++)
        {
            update[i] = _head;
        }
        _level = lvl;
    }

    // make new node
    uint_t nodeSize = sizeof(SkipListNode) + (lvl * sizeof(SkipListNode*));
    node = (SkipListNode*)(new byte_t[nodeSize]);
    node->set(object, lvl, update);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::clear()
{
    // delete all nodes except _head and _tail
    SkipListNode* curNode = _head->next();
    while (curNode != _tail)
    {
        SkipListNode* next = curNode->next();
        if (isOwner())
            delete curNode->get();
        delete curNode;
        curNode = next;
    }

    _tail->_prev = _head;
    uint_t i;
    for (i = 0; i <= _maxLevel; i++)
    {
        _head->_next[i] = _tail;
    }
    _items = 0;
    _level = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SkipList::remove(const Object& key)
{
    // find the node
    ASSERTD(_maxLevel < 32);
    SkipListNode* update[32];
    SkipListNode* node = findNode(key, find_ip, update);
    node = node->next();

    // removal fails if key not found
    if (compareObjects(node->get(), key) != 0)
    {
        return false;
    }

    removeNode(node, update);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::removeIt(BidIt& it)
{
    ASSERTD(*it != nullptr);
    auto& lit = utl::cast<SkipListIt>(it.getProxiedObject());
    ASSERTD(lit.isValid(this));
    SkipListNode* node = lit.node();
    SkipListNode* nodeNext = node->next();

    // get update links
    ASSERTD(_maxLevel < 32);
    SkipListNode* update[32];
    SkipListNode* searchNode = findNode(*(node->get()), find_ip, update);
    searchNode = searchNode->next();
    ASSERTD(compareObjects(searchNode->get(), node->get()) == 0);

    if (searchNode != node)
    {
        Object* saveObject = searchNode->get();
        searchNode->set(node->get());
        node->set(saveObject);
    }

    removeNode(searchNode, update);
    lit.setNode(nodeNext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::init(bool owner, bool multiSet, Ordering* ordering, uint_t maxLevel)
{
    setOwner(owner);
    setMultiSet(multiSet);
    setOrdering(ordering, sort_none);
    _maxLevel = maxLevel;

    // create head & tail
    uint_t headSize = sizeof(SkipListNode) + _maxLevel * sizeof(SkipListNode*);
    _head = reinterpret_cast<SkipListNode*>(new byte_t[headSize]);
    uint_t tailSize = sizeof(SkipListNode);
    _tail = reinterpret_cast<SkipListNode*>(new byte_t[tailSize]);
    _head->_object = nullptr;
    _head->_prev = nullptr;
    uint_t i;
    for (i = 0; i <= _maxLevel; i++)
    {
        _head->_next[i] = _tail;
    }
    _tail->_object = const_cast<MaxObject*>(&maxObject);
    _tail->_prev = _head;
    _tail->_next[0] = nullptr;

    // list level = 0 initially
    _level = 0;

#ifdef UTL_SKIPLIST_RNG
    // init prng
    _rng = new randutils::default_rng();
#else
    // init counter
    _counter = 1;
    _counterLim = (uint32_t)1 << _maxLevel;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::deInit()
{
    clear();
    delete[](byte_t*) _head;
    delete[](byte_t*) _tail;
#ifdef UTL_SKIPLIST_RNG
    delete _rng;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findIt(const Object& key, uint_t findType) const
{
    iterator it = const_cast_this->findIt(key, findType);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipList::iterator
SkipList::findIt(const Object& key, uint_t findType)
{
    if (findType == find_ip)
    {
        SkipListNode** update = new SkipListNode*[_maxLevel + 1];
        return iterator(this, findNode(key, findType, update)->next(), update);
    }
    else
    {
        return iterator(this, findNode(key, findType));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SkipListNode*
SkipList::findNode(const Object& key, uint_t findType, SkipListNode** update) const
{
    uint_t lvl;
    SkipListNode* node = _head;
    if (findType == find_ip)
    {
        ASSERTD(update != nullptr);
        for (lvl = _level; lvl != uint_t_max; lvl--)
        {
            SkipListNode* next = node->next(lvl);
            while (compareObjects(next->get(), key) < 0)
            {
                node = next;
                next = node->next(lvl);
            }
            update[lvl] = node;
        }
        return node;
    }
    else if (findType == find_first)
    {
        for (lvl = _level; lvl != uint_t_max; lvl--)
        {
            SkipListNode* next = node->next(lvl);
            while (compareObjects(next->get(), key) < 0)
            {
                node = next;
                next = node->next(lvl);
            }
        }
        node = node->next();
        if (compareObjects(node->get(), key) == 0)
        {
            return node;
        }
        else
        {
            return nullptr;
        }
    }
    else // find_last
    {
        ASSERTD(findType == find_last);
        for (lvl = _level; lvl != uint_t_max; lvl--)
        {
            SkipListNode* next = node->next(lvl);
            while (compareObjects(next->get(), key) <= 0)
            {
                node = next;
                next = node->next(lvl);
            }
        }
        if (compareObjects(node->get(), key) == 0)
        {
            return node;
        }
        else
        {
            return nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
SkipList::removeNode(SkipListNode* node, SkipListNode** update)
{
    // remove the node
    Object* object = node->get();
    node->remove(_level, update);
    if (isOwner())
        delete object;
    _items--;
    delete[](byte_t*) node;

    // adjust level
    while ((_level > 0) && (_head->next(_level) == _tail))
    {
        _level--;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
