#include <libutl/libutl.h>
#include <libutl/BinTree.h>
#include <libutl/MaxObject.h>
#include <libutl/AutoPtr.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::BinTree);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::BinTree(bool owner, bool multiSet, Ordering* ordering)
{
    init(owner, multiSet, ordering);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::steal(Object& rhs_)
{
    auto& rhs = utl::cast<BinTree>(rhs_);
    deInit();
    super::steal(rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::serializeFast(const RunTimeClass* rtc, Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        super::serialize(rtc, stream, io, mode);
        return;
    }

    // serialize the ordering
    Object* ordering = this->ordering();
    utl::serializeNullable(ordering, stream, io, mode);
    // serialize # of items
    utl::serialize(_items, stream, io_wr, mode);
    // serialize elements
    AutoPtr<FwdIt> begin = beginBFSNew();
    AutoPtr<FwdIt> end = endBFSNew();
    utl::serializeOut(*begin, *end, stream, mode, rtc == nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
BinTree::find(const Object& key) const
{
    Object* res;

    // find a node with a matching object
    BinTreeNode* node = findNode(key, isMultiSet() ? find_first : find_any);

    // if the node is nullptr, the result is nullptr
    if (node == nullptr)
    {
        res = nullptr;
    }
    // else the result is the found node's object
    else
    {
        res = node->get();
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findIt(const Object& key) const
{
    return findIt(key, isMultiSet() ? find_first : find_any);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findIt(const Object& key)
{
    return findIt(key, isMultiSet() ? find_first : find_any);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::findIt(const Object& key, BidIt& it)
{
    it = findIt(key, isMultiSet() ? find_first : find_any);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::findFirstIt(const Object& key, BidIt& it, bool insert)
{
    it = findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findFirstIt(const Object& key, bool insert) const
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findFirstIt(const Object& key, bool insert)
{
    return findIt(key, insert ? find_ip : find_first);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::findLastIt(const Object& key, BidIt& it)
{
    it = findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findLastIt(const Object& key) const
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findLastIt(const Object& key)
{
    return findIt(key, find_last);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BinTree::add(const Object* object)
{
    // determine where we will insert the object
    bool insertLeft;
    BinTreeNode* parent = findInsertionPoint(*object, &insertLeft);

    // if the insertion is disallowed
    if (parent == nullptr)
    {
        if (isOwner())
            delete object;
        return false;
    }

    // create a node for the object
    BinTreeNode* node = createNode(object);

    // are we the left or right child?
    if (insertLeft)
    {
        parent->setLeft(node);
    }
    else
    {
        parent->setRight(node);
    }

    // fix up the tree (re-balance as necessary)
    node->addFixup();

    // ensure root is set properly
    resetRoot();

    // update item count
    ++_items;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::insert(const Object* object, const BidIt& it_)
{
    auto& it = utl::cast<iterator>(it_.getProxiedObject());
    IFDEBUG(checkInsert(object, it, true));

    // create a node for the object
    BinTreeNode* node = createNode(object);

    // object becomes inorder predecessor of it.node()
    BinTreeNode* parent = it.node();
    ASSERTD(parent != nullptr);
    if (parent->left()->isLeaf())
    {
        parent->setLeft(node);
    }
    else
    {
        parent = parent->prev();
        parent->setRight(node);
    }

    // fix up the tree (re-balance as necessary)
    node->addFixup();

    // ensure root is set properly
    resetRoot();

    // update item count
    ++_items;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::clear()
{
    // remove all elements
    deInit();

    // re-initialize
    init(isOwner(), isMultiSet(), ordering());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
BinTree::remove(const Object& key)
{
    // find the node whose object matches the given key
    bool multiSet = this->isMultiSet();
    BinTreeNode* node = findNode(key, multiSet ? find_first : find_any);
    if (node == nullptr)
        return false;

    // remove the node
    removeNode(node);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::removeIt(BidIt& it)
{
    ASSERTD(!it.isConst());
    ASSERTD(*it != nullptr);

    // cast the iterator to a tree iterator
    auto& rbit = utl::cast<iterator>(it.getProxiedObject());
    ASSERTD(rbit.isValid(this));

    // get the node, and determine what its successor is
    BinTreeNode* node = rbit.node();
    BinTreeNode* next = node->next();

    // remove the node
    BinTreeNode* removedNode = removeNode(node);

    // If the node was removed from the tree, point the iterator
    // at the node's successor (which replaced the removed node)
    if (removedNode == node)
    {
        rbit.setNode(next);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
BinTree::sanityCheck() const
{
    // check for properly ordered items, and correct # of items
    size_t itemsFound = 0;
    iterator it = const_cast_this->begin(false);
    const Object* prevObject = nullptr;
    while (!it.isEnd())
    {
        const Object* nextObject = *it;
        if (prevObject != nullptr)
        {
            ASSERT(compareObjects(prevObject, nextObject) <= 0);
        }
        prevObject = nextObject;
        ++itemsFound;
        it.forward();
    }
    ASSERT(itemsFound == _items);
    ASSERT(_root->parent() == nullptr);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::clear(BinTreeNode* node)
{
    // if there is a left subtree, clear it
    if (!node->left()->isLeaf())
    {
        clear(node->left());
    }

    // if there is a right subtree, clear it
    if (!node->right()->isLeaf())
    {
        clear(node->right());
    }

    // get the node's object
    Object* object = node->get();
    // if we own the object, delete it
    if (isOwner() && (object != &maxObject))
    {
        delete object;
    }

    // delete the node
    delete node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTree::findFirstMatch(const Object& key, BinTreeNode* node) const
{
    BinTreeNode* res = node;

    for (;;)
    {
        // move left until we fail to match
        for (;;)
        {
            BinTreeNode* leftNode = node->left();
            if (leftNode->isLeaf())
            {
                return res;
            }
            node = leftNode;
            if (compareObjects(node->get(), &key) == 0)
            {
                res = node;
            }
            else
            {
                break;
            }
        }

        // move right until we *do* match
        for (;;)
        {
            BinTreeNode* rightNode = node->right();
            if (rightNode->isLeaf())
            {
                return res;
            }
            node = rightNode;
            if (compareObjects(node->get(), &key) == 0)
            {
                res = node;
                break;
            }
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTree::findInsertionPoint(const Object& key, bool* insertLeft) const
{
    // we begin searching at the root node
    bool multiSet = this->isMultiSet();
    BinTreeNode* node = _root;
    for (;;)
    {
        BinTreeNode* next;

        // get the current node's object
        Object* object = node->get();

        // compare the key with the node's object
        int cmp = compareObjects(object, &key);

        // doing insert? if object == key and multiples are not allowed, return nullptr
        if ((cmp == 0) && (insertLeft != nullptr) && !multiSet)
            return nullptr;

        // if key <= node, the next node is the left child
        if (cmp >= 0)
        {
            next = node->left();
        }
        // otherwise the next node is the right child
        else
        {
            next = node->right();
        }

        // if the next node is a leaf node, we're done
        if (next->isLeaf())
        {
            if (insertLeft != nullptr)
            {
                *insertLeft = (cmp >= 0);
            }
            else if (cmp < 0)
            {
                // this is not an insert, and key is > node's key -> ensure node is >= key
                node = node->next();
            }
            break;
        }
        // else set the current node = the next node
        else
        {
            node = next;
        }
    }
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findIt(const Object& key, uint_t findType) const
{
    iterator it = const_cast_this->findIt(key, findType);
    IFDEBUG(it.setConst(true));
    return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTree::iterator
BinTree::findIt(const Object& key, uint_t findType)
{
    if (findType == find_ip)
    {
        BinTreeNode* node = findInsertionPoint(key);
        return iterator(this, node);
    }
    else
    {
        // find a node whose object matches the key
        BinTreeNode* node = findNode(key, findType);

        // if we failed to find a node
        if (node == nullptr)
        {
            // the iterator will point to the end of the tree
            node = _root->rightMost();
        }

        // create and return the iterator
        return iterator(this, node);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTree::findLastMatch(const Object& key, BinTreeNode* node) const
{
    BinTreeNode* res = node;

    for (;;)
    {
        // move right until we fail to match
        for (;;)
        {
            BinTreeNode* rightNode = node->right();
            if (rightNode->isLeaf())
            {
                return res;
            }
            node = rightNode;
            if (compareObjects(node->get(), &key) == 0)
            {
                res = node;
            }
            else
            {
                break;
            }
        }

        // move left until we *do* match
        for (;;)
        {
            BinTreeNode* leftNode = node->left();
            if (leftNode->isLeaf())
            {
                return res;
            }
            node = leftNode;
            if (compareObjects(node->get(), &key) == 0)
            {
                res = node;
                break;
            }
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTree::findNode(const Object& key, uint_t findType) const
{
    BinTreeNode* res = nullptr;

    // begin searching at root
    BinTreeNode* node = _root;

    // while node is an interior node
    while (!node->isLeaf())
    {
        // get the node's object
        Object* object = node->get();

        // compare the key with the node's object
        int cmp = compareObjects(object, &key);

        // if they are equal, we're done
        if (cmp == 0)
        {
            res = node;
            break;
        }
        // if key < object, search the left subtree
        else if (cmp > 0)
        {
            node = node->left();
        }
        // otherwise search the right subtree
        else
        {
            node = node->right();
        }
    }

    // if we found a match, possibly find first or last match
    if (res != nullptr)
    {
        if (findType == find_first)
        {
            res = findFirstMatch(key, res);
        }
        else if (findType == find_last)
        {
            res = findLastMatch(key, res);
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BinTreeNode*
BinTree::removeNode(BinTreeNode* node)
{
    // If we're about to remove the root node, change the root pointer to its right child
    // (it definitely has a non-leaf right child b/c sentinel).  We want to ensure that after
    // the removal, root's parent field points to a valid node in the tree, so that we can
    // re-discover the root node after the removal.
    if (node == _root)
    {
        _root = _root->right();
    }

    // remove the node, set node to the address of the node to delete
    BinTreeNode *x, *y;
    node->remove(x, y);
    x->removeFixup(y);
    node = y;

    // re-set the root node
    resetRoot();

    // get the node's object
    Object* object = node->get();
    // if it's a real object (e.g. not MaxObject)
    if (object != &maxObject)
    {
        // if we own it, we have to clean up after it
        if (isOwner())
            delete object;
        // update item count
        --_items;
    }

    // delete the node
    delete node;

    // return the address of the deleted node
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::resetRoot()
{
    // while root is not root...
    while (_root->parent() != nullptr)
    {
        _root = _root->parent();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::init(bool owner, bool multiSet, Ordering* ordering)
{
    setOwner(owner);
    setMultiSet(multiSet);
    if (ordering != nullptr)
        setOrdering(ordering, sort_none);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
BinTree::deInit()
{
    // remove all objects
    clear(_root);
    _root = nullptr;
    _items = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
