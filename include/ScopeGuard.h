#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
// This code is based on an excellent talk by Andrei Alexandrescu at CppCon 2015.                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Run some code when exiting the current scope (no matter what).

   \code
   SCOPE_EXIT
   {
       // do things
   };
   \endcode
   \ingroup macros
*/
#define SCOPE_EXIT                                                                                 \
    auto UTL_ANONYMOUS_VARIABLE(scopeExit) = utl::ScopeExitMacroHelper() + [&]() noexcept

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Run some code when exiting the current scope due to an exception being thrown.

   \code
   SCOPE_FAIL
   {
       // do things
   };
   \endcode
   \ingroup macros
*/
#define SCOPE_FAIL                                                                                 \
    auto UTL_ANONYMOUS_VARIABLE(scopeFail) = utl::ScopeFailMacroHelper() + [&]() noexcept

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Run some code when exiting the current scope normally (without any exception being thrown).

   \code
   SCOPE_SUCCESS
   {
       // do things
   };
   \endcode
   \ingroup macros
*/
#define SCOPE_SUCCESS                                                                              \
    auto UTL_ANONYMOUS_VARIABLE(scopeSuccess) = utl::ScopeSuccessMacroHelper() + [&]() noexcept

////////////////////////////////////////////////////////////////////////////////////////////////////

class ScopeGuardImplBase
{
public:
    void
    dismiss() const
    {
        _dismissed = true;
    }

protected:
    ScopeGuardImplBase()
        : _dismissed(false)
    {
    }

    ~ScopeGuardImplBase()
    {
    }

protected:
    mutable bool _dismissed;

private:
    // Disable assignment
    ScopeGuardImplBase& operator=(const ScopeGuardImplBase&);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Callable>
class ScopeGuardExit : public ScopeGuardImplBase
{
public:
    explicit ScopeGuardExit(const Callable& callable)
        : _callable(callable)
    {
    }

    explicit ScopeGuardExit(Callable&& callable)
        : _callable(std::move(callable))
    {
    }

    ~ScopeGuardExit() noexcept
    {
        if (_dismissed)
            return;
        _callable();
    }

private:
    Callable _callable;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: use std::uncaught_exceptions when gcc supports it
class UncaughtExceptionCounter
{
public:
    UncaughtExceptionCounter()
        : _exceptionCount(std::uncaught_exceptions())
    {
    }

    bool
    isNewUncaughtException() noexcept
    {
        return std::uncaught_exceptions() > _exceptionCount;
    }

private:
    int getUncaughtExceptionCount() noexcept;

private:
    int _exceptionCount;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Callable, bool executeOnException>
class ScopeGuardForNewException : public ScopeGuardImplBase
{
public:
    explicit ScopeGuardForNewException(const Callable& callable)
        : _callable(callable)
    {
    }

    explicit ScopeGuardForNewException(Callable&& callable)
        : _callable(std::move(callable))
    {
    }

    ~ScopeGuardForNewException() noexcept
    {
        if (executeOnException == _ec.isNewUncaughtException())
        {
            if (_dismissed)
                return;
            _callable();
        }
    }

private:
    Callable _callable;
    UncaughtExceptionCounter _ec;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScopeExitMacroHelper
{
};

enum class ScopeFailMacroHelper
{
};

enum class ScopeSuccessMacroHelper
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Callable>
ScopeGuardExit<Callable>
makeExitGuard(Callable&& callable)
{
    return ScopeGuardExit<Callable>(std::forward(callable));
}

template <typename Callable>
ScopeGuardForNewException<Callable, true>
makeFailureGuard(Callable&& callable)
{
    return ScopeGuardForNewException<Callable, true>(std::forward(callable));
}

template <typename Callable>
ScopeGuardForNewException<Callable, false>
makeSuccessGuard(Callable&& callable)
{
    return ScopeGuardForNewException<Callable, false>(std::forward(callable));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Callable>
utl::ScopeGuardExit<Callable>
operator+(utl::ScopeExitMacroHelper, Callable&& callable)
{
    return utl::ScopeGuardExit<Callable>(std::forward<Callable>(callable));
}

template <typename Callable>
utl::ScopeGuardForNewException<Callable, true>
operator+(utl::ScopeFailMacroHelper, Callable&& callable)
{
    return utl::ScopeGuardForNewException<typename std::decay<Callable>::type, true>(
        std::forward<Callable>(callable));
}

template <typename Callable>
utl::ScopeGuardForNewException<Callable, false>
operator+(utl::ScopeSuccessMacroHelper, Callable&& callable)
{
    return utl::ScopeGuardForNewException<typename std::decay<Callable>::type, false>(
        std::forward<Callable>(callable));
}
