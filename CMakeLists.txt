cmake_minimum_required(VERSION 3.15)
set(CMAKE_VERBOSE_MAKEFILE OFF)

#### libutl #######################################################################################

# set project details
project(libutl VERSION 1.0
        DESCRIPTION "libUTL++"
        LANGUAGES CXX)

# project version number
set(LIBUTL_MAJOR_VERSION 1)
set(LIBUTL_MINOR_VERSION 0)
set(LIBUTL_PATCH_VERSION 0)
set(LIBUTL_VERSION ${LIBUTL_MAJOR_VERSION}.${LIBUTL_MINOR_VERSION}.${LIBUTL_PATCH_VERSION})

# let CMake find our modules in <source-dir>/etc/cmake/
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/etc/cmake")

# ${LIBUTL_CPPS}: our source files
file(GLOB LIBUTL_CPPS RELATIVE ${CMAKE_SOURCE_DIR} "u??/*.cpp")

# ${LIBUTL_HDRS}: our header files
file(GLOB LIBUTL_HDRS_INCLUDE RELATIVE ${CMAKE_SOURCE_DIR} "include/*.h")
list(APPEND LIBUTL_HDRS_INCLUDE "include/randutils.hpp")
file(GLOB LIBUTL_HDRS_UXX RELATIVE ${CMAKE_SOURCE_DIR} "u??/*.h")
set(LIBUTL_HDRS ${LIBUTL_HDRS_INCLUDE} ${LIBUTL_HDRS_UXX})

# ${LIBUTL_CPPS_AND_HDRS}: our headers and source files
set(LIBUTL_CPPS_AND_HDRS ${LIBUTL_CPPS} ${LIBUTL_HDRS})

# source groupings for Visual Studio
include(sourceGroups)
sourceGroups("${LIBUTL_CPPS_AND_HDRS}") # groupings for Visual Studio

#### libutl-objs ##################################################################################

# an object library (to avoid compiling everything twice (w/ and w/o PIC))
add_library(libutl-objs OBJECT ${LIBUTL_CPPS})

# sources
target_sources(libutl-objs PRIVATE ${LIBUTL_CPPS} ${LIBUTL_HDRS})

# we require PIC for the shared library
set_property(TARGET libutl-objs PROPERTY POSITION_INDEPENDENT_CODE 1)

# required libraries for each platform
if (WIN32)
  target_link_libraries(libutl-objs INTERFACE ws2_32)
  set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /IGNORE:4221")
else()
  target_link_libraries(libutl-objs INTERFACE pthread ssl crypto)
endif()

# compile options
include(targetCompileOptions)
targetCompileOptions(libutl-objs)

# include directories
target_include_directories(libutl-objs PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include>)

#### libutl_shared ################################################################################

# a shared library built from libutl-objs
add_library(libutl_shared SHARED)
target_link_libraries(libutl_shared PUBLIC libutl-objs)
set_property(TARGET libutl_shared PROPERTY PREFIX "")
set_property(TARGET libutl_shared PROPERTY DEBUG_POSTFIX _g)

#### libutl_static ################################################################################

# a static library built from libutl-objs
add_library(libutl_static STATIC)
target_link_libraries(libutl_static PUBLIC libutl-objs)
set_property(TARGET libutl_static PROPERTY PREFIX "")
set_property(TARGET libutl_static PROPERTY DEBUG_POSTFIX _g)

# headers (developers need them)
set_property(TARGET libutl_static PROPERTY PUBLIC_HEADER "${LIBUTL_HDRS}")

# downstream must link in the whole archive
if (MSVC)
  target_link_options(libutl_static INTERFACE
                      -WHOLEARCHIVE:$<TARGET_LINKER_FILE_NAME:libutl::libutl_static>)
else()
  target_link_options(libutl_static INTERFACE
                      -Wl,--whole-archive,$<TARGET_LINKER_FILE:libutl::libutl_static>
		      -Wl,--no-whole-archive)
endif()

#### BUILD ########################################################################################

# enable writing to the user package registry
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)

# register the build tree with the global CMake registry
export(PACKAGE libutl)

# build/<config>/libutl-config.cmake (config for BUILD)
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(etc/cmake/libutl-config.cmake.in
               "${PROJECT_BINARY_DIR}/libutl-config.cmake"
               @ONLY)

# build/<config>/libutl-config-version.cmake (config-version for BUILD)
configure_file(etc/cmake/libutl-config-version.cmake.in
               "${PROJECT_BINARY_DIR}/libutl-config-version.cmake"
               @ONLY)

# build/<config>/libutl-targets.cmake (exported library targets)
export(TARGETS libutl-objs libutl_shared libutl_static
       NAMESPACE libutl::
       FILE "${PROJECT_BINARY_DIR}/libutl-targets.cmake")

#### INSTALL ######################################################################################

# ${INSTALL_LIB_DIR}: <install-prefix>/lib/
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")

# ${INSTALL_INCLUDE_DIR}: <install-prefix>/include/
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

# ${INSTALL_CMAKE_DIR}: <install-prefix>/cmake           (MSVC)
#                       <install-prefix>/lib/cmake/acms  (g++)
if (WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/libutl)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# ${INSTALL_*_DIR}: all absolute paths
foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# build/CMakeFiles/libutl-config.cmake (config file for INSTALL)
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "\${LIBUTL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(etc/cmake/libutl-config.cmake.in
               "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libutl-config.cmake"
               @ONLY)

# install libraries to ${INSTALL_LIB_DIR}
# install headers to ${INSTALL_INCLUDE_DIR}/libutl
install(TARGETS       libutl-objs libutl_shared libutl_static
        EXPORT        libutl-targets
        LIBRARY       DESTINATION "${INSTALL_LIB_DIR}"            COMPONENT shlib
        RUNTIME       DESTINATION "${INSTALL_LIB_DIR}"            COMPONENT shlib
        ARCHIVE       DESTINATION "${INSTALL_LIB_DIR}"            COMPONENT dev
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/libutl" COMPONENT dev)

# install: build/libutl-config-version.cmake
#          build/CMakeFiles/libutl-config.cmake
#          ------------------------------------
#    ----> lib/cmake/libutl
install(FILES
        "${PROJECT_BINARY_DIR}/libutl-config-version.cmake"
        "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libutl-config.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}"
        COMPONENT dev)

# install: libutl-targets export file
#    ----> lib/CMake/libutl
install(EXPORT libutl-targets
        FILE libutl-targets.cmake
        NAMESPACE libutl::
        DESTINATION "${INSTALL_CMAKE_DIR}"
        COMPONENT dev)
