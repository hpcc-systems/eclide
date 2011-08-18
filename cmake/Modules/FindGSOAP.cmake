# - Try to find GSOAP
# Once done this will define
#  GSOAP_FOUND - System has GSOAP
#  GSOAP_INCLUDE_DIRS - The GSOAP include directories
#  GSOAP_LIBRARIES - The libraries needed to use GSOAP
#  GSOAP_DEFINITIONS - Compiler switches required for using GSOAP

find_path(GSOAP_INCLUDE_DIR stdsoap2.h
          HINTS ${GSOAP_ROOT} )

set(GSOAP_INCLUDE_DIRS ${GSOAP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GSOAP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GSOAP DEFAULT_MSG
                                  GSOAP_INCLUDE_DIR)

mark_as_advanced( GSOAP_INCLUDE_DIR )
