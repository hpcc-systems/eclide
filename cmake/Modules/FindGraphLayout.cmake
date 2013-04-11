# - Try to find GRAPHLAYOUT
# Once done this will define
#  GRAPHLAYOUT_FOUND - System has GRAPHLAYOUT
#  GRAPHLAYOUT_INCLUDE_DIRS - The GRAPHLAYOUT include directories
#  GRAPHLAYOUT_LIBRARIES - The libraries needed to use GRAPHLAYOUT
#  GRAPHLAYOUT_DEFINITIONS - Compiler switches required for using GRAPHLAYOUT

find_path(GRAPHLAYOUT_INCLUDE_DIR config.h
    HINTS ${GRAPHLAYOUT_ROOT}/GraphLayout )

find_library(GRAPHLAYOUT_LIBRARY NAMES npHPCCSystemsGraphViewControl
    HINTS ${CMAKE_CURRENT_BINARY_DIR}/../GraphControl/bin/HPCCSystemsGraphViewControl/RelWithDebInfo ${CMAKE_CURRENT_BINARY_DIR}/../GraphControl/bin/HPCCSystemsGraphViewControl/Release) 

find_file(GRAPHLAYOUT_DLL npHPCCSystemsGraphViewControl.dll
    HINTS ${CMAKE_CURRENT_BINARY_DIR}/../GraphControl/bin/HPCCSystemsGraphViewControl/RelWithDebInfo ${CMAKE_CURRENT_BINARY_DIR}/../GraphControl/bin/HPCCSystemsGraphViewControl/Release)

set(GRAPHLAYOUT_INCLUDE_DIRS ${GRAPHLAYOUT_INCLUDE_DIR} )
set(GRAPHLAYOUT_LIBRARIES ${GRAPHLAYOUT_LIBRARY} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GRAPHLAYOUT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GRAPHLAYOUT DEFAULT_MSG
                                  GRAPHLAYOUT_LIBRARIES GRAPHLAYOUT_INCLUDE_DIRS)

mark_as_advanced( GRAPHLAYOUT_INCLUDE_DIR )
