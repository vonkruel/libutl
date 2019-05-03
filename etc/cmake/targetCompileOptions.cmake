# compiler options for MSVC
if (MSVC)
    set(COMPILE_OPTIONS_COMMON /W1
                               /std:c++17
                               /permissive-
                               /MP
                               /D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1
                               /D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1
                               /D_CRT_SECURE_NO_DEPRECATE
                               /D_CRT_NONSTDC_NO_DEPRECATE
                               /DUTL_CC=UTL_CC_MSVC
                               /DUTL_HOST_OS=UTL_OS_WINDOWS)
    set(COMPILE_OPTIONS_DEBUG /DDEBUG)
    set(COMPILE_OPTIONS_RELEASE /DRELEASE)
else()
    set(COMPILE_OPTIONS_COMMON -Wall
                               -Wno-strict-aliasing
                               -Wno-uninitialized
                               -std=gnu++17
                               -fstrict-aliasing
                               -DUTL_CC=UTL_CC_GCC
                               -DUTL_HOST_OS=UTL_OS_LINUX)
    set(COMPILE_OPTIONS_DEBUG -DDEBUG)
    set(COMPILE_OPTIONS_RELEASE -DRELEASE)
endif()

# targetCompileOptions(TARGET)
#   Set project-wide compile options for a target.
function(targetCompileOptions TARGET)
  target_compile_options(${TARGET} PUBLIC ${COMPILE_OPTIONS_COMMON})
  target_compile_options(${TARGET} PUBLIC "$<$<CONFIG:DEBUG>:${COMPILE_OPTIONS_DEBUG}>")
  target_compile_options(${TARGET} PUBLIC "$<$<CONFIG:RELEASE>:${COMPILE_OPTIONS_RELEASE}>")
endfunction()
