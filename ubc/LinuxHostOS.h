#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/HostOS.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Linux host OS portability helper.

   LinuxHostOS specializes HostOS to provide implementations of its virtual methods that work
   on Linux.

   \author Adam McKee
   \ingroup hostos
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class LinuxHostOS : public HostOS
{
    UTL_CLASS_DECL(LinuxHostOS, HostOS);
    UTL_CLASS_DEFID;

public:
    virtual void chdir(const Pathname& path);
    virtual void daemonInit();

    /**
       Enable/disable canonical mode for the given TTY
       \param fd file descriptor for TTY
       \param canon canonical mode enabled?
    */
    void setCanonicalTTY(int fd, bool canon);

    /// \name Process Management
    //@{
    virtual int run(const Pathname& path, const TArray<String>& args);

    int
    run(const String& cmd)
    {
        return super::run(cmd);
    }
    //@}

    /// \name Sleeping
    //@{
    virtual void sleep(uint_t sec) const;
    virtual void usleep(uint64_t usec) const;
    virtual void yield() const;
    //@}

    /// \name Environment Variables
    //@{
    virtual String getEnv(const String& envVarName) const;
    virtual String setEnv(const String& envVarName, const String& envVarVal);
    virtual String remEnv(const String& envVarName);
    //@}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
