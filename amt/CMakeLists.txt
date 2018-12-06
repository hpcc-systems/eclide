PROJECT( AMT )

SET ( SRCS
    AMT.cpp
    AMT.h
    #AMTView.cpp
    #AMTView.h
    #CommandLine.cpp
    #CommandLine.h
    DependeesWnd.cpp
    DependeesWnd.h
    ../wlib/DiffView.cpp
    ../EditLog/EditLog.cpp
    Filter.h
    LogView.cpp
    LogView.h
    MainFrm2.cpp
    MainFrm2.h
    ../wlib/MultiTree.cpp
    #PlaceHolder.cpp
    #PlaceHolder.h
    RepositoryCompareDlg.cpp
    RepositoryCompareDlg.h
    RepositoryCompareView.cpp
    RepositoryCompareView.h
    #RepositorySynch.cpp
    #RepositorySynch.h
    Resource.h
    Selection.cpp
    Selection.h
    ../wlib/SourceView.cpp
    Status.h
    targetver.h
    TreePairNode.cpp
    TreePairNode.h
)

INCLUDE_DIRECTORIES (
    ../clib
    ../ecllib
    ../comms
    ../vdiff
    ../wlib

    ../wtlex
    ../tabctrls
    ../dockingwindows
    ../EclEditor

    ${Boost_INCLUDE_DIRS}
    ${WTL_INCLUDE_DIR}
    ${SCINTILLA_INCLUDE_DIR}
    ${BUGTRAP_DIR}
)

ADD_DEFINITIONS( -D_AFXDLL -D_USRDLL )

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_EXECUTABLE ( AMT WIN32
    ${SRCS}
    AMT.rc
)

ADD_DEPENDENCIES ( AMT
    CLIB
    COMMS
    WLIB
    BUGTRAP
)

TARGET_LINK_LIBRARIES( AMT
    CLIB
    COMMS
    WLIB
    BUGTRAP
    ${Boost_LIBRARIES}
)

SET_TARGET_PROPERTIES(AMT PROPERTIES LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")

INSTALL ( TARGETS AMT RUNTIME DESTINATION bin )
