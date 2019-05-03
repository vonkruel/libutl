#include <libutl/libutl.h>
#include <libutl/AutoPtr.h>
#include <libutl/Bool.h>
#include <libutl/MaxObject.h>
#include <libutl/String.h>
#include <libutl/Uint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL_ABC(utl::Object);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::clear()
{
    AutoPtr<utl::Object> newInstance = this->create();
    copy(*newInstance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Object::compare(const Object& rhs) const
{
    // default object comparison logic

    // if lhs or rhs has a key, we can re-start the comparison process using the key(s)
    const Object& thisKey = getKey();
    const Object& rhsKey = rhs.getKey();
    if ((&thisKey != this) || (&rhsKey != &rhs))
    {
        return thisKey.compare(rhsKey);
    }

    // as a last resort, compare addresses
    const void* lhsAddr = this;
    const void* rhsAddr = &rhs;
    return utl::compare(lhsAddr, rhsAddr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::copy(const Object& rhs)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::vclone(const Object& rhs)
{
    copy(rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::steal(Object& rhs)
{
    vclone(rhs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::dump(Stream& os, uint_t) const
{
    // get a string representation
    String s = toString();
    // get the object's key (if any)
    const Object& key = getKey();
    // if "toString" hasn't been overloaded (which is indicated by toString() returning only the
    // .. class name), also give the key's string representation.
    if ((s == getClassName()) && (&key != this) && (key.toString() != key.getClassName()))
    {
        s += ": ";
        s += key.toString();
    }
    os << s << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::dumpWithClassName(Stream& os, uint_t indent, uint_t level) const
{
    if (indent == 0)
    {
        os << "=== ";
    }
    os << getClassName() << endl;
    os.indent(indent);
    dump(os, level);
    os.unindent(indent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Object&
Object::getKey() const
{
    // no key by default
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const Object&
Object::getProxiedObject() const
{
    // not proxying anything by default
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object&
Object::getProxiedObject()
{
    // not proxying anything by default
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Object::hash(size_t size) const
{
    if (hasKey())
    {
        return getKey().hash(size);
    }
    else
    {
        size_t n = (size_t)this;
        return (n % (size_t)size);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::serialize(Stream&, uint_t, uint_t)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Object::serializeInNullable(Stream& is, uint_t mode)
{
    bool objectExists;
    utl::serialize(objectExists, is, io_rd, mode);
    if (objectExists)
    {
        return serializeInBoxed(is, mode);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::serializeOutNullable(const Object* object, Stream& os, uint_t mode)
{
    // first serialize a boolean value indicating whether or not an object is actually present
    bool objectExists = (object != nullptr);
    utl::serialize(objectExists, os, io_wr, mode);
    // if there is an object, serialize it (boxed)
    if (objectExists)
    {
        object->serializeOutBoxed(os, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::serializeNullable(Object*& object, Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
    {
        object = serializeInNullable(stream, mode);
    }
    else
    {
        serializeOutNullable(object, stream, mode);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object*
Object::serializeInBoxed(Stream& is, uint_t mode)
{
    // write the className
    String className;
    className.serialize(is, io_rd, mode);
    // use it to find the associated RunTimeClass object
    const RunTimeClass* rtc = RunTimeClass::find(className);
    // no RunTimeClass object -> we're done
    if (rtc == nullptr)
    {
        throw StreamSerializeEx(utl::clone(is.getNamePtr()));
    }
    // use the RunTimeClass object to create an instance of the class
    Object* res = rtc->create();
    AutoPtr<> resPtr = res;
    // serialize into this newly created instance
    res->serializeIn(is, mode);
    // don't destroy the object if it was serialized successfully
    resPtr.release();
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
Object::serializeOutBoxed(Stream& os, uint_t mode) const
{
    String className(getClassName());
    className.serialize(os, io_wr, mode);
    serializeOut(os, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
Object::toString() const
{
    String res = getClassName();
    const Object& key = getKey();
    if (&key != this)
    {
        res += ": ";
        res += key.toString();
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Object::operator String() const
{
    return toString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Object::allocatedSize() const
{
    return getClass()->size() + innerAllocatedSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
Object::innerAllocatedSize() const
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Object::addOwnedIt(const class FwdIt* it) const
{
    ABORT();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
Object::removeOwnedIt(const class FwdIt* it) const
{
    ABORT();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
