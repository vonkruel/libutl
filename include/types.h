#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

// MSVC-specific headers
#if UTL_CC == UTL_CC_MSVC
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
   Each of these types has global constants to represent its minimum and maximum values.
   For example, the minimum and maximum values of \b uint_t are defined by \b uint_t_min and
   \b uint_t_max, respectively.
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// typedefs ////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Floating-point value.
   \ingroup general
*/
typedef double double_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned character.
   \ingroup general
*/
typedef unsigned char byte_t;

/**
   Signed character.
   \ingroup general
*/
typedef signed char char_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned short integer.
   \ingroup general
*/
typedef unsigned short ushort_t;

/**
   Signed short integer.
   \ingroup general
*/
typedef signed short short_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned integer.
   \ingroup general
*/
typedef unsigned int uint_t;

/**
   Signed integer.
   \ingroup general
*/
typedef signed int int_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned long integer.
   \ingroup general
*/
typedef unsigned long ulong_t;

/**
   Signed long integer.
   \ingroup general
*/
typedef signed long long_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned 8-bit integer.
   \ingroup general
*/
typedef unsigned char uint8_t;

/**
   Signed 8-bit integer.
   \ingroup general
*/
typedef signed char int8_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned 16-bit integer.
   \ingroup general
*/
typedef unsigned short uint16_t;

/**
   Signed 16-bit integer.
   \ingroup general
*/
typedef signed short int16_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned 32-bit integer.
   \ingroup general
*/
typedef unsigned int uint32_t;

/**
   Signed 32-bit integer.
   \ingroup general
*/
typedef signed int int32_t;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Unsigned 64-bit integer.
   \ingroup general
*/
#if UTL_HOST_OS == UTL_OS_WINDOWS
typedef unsigned long long uint64_t;
#else
typedef unsigned long uint64_t;
#endif

/**
   Signed 64-bit integer.
   \ingroup general
*/
#if UTL_HOST_OS == UTL_OS_WINDOWS
typedef long long int64_t;
#else
typedef long int64_t;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// Minimum and Maximum Values //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum double_t value.
   \ingroup general
*/
extern const double_t double_t_min;

/**
   Maximum double_t value.
   \ingroup general
*/
extern const double_t double_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum byte_t value.
   \ingroup general
*/
extern const byte_t byte_t_min;

/**
   Maximum byte_t value.
   \ingroup general
*/
extern const byte_t byte_t_max;

/**
   Minimum char_t value.
   \ingroup general
*/
extern const char_t char_t_min;

/**
   Maximum char_t value.
   \ingroup general
*/
extern const char_t char_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum ushort_t value.
   \ingroup general
*/
extern const ushort_t ushort_t_min;

/**
   Maximum ushort_t value.
   \ingroup general
*/
extern const ushort_t ushort_t_max;

/**
   Minimum short_t value.
   \ingroup general
*/
extern const short_t short_t_min;

/**
   Maximum short_t value.
   \ingroup general
*/
extern const short_t short_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum uint_t value.
   \ingroup general
*/
extern const uint_t uint_t_min;

/**
   Maximum uint_t value.
   \ingroup general
*/
extern const uint_t uint_t_max;

/**
   Minimum int_t value.
   \ingroup general
*/
extern const int_t int_t_min;

/**
   Maximum int_t value.
   \ingroup general
*/
extern const int_t int_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum unsigned long value.
   \ingroup general
*/
extern const ulong_t ulong_t_min;

/**
   Maximum unsigned long value.
   \ingroup general
*/
extern const ulong_t ulong_t_max;

/**
   Minimum long value.
   \ingroup general
*/
extern const long_t long_t_min;

/**
   Maximum long value.
   \ingroup general
*/
extern const long_t long_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum size_t value.
   \ingroup general
*/
extern const size_t size_t_min;

/**
   Maximum size_t value.
   \ingroup general
*/
extern const size_t size_t_max;

/**
   Minimum ssize_t value.
   \ingroup general
*/
extern const ssize_t ssize_t_min;

/**
   Maximum ssize_t value.
   \ingroup general
*/
extern const ssize_t ssize_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum uint8_t value.
   \ingroup general
*/
extern const uint8_t uint8_t_min;

/**
   Maximum uint8_t value.
   \ingroup general
*/
extern const uint8_t uint8_t_max;

/**
   Minimum int8_t value.
   \ingroup general
*/
extern const int8_t int8_t_min;

/**
   Maximum int8_t value.
   \ingroup general
*/
extern const int8_t int8_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum uint16_t value.
   \ingroup general
*/
extern const uint16_t uint16_t_min;

/**
   Maximum uint16_t value.
   \ingroup general
*/
extern const uint16_t uint16_t_max;

/**
   Minimum int16_t value.
   \ingroup general
*/
extern const int16_t int16_t_min;

/**
   Maximum int16_t value.
   \ingroup general
*/
extern const int16_t int16_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum uint32_t value.
   \ingroup general
*/
extern const uint32_t uint32_t_min;

/**
   Maximum uint32_t value.
   \ingroup general
*/
extern const uint32_t uint32_t_max;

/**
   Minimum int32_t value.
   \ingroup general
*/
extern const int32_t int32_t_min;

/**
   Maximum int32_t value.
   \ingroup general
*/
extern const int32_t int32_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Minimum uint64_t value.
   \ingroup general
*/
extern const uint64_t uint64_t_min;

/**
   Maximum uint64_t value.
   \ingroup general
*/
extern const uint64_t uint64_t_max;

/**
   Minimum int64_t value.
   \ingroup general
*/
extern const int64_t int64_t_min;

/**
   Maximum int64_t value.
   \ingroup general
*/
extern const int64_t int64_t_max;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Printf //////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
   Provide printf formatting strings for common types.

   \author Adam McKee
   \ingroup general
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T> struct Printf
{
    static const char* formatType;
    static const char* formatLength;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;

////////////////////////////////////////////////////////////////////////////////////////////////////

// these names may exist in other headers (& should be defined the same way everywhere)
// we do this to make sure they're defined
using utl::byte_t;       // unsigned char
using utl::int16_t;      // 16-bit signed integer
using utl::int32_t;      // 32-bit signed integer
using utl::int64_t;      // 64-bit signed integer
using utl::int64_t_max;  // maximum value for int64_t
using utl::int64_t_min;  // minimum value for int64_t
using utl::int8_t;       // 8-bit signed integer
using utl::uint16_t;     // 16-bit unsigned integer
using utl::uint32_t;     // 32-bit unsigned integer
using utl::uint32_t_max; // maximum value for uint32_t
using utl::uint64_t;     // 64-bit unsigned integer
using utl::uint64_t_max; // maximum value for uint64_t
using utl::uint8_t;      // 8-bit unsigned integer
using utl::uint_t;       // unsigned integer
using utl::uint_t_max;   // maximum value for uint_t
using utl::ulong_t;      // unsigned long