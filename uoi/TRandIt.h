UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Templated proxy for RandIt.

   TRandIt is a template version of RandIt that provides ease of use by reducing the need to
   perform typecasts.  It acts as a proxy for a random access iterator (RandIt), returning (T*)
   instead of (Object*) where appropriate.

   \author Adam McKee
   \ingroup iterator
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T = Object>
class TRandIt final : public RandIt
{
    UTL_CLASS_DECL_TPL(TRandIt, T, RandIt);

public:
    /**
       Constructor.
       \param it iterator to proxy for
    */
    TRandIt(RandIt* it)
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

    virtual void
    seek(size_t offset)
    {
        _it->seek(offset);
    }

    virtual T*
    get() const
    {
        return utl::cast<T>(_it->get());
    }

    virtual T*
    get(size_t offset) const
    {
        return _it->get(offset);
    }

    virtual size_t
    offset() const
    {
        return _it->offset();
    }

    virtual void
    set(size_t offset, const Object* object)
    {
        _it->set(offset, object);
    }

    virtual void
    set(const Object* object)
    {
        _it->set(object);
    }

    virtual size_t
    size() const
    {
        return _it->size();
    }

    virtual size_t
    subtract(const RandIt& it) const
    {
        return _it->subtract(it);
    }

    /**
       Get the proxied iterator.
       \return proxied iterator
    */
    RandIt*
    iterator() const
    {
        return _it;
    }

    /**
       Set the iterator.
       \param it iterator to proxy for.
    */
    void
    setIt(RandIt* it)
    {
        if (it != _it)
        {
            delete _it;
            _it = it;
        }
    }

    /**
       Pointer dereference operator -- return a T* instead of an Object*.
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
    TRandIt&
    operator=(RandIt* it)
    {
        setIt(it);
        return *this;
    }

    // for STL
    typedef T* value_type;
    typedef T*& reference;
    typedef T** pointer;
    typedef std::random_access_iterator_tag iterator_category;
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
    RandIt* _it;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
int
TRandIt<T>::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(TRandIt<T>))
    {
        auto& it = utl::cast<TRandIt<T>>(rhs);
        res = compareNullable(_it, it._it, nullptr);
    }
    else
    {
        auto& it = utl::cast<RandIt>(rhs);
        res = compareNullable(_it, &it, nullptr);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
TRandIt<T>::copy(const Object& rhs)
{
    if (rhs.isA(TRandIt<T>))
    {
        auto& it = utl::cast<TRandIt<T>>(rhs);
        setIt(utl::clone(it._it));
    }
    else
    {
        auto& it = utl::cast<RandIt>(rhs);
        setIt(it.clone());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
TRandIt<T>::steal(Object& rhs_)
{
    if (rhs_.isA(TRandIt<T>))
    {
        auto& rhs = utl::cast<TRandIt<T>>(rhs_);
        if (_it != nullptr)
            delete _it;
        _it = rhs._it;
        rhs._it = nullptr;
    }
    else
    {
        auto& rhs = utl::cast<RandIt>(rhs_);
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

UTL_CLASS_IMPL_TPL(utl::TRandIt, T);
