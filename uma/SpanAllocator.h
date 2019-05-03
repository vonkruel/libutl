#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/SpanCol.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Span allocator.

   Allocate spans from a one-dimensional free-store.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D = T> class SpanAllocator : public SpanCol<T, D>
{
    UTL_CLASS_DECL_TPL2_TPL2(SpanAllocator, T, D, SpanCol, T, D);

public:
    typedef typename SpanCol<T, D>::iterator iterator;

public:
    virtual void copy(const Object& object);

    virtual void dump(Stream& os, uint_t level = uint_t_max) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /**
       Allocate a span of the given size.
       \return span of requested size (nil if allocation fails)
       \param size allocation size
    */
    Span<T, D> alloc(D size);

    /** Clear all allocations. */
    void clear();

    /** De-allocate the given span. */
    void
    dealloc(const Span<T, D>& span)
    {
        if (this->isOwner())
            dealloc(span.clone());
        else
            dealloc(&span);
    }

    /** De-allocate the given span. */
    void
    dealloc(const Span<T, D>* span)
    {
        this->add(span);
    }

    virtual bool
    remove(const Object& key)
    {
        return super::remove(key) && _spans.remove(key.getKey());
    }

    /** Remove the given span. */
    void
    remove(const Span<T, D>& span)
    {
        super::remove(span);
    }

protected:
    virtual void onAdd(const Object& object);
    virtual void onRemove(const Object& object);
    virtual Span<T, D> remove(Object& object, const Span<T, D>& span);

private:
    void init();
    void
    deInit()
    {
    }

private:
    RBtree _spans;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::clear()
{
    super::clear();
    _spans.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::copy(const Object& object)
{
    auto& sa = utl::cast<SpanAllocator<T, D>>(object);
    super::copy(sa);
    _spans.clear();
    for (auto span : *this)
    {
        _spans.add(span);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::dump(Stream& os, uint_t level) const
{
    super::dump(os, level);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::serialize(Stream& stream, uint_t io, uint_t mode)
{
    super::serializeFast(Span<T, D>::getThisClass(), stream, io, mode);
    if (io == io_rd)
    {
        for (auto span : *this)
        {
            _spans.add(span);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>
SpanAllocator<T, D>::alloc(D size)
{
    Span<T, D> res;

    // find the smallest free span that is large enough
    const BinTreeNode* node = _spans.root().node();
    D bestSize;
    bool found = false;
    while (!node->isLeaf())
    {
        auto& nodeKey = node->get()->getKey();
        if (!nodeKey.isA2(Span, T, D))
        {
            node = node->left();
            continue;
        }
        const Span<T, D>& span = (const Span<T, D>&)nodeKey;
        if (span.size() < size)
        {
            node = node->right();
        }
        else
        {
            if (!found || (span.size() < bestSize))
            {
                res.setBegin(span.begin());
                res.setEnd(span.begin() + size);
                bestSize = span.size();
                found = true;
            }
            node = node->left();
        }
    }

    // remove the span if the allocation is successful
    if (found)
    {
        res.setRelaxed(true);
        remove(res);
        res.setRelaxed(false);
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::onAdd(const Object& object)
{
    _spans.add(object.getKey());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::onRemove(const Object& object)
{
    _spans.remove(object.getKey());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>
SpanAllocator<T, D>::remove(Object& object, const Span<T, D>& span)
{
    const Span<T, D>& objSpan = (const Span<T, D>&)object.getKey();
    _spans.remove(objSpan);
    Span<T, D> rhsSpan = super::remove(object, span);
    if (!objSpan.isNil())
    {
        _spans.add(objSpan);
    }
    return rhsSpan;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanAllocator<T, D>::init()
{
    _spans.setOwner(false);
    _spans.setOrdering(new SpanSizeOrdering<T, D>(), sort_none);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL2(utl::SpanAllocator, T, D);
