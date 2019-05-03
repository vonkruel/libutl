#include <libutl/libutl.h>
#include <float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

/// floating-point /////////////////////////////////////////////////////////////////////////////////

const double_t double_t_min = std::numeric_limits<double>::lowest();
const double_t double_t_max = std::numeric_limits<double>::max();

/// width not specified ////////////////////////////////////////////////////////////////////////////

const byte_t byte_t_min = (byte_t)0x00;
const byte_t byte_t_max = (byte_t)0xff;
const char_t char_t_min = (char_t)0x80;
const char_t char_t_max = (char_t)0x7f;

const ushort_t ushort_t_min = (ushort_t)0x0000;
const ushort_t ushort_t_max = (ushort_t)0xffff;
const short_t short_t_min = (short_t)0x8000;
const short_t short_t_max = (short_t)0x7fff;

const uint_t uint_t_min = (uint_t)0x00000000;
const uint_t uint_t_max = (uint_t)0xffffffff;
const int_t int_t_min = (int_t)0x80000000;
const int_t int_t_max = (int_t)0x7fffffff;

#if UTL_SIZEOF_LONG == 4
const ulong_t ulong_t_min = (ulong_t)0x00000000;
const ulong_t ulong_t_max = (ulong_t)0xffffffff;
const long_t long_t_min = (long_t)0x80000000;
const long_t long_t_max = (long_t)0x7fffffff;
#else
const ulong_t ulong_t_min = (ulong_t)0x0000000000000000UL;
const ulong_t ulong_t_max = (ulong_t)0xffffffffffffffffUL;
const long_t long_t_min = (long_t)0x8000000000000000L;
const long_t long_t_max = (long_t)0x7fffffffffffffffL;
#endif

const size_t size_t_min = (size_t)0x0000000000000000ULL;
const size_t size_t_max = (size_t)0xffffffffffffffffULL;
const ssize_t ssize_t_min = (ssize_t)0x8000000000000000LL;
const ssize_t ssize_t_max = (ssize_t)0x7fffffffffffffffLL;

/// 8-bit //////////////////////////////////////////////////////////////////////////////////////////

const uint8_t uint8_t_min = (uint8_t)0x00;
const uint8_t uint8_t_max = (uint8_t)0xff;
const int8_t int8_t_min = (int8_t)0x80;
const int8_t int8_t_max = (int8_t)0x7f;

/// 16-bit /////////////////////////////////////////////////////////////////////////////////////////

const uint16_t uint16_t_min = (uint16_t)0x0000;
const uint16_t uint16_t_max = (uint16_t)0xffff;
const int16_t int16_t_min = (int16_t)0x8000;
const int16_t int16_t_max = (int16_t)0x7fff;

/// 32-bit /////////////////////////////////////////////////////////////////////////////////////////

const uint32_t uint32_t_min = (uint32_t)0x00000000;
const uint32_t uint32_t_max = (uint32_t)0xffffffff;
const int32_t int32_t_min = (int32_t)0x80000000;
const int32_t int32_t_max = (int32_t)0x7fffffff;

/// 64-bit ////////////////////////////////////////////////////////////////////////////////////////

const uint64_t uint64_t_min = (uint64_t)0x0000000000000000ULL;
const uint64_t uint64_t_max = (uint64_t)0xffffffffffffffffULL;
const int64_t int64_t_min = (int64_t)0x8000000000000000LL;
const int64_t int64_t_max = (int64_t)0x7fffffffffffffffLL;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Printf /////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<byte_t>::formatType = "c";
template <> const char* Printf<byte_t>::formatLength = "hh";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<char_t>::formatType = "c";
template <> const char* Printf<char_t>::formatLength = "hh";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<char>::formatType = "c";
template <> const char* Printf<char>::formatLength = "hh";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<char*>::formatType = "s";
template <> const char* Printf<char*>::formatLength = "";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<ushort_t>::formatType = "hu";
template <> const char* Printf<ushort_t>::formatLength = "h";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<short_t>::formatType = "hd";
template <> const char* Printf<short_t>::formatLength = "h";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<uint_t>::formatType = "u";
template <> const char* Printf<uint_t>::formatLength = "";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<int_t>::formatType = "d";
template <> const char* Printf<int_t>::formatLength = "";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<ulong_t>::formatType = "lu";
template <> const char* Printf<ulong_t>::formatLength = "l";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<long_t>::formatType = "ld";
template <> const char* Printf<long_t>::formatLength = "l";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<double>::formatType = "f";
template <> const char* Printf<double>::formatLength = "";

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_SIZEOF_LONG == 4

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<uint64_t>::formatType = "llu";
template <> const char* Printf<uint64_t>::formatLength = "ll";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<int64_t>::formatType = "lld";
template <> const char* Printf<int64_t>::formatLength = "ll";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<void*>::formatType = "x";
template <> const char* Printf<void*>::formatLength = "ll";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<byte_t*>::formatType = "x";
template <> const char* Printf<byte_t*>::formatLength = "ll";

////////////////////////////////////////////////////////////////////////////////////////////////////

#else // UTL_SIZEOF_LONG == 8

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<void*>::formatType = "x";
template <> const char* Printf<void*>::formatLength = "l";

////////////////////////////////////////////////////////////////////////////////////////////////////

template <> const char* Printf<byte_t*>::formatType = "x";
template <> const char* Printf<byte_t*>::formatLength = "l";

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
