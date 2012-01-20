project( ECLIDE )

set ( SRCS
		#AttributeView.cpp
		#AttributeView.h
		BasicExcel.cpp
		ChildAttributeFrame.cpp
		ChildAttributeFrame.h
		ChildBuilderFrame.cpp
		ChildBuilderFrame.h
		#ChildEclFrame.cpp
		#ChildEclFrame.h
		ChildFrame.cpp
		ChildFrame.h
		ChildGraphFrame.cpp
		ChildGraphFrame.h
		#ChildResultFrame.cpp
		#ChildResultFrame.h
		#Combos.cpp
		#Combos.h
		#CommandManager.cpp
		#CommandManager.h
		CommentDlg.cpp
		CommentDlg.h
		DataView.cpp
		DataView.h
		DebugGraphView.cpp
		DebugGraphView.h
		DebugView.cpp
		DebugView.h
		DfuFrame.cpp
		DfuFrame.h
		DfuTreeNode.cpp
		DfuTreeNode.h
		DfuView.cpp
		DfuView.h
		DialogAddBreakpoint.cpp
		DialogAddBreakpoint.h
		DockableBreakpointView.cpp
		DockableBreakpointView.h
		DockableDataViews.cpp
		DockableDataViews.h
		DockablePropertiesView.cpp
		DockablePropertiesView.h
		DockablePropertyGridViews.cpp
		DockablePropertyGridViews.h
		DockableSearchView.cpp
		DockableSearchView.h
		#DockedFrame.cpp
		#DockedFrame.h
		EclDlg.cpp
		EclDlg.h
		EclDlgAttribute.cpp
		EclDlgAttribute.h
		EclDlgBuilder.cpp
		EclDlgBuilder.h
		#EclViewDataStruct.h
		ErrorFrame.cpp
		ErrorFrame.h
		ExceptionView.cpp
		ExceptionView.h
		#GraphFrm.cpp
		#GraphFrm.h
		GraphView.cpp
		GraphView.h
		GraphView2.cpp
		GraphView2.h
		GraphView3.cpp
		GraphView3.h
		#grid.cpp
		#grid.h
		GridMixIn.h
		#HistoryDlg.cpp
		#HistoryDlg.h
		HistoryView.cpp
		HistoryView.h
		#HtmlFrame.h
		#HtmlView.h
		IdlePump.h
		InsAttrDlg.cpp
		InsAttrDlg.h
		ListRow.cpp
		ListRow.h
		mainfrm.cpp
		mainfrm.h
		#MDIFrame.cpp
		#MDIFrame.h
		OpenWorkunit.cpp
		OpenWorkunit.h
		OutputView.cpp
		OutputView.h
		#prefdlg.cpp
		#prefdlg.h
		PropertyGridView.cpp
		PropertyGridView.h
		QueryBuilder.rc
		QueryBuilder.cpp
		QueryBuilder.h
		#RelationshipView.h
		#RelBinder.cpp
		#RelBinder.h
		#RelChildFrm.h
		#RelLabelView.h
		#RelTableView.h
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
		#RoxieNetView.cpp
		#RoxieNetView.h
		SortList.h
		SourceDataView.cpp
		SourceDataView.h
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
		#targetver.h
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

		../EditLog/debug.h
		../EditLog/EditLog.cpp
		../EditLog/EditLog.h
		../EditLog/editlog_stream.h
		../EditLog/SubclassWnd.cpp
		../EditLog/SubclassWnd.h

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
		../GraphViewCtl
		../sgrid
		../EditLog

		../wtlex
		../tabctrls
		../dockingwindows

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
		GRAPHVIEWCTL
		WLIB
		BUGTRAP
	)

target_link_libraries( ECLIDE
		CLIB
		COMMS
		GRAPHVIEWCTL
		WLIB
		BUGTRAP
	)

set_target_properties(ECLIDE PROPERTIES
    LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")

install ( TARGETS ECLIDE RUNTIME DESTINATION bin )