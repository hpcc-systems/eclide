# - Try to find ZLIB
# Once done this will define
#  ZLIB_FOUND - System has ZLIB
#  ZLIB_INCLUDE_DIRS - The ZLIB include directories
#  ZLIB_LIBRARIES - The libraries needed to use ZLIB
#  ZLIB_DEFINITIONS - Compiler switches required for using ZLIB

find_path(ZLIB_INCLUDE_DIR include/zlib.h
          HINTS ${ZLIB_ROOT} )

set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ZLIB DEFAULT_MSG
                                  ZLIB_INCLUDE_DIR)

mark_as_advanced( ZLIB_INCLUDE_DIR )
