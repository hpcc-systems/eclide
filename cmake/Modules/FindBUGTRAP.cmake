# - Try to find BUGTRAP
# Once done this will define
#  BUGTRAP_FOUND - System has BUGTRAP
#  BUGTRAP_INCLUDE_DIRS - The BUGTRAP include directories
#  BUGTRAP_LIBRARIES - The libraries needed to use BUGTRAP
#  BUGTRAP_DEFINITIONS - Compiler switches required for using BUGTRAP

find_path(BUGTRAP_INCLUDE_DIR bugtrap.h
          HINTS ${BUGTRAP_ROOT} )

set(BUGTRAP_INCLUDE_DIRS ${BUGTRAP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set BUGTRAP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(BUGTRAP DEFAULT_MSG
                                  BUGTRAP_INCLUDE_DIR)

mark_as_advanced( BUGTRAP_INCLUDE_DIR )
