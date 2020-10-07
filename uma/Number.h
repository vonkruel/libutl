#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/String.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Numeric value.

   \author Adam McKee
   \ingroup math
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class Number : public Object
{
    UTL_CLASS_DECL_TPL(Number, T, Object);

public:
    /**
       Constructor.
       \param n initial value
    */
    Number(T n)
    {
        set(n);
    }

    virtual void copy(const Object& rhs);

    virtual int compare(const Object& rhs) const;

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    String toString() const;

    /**
       Convert to string.
       \param fmt sprintf(3) format string
    */
    String toString(const char* fmt) const;

    /** Get the value. */
    T
    get() const
    {
        return _n;
    }

    /** Set the value. */
    void
    set(T n)
    {
        _n = n;
    }

    /** Set from a string representation. */
    virtual Number<T>&
    set(const String&)
    {
        ABORT();
        return self;
    }

    /** Conversion to built-in type. */
    operator T() const
    {
        return _n;
    }

    /// \name Operations (non-const)
    //@{
    Number<T>&
    operator+=(T rhs)
    {
        _n += rhs;
        return self;
    }

    Number<T>&
    operator-=(T rhs)
    {
        _n -= rhs;
        return self;
    }

    Number<T>&
    operator*=(T rhs)
    {
        _n *= rhs;
        return self;
    }

    Number<T>&
    operator/=(T rhs)
    {
        _n /= rhs;
        return self;
    }

    Number<T>&
    operator+=(const Number<T>& rhs)
    {
        _n += rhs._n;
        return self;
    }

    Number<T>&
    operator-=(const Number<T>& rhs)
    {
        _n -= rhs._n;
        return self;
    }

    Number<T>&
    operator*=(const Number<T>& rhs)
    {
        _n *= rhs._n;
        return self;
    }

    Number<T>&
    operator/=(const Number<T>& rhs)
    {
        _n /= rhs._n;
        return self;
    }
    //@}

    /// \name Operations (const)
    //@{
    T
    operator+(T rhs) const
    {
        return _n + rhs;
    }

    T
    operator-(T rhs) const
    {
        return _n - rhs;
    }

    T operator*(T rhs) const
    {
        return _n * rhs;
    }

    T
    operator/(T rhs) const
    {
        return _n / rhs;
    }

    T
    operator+(const Number<T>& rhs) const
    {
        return _n + rhs._n;
    }

    T
    operator-(const Number<T>& rhs) const
    {
        return _n - rhs._n;
    }

    Number<T> operator*(const Number<T>& rhs) const
    {
        return _n * rhs._n;
    }

    Number<T>
    operator/(const Number<T>& rhs) const
    {
        return _n / rhs._n;
    }

    Number<T>&
    operator++()
    {
        ++_n;
        return self;
    }

    Number<T>&
    operator--()
    {
        --_n;
        return self;
    }

    T
    operator++(int)
    {
        return _n++;
    }

    T
    operator--(int)
    {
        return _n--;
    }
    //@}
protected:
    T _n;

private:
    void
    init()
    {
        _n = 0;
    }

    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Tokenizer.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::Number, T);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
Number<T>::copy(const Object& rhs)
{
    auto& num = utl::cast<Number<T>>(rhs);
    super::copy(num);
    _n = num._n;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
int
Number<T>::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(Number<T>))
    {
        auto& num = utl::cast<Number<T>>(rhs);
        res = utl::compare(_n, num._n);
    }
    else
    {
        res = super::compare(rhs);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
Number<T>::serialize(Stream& stream, uint_t io, uint_t mode)
{
    super::serialize(stream, io, mode);
    if (io == io_rd)
    {
        String s;
        s.serializeIn(stream, mode);
        set(s);
    }
    else
    {
        toString().serializeOut(stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
String
Number<T>::toString() const
{
    char fmt[64] = "%";
    strcat(fmt, Printf<T>::formatType);
    return toString(fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
String
Number<T>::toString(const char* fmt) const
{
    char buf[256];
    sprintf(buf, fmt, _n);
    return buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
