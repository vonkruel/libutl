#pragma once

#if UTL_HOST_OS == UTL_OS_LINUX
#include <libutl/NetServer_linux.h>
#endif

#if UTL_HOST_OS == UTL_OS_WINDOWS
#include <libutl/NetServer_windows.h>
#endif
