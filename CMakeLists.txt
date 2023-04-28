# CMakeLists.txt  ---
CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

set(VCPKG_FILES_DIR "${CMAKE_BINARY_DIR}" CACHE STRING "Folder for vcpkg download, build and installed files")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg)
set(VCPKG_OVERLAY_PORTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_overlays)
set(VCPKG_INSTALLED_DIR "${VCPKG_FILES_DIR}/vcpkg_installed")
set(VCPKG_INSTALL_OPTIONS "--downloads-root=${VCPKG_FILES_DIR}/vcpkg_downloads;--x-buildtrees-root=${VCPKG_FILES_DIR}/vcpkg_buildtrees;--x-packages-root=${VCPKG_FILES_DIR}/vcpkg_packages")
set(VCPKG_VERBOSE OFF)

PROJECT(ECLIDE)

# Build tag generation ---
INCLUDE("${PROJECT_SOURCE_DIR}/version.cmake")

# Install Targets ---
SET (EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Location of BIN files")
SET (LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Location of LIB files")

# Third Party Libraries ---
SET(CMAKE_INSTALL_MFC_LIBRARIES TRUE)
FIND_PACKAGE(Boost REQUIRED COMPONENTS date_time filesystem program_options serialization signals thread)
FIND_PATH(WTL_INCLUDE_DIR wtl/atlapp.h)
FIND_PATH(VCPKG_ROOT_DIR include/wtl/atlapp.h)
SET(GSOAP_DIR ${VCPKG_ROOT_DIR}/share/gsoap)
FIND_PATH(SCINTILLA_INCLUDE_DIR Platform.h HINTS ${VCPKG_ROOT_DIR}/include/scintilla/include)
FIND_PATH(SCINTILLA_SOURCE_DIR AutoComplete.cxx HINTS ${VCPKG_ROOT_DIR}/share/scintilla/src)
FIND_PATH(SCINTILLA_LEXLIB_DIR Accessor.cxx HINTS ${VCPKG_ROOT_DIR}/share/scintilla/LexLib)
FIND_PATH(SCINTILLA_WIN32_DIR PlatWin.cxx HINTS ${VCPKG_ROOT_DIR}/share/scintilla/Win32)
FIND_PATH(BUGTRAP_DIR BugTrap.h HINTS ${VCPKG_ROOT_DIR}/share/bugtrap/source/Client)
FIND_PATH(BUGTRAP_ZLIB_DIR zlib.def HINTS ${VCPKG_ROOT_DIR}/share/bugtrap/source/zlib)

FILE(GLOB CEF_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/*")
FIND_PATH(CEF_ROOT_DIR
    NAMES Release/libcef.lib
    PATHS ${CEF_SEARCH_PATHS}
)

MESSAGE("includes")
MESSAGE("  - boost:  ${Boost_INCLUDE_DIRS}")
MESSAGE("  - wtl:  ${WTL_INCLUDE_DIR}")
MESSAGE("  - Scintilla:  ${SCINTILLA_INCLUDE_DIR}")
MESSAGE("  - CEF:  ${CEF_ROOT_DIR}")

MESSAGE("sources")
MESSAGE("  - Scintilla")
MESSAGE("    - src:  ${SCINTILLA_SOURCE_DIR}")
MESSAGE("    - LexLib:  ${SCINTILLA_LEXLIB_DIR}")
MESSAGE("    - Win32:  ${SCINTILLA_WIN32_DIR}")
MESSAGE("  - gsoap")
MESSAGE("    - dir:  ${GSOAP_DIR}")
MESSAGE("  - bugtrap")
MESSAGE("    - src:  ${BUGTRAP_DIR}")
MESSAGE("    - zlib:  ${BUGTRAP_ZLIB_DIR}")

# Compiler Flags  ---
ADD_DEFINITIONS(-DBOOST_SIGNALS_NO_DEPRECATION_WARNING -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE)

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
    IF(MSVC)
        GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
        SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
        SET(Sources ${${SourcesVar}})

        SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
            PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
            OBJECT_OUTPUTS "${PrecompiledBinary}"
        )
        SET_SOURCE_FILES_PROPERTIES(${Sources}
            PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
            OBJECT_DEPENDS "${PrecompiledBinary}"
        )
        # Add precompiled header to SourcesVar
        LIST(APPEND ${SourcesVar} ${PrecompiledSource})
        LIST(APPEND ${SourcesVar} ${PrecompiledHeader})

        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /FI\"${PrecompiledHeader}\"")
    ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

# Projects  ---
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ADD_SUBDIRECTORY (BugTrap)
SET_PROPERTY(TARGET BUGTRAP PROPERTY FOLDER "3rdParty")
ADD_SUBDIRECTORY (en_us)
ADD_SUBDIRECTORY (clib)
ADD_SUBDIRECTORY (EclLib)
ADD_SUBDIRECTORY (sgrid)
ADD_SUBDIRECTORY (EclEditor)
ADD_SUBDIRECTORY (comms)
ADD_SUBDIRECTORY (wlib)
ADD_SUBDIRECTORY (eclide)
ADD_SUBDIRECTORY (amt)

# Documents  ---
IF(EXISTS "${PROJECT_SOURCE_DIR}/docs/ECLReference.chm")
    INSTALL (PROGRAMS ${PROJECT_SOURCE_DIR}/docs/ECLReference.chm DESTINATION bin)
ELSE ()
    MESSAGE("Skipping ${PROJECT_SOURCE_DIR}/docs/ECLReference.chm (file does not exist)")
ENDIF ()

IF(EXISTS "${PROJECT_SOURCE_DIR}/docs/ECLReference_PT_BR.chm")
    INSTALL (PROGRAMS ${PROJECT_SOURCE_DIR}/docs/ECLReference_PT_BR.chm DESTINATION bin)
ELSE ()
    MESSAGE("Skipping ${PROJECT_SOURCE_DIR}/docs/ECLReference_PT_BR.chm (file does not exist)")
ENDIF ()

SET(DOC_FILES
    ${PROJECT_SOURCE_DIR}/docs/LangColorDUD.xml
    ${PROJECT_SOURCE_DIR}/docs/LangColorECL.xml
    ${PROJECT_SOURCE_DIR}/docs/LangColorESDL.xml
    ${PROJECT_SOURCE_DIR}/docs/LangColorKEL.xml
    ${PROJECT_SOURCE_DIR}/docs/LangColorSalt.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageColor.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageReference.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageRefDUD.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageRefECL.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageRefESDL.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageRefKEL.xml
    ${PROJECT_SOURCE_DIR}/docs/LanguageRefSalt.xml
    ${PROJECT_SOURCE_DIR}/docs/SyntaxColorSamples.xml
)
INSTALL (FILES ${DOC_FILES} DESTINATION bin)
INSTALL (FILES ${PROJECT_SOURCE_DIR}/LICENSE.txt DESTINATION ".")

# Build tag generation  ---
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH64BIT 1)
ELSE ()
    SET(ARCH64BIT 0)
ENDIF ()
MESSAGE ("-- 64bit architecture is ${ARCH64BIT}")

SET(projname ${HPCC_PROJECT})
SET(audience ${HPCC_AUDIENCE})
SET(majorver ${HPCC_MAJOR})
SET(minorver ${HPCC_MINOR})
SET(point ${HPCC_POINT})
IF("${HPCC_MATURITY}" STREQUAL "release")
    SET(stagever "${HPCC_SEQUENCE}")
ELSE()
    SET(stagever "${HPCC_MATURITY}${HPCC_SEQUENCE}")
ENDIF()
SET(version ${majorver}.${minorver}.${point})

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    SET(stagever "${stagever}-Debug")
ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

# CPack  ---
INCLUDE("InstallRequiredSystemLibraries")
INSTALL(DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/Debug/ CONFIGURATIONS Debug DESTINATION bin FILES_MATCHING PATTERN "*.dll")
INSTALL(DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/MinSizeRel/ CONFIGURATIONS MinSizeRel DESTINATION bin FILES_MATCHING PATTERN "*.dll")
INSTALL(DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/Release/ CONFIGURATIONS Release DESTINATION bin FILES_MATCHING PATTERN "*.dll")
INSTALL(DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/RelWithDebInfo/ CONFIGURATIONS RelWithDebInfo DESTINATION bin FILES_MATCHING PATTERN "*.dll")

SET(CPACK_PACKAGE_VENDOR "HPCC Systems")
SET(CPACK_PACKAGE_NAME "hpccsystems-${projname}")
SET(CPACK_PACKAGE_VERSION_MAJOR ${majorver})
SET(CPACK_PACKAGE_VERSION_MINOR ${minorver})
SET(CPACK_PACKAGE_VERSION_PATCH ${point}${stagever})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HPCC Systems ECL IDE.")

SET(CPACK_PACKAGE_CONTACT "HPCCSystems <ossdevelopment@lexisnexis.com>")

SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/CPackReadMe.txt")

SET(CPACK_RPM_PACKAGE_VERSION "${audience}")
IF(${ARCH64BIT} EQUAL 1)
    SET(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
ELSE(${ARCH64BIT} EQUAL 1)
    SET(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
ENDIF (${ARCH64BIT} EQUAL 1)
SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CPACK_RPM_PACKAGE_ARCHITECTURE}")
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    SET(CPACK_STRIP_FILES TRUE)
ENDIF()
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_RPM_PACKAGE_VERSION}_${version}-${stagever}${CPACK_SYSTEM_NAME}")
MESSAGE ("-- Current release version is ${CPACK_PACKAGE_FILE_NAME}")

find_file(CLIENTTOOLS_PACKAGE_FILE "hpccsystems-clienttools-community_${version}-${stagever}${CPACK_SYSTEM_NAME}.exe" HINTS ${CMAKE_CURRENT_BINARY_DIR}/../HPCC-Platform )
if (CLIENTTOOLS_PACKAGE_FILE)
    install ( PROGRAMS ${CLIENTTOOLS_PACKAGE_FILE} DESTINATION tmp )
    get_filename_component(CLIENTTOOLS_PACKAGE_FILE_NAME ${CLIENTTOOLS_PACKAGE_FILE} NAME)
    list ( APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        ExecWait '$INSTDIR\\\\tmp\\\\${CLIENTTOOLS_PACKAGE_FILE_NAME} /S /D=$INSTDIR\\\\..\\\\clienttools'
")
endif (CLIENTTOOLS_PACKAGE_FILE)

# Calculate GIT Build Tag  ---
SET(BUILD_TAG "${CPACK_RPM_PACKAGE_VERSION}_${version}-${stagever}")
IF(USE_GIT_DESCRIBE OR CHECK_GIT_TAG)
    FETCH_GIT_TAG (${CMAKE_SOURCE_DIR} ${CPACK_RPM_PACKAGE_VERSION}_${version} GIT_BUILD_TAG)
    MESSAGE ("-- Git tag is '${GIT_BUILD_TAG}'")
    IF(NOT "${GIT_BUILD_TAG}" STREQUAL "${BUILD_TAG}")
        IF(CHECK_GIT_TAG)
            MESSAGE(FATAL_ERROR "Git tag '${GIT_BUILD_TAG}' does not match source version '${BUILD_TAG}'")
        ELSE()
            IF(NOT "${GIT_BUILD_TAG}" STREQUAL "") # probably means being built from a tarball...
                SET(BUILD_TAG "${BUILD_TAG}[${GIT_BUILD_TAG}]")
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF()
MESSAGE ("-- Build tag is '${BUILD_TAG}'")

SET(DIR_NAME "HPCCSystems")
SET(version ${HPCC_MAJOR}.${HPCC_MINOR}.${HPCC_POINT})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${DIR_NAME}\\\\${version}\\\\eclide")
####  Change the following if you _don't_ want the installer to uninstall previous versions  ####
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "eclide_${HPCC_MAJOR}")
SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\hpccsystems.com")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\eclide.exe")
SET(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
IF("${FB_PLATFORM_ARCH_NAME}" EQUAL "x86_64")
    SET (CPACK_NSIS_DISPLAY_NAME "ECL IDE x64")
ELSE ()
    SET (CPACK_NSIS_DISPLAY_NAME "ECL IDE")
ENDIF ()
LIST (APPEND CPACK_NSIS_CREATE_ICONS_EXTRA "
    CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ECL IDE.lnk\\\" \\\"$INSTDIR\\\\bin\\\\eclide.exe\\\"
    CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ECL Reference Help.lnk\\\" \\\"\$INSTDIR\\\\bin\\\\ECLReference.chm\\\"
")
LIST (APPEND CPACK_NSIS_DELETE_ICONS_EXTRA "
    Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\ECL IDE.lnk\\\"
    Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\ECL Reference Help.lnk\\\"
")
SET(CPACK_NSIS_DEFINES "
    !define MUI_STARTMENUPAGE_DEFAULTFOLDER \\\"${CPACK_PACKAGE_VENDOR}\\\\${version}\\\\${CPACK_NSIS_DISPLAY_NAME}\\\"
    !define MUI_FINISHPAGE_NOAUTOCLOSE
")

IF (EXISTS "${PROJECT_SOURCE_DIR}/../sign/passphrase.txt")
    FILE(STRINGS "${PROJECT_SOURCE_DIR}/../sign/passphrase.txt" PFX_PASSWORD LIMIT_COUNT 1)

    ADD_CUSTOM_TARGET(SIGN
        COMMAND signtool sign /f "${PROJECT_SOURCE_DIR}/../sign/hpcc_code_signing.pfx" /fd "SHA256" /p "${PFX_PASSWORD}" /tr "http://timestamp.digicert.com" "${CMAKE_BINARY_DIR}/${CPACK_PACKAGE_NAME}*.exe"
        COMMENT "Digital Signature"
    )
    ADD_DEPENDENCIES(SIGN PACKAGE)
    SET_PROPERTY(TARGET SIGN PROPERTY FOLDER "CMakePredefinedTargets")
ENDIF ()

INCLUDE (CPack)

# Configure Scripts  ---
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/config.h"
 )

CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/doStageECLIDE.bat.in"
    "${PROJECT_SOURCE_DIR}/doStageECLIDE.bat"
 )
