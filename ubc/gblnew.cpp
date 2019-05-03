#include <libutl/libutl.h>
#include <libutl/Mutex.h>
#include <libutl/Thread.h>
#include <libutl/gblnew.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG_MSVC
UTL_NS_BEGIN;
void
memReportLeaks()
{
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtDumpMemoryLeaks();
}
UTL_NS_END;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////////

#undef new

///////////////////////////////////////////////////////////////////////////////////////////////////

//#define DEBUG_MODULE

///////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// COMMON definitions ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

static const size_t gblnew_zeroBytePtr = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// DEBUG definitions /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class AllocRec
{
public:
    uintptr_t magic;
    size_t size;
    void* addr;
    const char* file;
    uint_t line;
    AllocRec *prev, *next;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

static void gblnew_printBlock(const AllocRec* allocRec, const char* title);

///////////////////////////////////////////////////////////////////////////////////////////////////

static Mutex gblnew_mutex;
static std::atomic<size_t> gblnew_numBlocks(0);
static std::atomic<size_t> gblnew_numBytes(0);
static AllocRec* gblnew_allocList = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// DEBUG functions ///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void
memReportLeaks()
{
    memReportLeaks(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void
memReportLeaks(bool includeUnknown)
{
    size_t outstanding = 0;
    size_t total = 0;
    AllocRec* cur = gblnew_allocList;
    auto lim = gblnew_numBlocks.load();
    for (size_t i = 0; i != lim; ++i)
    {
        if (includeUnknown || (cur->file != nullptr))
        {
            if (outstanding++ != 0)
            {
                fprintf(stderr, "\n");
            }
            total += cur->size;
            gblnew_printBlock(cur, "leaked block");
        }
        cur = cur->next;
    }
    if (outstanding > 0)
    {
        fprintf(stderr, "--- %zu block(s) leaked for a total of %zu bytes.\n", outstanding, total);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void
memPrintBlocks()
{
    for (AllocRec* cur = gblnew_allocList; cur != nullptr; cur = cur->next)
    {
        gblnew_printBlock(cur, "block");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

///////////////////////////////////////////////////////////////////////////////////////////////////

void
gblnew_printBlock(const AllocRec* allocRec, const char* title)
{
    size_t i;

    // print title
    ASSERT(strlen(title) <= 42);
    size_t numDashes = 42 - strlen(title);
    fprintf(stderr, "--- %s ", title);
    for (i = 0; i != numDashes; ++i)
        fprintf(stderr, "-");
    fprintf(stderr, "\n");

    // print source file, line # (or report that the information is unavailable)
    if (allocRec->file == nullptr)
    {
        fprintf(stderr, "<source file and line # unavailable>\n");
    }
    else
    {
        fprintf(stderr, "\"%s\", line %u\n", allocRec->file, allocRec->line);
    }

    // print "<size> bytes @ 0x<addr>"
    auto size = allocRec->size;
    auto ptr = static_cast<byte_t*>(allocRec->addr);
    fprintf(stderr, "%zu bytes @ 0x%.16zx\n", size, reinterpret_cast<size_t>(ptr));

    // print up to 128 bytes of the block data in hex & ascii form
    for (auto lim = ptr + min(size, (size_t)128); ptr < lim; ptr += 16)
    {
        fprintf(stderr, "  ");
        auto n = min((size_t)16, (size_t)(lim - ptr));
        size_t pos;
        for (pos = 0; pos != n; ++pos)
        {
            auto c = ptr[pos];
            fprintf(stderr, "%02x ", static_cast<unsigned int>(c));
        }
        fprintf(stderr, "   ");
        for (pos = 0; pos != n; ++pos)
        {
            auto c = ptr[pos];
            if (isgraph(c))
            {
                fprintf(stderr, "%c", static_cast<int>(c));
            }
            else
            {
                fprintf(stderr, " ");
            }
        }
        fprintf(stderr, "\n");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void*
operator new[](size_t size)
{
    return operator new(size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void
operator delete[](void* ptr) noexcept
{
    operator delete(ptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void*
operator new[](size_t size, const char* file, uint_t line)
{
    return operator new(size, file, line);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void
operator delete[](void* ptr, const char* file, uint_t line) noexcept
{
    operator delete(ptr, file, line);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void*
operator new(size_t size)
{
    return operator new(size, nullptr, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void
operator delete(void* ptr) noexcept
{
    operator delete(ptr, nullptr, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void*
operator new(size_t size, const char* file, uint_t line)
{
    // allocating 0 bytes is OK - return a special-purpose pointer
    // (this pointer should never be dereferenced...)
    if (size == 0)
        return const_cast<size_t*>(&gblnew_zeroBytePtr);

    // always allocate a multiple of sizeof(void*) bytes
    size = utl::nextMultipleOfPow2(sizeof(void*), size);

    // allocate space for: (AllocRec, blockAddr, BLOCK, blockAddr)
    auto blockAddr = malloc(size + sizeof(AllocRec) + 2 * sizeof(void*));
    if (blockAddr == nullptr)
    {
        throw std::bad_alloc();
    }

    // address the AllocRec node
    auto allocRec = static_cast<AllocRec*>(blockAddr);

    // set magic header
    auto vpp = reinterpret_cast<void**>(allocRec + 1);
    *vpp++ = blockAddr;

    // vpp now points to the beginning of the caller's allocation
    auto res = static_cast<void*>(vpp);

    // set magic footer
    *(vpp + (size / sizeof(void*))) = blockAddr;

    // set fields in allocRec
    allocRec->magic = reinterpret_cast<uintptr_t>(res) + size;
    allocRec->size = size;
    allocRec->addr = res;
    allocRec->prev = nullptr;
    allocRec->file = file;
    allocRec->line = line;

    // allocRec becomes the head of the allocation list
    gblnew_mutex.lock();
    allocRec->next = gblnew_allocList;
    if (gblnew_allocList != nullptr)
    {
        gblnew_allocList->prev = allocRec;
    }
    gblnew_allocList = allocRec;
    gblnew_mutex.unlock();

    // update stats
    gblnew_numBlocks.fetch_add(1, std::memory_order_relaxed);
    gblnew_numBytes.fetch_add(size, std::memory_order_relaxed);

    return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void
operator delete(void* ptr, const char*, uint_t) noexcept
{
    // deleting nullptr is OK and is a no-op
    if (ptr == nullptr)
        return;

    // also a no-op if we delete the 'zeroBytePtr'
    if (ptr == static_cast<const void*>(&gblnew_zeroBytePtr))
    {
        if (gblnew_zeroBytePtr != 0)
        {
            fprintf(stderr, "magic # check failed freeing zero-byte block!\n");
        }
        return;
    }

    // find the AllocRec, check its magic
    auto blockAddr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(AllocRec) -
                                             sizeof(void*));
    auto allocRec = static_cast<AllocRec*>(blockAddr);
    auto vpp = static_cast<void**>(ptr) - 1;
    if ((allocRec == nullptr) ||
        (allocRec->magic != (reinterpret_cast<uintptr_t>(ptr) + allocRec->size)))
    {
        fprintf(stderr, "magic # check failed freeing block: 0x%.16zx\n",
                reinterpret_cast<size_t>(ptr));
        fprintf(stderr, "allocRec: 0x%.16zx\n", reinterpret_cast<size_t>(allocRec));
        return;
    }
    allocRec->magic = 0;
    auto size = allocRec->size;

    // check magic header
    if (*vpp != blockAddr)
    {
        gblnew_printBlock(allocRec, "wrote before beginning of block");
    }
    *vpp++ = nullptr;

    // check magic footer
    vpp += (size / sizeof(void*));
    if (*vpp != blockAddr)
    {
        gblnew_printBlock(allocRec, "wrote past end of block");
    }
    *vpp = nullptr;

    // update stats
    gblnew_numBlocks.fetch_sub(1, std::memory_order_relaxed);
    gblnew_numBytes.fetch_sub(size, std::memory_order_relaxed);

    // unlink the AllocRec from the list
    gblnew_mutex.lock();
    if (allocRec->prev != nullptr)
        allocRec->prev->next = allocRec->next;
    if (allocRec->next != nullptr)
        allocRec->next->prev = allocRec->prev;
    if (allocRec == gblnew_allocList)
        gblnew_allocList = allocRec->next;
    gblnew_mutex.unlock();

    // free the block
    free(blockAddr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG
