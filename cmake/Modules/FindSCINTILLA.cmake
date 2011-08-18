# - Try to find SCINTILLA
# Once done this will define
#  SCINTILLA_FOUND - System has SCINTILLA
#  SCINTILLA_INCLUDE_DIRS - The SCINTILLA include directories
#  SCINTILLA_LIBRARIES - The libraries needed to use SCINTILLA
#  SCINTILLA_DEFINITIONS - Compiler switches required for using SCINTILLA

find_path(SCINTILLA_INCLUDE_DIR include/scintilla.h
          HINTS ${SCINTILLA_ROOT} )

set(SCINTILLA_INCLUDE_DIRS ${SCINTILLA_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SCINTILLA_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SCINTILLA DEFAULT_MSG
                                  SCINTILLA_INCLUDE_DIR)

mark_as_advanced( SCINTILLA_INCLUDE_DIR )
