#pragma once

// libutl configuration
#include <libutl/host.h>
#include "utl_config.h"

// MSVC: use the CRT's heap debugging facility
#if UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG_MSVC
#define _CRTDBG_MAP_ALLOC
#endif

// common headers
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

// MSVC: use the CRT's heap debugging facility
#if UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG_MSVC
#include <crtdbg.h>
#undef realloc
#undef strdup
#endif

// MSVC-specific headers
#if UTL_CC == UTL_CC_MSVC
#include <io.h>
#endif

// UNIX-specific headers
#if (UTL_HOST_TYPE == UTL_HT_UNIX) || (UTL_CC == UTL_CC_MINGW)
#include <unistd.h>
#endif

// STL headers
#include <algorithm>
#include <atomic>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

// Unix-like system headers
#if UTL_HOST_TYPE == UTL_HT_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

// MSVC helper macros
#if UTL_CC == UTL_CC_MSVC
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

// namespace macros
#define UTL_NS_DECLARE                                                                             \
    namespace utl                                                                                  \
    {                                                                                              \
    }
#define UTL_NS_BEGIN                                                                               \
    namespace utl                                                                                  \
    {
#define UTL_NS_END }
#define UTL_NS_USE using namespace utl;

/// UTL++ namespace
UTL_NS_DECLARE;

// libutl headers
#include <libutl/types.h>
#include <libutl/macros.h>
#include <libutl/ScopeGuard.h>
#include <libutl/util.h>
#include <libutl/gblnew.h>
#include <libutl/RunTimeClass.h>
#include <libutl/Object.h>
#include <libutl/util_inl.h>
