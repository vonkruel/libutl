#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

// #include the host system's threads & synchronization headers
#if UTL_HOST_OS == UTL_OS_WINDOWS
#include <libutl/win32api.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_BEGIN;

////////////////////////////////////////////////////////////////////////////////////////////////////

#if UTL_HOST_OS == UTL_OS_WINDOWS
using thread_handle_t = HANDLE;
#else
using thread_handle_t = pthread_t;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

UTL_NS_END;
