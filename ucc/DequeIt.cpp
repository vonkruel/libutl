#include <libutl/libutl.h>
#include <libutl/Deque.h>
#include <libutl/MaxObject.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#define BLOCK_SIZE 1024

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::DequeIt);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

int
DequeIt::compare(const Object& rhs_) const
{
    auto& rhs = utl::cast<DequeIt>(rhs_);
    ASSERTD(hasSameOwner(rhs));
    int res = utl::compare(_blockPtr, rhs._blockPtr);
    if (res != 0)
        return res;
    return utl::compare(_blockPos, rhs._blockPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DequeIt::copy(const Object& rhs_)
{
    auto& rhs = utl::cast<DequeIt>(rhs_);
    super::copy(rhs_);
    _deque = rhs._deque;
    _blockPtr = rhs._blockPtr;
    _blockPos = rhs._blockPos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DequeIt::forward(size_t dist)
{
    ASSERTD(isValid(_deque));
    while (dist-- > 0)
    {
        // don't move beyond the end (one past the tail)
        if ((_blockPtr == _deque->_endBlock) && (_blockPos == _deque->_endPos))
            return;

        if (++_blockPos == BLOCK_SIZE)
        {
            _blockPos = 0;
            if (++_blockPtr == _deque->_blocksLim)
                _blockPtr = _deque->_blocks;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DequeIt::reverse(size_t dist)
{
    ASSERTD(isValid(_deque));
    while (dist-- > 0)
    {
        // don't move before the head
        if ((_blockPtr == _deque->_beginBlock) && (_blockPos == _deque->_beginPos))
            return;

        if (--_blockPos == uint32_t_max)
        {
            _blockPos = BLOCK_SIZE - 1;
            if (--_blockPtr < _deque->_blocks)
                _blockPtr = _deque->_blocksLim - 1;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
DequeIt::get() const
{
    ASSERTD(isValid(_deque));
    ASSERTD(_blockPtr != nullptr);
    ASSERTD(_blockPos != uint32_t_max);
    return (*_blockPtr)[_blockPos];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
DequeIt::set(const Object* object)
{
    ASSERTD(!isConst());
    ASSERTD(isValid(_deque));
    ASSERTD(_blockPtr != nullptr);
    ASSERTD(_blockPos != uint32_t_max);
    Object*& ptr = (*_blockPtr)[_blockPos];
    Object* dequeObject = *ptr;
    if (dequeObject == nullptr)
    {
        if (object == nullptr)
        {
            return;
        }
        else
        {
            // we can only add at the tail
            ASSERTD((_blockPtr == _deque->_endBlock) && (_blockPos == _deque->_endPos));
            _deque->pushBack(object);
            _blockPtr = _deque->_endBlock;
            _blockPos = _deque->_endPos;
        }
    }
    else // dequeObject != nullptr
    {
        if (object == nullptr)
        {
            // we can only remove head or tail
            if ((_blockPtr == _deque->_beginBlock) && (_blockPos == _deque->_beginPos))
            {
                _deque->removeFront();
                _blockPtr = _deque->_beginBlock;
                _blockPos = _deque->_beginPos;
            }
            else
            {
                ASSERTD((_blockPtr == _deque->_endBlock) && (_blockPos == _deque->_endPos));
                _deque->removeBack();
                _blockPtr = _deque->_endBlock;
                _blockPos = _deque->_endPos;
            }
        }
        else
        {
            // replace the object
            if (object == dequeObject)
                return;
            if (_deque->isOwner())
                delete dequeObject;
            ptr = const_cast<Object*>(object);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
bool
DequeIt::isValid(const utl::Object* owner) const
{
    if (!BidIt::isValid(owner))
        return false;
    if ((_blockPtr < _deque->_blocks) || (_blockPtr > _deque->_blocksLim))
        return false;
    if (_blockPos >= BLOCK_SIZE)
        return false;
    if ((_blockPtr == _deque->_beginBlock) && (_blockPos < _deque->_beginPos))
        return false;
    if ((_blockPtr == _deque->_endBlock) && (_blockPos > _deque->_endPos))
        return false;
    if (_deque->_beginBlock <= _deque->_endBlock)
    {
        if (_blockPtr < _deque->_beginBlock)
            return false;
        if (_blockPtr > _deque->_endBlock)
            return false;
    }
    else
    {
        if ((_blockPtr > _deque->_endBlock) && (_blockPtr < _deque->_beginBlock))
            return false;
    }
    return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
