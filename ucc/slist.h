#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Collection.h>
#include <libutl/SlistNode.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Compare two lists.
   \return < 0 if self < rhs, 0 if self = rhs, > 0 if self > rhs
   \param lhs head of left-hand-side list
   \param rhs head of right-hand-side list
   \param ordering (optional) ordering
*/
int slist_compare(const SlistNode* lhs, const SlistNode* rhs, const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Make a copy of a list.
   \param outList head of newly created list
   \param outTail tail of newly created list (can be nullptr)
   \param list head of list to copy
   \param owner ownership flag
*/
void
slist_clone(SlistNode*& outList, SlistNode** outTail, const SlistNode* list, bool owner = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Serialize a list.
   \param list head of list to serialize
   \param tail tail of list to serialize (can be nullptr)
   \param owner ownership flag
   \param stream stream to serialize from/to
   \param io see utl::io_t
   \param mode see utl::serialize_t
*/
void slist_serialize(SlistNode*& list,
                     SlistNode** tail,
                     bool owner,
                     Stream& stream,
                     uint_t io,
                     uint_t mode = ser_default);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Clear a list.
   \param list head of list to clear
   \param tail tail of list to clear (can be nullptr)
   \param owner (optional : true) ownership flag
*/
void slist_clear(SlistNode*& list, SlistNode** tail, bool owner = true);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Count the number of items in a list.
   \return item count
   \param list head of list
*/
size_t slist_items(const SlistNode* list);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the tail node.
   \return tail node
   \param list head of list
*/
SlistNode* slist_tail(SlistNode* list);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine the tail node.
   \return tail node
   \param list head of list
*/
const SlistNode* slist_tail(const SlistNode* list);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Add an object to a list.
   \param list head of list
   \param tail tail of list (can be nullptr)
   \param object object to be added
   \param sorted (optional : true) add in correct order?
   \param ordering (optional) ordering
*/
void slist_add(SlistNode*& list,
               SlistNode** tail,
               const Object* object,
               bool sorted = false,
               const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Remove the object matching the given key from the list.
   \return true if object found and removed, false otherwise
   \param list head of list
   \param tail tail of list (can be nullptr)
   \param key search key
   \param owner (optional : true) ownership flag
   \param sorted (optional : false) sorted list?
   \param ordering (optional) ordering
*/
bool slist_remove(SlistNode*& list,
                  SlistNode** tail,
                  const Object& key,
                  bool owner = true,
                  bool sorted = false,
                  const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Remove the given node from the given list.
   \param list head of list
   \param tail tail of list (can be nullptr)
   \param node node to be removed
   \param prev node's predecessor
   \param owner ownership flag
*/
void
slist_removeNode(SlistNode*& list, SlistNode** tail, SlistNode* node, SlistNode* prev, bool owner);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Find the object matching the given key in the given list.
   \param list head of list
   \param key search key
   \param sorted (optional : false) sorted list?
   \param ordering (optional) ordering
*/
Object* slist_find(SlistNode* list,
                   const Object& key,
                   bool sorted = false,
                   const Ordering* ordering = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Find the insertion point for the given key in the given list.
   \return node that key should be inserted before/after
   \param list head of list
   \param key search key
   \param sorted (optional : false) sorted list?
   \param ordering (optional) ordering
   \param cmpRes (optional) result of comparing returned node's object with given key
   \param prev (optional) new node's predecessor
*/
SlistNode* slist_findInsertionPoint(SlistNode* list,
                                    const Object& key,
                                    bool sorted = false,
                                    const Ordering* ordering = nullptr,
                                    int* cmpRes = nullptr,
                                    SlistNode** prev = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Find the node whose object matches the given key.
   \return found node (nullptr if none)
   \param list head of list
   \param key search key
   \param sorted (optional : false) sorted list?
   \param ordering (optional) ordering
   \param prev (optional) node's predecessor
*/
SlistNode* slist_findNode(SlistNode* list,
                          const Object& key,
                          bool sorted = false,
                          const Ordering* ordering = nullptr,
                          SlistNode** prev = nullptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
