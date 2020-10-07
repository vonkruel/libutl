#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A "smart" pointer that can also be dumb.

   AutoPtr is similar to std::unique_ptr<>, but it attaches an ownership flag to the underlying
   pointer, instead of always being responsible for deleting the referenced object.

   AutoPtr assumes ownership in all cases except these:

   \arg 1. move-construction/assignment from another AutoPtr that does not have ownership
   \arg 2. after set() is called with \b owner = false explicitly specified
   \arg 3. after release() is called

   AutoPtr<> isn't really in competition with std::unique_ptr<>, because the ability to act like
   a "dumb/regular" pointer is either helpful or not helpful in each case.  When it's not helpful,
   there's no reason to consider the use of AutoPtr<>.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T = Object>
class AutoPtr
{
public:
    /** Constructor. */
    AutoPtr()
    {
        init();
    }

    /**
       Constructor.
       \param ptr pointer
       \param owner \b owner flag
    */
    AutoPtr(T* ptr, bool owner = true)
    {
        init();
        set(ptr, owner);
    }

    /** Move constructor (moving AutoPtr<T>). */
    AutoPtr(AutoPtr<T>&& rhs) noexcept
    {
        init();
        steal(rhs);
    }

    /** Move constructor (moving AutoPtr<NonT>). */
    template <typename NonT>
    AutoPtr(AutoPtr<NonT>&& rhs) noexcept
    {
        init();
        stealNonT(rhs);
    }

    /** Destructor. */
    ~AutoPtr()
    {
        deInit();
    }

    /// \name Misc. Accessors
    //@{
    /** Get the pointer. */
    T*
    get() const
    {
        return _ptr;
    }

    /** Determine whether the pointer is nullptr. */
    bool
    isNull() const
    {
        return (_ptr == nullptr);
    }

    /** Get \b owner flag. */
    bool
    isOwner() const
    {
        return _owner;
    }
    //@}

    /// \name Manipulation
    //@{
    /**
       Delete the pointee if the \b owner flag is true.
       Set pointer = nullptr.
    */
    void
    clear()
    {
        if (_owner)
            delete _ptr;
        _ptr = nullptr;
        _owner = false;
    }

    /**
       Release ownership (while keeping the pointer).
       \return the underlying pointer
    */
    T*
    release()
    {
        _owner = false;
        return _ptr;
    }

    /** Set new pointer and ownership flag. */
    void
    set(T* ptr, bool owner = true)
    {
        if (ptr != _ptr)
        {
            if (_owner)
                delete _ptr;
            _ptr = ptr;
        }
        _owner = owner;
    }

    /** Set new pointer and ownership flag. */
    template <typename NonT>
    void
    setNonT(NonT* ptr, bool owner = true)
    {
        set(dynamic_cast<T*>(ptr), owner);
        ASSERTD(_ptr != nullptr);
    }
    //@}

    /// \name Assignment Operators
    //@{
    /** Steal from AutoPtr<T>. */
    void
    steal(AutoPtr<T>& rhs)
    {
        set(rhs.get(), rhs.isOwner());
        rhs.release();
    }

    /** Steal from AutoPtr<NonT>. */
    template <typename NonT>
    void
    stealNonT(AutoPtr<NonT>& rhs)
    {
        setNonT(rhs.get(), rhs.isOwner());
        rhs.release();
    }

    /** Move assignment from AutoPtr<T>. */
    AutoPtr<T>&
    operator=(AutoPtr<T>&& rhs) noexcept
    {
        steal(rhs);
        return self;
    }

    /** Move assignment from AutoPtr<NonT>. */
    template <typename NonT>
    AutoPtr<T>&
    operator=(const AutoPtr<NonT>&& rhs) noexcept
    {
        steal(rhs);
        return self;
    }

    /** Assignment operator from T*. */
    AutoPtr<T>&
    operator=(T* ptr)
    {
        set(ptr);
        return self;
    }

    /** Assignment operator from NonT*. */
    template <typename NonT>
    AutoPtr<T>&
    operator=(NonT* ptr)
    {
        setNonT(ptr);
        return self;
    }
    //@}

    /// \name Pointer Dereference and Conversion Operators
    //@{
    /** Pointer dereference operator. */
    T& operator*() const
    {
        return *ptr();
    }

    /** Member access operator. */
    T* operator->() const
    {
        return ptr();
    }

    /** Conversion to pointer. */
    operator T*() const
    {
        return _ptr;
    }
    //@}
protected:
    T*
    ptr() const
    {
        ASSERTD(_ptr != nullptr);
        return _ptr;
    }

protected:
    T* _ptr;
    bool _owner;

private:
    void
    init()
    {
        _ptr = nullptr;
        _owner = false;
    }
    void
    deInit()
    {
        if (_owner)
            delete _ptr;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename X, typename Y>
bool
operator==(const utl::AutoPtr<X>& lhs, const utl::AutoPtr<Y>& rhs)
{
    return (lhs.get() == rhs.get());
}

template <typename T>
bool
operator==(const utl::AutoPtr<T>& lhs, std::nullptr_t rhs)
{
    return (lhs.get() == nullptr);
}

template <typename T>
bool
operator==(std::nullptr_t lhs, const utl::AutoPtr<T>& rhs)
{
    return (rhs.get() == nullptr);
}

template <typename X, typename Y>
bool
operator!=(const utl::AutoPtr<X>& lhs, const utl::AutoPtr<Y>& rhs)
{
    return (lhs.get() != rhs.get());
}

template <typename T>
bool
operator!=(const utl::AutoPtr<T>& lhs, std::nullptr_t rhs)
{
    return (lhs.get() != nullptr);
}

template <typename T>
bool
operator!=(std::nullptr_t lhs, const utl::AutoPtr<T>& rhs)
{
    return (rhs.get() != nullptr);
}
