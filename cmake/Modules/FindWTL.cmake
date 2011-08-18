# - Try to find WTL
# Once done this will define
#  WTL_FOUND - System has WTL
#  WTL_INCLUDE_DIRS - The WTL include directories
#  WTL_LIBRARIES - The libraries needed to use WTL
#  WTL_DEFINITIONS - Compiler switches required for using WTL

find_path(WTL_INCLUDE_DIR include/atlapp.h
          HINTS ${WTL_ROOT} )

set(WTL_INCLUDE_DIRS ${WTL_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set WTL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(WTL DEFAULT_MSG
                                  WTL_INCLUDE_DIR)

mark_as_advanced( WTL_INCLUDE_DIR )
