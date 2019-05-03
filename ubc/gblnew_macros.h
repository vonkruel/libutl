#undef new
#if UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG
#define new new (__FILE__, __LINE__)
#elif (UTL_GBLNEW_MODE == UTL_GBLNEW_MODE_DEBUG_MSVC)
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
