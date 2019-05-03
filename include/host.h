#pragma once

// UTL_CC values
#define UTL_CC_GCC 1
#define UTL_CC_MINGW 2
#define UTL_CC_MSVC 3

// UTL_HOST_TYPE values
#define UTL_HT_UNIX 1
#define UTL_HT_WINDOWS 2

// UTL_HOST_OS values
#define UTL_OS_LINUX 1
#define UTL_OS_WINDOWS 2

// determine host type
#if UTL_HOST_OS == UTL_OS_LINUX
#define UTL_HOST_TYPE UTL_HT_UNIX
#elif UTL_HOST_OS == UTL_OS_WINDOWS
#define UTL_HOST_TYPE UTL_HT_WINDOWS
#else
#error UTL_HOST_OS has unknown value.
#endif

// determine sizeof(long)
#if UTL_HOST_OS == UTL_OS_LINUX
#define UTL_SIZEOF_LONG 8
#else
#define UTL_SIZEOF_LONG 4
#endif

// UTL_HOST_ARCH (default value)
#ifndef UTL_HOST_ARCH
#define UTL_HOST_ARCH UTL_ARCH_AMD64
#endif

// UTL_ARCH values
#define UTL_ARCH_AMD64 1

// set cache line size, endian-ness, word size
#if UTL_HOST_ARCH == UTL_ARCH_AMD64
#define UTL_ARCH_CACHE_LINE_SIZE 64
#define UTL_ARCH_LITTLE_ENDIAN
#define UTL_ARCH_WORDSIZE_BITS 64
#define UTL_ARCH_WORDSIZE_BYTES 8
#else
#error UTL_HOST_ARCH has unknown value.
#endif
