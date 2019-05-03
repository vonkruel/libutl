#include <libutl/libutl.h>
#include <libutl/win32api.h>
#include <libutl/Bool.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/FDstream.h>
#include <libutl/Float.h>
#include <libutl/HostOS.h>
#include <libutl/LogMgr.h>
#include <libutl/Ordering.h>
#include <libutl/Pathname.h>
#include <libutl/Thread.h>
#include <libutl/Time.h>
#include <libutl/URI.h>
#include <limits>

// OpenSSL headers (not under Windows for now)
#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <libutl/Mutex.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

#undef new
#include <random>
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

static bool utl_initialized = false;
static uint_t serializeMode = 0;

// for OpenSSL
#if UTL_HOST_TYPE == UTL_HT_UNIX
static SSL_CTX* ssl_ctx = nullptr;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

void
init()
{
    ASSERTD(!utl_initialized);

    // init Thread
    Thread::utl_init();

    // set names for cin, cout, cerr
    cin.setName("standard input");
    cout.setName("standard output");
    cerr.setName("standard error");

    // set line buffering for cout, cerr if they are TTYs
    if (cout.isTTY())
        cout.setLineBuffered(true);
    if (cerr.isTTY())
        cerr.setLineBuffered(true);

// create the HostOS object
#if UTL_HOST_TYPE == UTL_HT_UNIX
    hostOS = new LinuxHostOS();
#else
    hostOS = new WindowsHostOS();
#endif

    // set up Stream class
    Stream::utl_init();

    // set up URI class
    URI::utl_init();

#if UTL_HOST_TYPE == UTL_HT_UNIX
    // ignore SIGPIPE
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    ASSERTFNZ(sigaction(SIGPIPE, &sa, nullptr));

    // init OpenSSL
    SSL_library_init();
    ERR_load_BIO_strings();
    ssl_ctx = SSL_CTX_new(TLS_method());
    SSL_CTX_load_verify_locations(ssl_ctx, nullptr, "/usr/lib/ssl/certs");
#endif

#if UTL_HOST_TYPE == UTL_HT_WINDOWS
    // init Winsock
    WSADATA wsaData;
    ASSERTFNZ(WSAStartup(MAKEWORD(2, 2), &wsaData));
#endif

    utl_initialized = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
deInit()
{
    ASSERTD(utl_initialized);
    utl_initialized = false;

    // close/detach cin
    cin.setName(nullptr);
    Stream* cinStream = cin.getStream();
    if (cinStream != nullptr)
    {
        if (cinStream->isA(FDstream))
        {
            utl::cast<FDstream>(cinStream)->detach();
        }
        cin.close();
    }

    // close/detach cout
    cout.setName(nullptr);
    Stream* coutStream = cout.getStream();
    if (coutStream != nullptr)
    {
        try
        {
            cout.flush();
        }
        catch (...)
        {
        }
        if (coutStream->isA(FDstream))
        {
            utl::cast<FDstream>(coutStream)->detach();
        }
        cout.close();
    }

    // close/detach cerr
    cerr.setName(nullptr);
    Stream* cerrStream = cerr.getStream();
    if (cerrStream != nullptr)
    {
        try
        {
            cerr.flush();
        }
        catch (...)
        {
        }
        if (cerrStream->isA(FDstream))
        {
            utl::cast<FDstream>(cerrStream)->detach();
        }
        cerr.close();
    }

    // delete the HostOS object
    delete hostOS;
    hostOS = nullptr;

    // de-init URI
    URI::utl_deInit();

    // free memory allocated by the logMgr
    LogMgr::utl_deInit();

    // deInit Thread
    Thread::utl_deInit();

    // deInit RunTimeClass
    RunTimeClass::utl_deInit();

// specialized cleanup for different systems
#if UTL_HOST_TYPE == UTL_HT_UNIX
    // deInit SSL
    SSL_CTX_free(ssl_ctx);
#endif

#if UTL_HOST_TYPE == UTL_HT_WINDOWS
    // deInit Winsock
    WSACleanup();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
initialized()
{
    return utl_initialized;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
abort(const char* filename, int lineNo, const char* text)
{
    if ((filename != nullptr) || (text != nullptr))
    {
        fprintf(stderr, "ABORT: ");
        if (filename != nullptr)
        {
            fprintf(stderr, "\"%s\", line %d\n", filename, lineNo);
        }
        if (text != nullptr)
        {
            fprintf(stderr, "%s\n", text);
        }
    }

    ::abort();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
abort(const char* text)
{
    utl::abort(nullptr, 0, text);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int
compare(bool lhs, bool rhs)
{
    if (lhs == rhs)
        return 0;
    if (!lhs && rhs)
        return -1;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
die(const char* filename, int lineNo, const char* text, int exitCode)
{
    if (filename != nullptr)
    {
        fprintf(stderr, "DIE: \"%s\", line %d\n", filename, lineNo);
    }
    if (text != nullptr)
    {
        fprintf(stderr, "%s\n", text);
    }
    exit(exitCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
die(const char* text, int exitCode)
{
    die(nullptr, 0, text, exitCode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

uint_t
getSerializeMode()
{
    return serializeMode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
setSerializeMode(uint_t mode)
{
    serializeMode = mode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void*
realloc(void* ptr, size_t size, size_t newSize)
{
    if (size == newSize)
        return ptr;
    void* newPtr = (newSize == 0) ? nullptr : new byte_t[newSize];
    memcpy(newPtr, ptr, min(size, newSize));
    delete[](byte_t*) ptr;
    return newPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char*
strdup(const char* str)
{
    if (str == nullptr)
        return nullptr;
    int len = strlen(str);
    char* res = new char[len + 1];
    strcpy(res, str);
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SSL_CTX*
sslContext()
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    return ssl_ctx;
#else
    ABORT();
    return nullptr;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool
sslVerifyLocations(const char* caFile, const char* caPath)
{
#if UTL_HOST_TYPE == UTL_HT_UNIX
    int res = SSL_CTX_load_verify_locations(ssl_ctx, caFile, caPath);
    return (res == 1);
#else
    ABORT();
    return false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(bool& b, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            b = Bool(str).get();
        }
        else
        {
            Bool(b).toString().serialize(stream, io, mode);
        }
    }
    else
    {
        if (io == io_rd)
        {
            byte_t c = stream.get();
            b = (c != 0);
        }
        else
        {
            stream.put((byte_t)(b ? 1 : 0));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(int8_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serializeIn(stream, mode);
            i = strtol(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%d (char)", (int)i);
            String(str, false).serializeOut(stream, mode);
        }
    }
    else
    {
        if (io == io_rd)
        {
            i = stream.get();
        }
        else
        {
            stream.put(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(char& c, Stream& stream, uint_t io, uint_t mode)
{
    if (io == io_rd)
        stream.get(c);
    else
        stream.put(c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(uint8_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serializeIn(stream, mode);
            i = strtol(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%d", (int)i);
            String(str, false).serializeOut(stream, mode);
        }
    }
    else
    {
        if (io == io_rd)
        {
            i = stream.get();
        }
        else
        {
            stream.put(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(int16_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtol(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%d", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = ((uint16_t)stream.get() << 8) | (uint16_t)stream.get();
        }
        else
        {
            stream.put((byte_t)((i & 0x0000ff00U) >> 8));
            stream.put((byte_t)(i & 0x000000ffU));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(int16_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(int16_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(uint16_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtol(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%d", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = ((uint16_t)stream.get() << 8) | (uint16_t)stream.get();
        }
        else
        {
            stream.put((byte_t)((i & 0x0000ff00U) >> 8));
            stream.put((byte_t)(i & 0x000000ffU));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(uint16_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(uint16_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(int32_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtol(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%d", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = ((uint32_t)stream.get() << 24) | ((uint32_t)stream.get() << 16) |
                ((uint32_t)stream.get() << 8) | (uint32_t)stream.get();
        }
        else
        {
            stream.put((byte_t)((i & 0xff000000U) >> 24));
            stream.put((byte_t)((i & 0x00ff0000U) >> 16));
            stream.put((byte_t)((i & 0x0000ff00U) >> 8));
            stream.put((byte_t)(i & 0x000000ffU));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(int32_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(int32_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(uint32_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtoul(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%u", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = ((uint32_t)stream.get() << 24) | ((uint32_t)stream.get() << 16) |
                ((uint32_t)stream.get() << 8) | (uint32_t)stream.get();
        }
        else
        {
            stream.put((byte_t)((i & 0xff000000U) >> 24));
            stream.put((byte_t)((i & 0x00ff0000U) >> 16));
            stream.put((byte_t)((i & 0x0000ff00U) >> 8));
            stream.put((byte_t)(i & 0x000000ffU));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(uint32_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(uint32_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(long& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtol(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%ld", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = 0;
#if UTL_SIZEOF_LONG == 8
            i = ((uint64_t)stream.get() << 56) | ((uint64_t)stream.get() << 48) |
                ((uint64_t)stream.get() << 40) | ((uint64_t)stream.get() << 32);
#endif
            i |= ((uint64_t)stream.get() << 24) | ((uint64_t)stream.get() << 16) |
                 ((uint64_t)stream.get() << 8) | (uint64_t)stream.get();
        }
        else
        {
#if UTL_SIZEOF_LONG == 8
            stream.put((byte_t)((i & 0xff00000000000000ULL) >> 56));
            stream.put((byte_t)((i & 0x00ff000000000000ULL) >> 48));
            stream.put((byte_t)((i & 0x0000ff0000000000ULL) >> 40));
            stream.put((byte_t)((i & 0x000000ff00000000ULL) >> 32));
#endif
            stream.put((byte_t)((i & 0x00000000ff000000ULL) >> 24));
            stream.put((byte_t)((i & 0x0000000000ff0000ULL) >> 16));
            stream.put((byte_t)((i & 0x000000000000ff00ULL) >> 8));
            stream.put((byte_t)((i & 0x00000000000000ffULL)));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(long));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(long));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(ulong_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtoul(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%lu", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = 0;
#if UTL_SIZEOF_LONG == 8
            i = ((ulong_t)stream.get() << 56) | ((ulong_t)stream.get() << 48) |
                ((ulong_t)stream.get() << 40) | ((ulong_t)stream.get() << 32);
#endif
            i |= ((ulong_t)stream.get() << 24) | ((ulong_t)stream.get() << 16) |
                 ((ulong_t)stream.get() << 8) | (ulong_t)stream.get();
        }
        else
        {
#if UTL_SIZEOF_LONG == 8
            stream.put((byte_t)((i & 0xff00000000000000ULL) >> 56));
            stream.put((byte_t)((i & 0x00ff000000000000ULL) >> 48));
            stream.put((byte_t)((i & 0x0000ff0000000000ULL) >> 40));
            stream.put((byte_t)((i & 0x000000ff00000000ULL) >> 32));
#endif
            stream.put((byte_t)((i & 0x00000000ff000000ULL) >> 24));
            stream.put((byte_t)((i & 0x0000000000ff0000ULL) >> 16));
            stream.put((byte_t)((i & 0x000000000000ff00ULL) >> 8));
            stream.put((byte_t)((i & 0x00000000000000ffULL)));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(ulong_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(ulong_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_SIZEOF_LONG == 4

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(int64_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtoll(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%lld", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = ((uint64_t)stream.get() << 56) | ((uint64_t)stream.get() << 48) |
                ((uint64_t)stream.get() << 40) | ((uint64_t)stream.get() << 32) |
                ((uint64_t)stream.get() << 24) | ((uint64_t)stream.get() << 16) |
                ((uint64_t)stream.get() << 8) | (uint64_t)stream.get();
        }
        else
        {
            stream.put((byte_t)((i & 0xff00000000000000ULL) >> 56));
            stream.put((byte_t)((i & 0x00ff000000000000ULL) >> 48));
            stream.put((byte_t)((i & 0x0000ff0000000000ULL) >> 40));
            stream.put((byte_t)((i & 0x000000ff00000000ULL) >> 32));
            stream.put((byte_t)((i & 0x00000000ff000000ULL) >> 24));
            stream.put((byte_t)((i & 0x0000000000ff0000ULL) >> 16));
            stream.put((byte_t)((i & 0x000000000000ff00ULL) >> 8));
            stream.put((byte_t)((i & 0x00000000000000ffULL)));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(int64_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(int64_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(uint64_t& i, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if (mode & ser_readable)
    {
        if (io == io_rd)
        {
            String str;
            str.serialize(stream, io, mode);
            i = strtoull(str.get(), nullptr, 10);
        }
        else
        {
            char str[16];
            sprintf(str, "%llu", i);
            stream << str << endl;
        }
    }
    else if (mode & ser_compact)
    {
        if (io == io_rd)
        {
            i = (int64_t)((uint64_t)stream.get() << 56) | ((uint64_t)stream.get() << 48) |
                ((uint64_t)stream.get() << 40) | ((uint64_t)stream.get() << 32) |
                ((uint64_t)stream.get() << 24) | ((uint64_t)stream.get() << 16) |
                ((uint64_t)stream.get() << 8) | (uint64_t)stream.get();
        }
        else
        {
            stream.put((byte_t)((i & 0xff00000000000000ULL) >> 56));
            stream.put((byte_t)((i & 0x00ff000000000000ULL) >> 48));
            stream.put((byte_t)((i & 0x0000ff0000000000ULL) >> 40));
            stream.put((byte_t)((i & 0x000000ff00000000ULL) >> 32));
            stream.put((byte_t)((i & 0x00000000ff000000ULL) >> 24));
            stream.put((byte_t)((i & 0x0000000000ff0000ULL) >> 16));
            stream.put((byte_t)((i & 0x000000000000ff00ULL) >> 8));
            stream.put((byte_t)((i & 0x00000000000000ffULL)));
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&i, sizeof(uint64_t));
        }
        else
        {
            stream.write((byte_t*)&i, sizeof(uint64_t));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UTL_SIZEOF_LONG == 4

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(double& n, Stream& stream, uint_t io, uint_t mode)
{
    if (mode & ser_default)
        mode |= getSerializeMode();
    if ((mode & ser_readable) || (mode & ser_compact))
    {
        if (io == utl::io_rd)
        {
            String str;
            str.serializeIn(stream, mode);
            if (str == "+INF")
                n = double_t_max;
            else if (str == "-INF")
                n = double_t_min;
            else
                n = Float(str);
        }
        else
        {
            String str = Float(n).toString(uint_t_max);
            str.serializeOut(stream, mode);
        }
    }
    else // ser_nonportable
    {
        if (io == io_rd)
        {
            stream.read((byte_t*)&n, sizeof(double));
        }
        else
        {
            stream.write((byte_t*)&n, sizeof(double));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void
serialize(Object& object, Stream& stream, uint_t io, uint_t mode)
{
    object.serialize(stream, io, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

// a hack for randutils on MinGW where std::this_thread is absent (decl @ include/RandUtils.h)
#include <libutl/RandUtils.h>
#if UTL_CC == UTL_CC_MINGW
namespace std
{
size_t
this_thread::get_id()
{
    return (size_t)utl::Thread::get();
}
} // namespace std
#endif
