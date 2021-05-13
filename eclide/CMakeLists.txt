PROJECT( ECLIDE )

SET ( SRCS
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
    #RoxieDebugSession.cpp
    #RoxieDebugSession.h
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

    eclide.exe.manifest
)

INCLUDE_DIRECTORIES (
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

    ${WTL_INCLUDE_DIR}
    ${SCINTILLA_INCLUDE_DIR}
    ${BUGTRAP_DIR}
    ${CEF_ROOT_DIR}
)

ADD_DEFINITIONS( -D_AFXDLL -D_USRDLL )

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_EXECUTABLE ( ECLIDE WIN32
    ${SRCS}
    QueryBuilder.rc
)

ADD_DEPENDENCIES ( ECLIDE
    CLIB
    COMMS
    WLIB
    BUGTRAP
)

TARGET_LINK_LIBRARIES( ECLIDE
    CLIB
    COMMS
    WLIB
    BUGTRAP
)

SET_TARGET_PROPERTIES(ECLIDE PROPERTIES LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")

INSTALL ( TARGETS ECLIDE RUNTIME DESTINATION bin )
