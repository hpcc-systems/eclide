IF ( "${EXTERNALS_DIRECTORY}" STREQUAL "" )
    SET ( EXTERNALS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../svn_HPCC/dev/Third_party"
          CACHE PATH "SVN Third Party Folders" )
ENDIF ()

set ( AGG_ROOT "${EXTERNALS_DIRECTORY}/agg" )
find_package( AGG )
if ( AGG_FOUND )
else ( )
    MESSAGE( "AGG not found:  " ${AGG_ROOT})
endif ( )

set ( BUGTRAP_ROOT "${EXTERNALS_DIRECTORY}/bugtrap/win32/bugtrap" )
find_package( BUGTRAP )
if ( BUGTRAP_FOUND )
else ( )
    MESSAGE( "BUGTRAP not found:  " ${BUGTRAP_ROOT})
endif ( )

set ( GSOAP_ROOT "${EXTERNALS_DIRECTORY}/gsoap" )
find_package( GSOAP )
if ( GSOAP_FOUND )
else ( )
    MESSAGE( "GSOAP not found:  " ${GSOAP_ROOT})
endif ( )

set ( SCINTILLA_ROOT "${CMAKE_SOURCE_DIR}/third_party/scintilla" )
find_package( SCINTILLA )
if ( SCINTILLA_FOUND )
else ( )
    MESSAGE( "SCINTILLA not found:  " ${SCINTILLA_ROOT})
endif ( )

set ( WTL_ROOT "${EXTERNALS_DIRECTORY}/wtl" )
find_package( WTL )
if ( WTL_FOUND )
else ( )
    MESSAGE( "WTL libraries were not found:  " ${WTL_ROOT})
endif ( )

set ( ZLIB_ROOT "${EXTERNALS_DIRECTORY}/bugtrap/win32/zlib" )
find_package( ZLIB )
if ( ZLIB_FOUND )
else ( )
    MESSAGE( "ZLIB not found:  " ${ZLIB_ROOT})
endif ( )

SET( Boost_USE_STATIC_LIBS ON )
FIND_PACKAGE( Boost REQUIRED COMPONENTS system filesystem thread)
IF( Boost_FOUND )
ELSE( Boost_FOUND )
    MESSAGE( "BOOST libraries were not found:  " ${BOOST_ROOT})
ENDIF( Boost_FOUND )

if (WIN32)
    SET ( FREETYPE_INCLUDE_DIR_freetype2 ${EXTERNALS_DIRECTORY}/freetype2/include )
    SET ( FREETYPE_INCLUDE_DIR_ft2build ${EXTERNALS_DIRECTORY}/freetype2/include )
    SET ( FREETYPE_LIBRARY ${EXTERNALS_DIRECTORY}/freetype2/objs/win32/vc2008/freetype239MT.lib )
else (WIN32)
endif (WIN32)
FIND_PACKAGE( Freetype )
IF( FREETYPE_FOUND )
ELSE( FREETYPE_FOUND )
    MESSAGE( "Freetype library was not found" )
ENDIF( FREETYPE_FOUND )