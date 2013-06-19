project ( ECLIDE )
cmake_minimum_required (VERSION 2.8)

set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

##################################################################

# Build tag generation

INCLUDE (  "${PROJECT_SOURCE_DIR}/version.cmake" )

##################################################################

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

if (WIN32)
	string(REGEX REPLACE "/Zm[0-9]+ *" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
	ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE )
	SET(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /OPT:ICF")
	SET(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /OPT:ICF")
	SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /OPT:ICF")
ELSE (WIN32)

ENDIF (WIN32)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

ADD_SUBDIRECTORY ( BugTrap )
set_property(TARGET BUGTRAP PROPERTY FOLDER "3rdParty")

ADD_SUBDIRECTORY ( clib )
ADD_SUBDIRECTORY ( comms )
ADD_SUBDIRECTORY ( EclEditor )
ADD_SUBDIRECTORY ( EclLib )
ADD_SUBDIRECTORY ( en_us )
ADD_SUBDIRECTORY ( GraphViewCtl )
ADD_SUBDIRECTORY ( sgrid )
ADD_SUBDIRECTORY ( wlib )
ADD_SUBDIRECTORY ( eclide )
ADD_SUBDIRECTORY ( amt )
set ( DOC_FILES 
    ${PROJECT_SOURCE_DIR}/docs/ECLReference.chm
    ${PROJECT_SOURCE_DIR}/docs/LanguageColor.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageReference.xml
  )
install ( FILES ${DOC_FILES} DESTINATION bin )
Install ( FILES ${PROJECT_SOURCE_DIR}/License.txt DESTINATION "." )

##############################################
# Build tag generation

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set ( ARCH64BIT 1 )
else ()
    set ( ARCH64BIT 0 )
endif ()
message ("-- 64bit architecture is ${ARCH64BIT}")

set(projname ${HPCC_PROJECT})
set(majorver ${HPCC_MAJOR})
set(minorver ${HPCC_MINOR})
set(point ${HPCC_POINT})
if ( "${HPCC_MATURITY}" STREQUAL "release" )
    set(stagever "${HPCC_SEQUENCE}")
else()
    set(stagever "${HPCC_MATURITY}${HPCC_SEQUENCE}")
endif()
set(version ${majorver}.${minorver}.${point})

IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set( stagever "${stagever}-Debug" )
ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
##############################################

####  CPACK  ####
set ( CMAKE_INSTALL_MFC_LIBRARIES TRUE )
set ( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE )
include ( InstallRequiredSystemLibraries )
foreach ( CMAKE_INSTALL_SYSTEM_RUNTIME_LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} )
    ##  Skip non unicode version of MFC libs (big)  ##
    string(REGEX MATCH ".*mfc.*0\\.dll" SKIP_FILE "${CMAKE_INSTALL_SYSTEM_RUNTIME_LIB}" )
    if ( SKIP_FILE )
        message("Skipping non unicode MFC install:  ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIB}")
    else ( SKIP_FILE )
        install ( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIB} DESTINATION bin )
    endif ( SKIP_FILE)
endforeach ( CMAKE_INSTALL_SYSTEM_RUNTIME_LIB )

set ( CPACK_PACKAGE_VENDOR "HPCC Systems" )
set ( CPACK_PACKAGE_NAME "hpccsystems-eclide" )
set ( CPACK_PACKAGE_VERSION_MAJOR ${majorver})
set ( CPACK_PACKAGE_VERSION_MINOR ${minorver})
set ( CPACK_PACKAGE_VERSION_PATCH ${point}${stagever})
set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "HPCC Systems ECL IDE." )

set ( CPACK_PACKAGE_CONTACT "HPCCSystems <ossdevelopment@lexisnexis.com>" )

set ( CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/License.txt" )
set ( CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/CPackReadMe.txt" )

set ( CPACK_RPM_PACKAGE_VERSION "${projname}" )
if ( ${ARCH64BIT} EQUAL 1 )
    set ( CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
else( ${ARCH64BIT} EQUAL 1 )
    set ( CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
endif ( ${ARCH64BIT} EQUAL 1 )
set ( CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CPACK_RPM_PACKAGE_ARCHITECTURE}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set ( CPACK_STRIP_FILES TRUE)
endif()
set ( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_RPM_PACKAGE_VERSION}-${version}-${stagever}${CPACK_SYSTEM_NAME}" )
MESSAGE ("-- Current release version is ${CPACK_PACKAGE_FILE_NAME}")

set(CLIENTTOOLS_PACKAGE_FILE_NAME "hpccsystems-clienttools_${CPACK_RPM_PACKAGE_VERSION}-${version}-${stagever}${CPACK_SYSTEM_NAME}.exe")
find_file(CLIENTTOOLS_PACKAGE_FILE ${CLIENTTOOLS_PACKAGE_FILE_NAME} HINTS ${CMAKE_CURRENT_BINARY_DIR}/../HPCC-Platform )
install ( PROGRAMS ${CLIENTTOOLS_PACKAGE_FILE} DESTINATION tmp )
list ( APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " 
    ExecWait '$INSTDIR\\\\tmp\\\\${CLIENTTOOLS_PACKAGE_FILE_NAME} /S /D=$INSTDIR\\\\..\\\\clienttools'
")

###
## Run file configuration to set build tag along with install lines for generated
## config files.
###
set( BUILD_TAG "${CPACK_RPM_PACKAGE_VERSION}_${version}-${stagever}")
if (USE_GIT_DESCRIBE OR CHECK_GIT_TAG)
    FETCH_GIT_TAG (${CMAKE_SOURCE_DIR} ${CPACK_RPM_PACKAGE_VERSION}_${version} GIT_BUILD_TAG)
    message ("-- Git tag is '${GIT_BUILD_TAG}'")
    if (NOT "${GIT_BUILD_TAG}" STREQUAL "${BUILD_TAG}")
        if (CHECK_GIT_TAG)
            message(FATAL_ERROR "Git tag '${GIT_BUILD_TAG}' does not match source version '${BUILD_TAG}'" )
        else()
            if(NOT "${GIT_BUILD_TAG}" STREQUAL "") # probably means being built from a tarball...
                set( BUILD_TAG "${BUILD_TAG}[${GIT_BUILD_TAG}]")
            endif()
        endif()
    endif()
endif()
message ("-- Build tag is '${BUILD_TAG}'")

if ( WIN32 )
    set( DIR_NAME "HPCCSystems" )
    set( version ${HPCC_MAJOR}.${HPCC_MINOR}.${HPCC_POINT} )
    set ( CPACK_PACKAGE_INSTALL_DIRECTORY "${DIR_NAME}\\\\${version}\\\\eclide" )
    set ( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "eclide_${version}" )
    set ( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
    set ( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\hpccsystems.com" )
    set ( CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\eclide.exe" )
    set ( CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT} )
    set ( CPACK_PACKAGE_INSTALL_PLUGINFILE "bin\\\\npHPCCSystemsGraphViewControl.dll")
    if ( "${FB_PLATFORM_ARCH_NAME}" EQUAL "x86_64" )
        SET ( CPACK_NSIS_DISPLAY_NAME "ECL IDE x64" )
        list ( APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " 
            ExecWait '\\\"$SYSDIR\\\\regsvr32.exe\\\" /s \\\"$INSTDIR\\\\${CPACK_PACKAGE_INSTALL_PLUGINFILE}\\\"'
        " )
        list ( APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "  
            ExecWait '\\\"$SYSDIR\\\\regsvr32.exe\\\" /s /u \\\"$INSTDIR\\\\${CPACK_PACKAGE_INSTALL_PLUGINFILE}\\\"'
        " )
    else ()
        SET ( CPACK_NSIS_DISPLAY_NAME "ECL IDE" )
        list ( APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " 
            RegDLL '$INSTDIR\\\\${CPACK_PACKAGE_INSTALL_PLUGINFILE}'
        " )
        list ( APPEND CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "  
            UnRegDLL '$INSTDIR\\\\${CPACK_PACKAGE_INSTALL_PLUGINFILE}'
        " )
    endif ()
    list ( APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "  
        CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ECL IDE.lnk\\\" \\\"$INSTDIR\\\\bin\\\\eclide.exe\\\"
        CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ECL Reference Help.lnk\\\" \\\"\$INSTDIR\\\\bin\\\\ECLReference.chm\\\"
    ")
    list ( APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "  
        Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\ECL IDE.lnk\\\"
        Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\ECL Reference Help.lnk\\\"
    ")
    set ( CPACK_NSIS_DEFINES "
        !define MUI_STARTMENUPAGE_DEFAULTFOLDER \\\"${CPACK_PACKAGE_VENDOR}\\\\${version}\\\\${CPACK_NSIS_DISPLAY_NAME}\\\"
        !define MUI_FINISHPAGE_NOAUTOCLOSE
    ")

    file(STRINGS "${PROJECT_SOURCE_DIR}/../sign/passphrase.txt" PFX_PASSWORD LIMIT_COUNT 1)

    add_custom_target(SIGN 
        COMMAND signtool sign /f "${PROJECT_SOURCE_DIR}/../sign/hpcc_code_signing.pfx" /p "${PFX_PASSWORD}" /t "http://timestamp.verisign.com/scripts/timstamp.dll" "${CMAKE_BINARY_DIR}/${CPACK_PACKAGE_NAME}*.exe"
        COMMENT "Digital Signature"
    )
    add_dependencies(SIGN PACKAGE)
    set_property(TARGET SIGN PROPERTY FOLDER "CMakePredefinedTargets")
endif ( )

include ( CPack )
####  #####  ####
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )

configure_file (
  "${PROJECT_SOURCE_DIR}/doStageECLIDE.bat.in"
  "${PROJECT_SOURCE_DIR}/doStageECLIDE.bat"
  )

configure_file (
  "${PROJECT_SOURCE_DIR}/setup/defines.wxi.in"
  "${PROJECT_SOURCE_DIR}/setup/defines.wxi"
  )

