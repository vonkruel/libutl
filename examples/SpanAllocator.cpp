#include <libutl/libutl.h>
#include <libutl/Application.h>
#include <libutl/BufferedFDstream.h>
#include <libutl/SpanAllocator.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_USE;
UTL_APP(Test);
UTL_MAIN_RL(Test);

////////////////////////////////////////////////////////////////////////////////////////////////////

int
Test::run(int, char**)
{
    SpanAllocator<size_t> allocator;

    // allocate space
    cout << "--- start with a 32 byte heap" << endl;
    allocator.free(Span<size_t>(0, 32));
    allocator.dump(cout);
    cout << "--- allocate 8 bytes" << endl;
    Span<size_t> block0 = allocator.alloc(8);
    allocator.dump(cout);
    cout << "--- allocate 16 bytes" << endl;
    Span<size_t> block1 = allocator.alloc(16);
    allocator.dump(cout);
    cout << "--- allocate 8 bytes" << endl;
    Span<size_t> block2 = allocator.alloc(8);
    allocator.dump(cout);

    // free space
    cout << "--- free first block" << endl;
    allocator.free(block0);
    allocator.dump(cout);
    cout << "--- free last block" << endl;
    allocator.free(block2);
    allocator.dump(cout);
    cout << "--- free middle block" << endl;
    allocator.free(block1);
    allocator.dump(cout);

    return 0;
}
