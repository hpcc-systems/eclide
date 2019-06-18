project( AMT )

set ( SRCS
		AMT.cpp
		AMT.h
		AMT.rc
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

include_directories (
		../clib
		../ecllib
		../comms
		../vdiff
		../wlib

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
add_executable ( AMT WIN32
		${SRCS}
	)

add_dependencies ( AMT
		CLIB
		COMMS
		WLIB
		BUGTRAP
	)

target_link_libraries( AMT
		CLIB
		COMMS
		WLIB
		BUGTRAP
	)

set_target_properties(AMT PROPERTIES
    LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")

install ( TARGETS AMT RUNTIME DESTINATION bin )