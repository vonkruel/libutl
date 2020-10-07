#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////
// RunTimeClass ////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Store information about a class.

   RunTimeClass is the basis of a very limited reflection capability in UTL++.
   For each utl::Object-derived class, there is an instance of a RunTimeClass-derived type.

   \author Adam McKee
   \ingroup reflection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class RunTimeClass
{
public:
    /**
       Constructor.
       \param baseClass base class of this class
    */
    RunTimeClass(const RunTimeClass* baseClass)
        : _baseClass(baseClass)
        , _bases(nullptr)
    {
    }

    /**
       Copy constructor.
       \param runTimeClass RunTimeClass to copy
    */
    RunTimeClass(const RunTimeClass& runTimeClass)
        : _baseClass(runTimeClass._baseClass)
        , _bases(nullptr)
    {
    }

    virtual ~RunTimeClass()
    {
        auto bases = _bases.load(std::memory_order_relaxed);
        if ((bases != nullptr) && (bases != _basesInline))
        {
            free(bases);
        }
    }

    /** Print class names. */
    static void printClassNames();

    /**
       Add a class to the repository of known classes.  Only a concrete class may be added or
       found.  You should not need to call this method.
       \see RunTimeClass::find
       \param rtc RunTimeClass to add
    */
    static void add(RunTimeClass* rtc);

    /**
       Create a new instance of this class.
       \return newly created instance of this class
    */
    virtual Object*
    create() const
    {
        ABORT();
        return nullptr;
    }

    /**
       Find the RunTimeClass by the given name.
       \return found RunTimeClass (nullptr if none)
       \param className name of class to find
    */
    static const RunTimeClass* find(const char* className);

    /**
       Determine whether thisClass.isA(givenClass).
       \return true if this isA rtc, false otherwise
       \param rtc RunTimeClass to compare against
    */
    bool _isA(const RunTimeClass* rtc) const;

    /** Get the base class. */
    const RunTimeClass*
    baseClass() const
    {
        return _baseClass;
    }

    /** Get the name. */
    const char*
    name() const
    {
        return vname();
    }

    /** Get the name. */
    virtual const char*
    vname() const
    {
        ABORT();
        return nullptr;
    }

    /** Get the size of this class (as in sizeof()). */
    virtual size_t
    size() const
    {
        ABORT();
        return 0;
    }

    /** De-allocate memory allocated by instances. */
    static void utl_deInit();

private:
    const RunTimeClass* _baseClass;
    mutable std::atomic<const RunTimeClass**> _bases;
    mutable const RunTimeClass* _basesInline[8];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// TRunTimeClass ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Template version of RunTimeClass.

   TRunTimeClass inherits from RunTimeClass, and implements methods that apply to both abstract
   and concrete classes.

   \author Adam McKee
   \ingroup reflection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class TRunTimeClass : public RunTimeClass
{
public:
    /**
       Constructor.
       \param baseClass base class of this class
    */
    TRunTimeClass(const RunTimeClass* baseClass)
        : RunTimeClass(baseClass)
    {
    }
    virtual ~TRunTimeClass()
    {
    }

    static const char*
    name()
    {
        return _name;
    }

    virtual const char*
    vname() const
    {
        return _name;
    }

    virtual size_t
    size() const
    {
        return sizeof(T);
    }

private:
    static const char* _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// TConcreteRunTimeClass ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Encapsulate class-related knowledge for concrete classes.

   TConcreteRunTimeClass inherits from TRunTimeClass, and implements certain virtual methods
   that only apply to concrete (non-abstract) classes.

   \author Adam McKee
   \ingroup reflection
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class TConcreteRunTimeClass : public TRunTimeClass<T>
{
public:
    /**
       Constructor.
       \param baseClass base class of this class
    */
    TConcreteRunTimeClass(const RunTimeClass* baseClass)
        : TRunTimeClass<T>(baseClass)
    {
        this->add(this);
    }

    virtual ~TConcreteRunTimeClass()
    {
    }

    virtual Object*
    create() const
    {
        return new T();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the class is derived from the given class.
   \see Object::_isA()
   \see RunTimeClass::_isA()
   \ingroup reflection
*/
#define isA(className) _isA(CLASS(className))

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Determine whether the class is derived from a template class instantiated with the two
   given parameters.
   \see Object::_isA()
   \see RunTimeClass::_isA()
   \ingroup reflection
*/
#define isA2(className, T1, T2) _isA(CLASS2(className, T1, T2))

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Get the RunTimeClass object for the given class.
   \ingroup reflection
*/
#define CLASS(className) (className::getThisClass())

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Get the RunTimeClass object for a template class instantiated with the two given parameters.
   \ingroup reflection
*/
#define CLASS2(className, T1, T2) (className<T1, T2>::getThisClass())

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declares the methods that are associated with run-time type identification (RTTI).
   \ingroup reflection
*/
#define UTL_CLASS_DECL_RTTI_ABC(className)                                                         \
public:                                                                                            \
    virtual const utl::RunTimeClass* getClass() const;                                             \
    virtual const char* getClassName() const;                                                      \
    static const utl::RunTimeClass* getThisClass();                                                \
    static const char* getThisClassName();                                                         \
    static const utl::RunTimeClass* getBaseClass();                                                \
    static const char* getBaseClassName();                                                         \
                                                                                                   \
private:                                                                                           \
    static const utl::TRunTimeClass<className> ___rtc UTL_ATTRIBUTE_USED;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declares the methods that are associated with run-time type identification (RTTI).
   \ingroup reflection
*/
#define UTL_CLASS_DECL_RTTI(className)                                                             \
public:                                                                                            \
    virtual const utl::RunTimeClass* getClass() const;                                             \
    virtual const char* getClassName() const;                                                      \
    static const utl::RunTimeClass* getThisClass();                                                \
    static const char* getThisClassName();                                                         \
    static const utl::RunTimeClass* getBaseClass();                                                \
    static const char* getBaseClassName();                                                         \
                                                                                                   \
private:                                                                                           \
    static const utl::TConcreteRunTimeClass<className> ___rtc UTL_ATTRIBUTE_USED;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declares the methods that are associated with run-time type identification (RTTI) for
   a template class with one parameter.
   \ingroup reflection
*/
#define UTL_CLASS_DECL_RTTI_TPL(className, T)                                                      \
public:                                                                                            \
    virtual const utl::RunTimeClass* getClass() const;                                             \
    virtual const char* getClassName() const;                                                      \
    static const utl::RunTimeClass* getThisClass();                                                \
    static const char* getThisClassName();                                                         \
    static const utl::RunTimeClass* getBaseClass();                                                \
    static const char* getBaseClassName();                                                         \
                                                                                                   \
private:                                                                                           \
    static const utl::TConcreteRunTimeClass<className<T>> ___rtc UTL_ATTRIBUTE_USED;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Declares the methods that are associated with run-time type identification (RTTI) for
   a template class with two parameters.
   \ingroup reflection
*/
#define UTL_CLASS_DECL_RTTI_TPL2(className, T1, T2)                                                \
public:                                                                                            \
    virtual const utl::RunTimeClass* getClass() const;                                             \
    virtual const char* getClassName() const;                                                      \
    static const utl::RunTimeClass* getThisClass();                                                \
    static const char* getThisClassName();                                                         \
    static const utl::RunTimeClass* getBaseClass();                                                \
    static const char* getBaseClassName();                                                         \
                                                                                                   \
private:                                                                                           \
    static const utl::TConcreteRunTimeClass<className<T1, T2>> ___rtc UTL_ATTRIBUTE_USED;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define UTL_CLASS_IMPL_RTTI_CMN(className)                                                         \
    const utl::RunTimeClass* className::getClass() const                                           \
    {                                                                                              \
        return &___rtc;                                                                            \
    }                                                                                              \
    const char* className::getClassName() const                                                    \
    {                                                                                              \
        return ___rtc.name();                                                                      \
    }                                                                                              \
    const utl::RunTimeClass* className::getThisClass()                                             \
    {                                                                                              \
        return &___rtc;                                                                            \
    }                                                                                              \
    const char* className::getThisClassName()                                                      \
    {                                                                                              \
        return ___rtc.name();                                                                      \
    }                                                                                              \
    const utl::RunTimeClass* className::getBaseClass()                                             \
    {                                                                                              \
        return ___rtc.baseClass();                                                                 \
    }                                                                                              \
    const char* className::getBaseClassName()                                                      \
    {                                                                                              \
        return getBaseClass()->name();                                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implements the methods that are associated with run-time type identification (RTTI).
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_RTTI_ABC(className)                                                         \
    UTL_CLASS_IMPL_RTTI_CMN(className);                                                            \
    const utl::TRunTimeClass<className> className::___rtc(CLASS(className::super));                \
    template class utl::TRunTimeClass<className>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implements the methods that are associated with run-time type identification (RTTI).
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_RTTI(className)                                                             \
    UTL_CLASS_IMPL_RTTI_CMN(className);                                                            \
    const utl::TConcreteRunTimeClass<className> className::___rtc(CLASS(className::super));        \
    template class utl::TRunTimeClass<className>;                                                  \
    template class utl::TConcreteRunTimeClass<className>;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implements the methods that are associated with run-time type identification (RTTI) for
   a template class.
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_RTTI_TPL(className, T)                                                      \
    template <typename T>                                                                          \
    const utl::RunTimeClass* className<T>::getClass() const                                        \
    {                                                                                              \
        return &___rtc;                                                                            \
    }                                                                                              \
    template <typename T>                                                                          \
    const char* className<T>::getClassName() const                                                 \
    {                                                                                              \
        return ___rtc.name();                                                                      \
    }                                                                                              \
    template <typename T>                                                                          \
    const utl::RunTimeClass* className<T>::getThisClass()                                          \
    {                                                                                              \
        return &___rtc;                                                                            \
    }                                                                                              \
    template <typename T>                                                                          \
    const char* className<T>::getThisClassName()                                                   \
    {                                                                                              \
        return ___rtc.name();                                                                      \
    }                                                                                              \
    template <typename T>                                                                          \
    const utl::RunTimeClass* className<T>::getBaseClass()                                          \
    {                                                                                              \
        return ___rtc.baseClass();                                                                 \
    }                                                                                              \
    template <typename T>                                                                          \
    const char* className<T>::getBaseClassName()                                                   \
    {                                                                                              \
        return getBaseClass()->name();                                                             \
    }                                                                                              \
    template <typename T>                                                                          \
    const utl::TConcreteRunTimeClass<className<T>> className<T>::___rtc(                           \
        CLASS(className<T>::super));

////////////////////////////////////////////////////////////////////////////////////////////////////

#define UTL_CLASS_IMPL_RTTI_TPL2_CMN(className, T1, T2)                                            \
    template <typename T1, typename T2>                                                            \
    const utl::RunTimeClass* className<T1, T2>::getClass() const                                   \
    {                                                                                              \
        return &___rtc;                                                                            \
    }                                                                                              \
    template <typename T1, typename T2>                                                            \
    const char* className<T1, T2>::getClassName() const                                            \
    {                                                                                              \
        return ___rtc.name();                                                                      \
    }                                                                                              \
    template <typename T1, typename T2>                                                            \
    const utl::RunTimeClass* className<T1, T2>::getThisClass()                                     \
    {                                                                                              \
        return &___rtc;                                                                            \
    }                                                                                              \
    template <typename T1, typename T2>                                                            \
    const char* className<T1, T2>::getThisClassName()                                              \
    {                                                                                              \
        return ___rtc.name();                                                                      \
    }                                                                                              \
    template <typename T1, typename T2>                                                            \
    const utl::RunTimeClass* className<T1, T2>::getBaseClass()                                     \
    {                                                                                              \
        return ___rtc.baseClass();                                                                 \
    }                                                                                              \
    template <typename T1, typename T2>                                                            \
    const char* className<T1, T2>::getBaseClassName()                                              \
    {                                                                                              \
        return getBaseClass()->name();                                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Implements the methods that are associated with run-time type identification (RTTI) for
   a template class.
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_RTTI_TPL2(className, T1, T2)                                                \
    UTL_CLASS_IMPL_RTTI_TPL2_CMN(className, T1, T2);                                               \
    template <typename T1, typename T2>                                                            \
    const utl::TConcreteRunTimeClass<className<T1, T2>> className<T1, T2>::___rtc(                 \
        className<T1, T2>::super::getThisClass());

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Define the name of a class.
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_NAME(className)                                                             \
    template <>                                                                                    \
    const char* utl::TRunTimeClass<className>::_name = #className;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Define the name of a template class with one parameter.
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_NAME_TPL(className, T)                                                      \
    template <>                                                                                    \
    const char* utl::TRunTimeClass<className<T>>::_name = #className "<" #T ">";

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Define the name of a template class with two parameters.
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_NAME_TPL2(className, T1, T2)                                                \
    template <>                                                                                    \
    const char* utl::TRunTimeClass<className<T1, T2>>::_name = #className "<" #T1 "," #T2 ">";

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Define the name of a template class with one parameter (which itself is a template class with
   two parameters).
   \ingroup reflection
*/
#define UTL_CLASS_IMPL_NAME_TPLxTPL2(className, TC, T1, T2)                                        \
    template <>                                                                                    \
    const char* utl::TRunTimeClass<className<TC<T1, T2>>>::_name =                                 \
        #className "<" #TC "<" #T1 "," #T2 ">"                                                     \
                   ">";

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
