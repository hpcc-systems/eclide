project ( ECLIDE )
cmake_minimum_required (VERSION 2.8)

set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

IF ( WIN32 )
    SET ( EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Location of BIN files" )
    SET ( LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Location of LIB files" )
ELSE ( WIN32 )
    SET ( EXECUTABLE_OUTPUT_PATH "/usr/lib/mozilla/plugins" )
    SET ( LIBRARY_OUTPUT_PATH "/usr/lib/mozilla/plugins" )
ENDIF ( WIN32 )

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
    				PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
    					OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
    				PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
    					OBJECT_DEPENDS "${PrecompiledBinary}") 
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
    LIST(APPEND ${SourcesVar} ${PrecompiledHeader})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

include ( "${CMAKE_SOURCE_DIR}/cmake/Modules/Externals.cmake" )

SET ( TARGET_EE "${TARGET_EE}" CACHE BOOL "Target EE Build" )
if ( TARGET_EE )
    MESSAGE( "Targeting EE")
    ADD_DEFINITIONS( -D_TARGET_EE )
endif ( TARGET_EE )

if (WIN32)
    ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE )
	SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /OPT:ICF")
	SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /OPT:ICF")
	SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /OPT:ICF")
ELSE (WIN32)

ENDIF (WIN32)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

ADD_SUBDIRECTORY ( BugTrap )
set_property(TARGET BUGTRAP PROPERTY FOLDER "3rdParty")
ADD_SUBDIRECTORY ( zlib )
set_property(TARGET ZLIB PROPERTY FOLDER "3rdParty")

ADD_SUBDIRECTORY ( clib )
ADD_SUBDIRECTORY ( comms )
ADD_SUBDIRECTORY ( EclEditor )
ADD_SUBDIRECTORY ( EclLib )
ADD_SUBDIRECTORY ( en_us )
ADD_SUBDIRECTORY ( GraphViewCtl )
ADD_SUBDIRECTORY ( sgrid )
ADD_SUBDIRECTORY ( wlib )
ADD_SUBDIRECTORY ( eclide )
if ( TARGET_EE )
ADD_SUBDIRECTORY ( amt )
endif ( TARGET_EE )
