#include <libutl/libutl.h>
#include <libutl/Deque.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

// if you change this, also change it in DequeIt
#define BLOCK_SIZE 1024

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::Deque);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::Deque(bool owner, Ordering* ordering)
{
    init(owner, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::steal(Object& rhs_)
{
    auto& rhs = utl::cast<Deque>(rhs_);
    deInit();
    super::steal(rhs);
    _blocks = rhs._blocks;
    _blocksLim = rhs._blocksLim;
    _beginBlock = rhs._beginBlock;
    _endBlock = rhs._endBlock;
    _beginPos = rhs._beginPos;
    _endPos = rhs._endPos;
    rhs.init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Deque::innerAllocatedSize() const
{
    size_t sz = super::innerAllocatedSize();
    size_t numBlocks = (_blocksLim - _blocks);
    sz += numBlocks * sizeof(Object**);
    sz += numBlocks * (BLOCK_SIZE * sizeof(Object*));
    return sz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Deque::get(size_t idx) const
{
    ASSERTD(idx < this->items());
    size_t pos = (size_t)_beginPos + idx;
    size_t blockIdx = pos / BLOCK_SIZE;
    size_t blockPos = pos % BLOCK_SIZE;
    size_t numBlocks = (_blocksLim - _blocks);
    blockIdx = ((_beginBlock - _blocks) + blockIdx) & (numBlocks - 1);
    return _blocks[blockIdx][blockPos];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::iterator
Deque::findIt(const Object& key) const
{
    auto it = const_cast_this->findIt(key);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Deque::iterator
Deque::findIt(const Object& key)
{
    DequeIt it;
    linearSearch(begin(), end(), key, it, ordering());
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::findIt(const Object& key, BidIt& it)
{
    auto res = findIt(key);
    it = res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Deque::add(const Object* object)
{
    return pushBack(object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Deque::pushFront(const Object* object)
{
    if (!isMultiSet() && has(*object))
    {
        if (isOwner())
            delete object;
        return false;
    }

    // move begin pointer back (grow if necessary)
    prev(_beginBlock, _beginPos);
    if ((_beginPos == (BLOCK_SIZE - 1)) && (_beginBlock == _endBlock))
    {
        next(_beginBlock, _beginPos);
        grow();
        prev(_beginBlock, _beginPos);
    }

    // object is new head
    ASSERTD((*_beginBlock)[_beginPos] == nullptr);
    (*_beginBlock)[_beginPos] = const_cast<Object*>(object);
    ++_items;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Deque::pushBack(const Object* object)
{
    if (!isMultiSet() && has(*object))
    {
        if (isOwner())
            delete object;
        return false;
    }

    // object is new tail
    ASSERTD((*_endBlock)[_endPos] == nullptr);
    (*_endBlock)[_endPos] = const_cast<Object*>(object);
    ++_items;

    // move end pointer forward (grow if necessary)
    next(_endBlock, _endPos);
    if ((_endPos == 0) && (_beginBlock == _endBlock))
    {
        prev(_endBlock, _endPos);
        grow();
        next(_endBlock, _endPos);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::clear()
{
    // TODO: optimize this
    while (removeFront())
        ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Deque::removeFront()
{
    if (_items == 0)
        return false;
    auto& ptr = (*_beginBlock)[_beginPos];
    if (isOwner())
        delete ptr;
    ptr = nullptr;
    next(_beginBlock, _beginPos);
    --_items;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Deque::removeBack()
{
    if (_items == 0)
        return false;
    prev(_endBlock, _endPos);
    auto& ptr = (*_endBlock)[_endPos];
    ASSERTD(ptr != nullptr);
    if (isOwner())
        delete ptr;
    ptr = nullptr;
    --_items;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Deque::popFront()
{
    if (_items == 0)
        return nullptr;
    auto& ptr = (*_beginBlock)[_beginPos];
    ASSERTD(ptr != nullptr);
    auto res = ptr;
    ptr = nullptr;
    next(_beginBlock, _beginPos);
    --_items;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Deque::popBack()
{
    if (_items == 0)
        return nullptr;
    prev(_endBlock, _endPos);
    auto& ptr = (*_endBlock)[_endPos];
    ASSERTD(ptr != nullptr);
    auto res = ptr;
    ptr = nullptr;
    --_items;
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::init(bool owner, Ordering* ordering)
{
    setOwner(owner);
    if (ordering != nullptr)
        setOrdering(ordering, sort_none);
    _blocks = new Object**[1];
    _blocks[0] = new Object*[BLOCK_SIZE];
    memset(_blocks[0], 0, BLOCK_SIZE * sizeof(Object*));
    _blocksLim = _blocks + 1;
    _beginBlock = _blocks;
    _endBlock = _blocks;
    _beginPos = 0;
    _endPos = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::deInit()
{
    if (isOwner())
        clear();
    for (auto ptr = _blocks; ptr != _blocksLim; ++ptr)
    {
        delete[] * ptr;
    }
    delete[] _blocks;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::grow()
{
    // when this is called, all blocks are in use

    // remember some things
    size_t oldNumBlocks = (_blocksLim - _blocks);
    Object*** oldBlocks = _blocks;
    Object*** oldBlocksLim = _blocksLim;
    Object*** oldBeginBlock = _beginBlock;
    size_t numBlocks = oldNumBlocks * 2;

    // make a new allocation twice as large
    _blocks = new Object**[numBlocks];
    _blocksLim = _blocks + numBlocks;
    _beginBlock = _blocks;
    _endBlock = _blocks + oldNumBlocks - 1;

    // copy the old block pointers into the new allocation
    Object*** lhsBlockPtr = _beginBlock;
    Object*** rhsBlockPtr = oldBeginBlock;
    size_t copyCount = (oldBlocksLim - oldBeginBlock);
    memcpy(lhsBlockPtr, rhsBlockPtr, copyCount * sizeof(Object**));
    lhsBlockPtr += copyCount;
    rhsBlockPtr = oldBlocks;
    copyCount = (oldNumBlocks - copyCount);
    memcpy(lhsBlockPtr, rhsBlockPtr, copyCount * sizeof(Object**));
    lhsBlockPtr += copyCount;

    // add new blocks
    ASSERTD((lhsBlockPtr + oldNumBlocks) == _blocksLim);
    for (; lhsBlockPtr != _blocksLim; ++lhsBlockPtr)
    {
        *lhsBlockPtr = new Object*[BLOCK_SIZE];
        memset(*lhsBlockPtr, 0, BLOCK_SIZE * sizeof(Object*));
    }

    // forget the old allocation
    delete[] oldBlocks;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::next(Object***& blockPtr, uint32_t& blockPos)
{
    if (++blockPos == BLOCK_SIZE)
    {
        blockPos = 0;
        if (++blockPtr == _blocksLim)
            blockPtr = _blocks;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Deque::prev(Object***& blockPtr, uint32_t& blockPos)
{
    if (--blockPos == uint32_t_max)
    {
        blockPos = BLOCK_SIZE - 1;
        if (--blockPtr < _blocks)
            blockPtr = _blocksLim - 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
