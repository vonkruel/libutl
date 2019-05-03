UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Templated proxy for FwdIt.

   TFwdIt is a template version of FwdIt that provides ease of use by reducing the need to
   perform typecasts.  It acts as a proxy for a forward iterator (FwdIt), returning (T*)
   instead of (Object*) where appropriate.

   \author Adam McKee
   \ingroup iterator
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object> class TFwdIt final : public FwdIt
{
    UTL_CLASS_DECL_TPL(TFwdIt, T, FwdIt);

public:
    /**
       Constructor.
       \param it iterator to proxy for
    */
    TFwdIt(FwdIt* it)
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
    FwdIt*
    iterator() const
    {
        return _it;
    }

    /**
       Set the iterator.
       \param it iterator to proxy for
    */
    void
    setIt(FwdIt* it)
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
    TFwdIt&
    operator=(FwdIt* it)
    {
        setIt(it);
        return *this;
    }

    // for STL
    typedef T* value_type;
    typedef T*& reference;
    typedef T** pointer;
    typedef std::forward_iterator_tag iterator_category;
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
    FwdIt* _it;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
int
TFwdIt<T>::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(TFwdIt<T>))
    {
        auto& it = utl::cast<TFwdIt<T>>(rhs);
        res = compareNullable(_it, it._it, nullptr);
    }
    else
    {
        auto& it = utl::cast<FwdIt>(rhs);
        res = compareNullable(_it, &it, nullptr);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
TFwdIt<T>::copy(const Object& rhs)
{
    if (rhs.isA(TFwdIt<T>))
    {
        auto& it = utl::cast<TFwdIt<T>>(rhs);
        setIt(utl::clone(it._it));
    }
    else
    {
        auto& it = utl::cast<FwdIt>(rhs);
        setIt(it.clone());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
TFwdIt<T>::steal(Object& rhs_)
{
    if (rhs_.isA(TFwdIt<T>))
    {
        auto& rhs = utl::cast<TFwdIt<T>>(rhs_);
        if (_it != nullptr)
            delete _it;
        _it = rhs._it;
        rhs._it = nullptr;
    }
    else
    {
        auto& rhs = utl::cast<FwdIt>(rhs_);
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

UTL_CLASS_IMPL_TPL(utl::TFwdIt, T);
