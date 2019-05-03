#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reference to the object the method was invoked on.
   \ingroup macros
*/
#define self (*this)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reference to the object the method was invoked on (adding const qualifier).
   \ingroup macros
*/
#define const_self (static_cast<const std::decay<decltype(*this)>::type&>(*this))

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Pointer to the object the method was invoked on (adding const qualifier).
   \ingroup macros
*/
#define const_this (&const_cast_self)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Reference to the object the method was invoked on (casting away const).
   \ingroup macros
*/
#define const_cast_self (const_cast<std::decay<decltype(*this)>::type&>(*this))

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Pointer to the object the method was invoked on (casting away const).
   \ingroup macros
*/
#define const_cast_this (&const_cast_self)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declares a defect if x is false (or 0).
   \ingroup macros
*/
#undef ASSERT
#define ASSERT(x)                                                                                  \
    if (!(x))                                                                                      \
    utl::abort(__FILE__, __LINE__, "assertion failed: " #x)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Immediately terminates the program.
   \ingroup macros
*/
#define ABORT() utl::abort(__FILE__, __LINE__)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Terminates with an error message.
   \ingroup macros
*/
#define DIE(text) utl::die(__FILE__, __LINE__, text)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Expands to a no-op - sometimes useful when debugging.
   \ingroup macros
*/
#define BREAKPOINT                                                                                 \
    {                                                                                              \
        int __bp__ = 0;                                                                            \
        __bp__ = __bp__;                                                                           \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Expands to the name of an anonymous local variable.
   \ingroup macros
*/
#ifdef __COUNTER__
#define UTL_ANONYMOUS_VARIABLE(str) UTL_CONCATENATE(str, __COUNTER__)
#else
#define UTL_ANONYMOUS_VARIABLE(str) UTL_CONCATENATE(str, __LINE__)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Concatenate the given arguments (which must be strings).
   \ingroup macros
*/
#define UTL_CONCATENATE_IMPL(a, b) a##b

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Concatenate the given arguments (both of which may be subject to macro expansion themselves).
   \ingroup macros
*/
#define UTL_CONCATENATE(a, b) UTL_CONCATENATE_IMPL(a, b)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Conditional hint: indicate to the compiler that condition \b x is very likely to be true.
   \ingroup macros
*/
#if UTL_CC == UTL_CC_MSVC
#define likely(x) (x)
#else
#define likely(x) __builtin_expect(!!(x), 1)
#endif

/**
   Conditional hint: indicate to the compiler that condition \b x is very likely to be false.
   \ingroup macros
*/
#if UTL_CC == UTL_CC_MSVC
#define unlikely(x) (x)
#else
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Print a value (useful when debugging)
   \ingroup macros
*/
#define UTL_PRINT(x) cout << #x " = " << x << endl

////////////////////////////////////////////////////////////////////////////////////////////////////

#if (UTL_CC == UTL_CC_MSVC)
#define UTL_ATTRIBUTE_USED
#define UTL_ATTRIBUTE_VISIBLE
#else
#define UTL_ATTRIBUTE_USED __attribute__((__used__))
#define UTL_ATTRIBUTE_VISIBLE __attribute__((__externally_visible__))
#endif

/**
   \def UTL_ATTRIBUTE_USED
   Use this suffix on a static class data member to prevent it from being optimized away.
   \ingroup macros
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef ASSERTD
#ifdef DEBUG
#define ASSERTD ASSERT
#define ASSERTFNZ(x) ASSERT(x == 0)
#define ASSERTFNP(x) ASSERT(x != 0)
#define IFDEBUG(x) x
#define INLINE
#else
#define ASSERTD(x)
#define ASSERTFNZ(x) x
#define ASSERTFNP(x) x
#define IFDEBUG(x)
#define INLINE inline
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   \def ASSERTD(x)
   Do an assertion in \b DEBUG mode \b only
   \ingroup macros
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   \def ASSERTFNZ(x)
   Evaluate x, and in \b DEBUG mode, assert that x is zero.
   \ingroup macros
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   \def ASSERTFNP(x)
   Evaluate x, and in \b DEBUG mode, assert that x is non-zero.
   \ingroup macros
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   \def IFDEBUG(x)
   Do \b x in \b DEBUG mode \b only
   \ingroup macros
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   \def INLINE
   Expands to \c inline in \b RELEASE mode \b only
   \ingroup macros
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Instantiate a template class with the given parameter.
   \ingroup macros
*/
#define UTL_INSTANTIATE_TPL(className, T)                                                          \
    UTL_CLASS_IMPL_NAME_TPL(className, T)                                                          \
    template class className<T>;                                                                   \
    template class utl::TRunTimeClass<className<T>>;                                               \
    template class utl::TConcreteRunTimeClass<className<T>>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Instantiate a template class with two given parameters.
   \ingroup macros
*/
#define UTL_INSTANTIATE_TPL2(className, T1, T2)                                                    \
    UTL_CLASS_IMPL_NAME_TPL2(className, T1, T2)                                                    \
    template class className<T1, T2>;                                                              \
    template class utl::TRunTimeClass<className<T1, T2>>;                                          \
    template class utl::TConcreteRunTimeClass<className<T1, T2>>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Instantiate a template class with a single parameter, which itself is a template class with two
   parameters.
   \ingroup macros
*/
#define UTL_INSTANTIATE_TPLxTPL2(className, TC, T1, T2)                                            \
    UTL_CLASS_IMPL_NAME_TPLxTPL2(className, TC, T1, T2) template class className<TC<T1, T2>>;      \
    template class utl::TRunTimeClass<className<TC<T1, T2>>>;                                      \
    template class utl::TConcreteRunTimeClass<className<TC<T1, T2>>>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Execute the given system call function until its return value is non-negative or not EINTR.
   The return value of the function is stored in the variable \c err in the current scope.
   \ingroup macros
*/
#define UTL_EINTR_LOOP(SyscallFn)                                                                  \
    int err;                                                                                       \
    for (;;)                                                                                       \
    {                                                                                              \
        err = SyscallFn;                                                                           \
        if ((err >= 0) || (errno != EINTR))                                                        \
            break;                                                                                 \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A standard UTL++ implementation of \c main().
   \ingroup macros
*/
#define UTL_MAIN(appName)                                                                          \
    int main(int argc, char** argv)                                                                \
    {                                                                                              \
        appName* theApp = new appName;                                                             \
        int res = 255;                                                                             \
        try                                                                                        \
        {                                                                                          \
            res = theApp->run(argc, argv);                                                         \
        }                                                                                          \
        catch (utl::Exception & ex)                                                                \
        {                                                                                          \
            ex.dump(utl::cerr);                                                                    \
            ex.setObject(nullptr);                                                                 \
        }                                                                                          \
        delete theApp;                                                                             \
        return res;                                                                                \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   A standard UTL++ implementation of \c main() with a check for memory leaks in \b DEBUG mode.
   \ingroup macros
*/
#if defined(DEBUG)
#define UTL_MAIN_RL(appName)                                                                       \
    int main(int argc, char** argv)                                                                \
    {                                                                                              \
        appName* theApp = new appName;                                                             \
        int res = 255;                                                                             \
        try                                                                                        \
        {                                                                                          \
            res = theApp->run(argc, argv);                                                         \
        }                                                                                          \
        catch (utl::Exception & ex)                                                                \
        {                                                                                          \
            ex.dump(utl::cerr);                                                                    \
            ex.setObject(nullptr);                                                                 \
        }                                                                                          \
        delete theApp;                                                                             \
        utl::memReportLeaks();                                                                     \
        return res;                                                                                \
    }
#else
#define UTL_MAIN_RL(appName) UTL_MAIN(appName)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Iterate over the objects in a double-linked list.
   \ingroup macros
   \param head head node
   \param var_type contained type (e.g. MyClass)
   \param var_name name of current object (e.g. myObject)
*/
#define for_each_ln(head, var_type, var_name)                                                      \
    {                                                                                              \
        for (const ListNode* __ln = head; (__ln != nullptr) && !__ln->isSentinelTail();            \
             __ln = __ln->next())                                                                  \
        {                                                                                          \
            utl::Object* __lnPtr = __ln->get();                                                    \
            ASSERTD(__lnPtr->isA(var_type));                                                       \
            var_type& var_name = *(var_type*)__lnPtr;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Iterate over the objects in a single-linked list.
   \ingroup macros
   \param head head node
   \param var_type contained type (e.g. MyClass)
   \param var_name name of current object (e.g. myObject)
*/
#define for_each_sln(head, var_type, var_name)                                                     \
    {                                                                                              \
        for (const SlistNode* __ln = head; (__ln != nullptr) && !__ln->isSentinelTail();           \
             __ln = __ln->next())                                                                  \
        {                                                                                          \
            utl::Object* __lnPtr = __ln->get();                                                    \
            ASSERTD(__lnPtr->isA(var_type));                                                       \
            var_type& var_name = *(var_type*)__lnPtr;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Terminate a forEach block.
   \ingroup macros
*/
#define for_each_end                                                                               \
    }                                                                                              \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare that a class cannot do compare().
   \ingroup macros
*/
#define UTL_CLASS_NO_COMPARE                                                                       \
public:                                                                                            \
    virtual int compare(const utl::Object&) const                                                  \
    {                                                                                              \
        ABORT();                                                                                   \
        return 0;                                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare that a class cannot do copy().
   \ingroup macros
*/
#define UTL_CLASS_NO_COPY                                                                          \
public:                                                                                            \
    virtual void copy(const utl::Object&)                                                          \
    {                                                                                              \
        ABORT();                                                                                   \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare that a class has no key.
   \ingroup macros
*/
#define UTL_CLASS_NO_KEY(className)                                                                \
public:                                                                                            \
    virtual const utl::Object& getKey() const                                                      \
    {                                                                                              \
        return *this;                                                                              \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare that a class cannot do serialize().
   \ingroup macros
*/
#define UTL_CLASS_NO_SERIALIZE                                                                     \
public:                                                                                            \
    virtual void serialize(utl::Stream&, uint_t, uint_t)                                           \
    {                                                                                              \
        ABORT();                                                                                   \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare that a class cannot do vclone().
   \ingroup macros
*/
#define UTL_CLASS_NO_VCLONE                                                                        \
public:                                                                                            \
    virtual void vclone(const utl::Object&)                                                        \
    {                                                                                              \
        ABORT();                                                                                   \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Typedef for a class's own type, where the class has zero or one template parameters.
   \ingroup macros
*/
#define UTL_CLASS_TYPE(className)                                                                  \
private:                                                                                           \
    typedef className thisType;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Typedef for a class's own type, where the class has two template parameters.
   \ingroup macros
*/
#define UTL_CLASS_TYPE_TPL2(className, T1, T2)                                                     \
private:                                                                                           \
    typedef className<T1, T2> thisType;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   "super" typedef for a base class with zero or one template parameters.
   \ingroup macros
*/
#define UTL_CLASS_SUPER(baseClassName)                                                             \
public:                                                                                            \
    typedef baseClassName super;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   "super" typedef for a base class with two template parameters.
   \ingroup macros
*/
#define UTL_CLASS_SUPER_TPL2(baseClassName, T1, T2)                                                \
public:                                                                                            \
    typedef baseClassName<T1, T2> super;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   "super" typedef for a base class with one template parameter, which itself is a class with two
   template parameters.
   \ingroup macros
*/
#define UTL_CLASS_SUPER_TPL2_TPLxTPL2(baseClassName, T, T1, T2)                                    \
public:                                                                                            \
    typedef baseClassName<T<T1, T2>> super;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Default constructor - calls init().
   \ingroup macros
*/
#define UTL_CLASS_CONSTRUCT(className)                                                             \
public:                                                                                            \
    className()                                                                                    \
    {                                                                                              \
        init();                                                                                    \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Copy constructor - calls init(), then calls vclone(rhs).
   \ingroup macros
*/
#define UTL_CLASS_COPYCONSTRUCT(className)                                                         \
public:                                                                                            \
    className(const thisType& rhs)                                                                 \
    {                                                                                              \
        init();                                                                                    \
        this->vclone(rhs);                                                                         \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Copy constructor - calls init(), then calls steal(rhs).
   \ingroup macros
*/
#define UTL_CLASS_MOVECONSTRUCT(className)                                                         \
public:                                                                                            \
    className(thisType&& rhs) noexcept                                                             \
    {                                                                                              \
        init();                                                                                    \
        this->steal(rhs);                                                                          \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   In DEBUG build, make sure no exception tries to escape deInit().
   \ingroup general
*/
#if defined(DEBUG) && defined(UTL_SAFE_DEINIT_ENABLE)
#define UTL_SAFE_DEINIT                                                                            \
    SCOPE_FAIL                                                                                     \
    {                                                                                              \
        DIE("unexpected exception during object destruction");                                     \
    };                                                                                             \
    deInit();
#else
#define UTL_SAFE_DEINIT deInit();
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Virtual destructor - calls deInit().
   \ingroup macros
*/
#define UTL_CLASS_DESTRUCT(className)                                                              \
public:                                                                                            \
    virtual ~className()                                                                           \
    {                                                                                              \
        UTL_SAFE_DEINIT;                                                                           \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Default init() and deInit() (which are merely place-holders).
   \ingroup macros
*/
#define UTL_CLASS_DEFID                                                                            \
private:                                                                                           \
    void init()                                                                                    \
    {                                                                                              \
    }                                                                                              \
    void deInit()                                                                                  \
    {                                                                                              \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new instance of self's type (member function declaration).
   \ingroup macros
*/
#define UTL_CLASS_DECL_CREATE                                                                      \
public:                                                                                            \
    virtual thisType* create() const;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new copy of self (member function declaration).
   \ingroup macros
*/
#define UTL_CLASS_DECL_CLONE                                                                       \
public:                                                                                            \
    virtual thisType* clone() const;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   operator=(rhs) simply calls Object::copy(rhs).
   \ingroup macros
*/
#define UTL_CLASS_EQUALS(rhsClassName)                                                             \
public:                                                                                            \
    thisType& operator=(const rhsClassName& rhs)                                                   \
    {                                                                                              \
        this->copy(rhs);                                                                           \
        return *this;                                                                              \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   operator=(rhs) for the case where rhs is a temporary.
   \ingroup macros
*/
#define UTL_CLASS_EQUALS_MOVE                                                                      \
public:                                                                                            \
    thisType& operator=(thisType&& rhs) noexcept                                                   \
    {                                                                                              \
        this->steal(rhs);                                                                          \
        return *this;                                                                              \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Conversion to pointer.
   \ingroup macros
*/
#define UTL_CLASS_POINTER                                                                          \
public:                                                                                            \
    operator thisType*()                                                                           \
    {                                                                                              \
        return this;                                                                               \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Conversion to const pointer.
   \ingroup macros
*/
#define UTL_CLASS_CONSTPOINTER                                                                     \
public:                                                                                            \
    operator const thisType*() const                                                               \
    {                                                                                              \
        return this;                                                                               \
    }                                                                                              \
    operator const thisType*()                                                                     \
    {                                                                                              \
        return this;                                                                               \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implement serialize() for a class.
   \ingroup macros
*/
#define UTL_CLASS_SERIALIZE(className)                                                             \
    inline void serialize(className& object, Stream& stream, uint_t io,                            \
                          uint_t mode = utl::ser_default)                                          \
    {                                                                                              \
        object.serialize(stream, io, mode);                                                        \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declare that a type cannot be serialized.
   \ingroup macros
*/
#define UTL_TYPE_NO_SERIALIZE(typeName)                                                            \
    inline void serialize(typeName&, Stream&, uint_t, uint_t)                                      \
    {                                                                                              \
        ABORT();                                                                                   \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for an abstract base class (ABC).
   \ingroup macros
*/
#define UTL_CLASS_DECL_ABC(DC, BC)                                                                 \
    UTL_CLASS_TYPE(DC)                                                                             \
    UTL_CLASS_SUPER(BC)                                                                            \
    UTL_CLASS_DECL_RTTI_ABC(DC)                                                                    \
    UTL_CLASS_CONSTRUCT(DC)                                                                        \
    UTL_CLASS_DESTRUCT(DC)                                                                         \
    UTL_CLASS_DECL_CREATE                                                                          \
    UTL_CLASS_DECL_CLONE                                                                           \
    UTL_CLASS_EQUALS(DC)                                                                           \
    UTL_CLASS_EQUALS_MOVE                                                                          \
    UTL_CLASS_POINTER                                                                              \
    UTL_CLASS_CONSTPOINTER

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ class functionality.
   \ingroup macros
*/
#define UTL_CLASS_DECL_CMN(DC)                                                                     \
    UTL_CLASS_CONSTRUCT(DC)                                                                        \
    UTL_CLASS_COPYCONSTRUCT(DC)                                                                    \
    UTL_CLASS_MOVECONSTRUCT(DC)                                                                    \
    UTL_CLASS_DESTRUCT(DC)                                                                         \
    UTL_CLASS_DECL_CREATE                                                                          \
    UTL_CLASS_DECL_CLONE                                                                           \
    UTL_CLASS_EQUALS(utl::Object)                                                                  \
    UTL_CLASS_EQUALS(thisType)                                                                     \
    UTL_CLASS_EQUALS_MOVE                                                                          \
    UTL_CLASS_POINTER                                                                              \
    UTL_CLASS_CONSTPOINTER

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for a non-template class.
   \ingroup macros
*/
#define UTL_CLASS_DECL(DC, BC)                                                                     \
    UTL_CLASS_TYPE(DC)                                                                             \
    UTL_CLASS_SUPER(BC)                                                                            \
    UTL_CLASS_DECL_RTTI(DC)                                                                        \
    UTL_CLASS_DECL_CMN(DC)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for a non-template class that inherits from a
   template class with two parameters.
   \ingroup macros
*/
#define UTL_CLASS_DECL_NT_TPL2(DC, BC, T1, T2)                                                     \
    UTL_CLASS_TYPE(DC)                                                                             \
    UTL_CLASS_SUPER_TPL2(BC, T1, T2)                                                               \
    UTL_CLASS_DECL_RTTI(DC)                                                                        \
    UTL_CLASS_DECL_CMN(DC)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for a template class with one parameter.
   \ingroup macros
*/
#define UTL_CLASS_DECL_TPL(DC, T, BC)                                                              \
    UTL_CLASS_TYPE(DC<T>)                                                                          \
    UTL_CLASS_SUPER(BC)                                                                            \
    UTL_CLASS_DECL_RTTI_TPL(DC, T)                                                                 \
    UTL_CLASS_DECL_CMN(DC)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for a template class with two parameters,
   where the base class has zero or one template parameters.
   \ingroup macros
*/
#define UTL_CLASS_DECL_TPL2(DC, T1, T2, BC)                                                        \
    UTL_CLASS_TYPE_TPL2(DC, T1, T2)                                                                \
    UTL_CLASS_SUPER(BC)                                                                            \
    UTL_CLASS_DECL_RTTI_TPL2(DC, T1, T2)                                                           \
    UTL_CLASS_DECL_CMN(DC)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for a template class with two parameters,
   where the base class also has two template parameters.
   \ingroup macros
*/
#define UTL_CLASS_DECL_TPL2_TPL2(DC, DC_T1, DC_T2, BC, BC_T1, BC_T2)                               \
    UTL_CLASS_TYPE_TPL2(DC, DC_T1, DC_T2)                                                          \
    UTL_CLASS_SUPER_TPL2(BC, BC_T1, BC_T2)                                                         \
    UTL_CLASS_DECL_RTTI_TPL2(DC, DC_T1, DC_T2)                                                     \
    UTL_CLASS_DECL_CMN(DC)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declaration of standard UTL++ functionality for a template class with two parameters,
   where the base class has one template parameter, which itself is a class with two
   template parameters.
   \ingroup macros
*/
#define UTL_CLASS_DECL_TPL2_TPLxTPL2(DC, DC_T1, DC_T2, BC, BC_BT, BC_T1, BC_T2)                    \
private:                                                                                           \
    UTL_CLASS_TYPE_TPL2(DC, DC_T1, DC_T2)                                                          \
    UTL_CLASS_SUPER_TPL2_TPLxTPL2(BC, BC_BT, BC_T1, BC_T2);                                        \
    UTL_CLASS_DECL_RTTI_TPL2(DC, DC_T1, DC_T2)                                                     \
    UTL_CLASS_DECL_CMN(DC)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new instance of self's type (implementation for abstract class).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CREATE_ABC(className)                                                       \
    className* className::create() const                                                           \
    {                                                                                              \
        ABORT();                                                                                   \
        return nullptr;                                                                            \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new instance of self's type (implementation for concrete class).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CREATE(className)                                                           \
    className* className::create() const                                                           \
    {                                                                                              \
        return new className();                                                                    \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new instance of self's type (implementation for template class with one parameter).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CREATE_TPL(className, T)                                                    \
    template <class T> className<T>* className<T>::create() const                                  \
    {                                                                                              \
        return new className<T>();                                                                 \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new instance of self's type (implementation for template class with two parameters).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CREATE_TPL2(className, T1, T2)                                              \
    template <class T1, class T2> className<T1, T2>* className<T1, T2>::create() const             \
    {                                                                                              \
        return new className<T1, T2>();                                                            \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new copy of self (implementation for abstract class).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CLONE_ABC(className)                                                        \
    className* className::clone() const                                                            \
    {                                                                                              \
        ABORT();                                                                                   \
        return nullptr;                                                                            \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new copy of self (implementation for concrete class).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CLONE(className)                                                            \
    className* className::clone() const                                                            \
    {                                                                                              \
        auto object = new className();                                                             \
        object->vclone(*this);                                                                     \
        return object;                                                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new copy of self (implementation for template class with one parameter).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CLONE_TPL(className, T)                                                     \
    template <class T> className<T>* className<T>::clone() const                                   \
    {                                                                                              \
        auto object = new className<T>();                                                          \
        object->vclone(*this);                                                                     \
        return object;                                                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Create a new copy of self (implementation for template class with two parameters).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_CLONE_TPL2(className, T1, T2)                                               \
    template <class T1, class T2> className<T1, T2>* className<T1, T2>::clone() const              \
    {                                                                                              \
        auto object = new className<T1, T2>();                                                     \
        object->vclone(*this);                                                                     \
        return object;                                                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implementation of standard UTL++ functionality for an abstract base class (ABC).
   \ingroup macros
*/
#define UTL_CLASS_IMPL_ABC(className)                                                              \
    UTL_CLASS_IMPL_NAME(className)                                                                 \
    UTL_CLASS_IMPL_RTTI_ABC(className)                                                             \
    UTL_CLASS_IMPL_CREATE_ABC(className)                                                           \
    UTL_CLASS_IMPL_CLONE_ABC(className)                                                            \
    template const className& utl::cast<className>(const utl::Object&);                            \
    template className& utl::cast<className>(utl::Object&);                                        \
    template const className* utl::cast<className>(const utl::Object*);                            \
    template className* utl::cast<className>(utl::Object*);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implementation of standard UTL++ class functionality.
   \ingroup macros
*/
#define UTL_CLASS_IMPL(className)                                                                  \
    UTL_CLASS_IMPL_NAME(className)                                                                 \
    UTL_CLASS_IMPL_RTTI(className)                                                                 \
    UTL_CLASS_IMPL_CREATE(className)                                                               \
    UTL_CLASS_IMPL_CLONE(className)                                                                \
    template const className& utl::cast<className>(const utl::Object&);                            \
    template className& utl::cast<className>(utl::Object&);                                        \
    template const className* utl::cast<className>(const utl::Object*);                            \
    template className* utl::cast<className>(utl::Object*);

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implementation of standard UTL++ functionality for a template class.
   \ingroup macros
*/
#define UTL_CLASS_IMPL_TPL(className, T)                                                           \
    UTL_CLASS_IMPL_RTTI_TPL(className, T)                                                          \
    UTL_CLASS_IMPL_CREATE_TPL(className, T)                                                        \
    UTL_CLASS_IMPL_CLONE_TPL(className, T)

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implementation of standard UTL++ functionality for a template class with two parameters.
   \ingroup macros
*/
#define UTL_CLASS_IMPL_TPL2(className, T1, T2)                                                     \
    UTL_CLASS_IMPL_RTTI_TPL2(className, T1, T2)                                                    \
    UTL_CLASS_IMPL_CREATE_TPL2(className, T1, T2)                                                  \
    UTL_CLASS_IMPL_CLONE_TPL2(className, T1, T2)
