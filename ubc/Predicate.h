#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/FlagsMI.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Predicate ///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Logical predicate abstraction.

   A Predicate is a statement about an object that is either true or false.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Predicate : public Object, protected FlagsMI
{
    UTL_CLASS_DECL_ABC(Predicate, Object);
    UTL_CLASS_DEFID;

public:
    /** Get the ownership flag. */
    bool
    isOwner() const
    {
        return getFlag(flg_owner);
    }

    /** Set the ownership flag. */
    void
    setOwner(bool owner)
    {
        setFlag(flg_owner, owner);
    }

    /**
       Compare with an object by evaluating the predicate for the object.
       \return 0 if predicate is true for rhs, 1 otherwise
       \param rhs object to compare with
    */
    virtual int compare(const Object& rhs) const;

    /**
       Evaluate the predicate for the given object.
       \return true if object satisfies predicate, false otherwise
       \param object object to evaluate predicate for
    */
    virtual bool eval(const Object* object) const = 0;

    /**
       Another way of writing \ref eval.  For example:

       \code
       Object* object;
       .
       .
       .
       MyPredicate myPredicate;
       bool predicateSatisfied = myPredicate(object);
       \endcode

       \return true if object satisfies predicate, false otherwise
       \param object object to evaluate predicate for.
    */
    bool
    operator()(const Object* object) const
    {
        return eval(object);
    }

private:
    enum flg_t
    {
        flg_owner
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// IsA /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   is-A predicate.

   The IsA predicate takes a RunTimeClass as a parameter.  A given object satisfies the predicate
   iff its class is derived from this class.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class IsA : public Predicate
{
    UTL_CLASS_DECL(IsA, Predicate);

public:
    /**
       Constructor.
       \param rtc class to test inheritance against
    */
    IsA(const RunTimeClass* rtc)
    {
        _rtc = rtc;
    }

    /**
       Determine whether the given object is derived from the class.
       \return true if "is-A" relationship exists, false otherwise
       \param object given object
    */
    virtual bool
    eval(const Object* object) const
    {
        if ((_rtc == nullptr) || (object == nullptr))
            return false;
        return object->_isA(_rtc);
    }

private:
    void
    init()
    {
        _rtc = nullptr;
    }
    void
    deInit()
    {
    }
    const RunTimeClass* _rtc;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// UnaryPredicate //////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for single-parameter predicates.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class UnaryPredicate : public Predicate
{
    UTL_CLASS_DECL_ABC(UnaryPredicate, Predicate);

public:
    /**
       Constructor.
       \param object predicate parameter (make a copy if isOwner())
       \param owner \b owner flag for the parameter
    */
    UnaryPredicate(const Object& object, bool owner = true);

    /**
       Constructor.
       \param object predicate parameter
       \param owner \b owner flag for the parameter
    */
    UnaryPredicate(const Object* object, bool owner = true);

    /** See Object::clear(). */
    void
    clear()
    {
        setObject(nullptr);
    }

    /** Copy another UnaryPredicate by copying its parameter. */
    virtual void copy(const Object& rhs);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Get the parameter. */
    const Object*
    getObject() const
    {
        return _object;
    }

    /** Set the parameter (make a copy if isOwner() = true). */
    void
    setObject(const Object& object)
    {
        if (isOwner())
            setObject(object.clone());
        else
            setObject(&object);
    }

    /** Set the parameter. */
    void
    setObject(const Object* object)
    {
        if (isOwner())
            delete _object;
        _object = const_cast<Object*>(object);
    }

protected:
    Object* _object;

private:
    void
    init()
    {
        _object = nullptr;
        setOwner(true);
    }
    void
    deInit()
    {
        clear();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// BinaryPredicate /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for two-parameter predicates.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class BinaryPredicate : public Predicate
{
    UTL_CLASS_DECL_ABC(BinaryPredicate, Predicate);

public:
    /**
       Constructor.
       \param lhs left-hand-side parameter (make a copy if owner = true)
       \param rhs right-hand-side parameter (make a copy if owner = true)
       \param owner \b owner flag
    */
    BinaryPredicate(const Object& lhs, const Object& rhs, bool owner = true);

    /**
       Constructor.
       \param lhs left-hand-side parameter
       \param rhs right-hand-side parameter
       \param owner \b owner flag
    */
    BinaryPredicate(const Object* lhs, const Object* rhs, bool owner = true);

    /** See Object::clear. */
    void
    clear()
    {
        setLHS(nullptr);
        setRHS(nullptr);
    }

    virtual void copy(const Object& rhs);

    virtual void serialize(Stream& stream, uint_t io, uint_t mode = ser_default);

    /** Get the left-hand-side parameter. */
    const Object*
    getLHS() const
    {
        return _lhs;
    }

    /** Get the right-hand-side parameter. */
    const Object*
    getRHS() const
    {
        return _rhs;
    }

    /** Set the left-hand-side parameter (make a copy if isOwner() = true). */
    void
    setLHS(const Object& lhs)
    {
        if (isOwner())
            setLHS(lhs.clone());
        else
            setLHS(&lhs);
    }

    /** Set the left-hand-side parameter. */
    void
    setLHS(const Object* lhs)
    {
        if (isOwner())
            delete _lhs;
        _lhs = const_cast<Object*>(lhs);
    }

    /** Set the right-hand-side parameter (make a copy if isOwner() = true). */
    void
    setRHS(const Object& lhs)
    {
        if (isOwner())
            setRHS(lhs.clone());
        else
            setRHS(&lhs);
    }

    /** Set the right-hand-side parameter. */
    void
    setRHS(const Object* rhs)
    {
        if (isOwner())
            delete _rhs;
        _rhs = const_cast<Object*>(rhs);
    }

protected:
    Object* _lhs;
    Object* _rhs;

private:
    void
    init()
    {
        _lhs = _rhs = nullptr;
        setOwner(true);
    }
    void
    deInit()
    {
        clear();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// ComparisonPredicate /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for comparison predicates.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class ComparisonPredicate : public UnaryPredicate
{
    UTL_CLASS_DECL_ABC(ComparisonPredicate, UnaryPredicate);

public:
    /**
       Constructor.
       \param object predicate parameter (make a copy if isOwner())
       \param owner \b owner flag for the parameter
       \param ordering (optional : nullptr) ordering
    */
    ComparisonPredicate(const Object& object, bool owner = true, const Ordering* ordering = nullptr)
        : UnaryPredicate(object, owner)
    {
        _ordering = utl::clone(ordering);
    }

    /**
       Constructor.
       \param object predicate parameter
       \param owner \b owner flag for the parameter
       \param ordering (optional : nullptr) ordering
    */
    ComparisonPredicate(const Object* object, bool owner = true, const Ordering* ordering = nullptr)
        : UnaryPredicate(object, owner)
    {
        _ordering = utl::clone(ordering);
    }

    /**
       Compare the given object with self's object.
       \return < 0 if object < self, 0 if object = self, > 0 if object > self
       \param object to compare with
    */
    int
    cmp(const Object* object) const
    {
        return compareNullable(object, _object, _ordering);
    }

    /** Get the ordering. */
    const Ordering*
    ordering() const
    {
        return _ordering;
    }

    /** Set the ordering to a copy of the given ordering. */
    void
    setOrdering(const Ordering& ordering)
    {
        delete _ordering;
        _ordering = ordering.clone();
    }

    /** Set the ordering. */
    void
    setOrdering(Ordering* ordering)
    {
        delete _ordering;
        _ordering = ordering;
    }

private:
    void
    init()
    {
        _ordering = nullptr;
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
// Not /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Logical \b not predicate.

   The Not predicate takes a single Predicate parameter.  It is true iff
   its sub-predicate is false.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Not : public UnaryPredicate
{
    UTL_CLASS_DECL(Not, UnaryPredicate);
    UTL_CLASS_DEFID;

public:
    Not(const Predicate* pred, bool owner = true)
        : UnaryPredicate(pred, owner)
    {
    }

    virtual bool
    eval(const Object* object) const
    {
        if ((_object == nullptr) || (object == nullptr))
            return false;
        return !utl::cast<Predicate>(_object)->eval(object);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// And /////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Logical \b and predicate.

   The And predicate takes two Predicate parameters.  It is true iff
   both its sub-predicates are true.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class And : public BinaryPredicate
{
    UTL_CLASS_DECL(And, BinaryPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param lhs left-hand-side predicate (make a copy if owner = true)
       \param rhs right-hand-side predicate (make a copy if owner = true)
       \param owner \b owner flag
    */
    And(const Predicate& lhs, const Predicate& rhs, bool owner = true)
        : BinaryPredicate(lhs, rhs, owner)
    {
    }

    /**
       Constructor.
       \param lhs left-hand-side predicate
       \param rhs right-hand-side predicate
       \param owner \b owner flag
    */
    And(const Predicate* lhs, const Predicate* rhs, bool owner = true)
        : BinaryPredicate(lhs, rhs, owner)
    {
    }

    /**
       The object satisfies the predicate iff it satisfies both
       the sub-predicates.
    */
    virtual bool
    eval(const Object* object) const
    {
        return lhs()->eval(object) && rhs()->eval(object);
    }

    /** Get the left-hand-side predicate. */
    const Predicate*
    lhs() const
    {
        return utl::cast<Predicate>(_lhs);
    }

    /** Get the right-hand-side predicate. */
    const Predicate*
    rhs() const
    {
        return utl::cast<Predicate>(_rhs);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Or //////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Logical \b or predicate.

   The Or predicate takes two Predicate parameters.  It is true iff
   at least one of its sub-predicates is true.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class Or : public BinaryPredicate
{
    UTL_CLASS_DECL(Or, BinaryPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param lhs left-hand-side predicate (make a copy if owner = true)
       \param rhs right-hand-side predicate (make a copy if owner = true)
       \param owner \b owner flag
    */
    Or(const Predicate& lhs, const Predicate& rhs, bool owner = true)
        : BinaryPredicate(lhs, rhs, owner)
    {
    }

    /**
       Constructor.
       \param lhs left-hand-side predicate
       \param rhs right-hand-side predicate
       \param owner \b owner flag
    */
    Or(const Predicate* lhs, const Predicate* rhs, bool owner = true)
        : BinaryPredicate(lhs, rhs, owner)
    {
    }

    /**
       The object satisfies the predicate iff it satisfies at least
       one of the sub-predicates.
    */
    virtual bool
    eval(const Object* object) const
    {
        return lhs()->eval(object) || rhs()->eval(object);
    }

    /** Get the left-hand-side predicate. */
    const Predicate*
    lhs() const
    {
        return utl::cast<Predicate>(_lhs);
    }

    /** Get the right-hand-side predicate. */
    const Predicate*
    rhs() const
    {
        return utl::cast<Predicate>(_rhs);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// EqualTo /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Equal-To predicate.

   The EqualTo predicate takes a single object as a parameter.  A given
   object satisfies the predicate iff it is equal to this object.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class EqualTo : public ComparisonPredicate
{
    UTL_CLASS_DECL(EqualTo, ComparisonPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param object object to compare against
              (make a copy if owner = true)
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    EqualTo(const Object& object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       Constructor.
       \param object object to compare against
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    EqualTo(const Object* object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       The object satisfies the predicate iff it is equal to the
       associated object.
    */
    virtual bool
    eval(const Object* object) const
    {
        return (cmp(object) == 0);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// LessThan ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Less-Than predicate.

   The LessThan predicate takes a single object as a parameter.  A given
   object satisfies the predicate iff it is less than this object.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LessThan : public ComparisonPredicate
{
    UTL_CLASS_DECL(LessThan, ComparisonPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param object object to compare against
              (make a copy if owner = true)
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    LessThan(const Object& object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       Constructor.
       \param object object to compare against
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    LessThan(const Object* object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       The object satisfies the predicate iff it is less than the
       associated object.
    */
    virtual bool
    eval(const Object* object) const
    {
        return (cmp(object) < 0);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// LessThanOrEqualTo ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Less-Than-Or-Equal-To predicate.

   The LessThanEqualTo predicate takes a single object as a parameter.
   A given object satisfies the predicate iff it is less than or equal
   to this object.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LessThanOrEqualTo : public ComparisonPredicate
{
    UTL_CLASS_DECL(LessThanOrEqualTo, ComparisonPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param object object to compare against
              (make a copy if owner = true)
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    LessThanOrEqualTo(const Object& object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       Constructor.
       \param object object to compare against
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    LessThanOrEqualTo(const Object* object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       The object satisfies the predicate iff it is less than or
       equal to the associated object.
    */
    virtual bool
    eval(const Object* object) const
    {
        return (cmp(object) <= 0);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// GreaterThan /////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Greater-Than predicate.

   The GreaterThan predicate takes a single object as a parameter.
   A given object satisfies the predicate iff it is greater than this object.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class GreaterThan : public ComparisonPredicate
{
    UTL_CLASS_DECL(GreaterThan, ComparisonPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param object object to compare against
              (make a copy if owner = true)
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    GreaterThan(const Object& object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       Constructor.
       \param object object to compare against
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    GreaterThan(const Object* object, bool owner = true, const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       The object satisfies the predicate iff it is greater than
       the associated object.
    */
    virtual bool
    eval(const Object* object) const
    {
        return (cmp(object) > 0);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// GreaterThanOrEqualTo ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Greater-Than-Or-Equal-To predicate.

   The GreaterThanOrEqualTo predicate takes a single object as a parameter.
   A given object satisfies the predicate iff it is greater than or equal
   to this object.

   \author Adam McKee
   \ingroup predicate
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class GreaterThanOrEqualTo : public ComparisonPredicate
{
    UTL_CLASS_DECL(GreaterThanOrEqualTo, ComparisonPredicate);
    UTL_CLASS_DEFID;

public:
    /**
       Constructor.
       \param object object to compare against
              (make a copy if owner = true)
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    GreaterThanOrEqualTo(const Object& object,
                         bool owner = true,
                         const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       Constructor.
       \param object object to compare against
       \param owner (optional : true) \b owner flag
       \param ordering (optional) ordering
    */
    GreaterThanOrEqualTo(const Object* object,
                         bool owner = true,
                         const Ordering* ordering = nullptr)
        : ComparisonPredicate(object, owner, ordering)
    {
    }

    /**
       The object satisfies the predicate iff it is greater than or
       equal to the associated object.
    */
    virtual bool
    eval(const Object* object) const
    {
        return (cmp(object) >= 0);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   (! Predicate) constructs an instance of the Not predicate.
   \see Not
   \ingroup predicate
*/
inline utl::Not operator!(const utl::Predicate& pred)
{
    return utl::Not(pred, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   (Predicate && Predicate) constructs an instance of the And predicate.
   \see And
   \ingroup predicate
*/
inline utl::And
operator&&(const utl::Predicate& lhs, const utl::Predicate& rhs)
{
    return utl::And(lhs, rhs, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   (Predicate || Predicate) constructs an instance of the Or predicate.
   \see Or
   \ingroup predicate
*/
inline utl::Or
operator||(const utl::Predicate& lhs, const utl::Predicate& rhs)
{
    return utl::Or(lhs, rhs, false);
}
