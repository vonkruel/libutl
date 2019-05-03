#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <libutl/HostOS.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Windows host OS portability helper.

   WindowsHostOS specializes HostOS to provide implementations of its virtual methods that work
   on Windows.

   \author Adam McKee
   \ingroup hostos
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowsHostOS : public HostOS
{
    UTL_CLASS_DECL(WindowsHostOS, HostOS);
    UTL_CLASS_DEFID;

public:
    virtual void chdir(const Pathname& path);
    virtual void daemonInit();

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
