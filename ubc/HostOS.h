#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/Array.h>
#include <libutl/Pathname.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Abstraction for common OS functionality.

   \author Adam McKee
   \ingroup hostos
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class HostOS : public Object
{
    UTL_CLASS_DECL_ABC(HostOS, Object);
    UTL_CLASS_DEFID;

public:
    /**
       Set the current working directory.
       \param path new current working directory
    */
    virtual void chdir(const Pathname& path) = 0;

    /** Become a system/daemon process. */
    virtual void daemonInit() = 0;

    /// \name Process Management
    //@{
    /**
       Run an executable with specified parameters.
       \return exit code of given executable
       \param path pathname of executable to run
       \param args parameters to run executable with
    */
    virtual int run(const Pathname& path, const TArray<String>& args) = 0;

    /**
       A front-end for the virtual run().  From the given string, it determines the executable
       path and the parameters, and invokes run(path,args).  If you want to pass a parameter that
       contains whitespace, you must enclose it in double quotes, just as you would do with the
       shell.
       \return exit code of command
       \param cmd command to execute
    */
    int run(const String& cmd);
    //@}

    /// \name Sleeping
    //@{
    /** Sleep for the given number of milliseconds. */
    void
    msleep(uint64_t msec) const
    {
        usleep(msec * (uint64_t)1000);
    }

    /** Sleep for the given number of seconds. */
    virtual void sleep(uint_t sec) const = 0;

    /** Sleep for the given number of seconds and microseconds. */
    void
    sleep(uint_t sec, uint_t usec) const
    {
        sleep(sec);
        usleep(usec);
    }

    /** Sleep for the given number of microseconds. */
    virtual void usleep(uint64_t usec) const = 0;

    /** Yield the processor to some other process that wants to run. */
    virtual void yield() const;
    //@}

    /// \name Environment
    //@{
    /**
       Get an environment variable.
       \return value of environment variable (empty string if undefined)
    */
    virtual String getEnv(const String& envVarName) const = 0;

    /**
       Set an environment variable.
       \return old value of environment variable
       \param envVarName environment variable name
       \param envVarVal new value of environment variable
    */
    virtual String setEnv(const String& envVarName, const String& envVarVal) = 0;

    /**
       Remove an environment variable.
       \return old value of environment variable
       \param envVarName environment variable name
    */
    virtual String remEnv(const String& envVarName) = 0;
    //@}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Global pointer to HostOS-derived object.
   \ingroup hostos
*/
#if UTL_HOST_TYPE == UTL_HT_UNIX
class LinuxHostOS;
extern LinuxHostOS* hostOS;
#elif UTL_HOST_TYPE == UTL_HT_WINDOWS
class WindowsHostOS;
extern WindowsHostOS* hostOS;
#else
extern HostOS* hostOS;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <libutl/LinuxHostOS.h>
#else
#include <libutl/WindowsHostOS.h>
#endif
