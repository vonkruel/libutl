#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/RBtree.h>
#include <libutl/Span.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Span collection.

   SpanCol is just what it sounds like: a collection of Span objects.  For efficiency reasons,
   adjacent spans are merged together.  Also, the removal of a span may result in the removal of
   multiple existing spans or the bisection of a span.

   For example, suppose we are working with Span<uint_t> and we have these spans:

   \code
   [0,5)
   [10,20)
   \endcode

   Now we add [5,10).  It is merged with the adjacent spans:

   \code
   [0,20)
   \endcode

   Now suppose we remove [5,15).  This removal bisects [0,20):

   \code
   [0,5)
   [15,20)
   \endcode

   Removing [0,20) would remove both these spans, or we might remove [1,17):

   \code
   [0,1)
   [17,20)
   \endcode

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D = T> class SpanCol : public TRBtree<Span<T, D>>
{
    UTL_CLASS_DECL_TPL2_TPLxTPL2(SpanCol, T, D, TRBtree, Span, T, D);

public:
    typedef typename TRBtree<Span<T, D>>::iterator iterator;

public:
    /** Get the total size of all spans. */
    D
    totalSize() const
    {
        return _totalSize;
    }

    bool
    add(const Object& object)
    {
        if (this->isOwner())
            return add(object.clone());
        else
            return add(&object);
    }

    virtual bool add(const Object* object);

    T minBegin() const;

    T maxEnd() const;

    virtual bool isMergeable(const Object& lhs, const Object& rhs) const;

    void remove(const Span<T, D>& span);

    virtual bool
    remove(const Object& key)
    {
        return RBtree::remove(key);
    }

    Span<T, D>
    span() const
    {
        return Span<T, D>(minBegin(), maxEnd());
    }

protected:
    virtual void
    onAdd(const Object& object)
    {
    }
    virtual void
    onRemove(const Object& object)
    {
    }
    virtual void merge(Object& lhs, const Object& rhs);
    virtual Span<T, D> remove(Object& object, const Span<T, D>& span);

private:
    void
    init()
    {
        _totalSize = 0;
    }
    void
    deInit()
    {
    }

private:
    D _totalSize;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
bool
SpanCol<T, D>::add(const Object* object)
{
    auto& spanRef = utl::cast<Span<T, D>>(object->getKey());
    Span<T, D> span = spanRef;
    span.setRelaxed(true);

    if (span.isNil())
    {
        if (this->isOwner())
        {
            delete object;
        }
        return false;
    }

    iterator it = this->findFirstIt(span);

    while (it != this->end())
    {
        Object& curObj = **it;
        Span<T, D>& curSpan = (Span<T, D>&)curObj.getKey();

        if (curSpan != span)
        {
            break;
        }

        if (isMergeable(*object, curObj))
        {
            merge((Object&)*object, curObj);
            onRemove(curObj);
            this->removeIt(it);
            continue;
        }
        else
        {
            // Remove the span from curSpan
            Span<T, D> rhsSpan = remove(curObj, span);
            // When we remove the span from curSpan, we may bisect it.
            // If so, rhsSpan will contain the other half of curSpan.
            // We need to construct another object based on this
            // information and add it.
            if (!rhsSpan.isNil())
            {
                // clone curObj to get rhsObj
                Object* rhsObj = curObj.clone();

                // set the rhsSpan's begin, end time
                Span<T, D>& _rhsSpan = (Span<T, D>&)rhsObj->getKey();
                _rhsSpan.Span<T, D>::copy(rhsSpan);

                // add rhsObj
                add(rhsObj);
            }
            // If curSpan is NIL we should remove it entirely
            if (curSpan.isNil())
            {
                onRemove(curObj);
                this->removeIt(it);
                continue;
            }
        }

        it++;
    }

    // merge with adjacent spans

    // find a span that is adjacent on the left
    span.set(spanRef.begin() - 1, spanRef.begin());
    it = this->findIt(span);
    Object* adjObj = *it;
    if (adjObj != nullptr)
    {
        if (isMergeable(*object, *adjObj))
        {
            merge((Object&)*object, *adjObj);
            onRemove(*adjObj);
            this->removeIt(it);
        }
    }

    // find a span that is adjacent on the right
    span.set(spanRef.end(), spanRef.end() + 1);
    it = this->findIt(span);
    adjObj = *it;
    if (adjObj != nullptr)
    {
        if (isMergeable(*object, *adjObj))
        {
            merge((Object&)*object, *adjObj);
            onRemove(*adjObj);
            this->removeIt(it);
        }
    }

    // finally add span to the tree
    _totalSize += spanRef.size();
    onAdd(*object);
    RBtree::add(object);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
T
SpanCol<T, D>::minBegin() const
{
    T res = T();

    if (this->_items > 0)
    {
        res = ((Span<T, D>*)this->first())->begin();
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
T
SpanCol<T, D>::maxEnd() const
{
    T res = T();

    if (this->_items > 0)
    {
        res = ((Span<T, D>*)this->last())->end();
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
bool
SpanCol<T, D>::isMergeable(const Object& lhs, const Object& rhs) const
{
    const Span<T, D>& lhsSpan = (const Span<T, D>&)lhs.getKey();
    const Span<T, D>& rhsSpan = (const Span<T, D>&)rhs.getKey();
    Span<T, D> saveLHS = lhsSpan;
    ((Span<T, D>&)lhsSpan).Span<T, D>::copy(rhsSpan);
    bool res = (lhs == rhs);
    ((Span<T, D>&)lhsSpan).Span<T, D>::copy(saveLHS);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanCol<T, D>::remove(const Span<T, D>& _span)
{
    Span<T, D> span = _span;
    span.setRelaxed(true);

    iterator it = this->findFirstIt(span);

    while (it != this->end())
    {
        Object& curObj = **it;
        Span<T, D>& curSpan = (Span<T, D>&)(curObj.getKey());

        // if curSpan doesn't overlap with ts's span, we're done
        if (curSpan != span)
        {
            break;
        }

        // Remove the span from curSpan
        Span<T, D> rhsSpan = remove(curObj, span);
        // When we remove the span from curSpan, we may bisect it.
        // If so, rhsSpan will contain the other half of curSpan.
        // We need to construct another object based on this
        // information and add it.
        if (!rhsSpan.isNil())
        {
            // Clone curObj to get rhsObj
            Object* rhsObj = curObj.clone();

            // Set the rhsSpan's begin, end time
            Span<T, D>& _rhsSpan = (Span<T, D>&)rhsObj->getKey();
            _rhsSpan.Span<T, D>::copy(rhsSpan);

            // Add rhsObj
            onAdd(*rhsObj);
            add(rhsObj);
        }
        // If curSpan is NIL we should remove it entirely
        if (curSpan.isNil())
        {
            // don't call onRemove() because we called remove()
            this->removeIt(it);
            continue;
        }

        it++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
SpanCol<T, D>::merge(Object& lhs, const Object& rhs)
{
    Span<T, D>& lhsSpan = (Span<T, D>&)lhs.getKey();
    const Span<T, D>& rhsSpan = (const Span<T, D>&)rhs.getKey();
    lhsSpan.merge(rhsSpan);
    _totalSize -= rhsSpan.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>
SpanCol<T, D>::remove(Object& object, const Span<T, D>& span)
{
    Span<T, D>& objSpan = (Span<T, D>&)object.getKey();
    Span<T, D> overlap = objSpan.overlap(span);
    _totalSize -= overlap.size();
    return objSpan.remove(span);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL2(utl::SpanCol, T, D);
