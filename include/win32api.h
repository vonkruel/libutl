#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////////
// win32 ///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define WINAPI __stdcall
#define CreateEvent CreateEventA
#define CreateMutex CreateMutexA
#define CreateProcess CreateProcessA
#define CreateSemaphore CreateSemaphoreA
#define GetEnvironmentVariable GetEnvironmentVariableA
#define SetEnvironmentVariable SetEnvironmentVariableA

////////////////////////////////////////////////////////////////////////////////////////////////////

#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedCompareExchange _InterlockedCompareExchange

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef FALSE
#undef TRUE
#undef INFINITE
#undef MAKEWORD
#define FALSE 0
#define TRUE 1
#define INFINITE 0xffffffffU
#define MAKEWORD(a, b) ((WORD)(((BYTE)(a)) | (((WORD)((BYTE)(b))) << 8)))

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void* HANDLE;
typedef int BOOL;
typedef unsigned char BYTE;
typedef utl::ushort_t WORD;
typedef ulong_t DWORD;
typedef DWORD(WINAPI* LPTHREAD_START_ROUTINE)(void*);

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned __int64 UINT_PTR;
typedef unsigned __int64 ULONG_PTR;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _CRITICAL_SECTION
{
    void* DebugInfo;
    long LockCount;
    long RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    DWORD SpinCount;
} CRITICAL_SECTION;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _STARTUPINFO
{
    DWORD cb;
    void* lpReserved;
    void* lpDesktop;
    void* lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    void* lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFO;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _PROCESS_INFORMATION
{
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION;

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
    int WINAPI CloseHandle(HANDLE);
    HANDLE WINAPI CreateEventA(void*, BOOL, BOOL, void*);
    HANDLE WINAPI CreateMutexA(void*, BOOL, void*);
    BOOL WINAPI CreateProcessA(void*,  // application name
                               void*,  // command line
                               void*,  // process attributes
                               void*,  // thread attributes
                               BOOL,   // inherit handles?
                               DWORD,  // creation flags
                               void*,  // environment
                               void*,  // current directory
                               void*,  // startup information
                               void*); // process information
    HANDLE WINAPI CreateSemaphoreA(void*, long, long, void*);
    HANDLE WINAPI CreateThread(void*,                  // security attributes
                               DWORD,                  // stack size
                               LPTHREAD_START_ROUTINE, // start address
                               void*,                  // parameter
                               DWORD,                  // creation flags
                               DWORD*);                // thread id
    void WINAPI DeleteCriticalSection(CRITICAL_SECTION*);
    void WINAPI EnterCriticalSection(CRITICAL_SECTION*);
    HANDLE WINAPI GetCurrentThread();
    DWORD WINAPI GetCurrentThreadId();
    DWORD WINAPI GetEnvironmentVariableA(void*, void*, DWORD);
    BOOL WINAPI GetExitCodeProcess(HANDLE, DWORD*);
    BOOL WINAPI GetExitCodeThread(HANDLE, DWORD*);
    DWORD WINAPI GetLastError();
    BOOL WINAPI GetProcessTimes(HANDLE, FILETIME*, FILETIME*, FILETIME*, FILETIME*);
    void WINAPI InitializeCriticalSection(CRITICAL_SECTION*);
    long WINAPI InterlockedIncrement(long volatile* val);
    long WINAPI InterlockedDecrement(long volatile* val);
    long WINAPI InterlockedCompareExchange(long volatile* dest, long exchange, long comparand);
    void WINAPI LeaveCriticalSection(CRITICAL_SECTION*);
    BOOL WINAPI ReleaseMutex(HANDLE);
    HANDLE WINAPI ReleaseSemaphore(HANDLE, long, void*);
    BOOL WINAPI ResetEvent(HANDLE);
    BOOL WINAPI SetEnvironmentVariableA(void*, void*);
    BOOL WINAPI SetEvent(HANDLE);
    void WINAPI Sleep(DWORD);
    DWORD WINAPI SignalObjectAndWait(HANDLE, HANDLE, DWORD, BOOL);
    DWORD WINAPI TlsAlloc();
    BOOL WINAPI TlsFree(DWORD);
    void* WINAPI TlsGetValue(DWORD);
    BOOL WINAPI TlsSetValue(DWORD, void*);
    DWORD WINAPI WaitForSingleObject(HANDLE, DWORD);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// winsock2 ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int socklen_t;
typedef UINT_PTR SOCKET;

extern "C"
{
    int WINAPI closesocket(SOCKET s);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#define h_errno WSAGetLastError()
#define s_addr S_un.S_addr
#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define FD_SETSIZE 64U
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCK_STREAM 1
#define SOCKET_ERROR (-1)
#define SO_REUSEADDR 4
#define SO_KEEPALIVE 8
#define SO_LINGER 128
#define SO_ERROR 0x1007
#define SOL_SOCKET 0xffff
#define TCP_NODELAY 0x0001
#define WSABASEERR 10000
#define WSADESCRIPTION_LEN 256
#define WSAEADDRINUSE (WSABASEERR + 48)
#define WSAEADDRNOTAVAIL (WSABASEERR + 49)
#define WSAENETUNREACH (WSABASEERR + 51)
#define WSAECONNRESET (WSABASEERR + 54)
#define WSAECONNREFUSED (WSABASEERR + 61)
#define WSAEHOSTUNREACH (WSABASEERR + 65)
#define WSAHOST_NOT_FOUND (WSABASEERR + 1001)
#define WSATRY_AGAIN (WSABASEERR + 1002)
#define WSANO_RECOVERY (WSABASEERR + 1003)
#define WSANO_DATA (WSABASEERR + 1004)
#define WSASYS_STATUS_LEN 128

////////////////////////////////////////////////////////////////////////////////////////////////////

// timeval for MSVC (MinGW already provides it with <time.h>)
#if UTL_CC == UTL_CC_MSVC

struct timeval
{
    long tv_sec;
    long tv_usec;
};

#define timerisset(tvp) ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp)                                                                    \
    ((tvp)->tv_sec cmp(uvp)->tv_sec ||                                                             \
     ((tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp(uvp)->tv_usec))
#define timerclear(tvp) (tvp)->tv_sec = (tvp)->tv_usec = 0

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

struct fd_set
{
    uint_t fd_count;
    SOCKET fd_array[FD_SETSIZE];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct hostent
{
    char* h_name;
    char** h_aliases;
    short h_addrtype;
    short h_length;
    char** h_addr_list;
#define h_addr h_addr_list[0]
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct in_addr
{
    union {
        struct
        {
            utl::byte_t s_b1, s_b2, s_b3, s_b4;
        } S_un_b;
        struct
        {
            utl::ushort_t s_w1, s_w2;
        } S_un_w;
        ulong_t S_addr;
    } S_un;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct sockaddr
{
    utl::ushort_t sa_family;
    char sa_data[14];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct sockaddr_in
{
    short sin_family;
    utl::ushort_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct addrinfo
{
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ai_addrlen;
    char* ai_canonname;
    struct sockaddr* ai_addr;
    struct addrinfo* ai_next;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct linger
{
    WORD l_onoff;
    WORD l_linger;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct sockaddr_storage
{
    short ss_family;
    char __ss_pad1[6];   // pad to 8
    int64_t __ss_align;  // force alignment
    char __ss_pad2[112]; // pad to 128
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct WSADATA
{
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN + 1];
    char szSystemStatus[WSASYS_STATUS_LEN + 1];
    utl::ushort_t iMaxSockets;
    utl::ushort_t iMaxUdpDg;
    char* lpVendorInfo;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef FD_CLR
#define FD_CLR(fd, set)                                                                            \
    do                                                                                             \
    {                                                                                              \
        uint_t __i;                                                                                \
        for (__i = 0; __i < ((fd_set*)(set))->fd_count; __i++)                                     \
        {                                                                                          \
            if (((fd_set*)(set))->fd_array[__i] == ((SOCKET)fd))                                   \
            {                                                                                      \
                while (__i < ((fd_set*)(set))->fd_count - 1)                                       \
                {                                                                                  \
                    ((fd_set*)(set))->fd_array[__i] = ((fd_set*)(set))->fd_array[__i + 1];         \
                    __i++;                                                                         \
                }                                                                                  \
                ((fd_set*)(set))->fd_count--;                                                      \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef FD_SET
#define FD_SET(fd, set)                                                                            \
    do                                                                                             \
    {                                                                                              \
        uint_t __i;                                                                                \
        for (__i = 0; __i < ((fd_set*)(set))->fd_count; __i++)                                     \
        {                                                                                          \
            if (((fd_set*)(set))->fd_array[__i] == ((SOCKET)fd))                                   \
            {                                                                                      \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
        if (__i == ((fd_set*)(set))->fd_count)                                                     \
        {                                                                                          \
            if (((fd_set*)(set))->fd_count < FD_SETSIZE)                                           \
            {                                                                                      \
                ((fd_set*)(set))->fd_array[__i] = ((SOCKET)fd);                                    \
                ((fd_set*)(set))->fd_count++;                                                      \
            }                                                                                      \
        }                                                                                          \
    } while (0)

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef FD_ZERO
#define FD_ZERO(set) (((fd_set*)(set))->fd_count = 0)

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef FD_ISSET
#define FD_ISSET(fd, set) __WSAFDIsSet((SOCKET)(fd), (fd_set*)(set))

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
    SOCKET WINAPI accept(SOCKET, struct sockaddr*, int*);
    int WINAPI bind(SOCKET, const struct sockaddr*, int);
    int WINAPI closesocket(SOCKET);
    int WINAPI connect(SOCKET, const struct sockaddr*, int);
    int WINAPI getaddrinfo(const char*, const char*, const struct addrinfo*, struct addrinfo**);
    hostent* WINAPI gethostbyaddr(const char*, int, int);
    hostent* WINAPI gethostbyname(const char*);
    int WINAPI gethostname(char*, int);
    int WINAPI getsockopt(SOCKET, int, int, char*, int*);
    int WINAPI setsockopt(SOCKET, int, int, const char*, int);
    WORD WINAPI htons(WORD);
    WORD WINAPI ntohs(WORD);
    int WINAPI listen(SOCKET, int);
    int WINAPI recv(SOCKET, char*, int, int);
    SOCKET WINAPI socket(int, int, int);
    int WINAPI select(int, fd_set*, fd_set*, fd_set*, const struct timeval*);
    int WINAPI send(SOCKET, const char*, int, int);
    int WINAPI WSACleanup();
    int WINAPI WSAGetLastError();
    int WINAPI WSAStartup(WORD, WSADATA*);
    int WINAPI __WSAFDIsSet(SOCKET, fd_set*);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
