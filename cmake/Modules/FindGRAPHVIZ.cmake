# - Try to find GRAPHVIZ
# Once done this will define
#  GRAPHVIZ_FOUND - System has GRAPHVIZ
#  GRAPHVIZ_INCLUDE_DIRS - The GRAPHVIZ include directories
#  GRAPHVIZ_LIBRARIES - The libraries needed to use GRAPHVIZ
#  GRAPHVIZ_DEFINITIONS - Compiler switches required for using GRAPHVIZ

find_path(GRAPHVIZ_INCLUDE_DIR config.h
          HINTS ${GRAPHVIZ_ROOT} )

set(GRAPHVIZ_INCLUDE_DIRS ${GRAPHVIZ_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GRAPHVIZ_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GRAPHVIZ DEFAULT_MSG
                                  GRAPHVIZ_INCLUDE_DIR)

mark_as_advanced( GRAPHVIZ_INCLUDE_DIR )
