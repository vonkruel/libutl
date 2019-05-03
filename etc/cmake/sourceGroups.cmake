# sourceGroups(SRCS)
#   Create source groups based on sourcedir-relative paths of source files.
function(sourceGroups SRCS)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  foreach(FILE ${SRCS}) 
    # get absolute path to source file
    get_filename_component(ABSOLUTE_PATH "${FILE}" ABSOLUTE)
    # get absolute path to source file's parent directory
    get_filename_component(PARENT_DIR "${ABSOLUTE_PATH}" DIRECTORY)
    # remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    # group into "Source Files\<subdir>"
    set(GROUP "Source Files${GROUP}")
    # group into "Source Files" and "Header Files"
    #if ("${FILE}" MATCHES ".*\\.cpp")
    #  set(GROUP "Source Files${GROUP}")
    #elseif("${FILE}" MATCHES ".*\\.h")
    #  set(GROUP "Header Files${GROUP}")
    #endif()
    #message("source_group(${GROUP} FILES ${FILE})")
    source_group("${GROUP}" FILES "${FILE}")
  endforeach()
endfunction()
