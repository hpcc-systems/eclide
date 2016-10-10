project( ECLEDITOR )

set ( SRCS
		lexgeneral.cxx
		lexecl.cxx
		lexsalt.cxx
		lexesdl.cxx
		lexkel.cxx

		${SCINTILLA_INCLUDE_DIR}/include/SciLExer.h

		${ECLEDITOR_SOURCE_DIR}/lexers/LexGENERAL.cxx
		${ECLEDITOR_SOURCE_DIR}/lexers/LexSALT.cxx
		${ECLEDITOR_SOURCE_DIR}/lexers/LexKEL.cxx
		${ECLEDITOR_SOURCE_DIR}/lexers/LexESDL.cxx

		${SCINTILLA_INCLUDE_DIR}/Src/AutoComplete.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/AutoComplete.h
		${SCINTILLA_INCLUDE_DIR}/Src/CallTip.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/CallTip.h
		./Catalogue.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Catalogue.h
		${SCINTILLA_INCLUDE_DIR}/Src/CellBuffer.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/CellBuffer.h
		${SCINTILLA_INCLUDE_DIR}/Src/CharClassify.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/CharClassify.h
		${SCINTILLA_INCLUDE_DIR}/Src/ContractionState.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/ContractionState.h
		${SCINTILLA_INCLUDE_DIR}/Src/Decoration.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Decoration.h
		${SCINTILLA_INCLUDE_DIR}/Src/Document.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Document.h
		${SCINTILLA_INCLUDE_DIR}/Src/Editor.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Editor.h
		${SCINTILLA_INCLUDE_DIR}/Src/ExternalLexer.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/ExternalLexer.h
		${SCINTILLA_INCLUDE_DIR}/Src/FontQuality.h
		${SCINTILLA_INCLUDE_DIR}/Src/Indicator.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Indicator.h
		${SCINTILLA_INCLUDE_DIR}/Src/KeyMap.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/KeyMap.h
		#${SCINTILLA_INCLUDE_DIR}/Src/KeyWords.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/LineMarker.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/LineMarker.h
		${SCINTILLA_INCLUDE_DIR}/Src/Partitioning.h
		${SCINTILLA_INCLUDE_DIR}/Src/PerLine.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/PerLine.h
		${SCINTILLA_INCLUDE_DIR}/Src/PositionCache.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/PositionCache.h
		#${SCINTILLA_INCLUDE_DIR}/Src/PropSet.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/RESearch.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/RESearch.h
		${SCINTILLA_INCLUDE_DIR}/Src/RunStyles.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/RunStyles.h
		${SCINTILLA_INCLUDE_DIR}/Src/ScintillaBase.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/ScintillaBase.h
		${SCINTILLA_INCLUDE_DIR}/Src/Selection.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Selection.h
		${SCINTILLA_INCLUDE_DIR}/Src/SplitVector.h
		${SCINTILLA_INCLUDE_DIR}/Src/Style.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/Style.h
		${SCINTILLA_INCLUDE_DIR}/Src/SVector.h
		${SCINTILLA_INCLUDE_DIR}/Src/UniConversion.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/UniConversion.h
		${SCINTILLA_INCLUDE_DIR}/Src/ViewStyle.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/ViewStyle.h
		#${SCINTILLA_INCLUDE_DIR}/Src/WindowAccessor.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/XPM.cxx
		${SCINTILLA_INCLUDE_DIR}/Src/XPM.h

		${SCINTILLA_INCLUDE_DIR}/LexLib/Accessor.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/Accessor.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/CharacterSet.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/CharacterSet.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexAccessor.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerBase.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerBase.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerModule.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerModule.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerNoExceptions.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerNoExceptions.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerSimple.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/LexerSimple.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/OptionSet.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/PropSetSimple.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/PropSetSimple.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/StyleContext.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/StyleContext.h
		${SCINTILLA_INCLUDE_DIR}/LexLib/WordList.cxx
		${SCINTILLA_INCLUDE_DIR}/LexLib/WordList.h
		
#		${SCINTILLA_INCLUDE_DIR}/Win32/PlatformRes.h
		${SCINTILLA_INCLUDE_DIR}/Win32/PlatWin.cxx
		${SCINTILLA_INCLUDE_DIR}/Win32/ScintillaWin.cxx
		
		${SCINTILLA_INCLUDE_DIR}/Win32/ScintRes.rc
	)

include_directories (
	.
	${SCINTILLA_INCLUDE_DIR}/Include
	${SCINTILLA_INCLUDE_DIR}/LexLib
	${SCINTILLA_INCLUDE_DIR}/Src
		
	${WTL_INCLUDE_DIR}/include

	)
add_definitions( -DSCI_LEXER )
add_library ( ECLEDITOR SHARED
		${SRCS}
	)

target_link_libraries( ECLEDITOR
	imm32
	)

install ( TARGETS ECLEDITOR RUNTIME DESTINATION bin )