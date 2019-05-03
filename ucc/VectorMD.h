#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Vector.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// VectorMD ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstract base for n-dimensional vectors.

   \author Adam McKee
   \ingroup collection
*/

template <class T> class VectorMD : public Object
{
    UTL_CLASS_DECL_TPL(VectorMD, T, Object);

public:
    VectorMD(size_t numDims);

    /** Free the array. */
    void clear();

    virtual int compare(const Object& rhs) const;
    virtual void copy(const Object&);
    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    virtual size_t getSize() const;

protected:
    void setNumDims(size_t numDims);
    size_t _numDims;
    size_t* _dims;
    T* _array;

private:
    void
    init()
    {
        _numDims = 0;
        _dims = nullptr;
        _array = nullptr;
    }
    void
    deInit()
    {
        delete[] _dims;
        clear();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector1d ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   One-dimensional vector.

   \author Adam McKee
   \ingroup collection
*/

template <class T> class Vector1d : public VectorMD<T>
{
    UTL_CLASS_DECL_TPL(Vector1d, T, VectorMD<T>);

public:
    Vector1d(size_t dim0)
    {
        init();
        set(dim0);
    }

    void set(size_t dim0);

    const T&
    operator()(size_t idx0) const
    {
        return get(idx0);
    }

    T&
    operator()(size_t idx0)
    {
        return get(idx0);
    }

    const T& get(size_t idx0) const;

    T& get(size_t idx0);

private:
    void
    init()
    {
        this->setNumDims(1);
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector2d ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Two-dimensional vector.

   \author Adam McKee
   \ingroup collection
*/

template <class T> class Vector2d : public VectorMD<T>
{
    UTL_CLASS_DECL_TPL(Vector2d, T, VectorMD<T>);

public:
    Vector2d(size_t dim0, size_t dim1)
    {
        init();
        set(dim0, dim1);
    }

    void set(size_t dim0, size_t dim1);

    const T&
    operator()(size_t idx0, size_t idx1) const
    {
        return get(idx0, idx1);
    }

    T&
    operator()(size_t idx0, size_t idx1)
    {
        return get(idx0, idx1);
    }

    const T& get(size_t idx0, size_t idx1) const;

    T& get(size_t idx0, size_t idx1);

private:
    void
    init()
    {
        this->setNumDims(2);
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vector3d ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Three-dimensional vector.

   \author Adam McKee
   \ingroup collection
*/

template <class T> class Vector3d : public VectorMD<T>
{
    UTL_CLASS_DECL_TPL(Vector3d, T, VectorMD<T>);

public:
    Vector3d(size_t dim0, size_t dim1, size_t dim2)
    {
        init();
        set(dim0, dim1, dim2);
    }

    void set(size_t dim0, size_t dim1, size_t dim2);

    const T&
    operator()(size_t idx0, size_t idx1, size_t idx2) const
    {
        return get(idx0, idx1, idx2);
    }

    T&
    operator()(size_t idx0, size_t idx1, size_t idx2)
    {
        return get(idx0, idx1, idx2);
    }

    const T& get(size_t idx0, size_t idx1, size_t idx2) const;

    T& get(size_t idx0, size_t idx1, size_t idx2);

private:
    void
    init()
    {
        this->setNumDims(3);
    }
    void
    deInit()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> VectorMD<T>::VectorMD(size_t numDims)
{
    ASSERTD(numDims > 0);
    init();
    setNumDims(numDims);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
VectorMD<T>::clear()
{
    size_t i;
    for (i = 0; i < _numDims; i++)
    {
        _dims[i] = 0;
    }
    delete[] _array;
    _array = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
int
VectorMD<T>::compare(const Object& rhs) const
{
    int res;
    if (rhs.isA(VectorMD<T>))
    {
        auto& vect = utl::cast<VectorMD<T>>(rhs);
        res = utl::compare(_dims, _numDims, vect._dims, vect._numDims);
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
VectorMD<T>::copy(const Object& rhs)
{
    auto& vect = utl::cast<VectorMD<T>>(rhs);
    ASSERTD(_numDims == vect._numDims);
    if (memcmp(_dims, vect._dims, _numDims * sizeof(size_t)) != 0)
    {
        utl::copy(_dims, vect._dims, _numDims);
        delete[] _array;
        _array = new T[getSize()];
    }
    utl::copy(_array, vect._array, getSize());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
VectorMD<T>::serialize(Stream& stream, uint_t io, uint_t mode)
{
    // serialize _numDims, _dims[]
    utl::serialize(_numDims, stream, io, mode);
    if (io == io_rd)
    {
        delete[] _dims;
        _dims = new size_t[_numDims];
        utl::serialize(_dims, _numDims, stream, io, mode);

        delete[] _array;
        _array = nullptr;
        bool arrayExists;
        utl::serialize(arrayExists, stream, io, mode);

        if (arrayExists)
        {
            _array = new T[getSize()];
            utl::serialize(_array, getSize(), stream, io, mode);
        }
    }
    else
    {
        utl::serialize(_dims, _numDims, stream, io, mode);

        bool arrayExists = (_array != nullptr);
        utl::serialize(arrayExists, stream, io, mode);

        if (_array != nullptr)
        {
            utl::serialize(_array, getSize(), stream, io, mode);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
size_t
VectorMD<T>::getSize() const
{
    size_t res = _dims[0];
    size_t i;
    for (i = 1; i < _numDims; i++)
    {
        res *= _dims[i];
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
VectorMD<T>::setNumDims(size_t numDims)
{
    ASSERTD(_numDims == 0);
    _numDims = numDims;
    _dims = new size_t[_numDims];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
Vector1d<T>::set(size_t dim0)
{
    this->clear();
    this->_dims[0] = dim0;
    this->_array = new T[this->getSize()];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const T&
Vector1d<T>::get(size_t idx0) const
{
    ASSERTD(idx0 < this->_dims[0]);
    return this->_array[idx0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T&
Vector1d<T>::get(size_t idx0)
{
    ASSERTD(idx0 < this->_dims[0]);
    return this->_array[idx0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
Vector2d<T>::set(size_t dim0, size_t dim1)
{
    this->clear();
    this->_dims[0] = dim0;
    this->_dims[1] = dim1;
    this->_array = new T[this->getSize()];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const T&
Vector2d<T>::get(size_t idx0, size_t idx1) const
{
    ASSERTD(idx0 < this->_dims[0]);
    ASSERTD(idx1 < this->_dims[1]);
    return this->_array[(idx0 * this->_dims[1]) + idx1];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T&
Vector2d<T>::get(size_t idx0, size_t idx1)
{
    ASSERTD(idx0 < this->_dims[0]);
    ASSERTD(idx1 < this->_dims[1]);
    return this->_array[(idx0 * this->_dims[1]) + idx1];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void
Vector3d<T>::set(size_t dim0, size_t dim1, size_t dim2)
{
    this->clear();
    this->_dims[0] = dim0;
    this->_dims[1] = dim1;
    this->_dims[2] = dim2;
    this->_array = new T[this->getSize()];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const T&
Vector3d<T>::get(size_t idx0, size_t idx1, size_t idx2) const
{
    ASSERTD(idx0 < this->_dims[0]);
    ASSERTD(idx1 < this->_dims[1]);
    ASSERTD(idx2 < this->_dims[2]);
    return this->_array[(((idx0 * this->_dims[1]) + idx1) * this->_dims[2]) + idx2];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T&
Vector3d<T>::get(size_t idx0, size_t idx1, size_t idx2)
{
    ASSERTD(idx0 < this->_dims[0]);
    ASSERTD(idx1 < this->_dims[1]);
    ASSERTD(idx2 < this->_dims[2]);
    return this->_array[(((idx0 * this->_dims[1]) + idx1) * this->_dims[2]) + idx2];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_TPL(utl::Vector1d, T);
UTL_CLASS_IMPL_TPL(utl::Vector2d, T);
UTL_CLASS_IMPL_TPL(utl::Vector3d, T);
UTL_CLASS_IMPL_TPL(utl::VectorMD, T);
