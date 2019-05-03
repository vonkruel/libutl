#include <libutl/libutl.h>
#include <libutl/MaxObject.h>
#include <libutl/ObjectRegistry.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RegisteredObject ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class RegisteredObject : public Object
{
    UTL_CLASS_DECL(RegisteredObject, Object);
    UTL_CLASS_NO_COPY;
    UTL_CLASS_NO_COMPARE;

public:
    RegisteredObject(Object* object)
    {
        _object = object;
        _refCount = 1;
    }

    virtual size_t
    innerAllocatedSize() const
    {
        return _object->allocatedSize();
    }

    Object*
    get() const
    {
        return _object;
    }

    void
    addRef()
    {
        _refCount.fetch_add(1, std::memory_order_relaxed);
    }

    size_t
    removeRef()
    {
        return _refCount.fetch_sub(1, std::memory_order_relaxed) - 1;
    }

private:
    void
    init()
    {
        _object = nullptr;
        _refCount = 0;
    }
    void
    deInit()
    {
        delete _object;
    }

private:
    Object* _object;
    std::atomic_size_t _refCount;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// RegisteredObjectOrdering ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class RegisteredObjectOrdering : public Ordering
{
    UTL_CLASS_DECL(RegisteredObjectOrdering, Ordering);

public:
    RegisteredObjectOrdering(utl::Ordering* ordering)
    {
        _ordering = ordering;
    }

    virtual int cmp(const Object* lhs, const Object* rhs) const;

private:
    void
    init()
    {
        ABORT();
    }
    void
    deInit()
    {
        delete _ordering;
    }

private:
    Ordering* _ordering;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

int
RegisteredObjectOrdering::cmp(const Object* lhs, const Object* rhs) const
{
    // skip past RegisteredObject's to their contained objects
    ASSERTD((lhs != nullptr) && (rhs != nullptr));
    const RegisteredObject* ro;
    if (lhs->isA(RegisteredObject))
    {
        ro = utl::cast<RegisteredObject>(lhs);
        lhs = ro->get();
    }
    else if (lhs == &maxObject)
    {
        return 1;
    }
    if (rhs->isA(RegisteredObject))
    {
        ro = utl::cast<RegisteredObject>(rhs);
        rhs = ro->get();
    }

    // types are not the same -> result is comparison of type names
    auto lhsRTC = lhs->getClass();
    auto rhsRTC = rhs->getClass();
    ASSERTD((lhsRTC != nullptr) && (rhsRTC != nullptr));
    if (lhsRTC != rhsRTC)
    {
        int res = strcmp(lhsRTC->name(), rhsRTC->name());
        ASSERTD(res != 0);
        return res;
    }

    // types matched -> compare the objects
    if (_ordering == nullptr)
        return lhs->compare(*rhs);
    return _ordering->cmp(lhs, rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ObjectRegistry /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
ObjectRegistry::innerAllocatedSize() const
{
    RWlockGuard lock(_lock, io_rd);
    return _objects.innerAllocatedSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectRegistry::setOrdering(Ordering* ordering)
{
    _objects.setOrdering(new RegisteredObjectOrdering(ordering));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
ObjectRegistry::add(Object* object)
{
    // find the object
    RWlockGuard lock(_lock, io_rd);
    auto ro = utl::cast<RegisteredObject>(_objects.find(*object));

    // not found -> write-lock and try again
    if (ro == nullptr)
    {
        lock.wrlock();
        ro = utl::cast<RegisteredObject>(_objects.find(*object));
    }

    // found the object -> add a reference & return it
    if (ro != nullptr)
    {
        delete object;
        ro->addRef();
        return ro->get();
    }

    // add the object
    ro = new RegisteredObject(object);
    _objects += ro;
    return object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectRegistry::remove(Object* object)
{
    RWlockGuard lock(_lock, io_rd);

    // find ro
    RBtree::iterator it = _objects.findIt(*object);
    RegisteredObject* ro = (RegisteredObject*)*it;
    ASSERTD(ro != nullptr);

    // still other reference holders -> return
    if (ro->removeRef() > 0)
        return;

    // add the reference back...
    ro->addRef();

    // write-lock
    lock.wrlock();

    // no other reference-holders -> remove ro
    if (ro->removeRef() == 0)
    {
        _objects.removeIt(it);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::ObjectRegistry);
UTL_CLASS_IMPL(utl::RegisteredObject);
UTL_CLASS_IMPL(utl::RegisteredObjectOrdering);
