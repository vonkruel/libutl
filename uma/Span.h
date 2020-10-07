#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/MemStream.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Span relationships.
   \ingroup math
*/
enum span_op_t
{
    span_overlaps,    /**< span overlaps another */
    span_contains,    /**< span contains another */
    span_contained_in /**< span is contained by another */
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Span ////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Span of values.

   A span is defined by its begin and end values.  It includes all values
   greater than or equal to the begin value and less than the end value.
   We write a span like this: [begin, end).

   <b>Attributes</b>

   \arg <b><i>relaxed</i> flag</b> : If true for either span involved
   in a comparison, the two spans are judged to be equal if they overlap.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D = T>
class Span
    : public Object
    , protected FlagsMI
{
    UTL_CLASS_DECL_TPL2(Span, T, D, Object);

public:
    /**
       Constructor.
       \param begin span begin
       \param end span end
       \param relaxed relaxed comparisons?
    */
    Span(const T& begin, const T& end, bool relaxed = false);

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual String toString() const;

    /// \name Accessors.
    //@{
    /** Get the beginning of the span. */
    const T&
    begin() const
    {
        return _begin;
    }

    /** Set the beginning of the span. */
    void
    setBegin(const T& begin)
    {
        _begin = begin;
    }

    /** Get the end of the span. */
    const T&
    end() const
    {
        return _end;
    }

    /** Set the end of the span. */
    void
    setEnd(const T& end)
    {
        _end = end;
    }

    /** Set the beginning and end of the span. */
    void
    set(const T& begin, const T& end)
    {
        _begin = begin;
        _end = end;
    }

    /** Determine whether the span is \b nil. */
    bool
    isNil() const
    {
        return _begin >= _end;
    }

    /** Set the span to \b nil. */
    void
    setNil()
    {
        _begin = _end = T();
    }

    /** Get the \b relaxed flag. */
    bool
    isRelaxed() const
    {
        return getFlag(flg_relaxed);
    }

    /** Set the \b relaxed flag. */
    void
    setRelaxed(bool relaxed)
    {
        setFlag(flg_relaxed, relaxed);
    }

    /** Return the size (end - begin). */
    D
    size() const
    {
        return _end - _begin;
    }
    //@}

    /** Clip against the given span. */
    Span<T, D>& clip(const Span<T, D>& span);

    /** Clip the beginning against the given value. */
    Span<T, D>&
    clipBegin(const T& t)
    {
        if (t > _begin)
            _begin = t;
        return *this;
    }

    /** Clip the end against the given value. */
    Span<T, D>&
    clipEnd(const T& t)
    {
        if (t < _end)
            _end = t;
        return *this;
    }

    /** Determine whether self contains the given span. */
    bool
    contains(const Span<T, D>& span) const
    {
        return (_begin <= span._begin) && (_end >= span._end);
    }

    /** Determine whether self contains the given value. */
    bool
    contains(const T& v) const
    {
        return ((v >= _begin) && (v < _end));
    }

    /** Determine whether self is contained by the given span. */
    bool
    isContainedBy(const Span<T, D>& span) const
    {
        return span.contains(*this);
    }

    /** Merge with the given span. */
    void
    merge(const Span<T, D>& span)
    {
        _begin = min(_begin, span.begin());
        _end = max(_end, span.end());
    }

    /** Return the sub-span that overlaps with the given span. */
    Span<T, D> overlap(const Span<T, D>& span) const;

    /** Determine whether self overlaps with the given span. */
    bool
    overlaps(const Span<T, D>& span) const
    {
        return !overlap(span).isNil();
    }

    /** Return the length of the overlap with the given span. */
    D overlapSize(const Span<T, D>& span) const;

    /**
       Remove the given span from self.
       \return second remaining part of self (if self is bisected)
    */
    Span<T, D> remove(const Span<T, D>& span);

    /** Return self merged with the given span. */
    Span<T, D>
    operator+(const Span<T, D>& rhs)
    {
        Span<T, D> res(*this);
        res.merge(rhs);
        return res;
    }

    /** Merge with the given span. */
    const Span<T, D>&
    operator+=(const Span<T, D>& rhs)
    {
        merge(rhs);
        return *this;
    }

protected:
    T _begin;
    T _end;

private:
    enum flg_t
    {
        flg_relaxed
    };

private:
    void
    init(bool relaxed = false)
    {
        _begin = _end = T();
        setRelaxed(relaxed);
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// SpanSizeOrdering ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Order spans by size.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D = T>
class SpanSizeOrdering : public Ordering
{
    UTL_CLASS_DECL_TPL2(SpanSizeOrdering, T, D, Ordering);
    UTL_CLASS_DEFID;

public:
    virtual int cmp(const Object* lhs, const Object* rhs) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ObjectSpan //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Span of object-derived objects.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D = T>
class ObjectSpan : public Span<T, D>
{
    UTL_CLASS_DECL_TPL2_TPL2(ObjectSpan, T, D, Span, T, D);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param span span to copy
    */
    ObjectSpan(Span<T, D> span)
        : Span<T, D>(span)
    {
    }

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual String
    toString() const
    {
        return "[" + this->_begin.toString() + "," + this->_end.toString() + ")";
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Span ////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>::Span(const T& begin, const T& end, bool relaxed)
    : _begin(begin)
    , _end(end)
{
    setRelaxed(relaxed);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
int
Span<T, D>::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA2(Span, T, D))
    {
        auto& span = utl::cast<Span<T, D>>(rhs);
        // If "relaxed" comparison, the spans are equal if they overlap,
        // Else they must be exactly the same.
        if ((isRelaxed() || span.isRelaxed()) && overlaps(span))
        {
            res = 0;
        }
        else
        {
            res = utl::compare(_begin, span._begin);
            if (res != 0)
                return res;
            res = utl::compare(_end, span._end);
        }
    }
    else
    {
        res = super::compare(rhs);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
Span<T, D>::copy(const Object& rhs)
{
    auto& span = utl::cast<Span<T, D>>(rhs);
    FlagsMI::copyFlags(span);
    _begin = span._begin;
    _end = span._end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
Span<T, D>::serialize(Stream& stream, uint_t io, uint_t mode)
{
    utl::serialize(_begin, stream, io, mode);
    utl::serialize(_end, stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
String
Span<T, D>::toString() const
{
    MemStream ms(64);
    ms << "[" << _begin << "," << _end << ")" << '\0';
    return String(ms.takeString(), true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>&
Span<T, D>::clip(const Span<T, D>& span)
{
    // ensure begin >= span.begin
    // ensure end <= span.end
    if (_begin < span._begin)
        _begin = span._begin;
    if (_end > span._end)
        _end = span._end;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>
Span<T, D>::overlap(const Span<T, D>& span) const
{
    T beg = max(_begin, span._begin);
    T end = min(_end, span._end);
    return Span<T, D>(beg, end);
}

//////////////////////////////////////////////////////////////////////////////

template <class T, class D>
D
Span<T, D>::overlapSize(const Span<T, D>& span) const
{
    return utl::min(_end, span._end) - utl::max(_begin, span._begin);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
Span<T, D>
Span<T, D>::remove(const Span<T, D>& span)
{
    T t = T();
    Span<T, D> res(t, t);

    // if span contains* this, removing span makes* this NIL
    if (span.contains(*this))
    {
        setNil();
        return res;
    }

    // get the overlapping time span
    Span<T, D> overlap = span.overlap(*this);

    if (overlap.isNil())
    {
        return res;
    }

    // if begin == beginning of overlap, begin = end of overlap
    if (_begin == overlap.begin())
    {
        setBegin(overlap.end());
    }
    // if end == end of overlap, end = beginning of overlap
    else if (_end == overlap.end())
    {
        setEnd(overlap.begin());
    }
    // else we are being bi-sected
    else
    {
        // the result is the second section [overlap.end, end)
        res.setBegin(overlap.end());
        res.setEnd(end());
        // this becomes the first section [begin, overlap.begin)
        setEnd(overlap.begin());
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// SpanSizeOrdering ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
int
SpanSizeOrdering<T, D>::cmp(const Object* lhs, const Object* rhs) const
{
    const Object& lhsKey = lhs->getKey();
    const Object& rhsKey = rhs->getKey();
    if (!lhsKey.isA2(Span, T, D) || !rhsKey.isA2(Span, T, D))
    {
        return lhs->compare(*rhs);
    }

    const Span<T, D>& lhsSpan = (const Span<T, D>&)lhsKey;
    const Span<T, D>& rhsSpan = (const Span<T, D>&)rhsKey;
    int res = lhsSpan.size() - rhsSpan.size();
    if (res == 0)
    {
        res = lhsSpan.compare(rhsSpan);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// ObjectSpan //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T, class D>
void
ObjectSpan<T, D>::serialize(Stream& stream, uint_t io, uint_t mode)
{
    this->_begin.serialize(stream, io, mode);
    this->_end.serialize(stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL2(utl::Span, T, D);
UTL_CLASS_IMPL_TPL2(utl::SpanSizeOrdering, T, D);
UTL_CLASS_IMPL_TPL2(utl::ObjectSpan, T, D);
