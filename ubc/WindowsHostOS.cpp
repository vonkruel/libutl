#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <direct.h>
#include <sys/stat.h>
#include <libutl/win32api.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Exception.h>
#include <libutl/WindowsHostOS.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::WindowsHostOS);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
WindowsHostOS::chdir(const Pathname& path)
{
    int err = ::_chdir(path);
    if (err < 0)
        errToEx(path);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
WindowsHostOS::daemonInit()
{
    // how to become a daemon process on Windows?
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
WindowsHostOS::run(const Pathname& path, const TArray<String>& args)
{
    // make command line
    String commandLine = '"';
    commandLine += path;
    commandLine += '"';
    for (size_t i = 0; i < args.size(); ++i)
    {
        commandLine += " \"";
        commandLine += args(i);
        commandLine += "\"";
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);

    BOOL res = CreateProcess(nullptr, const_cast<char*>(commandLine.get()), nullptr, nullptr, FALSE,
                             0, nullptr, nullptr, &si, &pi);
    if (res == FALSE)
    {
        return 256;
    }

    // wait for process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);

    // get exit code
    DWORD exitCode = 255;
    ASSERTFNP(GetExitCodeProcess(pi.hProcess, &exitCode));

    // clean up
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exitCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
WindowsHostOS::sleep(uint_t sec) const
{
    Sleep(sec * 1000); // milliseconds
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
WindowsHostOS::usleep(uint64_t usec) const
{
    uint64_t msec = usec / (uint64_t)1000;
    if ((msec == 0) && (usec > 0))
        msec = 1;
    Sleep(msec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
WindowsHostOS::getEnv(const String& envVarName) const
{
    char* buf = new char[256];
    buf[0] = '\0';
    DWORD res = GetEnvironmentVariable(const_cast<char*>(envVarName.get()), buf, 256);
    ASSERT(res < 256);
    return String(buf, true, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
WindowsHostOS::setEnv(const String& envVarName, const String& envVarVal)
{
    String oldEnvVarVal = getEnv(envVarName);
    SetEnvironmentVariable(const_cast<char*>(envVarName.get()), const_cast<char*>(envVarVal.get()));
    return oldEnvVarVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
WindowsHostOS::remEnv(const String& envVarName)
{
    String oldEnvVarVal = getEnv(envVarName);
    SetEnvironmentVariable(const_cast<char*>(envVarName.get()), nullptr);
    return oldEnvVarVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
