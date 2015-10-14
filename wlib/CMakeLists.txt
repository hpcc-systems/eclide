project( WLIB )

set ( SRCS
		AboutDlg.cpp
		AboutDlg.h
		AnalyzeXML.cpp
		AnalyzeXML.h
		AttrDependecy.cpp
		AttrDependecy.h
		AttributeDlg.cpp
		AttributeDlg.h
		AttrListDlg.cpp
		AttrListDlg.h
		ChangePasswordDlg.cpp
		ChangePasswordDlg.h
		ColorPicker.cpp
		ColorPicker.h
		Combo.cpp
		Combo.h
		ControlFocus.h
		CtlColor.h
		DaliTreeNode.cpp
		DaliTreeNode.h
		DelimitedColumnInfo.cpp
		DelimitedColumnInfo.h
		#DiffView.cpp
		DiffView.h
		EclCommand.h
		EclExec.cpp
		EclExec.h
		#EclProgramManager.cpp
		#EclProgramManager.h
		#FileAnalyzer.cpp
		#FileAnalyzer.h
		FindDlgMgr.h
		FindReplace.h
		GotoDlg.cpp
		GotoDlg.h
		HtmlView.h
		HListBox.h
		#ImportRecordAnalyze.cpp
		#ImportRecordAnalyze.h
		#ImportRecordInfo.cpp
		#ImportRecordInfo.h
		InvokeColorPicker.h
		InvokeSprayWizard.h
		#LandingZoneFileDialog.cpp
		#LandingZoneFileDialog.h
		LangRef.cpp
		LangRef.h
		LoginDlg.cpp
		LoginDlg.h
		LZoneFolderTree.cpp
		LZoneFolderTree.h
		ModSelectDlg.cpp
		ModSelectDlg.h
		MouseTooltip.h
		MultiTree.cpp
		MultiTree.h
		MultiTreeNode.cpp
		MultiTreeNode.h
		npHPCCSystemsGraphViewControl.cpp
		npHPCCSystemsGraphViewControl.h
		PreferenceDlg.cpp
		PreferenceDlg.h
		RepositoryTreeNode.cpp
		RepositoryTreeNode.h
		RepositoryView.cpp
		RepositoryView.h
		SciSyntaxMarker.h
		scite.h
		ShellContextMenu.h
		ShellContextMenu.cpp
		SimplePromptDlg.cpp
		SimplePromptDlg.h
		SourceView.cpp
		SourceView.h
		#SplitterWnd.h
		StdString.h
		TreeNode.cpp
		TreeNode.h
		wlib.rc
		wlib.cpp
		wlib.h
		WorkspaceTreeNode.cpp
		WorkspaceTreeNode.h
		WorkspaceView.cpp
		WorkspaceView.h
		wutil.cpp
		wutil.h
		
		../wtlex/file_ver.cpp
		../wtlex/file_ver.h
		../wtlex/ColorButton.cpp
		../wtlex/ColorButton.h

		../docs/LanguageColor.xml
		../docs/LanguageReference.xml
	)

include_directories (
	../clib
	../ecllib
	../sgrid
	../comms
	../wtlex
	../dockingwindows
	
	${WTL_INCLUDE_DIR}/include
	${SCINTILLA_INCLUDE_DIRS}/include
	)

if (Boost_FOUND)
	include_directories ( ${Boost_INCLUDE_DIRS} )
	link_directories ( ${Boost_LIBRARY_DIRS} )
endif (Boost_FOUND)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)
add_library ( WLIB SHARED
		${SRCS}
	)

add_dependencies ( WLIB
		CLIB
		COMMS
		SGRID
	)

target_link_libraries( WLIB
		CLIB
		COMMS
		SGRID
	)

install ( TARGETS WLIB RUNTIME DESTINATION bin )