project( ECLIDE )

set ( SRCS
		BasicExcel.cpp
		BookmarksFrame.cpp
		BookmarksFrame.h
		BookmarksView.cpp
		BookmarksView.h
		ChildAttributeFrame.cpp
		ChildAttributeFrame.h
		ChildBuilderFrame.cpp
		ChildBuilderFrame.h
		ChildFrame.cpp
		ChildFrame.h
		CommentDlg.cpp
		CommentDlg.h
		CSortMFCListCtrl.cpp
		CSortMFCListCtrl.h
		DataView.cpp
		DataView.h
		DfuFrame.cpp
		DfuFrame.h
		DfuTreeNode.cpp
		DfuTreeNode.h
		DfuView.cpp
		DfuView.h
		EclDlg.cpp
		EclDlg.h
		EclDlgAttribute.cpp
		EclDlgAttribute.h
		EclDlgBuilder.cpp
		EclDlgBuilder.h
		ErrorFrame.cpp
		ErrorFrame.h
		ExceptionView.cpp
		ExceptionView.h
		GridMixIn.h
		HistoryView.cpp
		HistoryView.h
		IdlePump.h
		InsAttrDlg.cpp
		InsAttrDlg.h
		ListRow.cpp
		ListRow.h
		mainfrm.cpp
		mainfrm.h
		OpenWorkunit.cpp
		OpenWorkunit.h
		OutputView.cpp
		OutputView.h
		QueryBuilder.rc
		QueryBuilder.cpp
		QueryBuilder.h
		RepositoryFilterView.cpp
		RepositoryFilterView.h
		RepositoryFrame.cpp
		RepositoryFrame.h
		resource.h
		ResponseSaxHandler.cpp
		ResponseSaxHandler.h
		ResultView.cpp
		ResultView.h
		RoxieDebugSession.cpp
		RoxieDebugSession.h
		SortList.h
		SummaryView.cpp
		SummaryView.h
		SyntaxFrame.cpp
		SyntaxFrame.h
		SyntaxView.cpp
		SyntaxView.h
		TabbedChildWindowEx.h
		TableGridCtrl.cpp
		TableGridCtrl.h
		TabPane.cpp
		TabPane.h
		TestSocket.cpp
		TestSocket.h
		WorkspaceFrame.cpp
		WorkspaceFrame.h
		WorkunitFrame.cpp
		WorkunitFrame.h
		WorkunitView.cpp
		WorkunitView.h
		WtlDockablePane.cpp
		WtlDockablePane.h
		WtlMDIChildFrame.cpp
		WtlMDIChildFrame.h

		../EditLog/EditLog.cpp
		../EditLog/EditLog.h
		../EditLog/editlog_stream.h

		../wlib/diffview.cpp
		../wlib/diffview.h
		../wlib/RepositoryView.cpp
		../wlib/RepositoryView.h
		../wlib/SourceView.cpp
		../wlib/SourceView.h
		../wlib/WorkspaceView.cpp
		../wlib/WorkspaceView.h
	)

include_directories (
		../clib
		../ecllib
		../comms
		../wlib
		../sgrid
		../EditLog

		../wtlex
		../tabctrls
		../dockingwindows
		../EclEditor

		${WTL_INCLUDE_DIR}/include
		${SCINTILLA_INCLUDE_DIR}/include
		${BUGTRAP_INCLUDE_DIR}
	)

if (Boost_FOUND)
	include_directories ( ${Boost_INCLUDE_DIRS} )
	link_directories ( ${Boost_LIBRARY_DIRS} )
endif (Boost_FOUND)

ADD_DEFINITIONS( -D_AFXDLL -D_USRDLL )
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)
add_executable ( ECLIDE WIN32
		${SRCS}
	)

add_dependencies ( ECLIDE
		CLIB
		COMMS
		WLIB
		BUGTRAP
	)

target_link_libraries( ECLIDE
		CLIB
		COMMS
		WLIB
		BUGTRAP
	)

set_target_properties(ECLIDE PROPERTIES
    LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")

install ( TARGETS ECLIDE RUNTIME DESTINATION bin )