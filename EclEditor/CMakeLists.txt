PROJECT( ECLEDITOR )

find_path(SCINTILLA_INCLUDE_DIR Scintilla.h HINTS ${VCPKG_ROOT_DIR}/include/scintilla)
message(STATUS "SCINTILLA_INCLUDE_DIR: ${SCINTILLA_INCLUDE_DIR}")
find_path(LEXILLA_INCLUDE_DIR SciLexer.h HINTS ${VCPKG_ROOT_DIR}/include/lexilla)
message(STATUS "LEXILLA_INCLUDE_DIR: ${LEXILLA_INCLUDE_DIR}")

# Find the prebuilt libraries
find_library(SCINTILLA_LIBRARY Scintilla HINTS ${VCPKG_ROOT_DIR}/lib)
find_library(LEXILLA_LIBRARY Lexilla HINTS ${VCPKG_ROOT_DIR}/lib)

# Find the lexlib source files needed for custom lexers
find_path(LEXILLA_SOURCE_DIR Accessor.cxx HINTS ${VCPKG_ROOT_DIR}/share/lexilla/lexlib)

SET ( SRCS
    # Custom ECL lexers
    lexgeneral.cxx
    lexecl.cxx
    lexsalt.cxx
    lexesdl.cxx
    lexkel.cxx
    lexdud.cxx

    ${LEXILLA_INCLUDE_DIR}/SciLexer.h

    ${ECLEDITOR_SOURCE_DIR}/LexGENERAL.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexSALT.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexKEL.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexESDL.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexDUD.cxx

    # Catalogue for registering custom lexers
    ./Catalogue.cxx
    
    # Lexlib source files needed for custom lexers
    ${LEXILLA_SOURCE_DIR}/Accessor.cxx
    ${LEXILLA_SOURCE_DIR}/CharacterCategory.cxx
    ${LEXILLA_SOURCE_DIR}/CharacterSet.cxx
    ${LEXILLA_SOURCE_DIR}/LexAccessor.cxx
    ${LEXILLA_SOURCE_DIR}/LexerBase.cxx
    ${LEXILLA_SOURCE_DIR}/LexerModule.cxx
    ${LEXILLA_SOURCE_DIR}/LexerSimple.cxx
    ${LEXILLA_SOURCE_DIR}/PropSetSimple.cxx
    ${LEXILLA_SOURCE_DIR}/StyleContext.cxx
    ${LEXILLA_SOURCE_DIR}/WordList.cxx
)

INCLUDE_DIRECTORIES (
    .
    ${SCINTILLA_INCLUDE_DIR}
    ${LEXILLA_INCLUDE_DIR}
    ${LEXILLA_INCLUDE_DIR}/lexlib
)

ADD_DEFINITIONS( -DSCI_LEXER )

ADD_LIBRARY ( ECLEDITOR SHARED
    ${SRCS}
)

TARGET_LINK_LIBRARIES( ECLEDITOR PRIVATE
    ${SCINTILLA_LIBRARY}
    ${LEXILLA_LIBRARY}
    imm32
    Msimg32
    )

# Copy Scintilla.dll and Lexilla.dll to output directory after build
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES)
    set(SCINTILLA_DLL_DEBUG "${VCPKG_ROOT_DIR}/debug/bin/Scintilla.dll")
    set(SCINTILLA_DLL_RELEASE "${VCPKG_ROOT_DIR}/bin/Scintilla.dll")
    set(LEXILLA_DLL_DEBUG "${VCPKG_ROOT_DIR}/debug/bin/Lexilla.dll")
    set(LEXILLA_DLL_RELEASE "${VCPKG_ROOT_DIR}/bin/Lexilla.dll")
    
    add_custom_command(TARGET ECLEDITOR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<IF:$<CONFIG:Debug>,${SCINTILLA_DLL_DEBUG},${SCINTILLA_DLL_RELEASE}>"
            "$<TARGET_FILE_DIR:ECLEDITOR>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<IF:$<CONFIG:Debug>,${LEXILLA_DLL_DEBUG},${LEXILLA_DLL_RELEASE}>"
            "$<TARGET_FILE_DIR:ECLEDITOR>"
        COMMENT "Copying Scintilla.dll and Lexilla.dll to output directory"
    )
endif()

INSTALL ( TARGETS ECLEDITOR RUNTIME DESTINATION bin )

# Install Scintilla.dll and Lexilla.dll from vcpkg for packaging
install(FILES "$<IF:$<CONFIG:Debug>,${SCINTILLA_DLL_DEBUG},${SCINTILLA_DLL_RELEASE}>" 
        DESTINATION bin)
install(FILES "$<IF:$<CONFIG:Debug>,${LEXILLA_DLL_DEBUG},${LEXILLA_DLL_RELEASE}>" 
        DESTINATION bin)
