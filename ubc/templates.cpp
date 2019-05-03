#include <libutl/libutl.h>
#include <libutl/algorithms_inl.h>
#include <libutl/Array.h>
#include <libutl/AutoPtr.h>
#include <libutl/BoyerMooreSearch.h>
#include <libutl/Deque.h>
#include <libutl/Int.h>
#include <libutl/Mutex.h>
#include <libutl/Pair.h>
#include <libutl/PointerIntPair.h>
#include <libutl/RBtree.h>
#include <libutl/Regex.h>
#include <libutl/RingBuffer.h>
#include <libutl/Span.h>
#include <libutl/SpanAllocator.h>
#include <libutl/SpanCol.h>
#include <libutl/Stack.h>
#include <libutl/Time.h>
#include <libutl/VectorMD.h>

/// AutoPtr ////////////////////////////////////////////////////////////////////////////////////////

template class utl::AutoPtr<utl::BidIt>;
template class utl::AutoPtr<utl::FwdIt>;
template class utl::AutoPtr<utl::Object>;
template class utl::AutoPtr<utl::RandIt>;

/// BoyerMooreSearch ///////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::BoyerMooreSearch, char);
UTL_INSTANTIATE_TPL(utl::BoyerMooreSearch, byte_t);

/// Integer ////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Integer, char);
UTL_INSTANTIATE_TPL(utl::Integer, int);
UTL_INSTANTIATE_TPL(utl::Integer, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::Integer, long);
UTL_INSTANTIATE_TPL(utl::Integer, utl::ulong_t);
#if UTL_SIZEOF_LONG == 4
UTL_INSTANTIATE_TPL(utl::Integer, utl::int64_t);
UTL_INSTANTIATE_TPL(utl::Integer, utl::uint64_t);
#endif

/// Number /////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Number, char);
UTL_INSTANTIATE_TPL(utl::Number, int);
UTL_INSTANTIATE_TPL(utl::Number, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::Number, long);
UTL_INSTANTIATE_TPL(utl::Number, utl::ulong_t);
UTL_INSTANTIATE_TPL(utl::Number, double);
#if UTL_SIZEOF_LONG == 4
UTL_INSTANTIATE_TPL(utl::Number, utl::int64_t);
UTL_INSTANTIATE_TPL(utl::Number, utl::uint64_t);
#endif

/// Queue //////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Queue, utl::String);

/// Span ///////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL2(utl::Span, char, char);
UTL_INSTANTIATE_TPL2(utl::Span, utl::byte_t, utl::byte_t);
UTL_INSTANTIATE_TPL2(utl::Span, short, short);
UTL_INSTANTIATE_TPL2(utl::Span, utl::ushort_t, utl::ushort_t);
UTL_INSTANTIATE_TPL2(utl::Span, int, int);
UTL_INSTANTIATE_TPL2(utl::Span, utl::uint_t, utl::uint_t);
UTL_INSTANTIATE_TPL2(utl::Span, long, long);
UTL_INSTANTIATE_TPL2(utl::Span, utl::ulong_t, utl::ulong_t);
#if UTL_SIZEOF_LONG == 4
UTL_INSTANTIATE_TPL2(utl::Span, int64_t, int64_t);
UTL_INSTANTIATE_TPL2(utl::Span, utl::uint64_t, utl::uint64_t);
#endif

/// SpanAllocator //////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL2(utl::SpanAllocator, size_t, size_t);

/// SpanCol ////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL2(utl::SpanCol, size_t, size_t);
UTL_INSTANTIATE_TPL2(utl::SpanCol, utl::Time, utl::Duration);

/// SpanSizeOrdering ///////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL2(utl::SpanSizeOrdering, size_t, size_t);

/// Stack //////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Stack, utl::Object);

/// TArray /////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TArray, utl::Object);
UTL_INSTANTIATE_TPL(utl::TArray, utl::Span<size_t>);
UTL_INSTANTIATE_TPL(utl::TArray, utl::Stream);
UTL_INSTANTIATE_TPL(utl::TArray, utl::String);

/// TDeque /////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TDeque, utl::String);

/// TDequeIt ///////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TDequeIt, utl::String);

/// TBidIt /////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TBidIt, utl::Object);

/// TRBtree ////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TRBtree, utl::Pair);
UTL_INSTANTIATE_TPL(utl::TRBtree, utl::Span<size_t>);
UTL_INSTANTIATE_TPLxTPL2(utl::TRBtree, utl::Span, utl::Time, utl::Duration);

/// TRBtreeIt //////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::TRBtreeIt, utl::Pair);
UTL_INSTANTIATE_TPL(utl::TRBtreeIt, utl::Span<size_t>);
UTL_INSTANTIATE_TPLxTPL2(utl::TRBtreeIt, utl::Span, utl::Time, utl::Duration);

/// Vector /////////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Vector, char);
UTL_INSTANTIATE_TPL(utl::Vector, char*);
UTL_INSTANTIATE_TPL(utl::Vector, utl::byte_t);
UTL_INSTANTIATE_TPL(utl::Vector, utl::byte_t*);
UTL_INSTANTIATE_TPL(utl::Vector, int);
UTL_INSTANTIATE_TPL(utl::Vector, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::Vector, long);
UTL_INSTANTIATE_TPL(utl::Vector, utl::ulong_t);
#if UTL_SIZEOF_LONG == 4
UTL_INSTANTIATE_TPL(utl::Vector, utl::int64_t);
UTL_INSTANTIATE_TPL(utl::Vector, utl::uint64_t);
#endif
UTL_INSTANTIATE_TPL(utl::Vector, utl::MultiKeyObject);
UTL_INSTANTIATE_TPL(utl::Vector, void*);
UTL_INSTANTIATE_TPL(utl::Vector, utl::Object*);

// for Hashtable
using pip1_t =
    utl::PointerIntPair<utl::Object*, 1, unsigned int, utl::PointerLikeTraits<utl::Object*>>;
UTL_INSTANTIATE_TPL(utl::Vector, pip1_t);

/// VectorMD ///////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::VectorMD, utl::byte_t);
UTL_INSTANTIATE_TPL(utl::VectorMD, utl::ushort_t);
UTL_INSTANTIATE_TPL(utl::VectorMD, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::VectorMD, utl::ulong_t);
UTL_INSTANTIATE_TPL(utl::VectorMD, utl::Object*);

/// Vector_1d //////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Vector1d, utl::byte_t);
UTL_INSTANTIATE_TPL(utl::Vector1d, utl::ushort_t);
UTL_INSTANTIATE_TPL(utl::Vector1d, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::Vector1d, utl::ulong_t);
UTL_INSTANTIATE_TPL(utl::Vector1d, utl::Object*);

/// Vector_2d //////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Vector2d, utl::byte_t);
UTL_INSTANTIATE_TPL(utl::Vector2d, utl::ushort_t);
UTL_INSTANTIATE_TPL(utl::Vector2d, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::Vector2d, utl::ulong_t);
UTL_INSTANTIATE_TPL(utl::Vector2d, utl::Object*);

/// Vector_3d //////////////////////////////////////////////////////////////////////////////////////

UTL_INSTANTIATE_TPL(utl::Vector3d, utl::byte_t);
UTL_INSTANTIATE_TPL(utl::Vector3d, utl::ushort_t);
UTL_INSTANTIATE_TPL(utl::Vector3d, utl::uint_t);
UTL_INSTANTIATE_TPL(utl::Vector3d, utl::ulong_t);
UTL_INSTANTIATE_TPL(utl::Vector3d, utl::Object*);
