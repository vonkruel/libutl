UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Templated proxy for BidIt.

   TBidIt is a template version of BidIt that provides ease of use by reducing the need to
   perform typecasts.  It acts as a proxy for a bi-directional iterator (BidIt), returning
   (T*) instead of (Object*) where appropriate.

   \author Adam McKee
   \ingroup iterator
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object> class TBidIt final : public BidIt
{
    UTL_CLASS_DECL_TPL(TBidIt, T, BidIt);

public:
    /**
       Constructor.
       \param it iterator to proxy for
    */
    TBidIt(BidIt* it)
    {
        _it = it;
    }

    virtual int compare(const Object& rhs) const;

    virtual void copy(const Object& rhs);

    virtual void steal(Object& rhs);

    virtual const Object&
    getProxiedObject() const
    {
        return *_it;
    }

    virtual Object&
    getProxiedObject()
    {
        return *_it;
    }

    virtual void
    forward(size_t dist = 1)
    {
        _it->forward(dist);
    }

    virtual void
    reverse(size_t dist = 1)
    {
        _it->reverse(dist);
    }

    virtual T*
    get() const
    {
        return utl::cast<T>(_it->get());
    }

    virtual void
    set(const Object* object)
    {
        _it->set(object);
    }

    /**
       Get the proxied iterator.
       \return proxied iterator
    */
    BidIt*
    iterator() const
    {
        return _it;
    }

    /**
       Set the iterator.
       \param it iterator to proxy for
    */
    void
    setIt(BidIt* it)
    {
        if (it != _it)
        {
            delete _it;
            _it = it;
        }
    }

    /**
       Pointer dereference operator -- return a (T*) instead of an (Object*).
       \see get
    */
    T* operator*() const
    {
        return utl::cast<T>(_it->get());
    }

    /**
       Set the iterator.
       \see setIt
    */
    TBidIt&
    operator=(BidIt* it)
    {
        setIt(it);
        return *this;
    }

    // for STL
    typedef T* value_type;
    typedef T*& reference;
    typedef T** pointer;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

private:
    void
    init()
    {
        _it = nullptr;
    }
    void
    deInit()
    {
        delete _it;
    }

private:
    BidIt* _it;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
int
TBidIt<T>::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(TBidIt<T>))
    {
        auto& it = utl::cast<TBidIt<T>>(rhs);
        res = compareNullable(_it, it._it, nullptr);
    }
    else
    {
        auto& it = utl::cast<BidIt>(rhs);
        res = compareNullable(_it, &it, nullptr);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
TBidIt<T>::copy(const Object& rhs)
{
    if (rhs.isA(TBidIt<T>))
    {
        auto& it = utl::cast<TBidIt<T>>(rhs);
        setIt(utl::clone(it._it));
    }
    else
    {
        auto& it = utl::cast<BidIt>(rhs);
        setIt(it.clone());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
TBidIt<T>::steal(Object& rhs_)
{
    if (rhs_.isA(TBidIt<T>))
    {
        auto& rhs = utl::cast<TBidIt<T>>(rhs_);
        if (_it != nullptr)
            delete _it;
        _it = rhs._it;
        rhs._it = nullptr;
    }
    else
    {
        auto& rhs = utl::cast<BidIt>(rhs_);
        if (_it == nullptr)
        {
            _it = rhs.clone();
        }
        else
        {
            _it->steal(rhs);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::TBidIt, T);
