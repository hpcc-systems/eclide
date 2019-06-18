PROJECT( ECLEDITOR )

SET ( SRCS
    lexgeneral.cxx
    lexecl.cxx
    lexsalt.cxx
    lexesdl.cxx
    lexkel.cxx
    lexdud.cxx

    ${SCINTILLA_INCLUDE_DIR}/SciLExer.h

    ${ECLEDITOR_SOURCE_DIR}/LexGENERAL.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexSALT.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexKEL.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexESDL.cxx
    ${ECLEDITOR_SOURCE_DIR}/LexDUD.cxx

    ${SCINTILLA_SOURCE_DIR}/AutoComplete.cxx
    ${SCINTILLA_SOURCE_DIR}/AutoComplete.h
    ${SCINTILLA_SOURCE_DIR}/CallTip.cxx
    ${SCINTILLA_SOURCE_DIR}/CallTip.h
    ./Catalogue.cxx
    ${SCINTILLA_SOURCE_DIR}/Catalogue.h
    ${SCINTILLA_SOURCE_DIR}/CellBuffer.cxx
    ${SCINTILLA_SOURCE_DIR}/CellBuffer.h
    ${SCINTILLA_SOURCE_DIR}/CaseConvert.cxx
    ${SCINTILLA_SOURCE_DIR}/CaseConvert.h
    ${SCINTILLA_SOURCE_DIR}/CaseFolder.cxx
    ${SCINTILLA_SOURCE_DIR}/CaseFolder.h
    ${SCINTILLA_SOURCE_DIR}/CharClassify.cxx
    ${SCINTILLA_SOURCE_DIR}/CharClassify.h
    ${SCINTILLA_SOURCE_DIR}/ContractionState.cxx
    ${SCINTILLA_SOURCE_DIR}/ContractionState.h
    ${SCINTILLA_SOURCE_DIR}/Decoration.cxx
    ${SCINTILLA_SOURCE_DIR}/Decoration.h
    ${SCINTILLA_SOURCE_DIR}/Document.cxx
    ${SCINTILLA_SOURCE_DIR}/Document.h
    ${SCINTILLA_SOURCE_DIR}/Editor.cxx
    ${SCINTILLA_SOURCE_DIR}/Editor.h
    ${SCINTILLA_SOURCE_DIR}/EditModel.cxx
    ${SCINTILLA_SOURCE_DIR}/EditModel.h
    ${SCINTILLA_SOURCE_DIR}/EditView.cxx
    ${SCINTILLA_SOURCE_DIR}/EditView.h
    ${SCINTILLA_SOURCE_DIR}/ExternalLexer.cxx
    ${SCINTILLA_SOURCE_DIR}/ExternalLexer.h
    ${SCINTILLA_SOURCE_DIR}/FontQuality.h
    ${SCINTILLA_SOURCE_DIR}/Indicator.cxx
    ${SCINTILLA_SOURCE_DIR}/Indicator.h
    ${SCINTILLA_SOURCE_DIR}/KeyMap.cxx
    ${SCINTILLA_SOURCE_DIR}/KeyMap.h
    #${SCINTILLA_SOURCE_DIR}/KeyWords.cxx
    ${SCINTILLA_SOURCE_DIR}/LineMarker.cxx
    ${SCINTILLA_SOURCE_DIR}/LineMarker.h
    ${SCINTILLA_SOURCE_DIR}/MarginView.cxx
    ${SCINTILLA_SOURCE_DIR}/MarginView.h
    ${SCINTILLA_SOURCE_DIR}/Partitioning.h
    ${SCINTILLA_SOURCE_DIR}/PerLine.cxx
    ${SCINTILLA_SOURCE_DIR}/PerLine.h
    ${SCINTILLA_SOURCE_DIR}/PositionCache.cxx
    ${SCINTILLA_SOURCE_DIR}/PositionCache.h
    #${SCINTILLA_SOURCE_DIR}/PropSet.cxx
    ${SCINTILLA_SOURCE_DIR}/RESearch.cxx
    ${SCINTILLA_SOURCE_DIR}/RESearch.h
    ${SCINTILLA_SOURCE_DIR}/RunStyles.cxx
    ${SCINTILLA_SOURCE_DIR}/RunStyles.h
    ${SCINTILLA_SOURCE_DIR}/ScintillaBase.cxx
    ${SCINTILLA_SOURCE_DIR}/ScintillaBase.h
    ${SCINTILLA_SOURCE_DIR}/Selection.cxx
    ${SCINTILLA_SOURCE_DIR}/Selection.h
    ${SCINTILLA_SOURCE_DIR}/SplitVector.h
    ${SCINTILLA_SOURCE_DIR}/Style.cxx
    ${SCINTILLA_SOURCE_DIR}/Style.h
    #${SCINTILLA_SOURCE_DIR}/SVector.h
    ${SCINTILLA_SOURCE_DIR}/UniConversion.cxx
    ${SCINTILLA_SOURCE_DIR}/UniConversion.h
    ${SCINTILLA_SOURCE_DIR}/ViewStyle.cxx
    ${SCINTILLA_SOURCE_DIR}/ViewStyle.h
    #${SCINTILLA_SOURCE_DIR}/WindowAccessor.cxx
    ${SCINTILLA_SOURCE_DIR}/XPM.cxx
    ${SCINTILLA_SOURCE_DIR}/XPM.h

    ${SCINTILLA_LEXLIB_DIR}/Accessor.cxx
    ${SCINTILLA_LEXLIB_DIR}/Accessor.h
    ${SCINTILLA_LEXLIB_DIR}/CharacterCategory.cxx
    ${SCINTILLA_LEXLIB_DIR}/CharacterCategory.h
    ${SCINTILLA_LEXLIB_DIR}/CharacterSet.cxx
    ${SCINTILLA_LEXLIB_DIR}/CharacterSet.h
    ${SCINTILLA_LEXLIB_DIR}/LexAccessor.h
    ${SCINTILLA_LEXLIB_DIR}/LexerBase.cxx
    ${SCINTILLA_LEXLIB_DIR}/LexerBase.h
    ${SCINTILLA_LEXLIB_DIR}/LexerModule.cxx
    ${SCINTILLA_LEXLIB_DIR}/LexerModule.h
    ${SCINTILLA_LEXLIB_DIR}/LexerNoExceptions.cxx
    ${SCINTILLA_LEXLIB_DIR}/LexerNoExceptions.h
    ${SCINTILLA_LEXLIB_DIR}/LexerSimple.cxx
    ${SCINTILLA_LEXLIB_DIR}/LexerSimple.h
    ${SCINTILLA_LEXLIB_DIR}/OptionSet.h
    ${SCINTILLA_LEXLIB_DIR}/PropSetSimple.cxx
    ${SCINTILLA_LEXLIB_DIR}/PropSetSimple.h
    ${SCINTILLA_LEXLIB_DIR}/StyleContext.cxx
    ${SCINTILLA_LEXLIB_DIR}/StyleContext.h
    ${SCINTILLA_LEXLIB_DIR}/WordList.cxx
    ${SCINTILLA_LEXLIB_DIR}/WordList.h
    
#    ${SCINTILLA_INCLUDE_DIR}/Win32/PlatformRes.h
    ${SCINTILLA_WIN32_DIR}/PlatWin.cxx
    ${SCINTILLA_WIN32_DIR}/ScintillaWin.cxx
    ${SCINTILLA_WIN32_DIR}/HanjaDic.cxx
    ${SCINTILLA_WIN32_DIR}/HanjaDic.h
)

INCLUDE_DIRECTORIES (
    .
    ${SCINTILLA_INCLUDE_DIR}
    ${SCINTILLA_LEXLIB_DIR}
    ${SCINTILLA_SOURCE_DIR}
    ${SCINTILLA_WIN32_DIR}
        
    ${WTL_INCLUDE_DIR}
)

ADD_DEFINITIONS( -DSCI_LEXER )

ADD_LIBRARY ( ECLEDITOR SHARED
    ${SRCS}
    ${SCINTILLA_WIN32_DIR}/ScintRes.rc
)

TARGET_LINK_LIBRARIES( ECLEDITOR
    imm32
    Msimg32
    )

INSTALL ( TARGETS ECLEDITOR RUNTIME DESTINATION bin )
