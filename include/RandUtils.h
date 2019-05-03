#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

// a hack for randutils on MinGW where std::this_thread may be absent (impl @ ubc/util.cpp)
#if UTL_CC == UTL_CC_MINGW
namespace std
{
struct this_thread
{
    static size_t get_id();
};
} // namespace std
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#undef new
#include <libutl/randutils.hpp>
#include <libutl/gblnew_macros.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace randutils
{
using mt19937_32_rng = randutils::random_generator<std::mt19937>;
using mt19937_64_rng = randutils::random_generator<std::mt19937_64>;
} // namespace randutils
