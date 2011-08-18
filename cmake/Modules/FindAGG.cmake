# - Try to find AGG
# Once done this will define
#  AGG_FOUND - System has AGG
#  AGG_INCLUDE_DIRS - The AGG include directories
#  AGG_LIBRARIES - The libraries needed to use AGG
#  AGG_DEFINITIONS - Compiler switches required for using AGG

find_path(AGG_INCLUDE_DIR include/agg_config.h
          HINTS ${AGG_ROOT} )

set(AGG_INCLUDE_DIRS ${AGG_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set AGG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(AGG DEFAULT_MSG
                                  AGG_INCLUDE_DIR)

mark_as_advanced( AGG_INCLUDE_DIR )
