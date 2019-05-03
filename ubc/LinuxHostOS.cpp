#include <libutl/libutl.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_LINUX

////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <termios.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/Exception.h>
#include <libutl/LinuxHostOS.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_CLASS_IMPL(utl::LinuxHostOS);

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LinuxHostOS::chdir(const Pathname& path)
{
    int err = ::chdir(path);
    if (err < 0)
        errToEx(path);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LinuxHostOS::daemonInit()
{
    // NOTE: controlling TTY is associated with a session

    // fork to ensure we're not the session/group leader
    // (otherwise setsid(2) fails)
    pid_t pid = fork();
    ASSERT(pid >= 0);
    if (pid > 0)
    {
        exit(0);
    }

    // become session/group leader
    pid = setsid();
    ASSERT(pid >= 0);

    // Fork again : parent (which is session/group leader) exits.
    // Since the child is not session/group leader, it can never regain a controlling TTY.
    pid = fork();
    ASSERT(pid >= 0);
    if (pid > 0)
    {
        exit(0);
    }

    // close stdin, stdout, stderr
    cin.close();
    cout.close();
    cerr.close();

    // change to root directory
    chdir("/");

    // so we have complete control over permissions of files we create
    umask(0);

    // ignore SIGHUP by default
    signal(SIGHUP, SIG_IGN);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
LinuxHostOS::run(const Pathname& path, const TArray<String>& args)
{
    // create program arguments
    size_t i, numArgs = args.items();
    Vector<char*> i_args(numArgs + 1);
    for (i = 0; i < numArgs; i++)
    {
        i_args[i] = args(i).get();
    }
    i_args[i] = nullptr;

    // run the program
    int status;
    pid_t pid = fork();
    ASSERT(pid >= 0);
    if (pid == 0)
    {
        int err = execvp(path, i_args.get());
        if (err < 0)
            errToEx(path);
    }
    else
    {
        waitpid(pid, &status, 0);
    }
    return WEXITSTATUS(status);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LinuxHostOS::sleep(uint_t sec) const
{
    struct timespec tv;
    struct timespec rem;
    tv.tv_sec = sec;
    tv.tv_nsec = 0;
    for (;;)
    {
        int res = nanosleep(&tv, &rem);
        if (res == 0)
            break;
        ASSERTD(errno == EINTR);
        tv = rem;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LinuxHostOS::usleep(uint64_t usec) const
{
    struct timespec tv;
    struct timespec rem;
    tv.tv_sec = usec / (uint64_t)1000000;
    tv.tv_nsec = (usec % (uint64_t)1000000) * (uint64_t)1000;
    for (;;)
    {
        int res = nanosleep(&tv, &rem);
        if (res == 0)
            break;
        ASSERTD(errno == EINTR);
        tv = rem;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LinuxHostOS::yield() const
{
    sched_yield();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
LinuxHostOS::getEnv(const String& envVarName) const
{
    return String(getenv(envVarName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
LinuxHostOS::setEnv(const String& envVarName, const String& envVarVal)
{
    String oldEnvVarVal = getEnv(envVarName);
    setenv(envVarName, envVarVal, 1);
    return oldEnvVarVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

String
LinuxHostOS::remEnv(const String& envVarName)
{
    String oldEnvVarVal = getEnv(envVarName);
    unsetenv(envVarName);
    return oldEnvVarVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
LinuxHostOS::setCanonicalTTY(int fd, bool canon)
{
    // set/clear the CANON and ECHO bits on the termios
    struct termios i_termios;
    tcgetattr(fd, &i_termios);
    if (canon)
    {
        i_termios.c_lflag |= ECHO;
        i_termios.c_lflag |= ICANON;
    }
    else
    {
        i_termios.c_lflag &= ~ECHO;
        i_termios.c_lflag &= ~ICANON;
    }
    tcsetattr(fd, TCSANOW, &i_termios);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
