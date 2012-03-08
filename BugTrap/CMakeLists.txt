project( BUGTRAP )

set ( SRCS
		${BUGTRAP_INCLUDE_DIR}/AboutDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/AboutDlg.h
		${BUGTRAP_INCLUDE_DIR}/AnimProgressBar.cpp
		${BUGTRAP_INCLUDE_DIR}/AnimProgressBar.h
		${BUGTRAP_INCLUDE_DIR}/Array.h
		${BUGTRAP_INCLUDE_DIR}/AssemblyInfo.cpp
		${BUGTRAP_INCLUDE_DIR}/BaseStream.h
		${BUGTRAP_INCLUDE_DIR}/BTAtlWindow.h
		${BUGTRAP_INCLUDE_DIR}/BTMfcWindow.h
		${BUGTRAP_INCLUDE_DIR}/BTTrace.h
		${BUGTRAP_INCLUDE_DIR}/Buffer.h
		${BUGTRAP_INCLUDE_DIR}/BugTrap.rc
		${BUGTRAP_INCLUDE_DIR}/BugTrap.cpp
		${BUGTRAP_INCLUDE_DIR}/BugTrap.h
		${BUGTRAP_INCLUDE_DIR}/BugTrapNet.cpp
		${BUGTRAP_INCLUDE_DIR}/BugTrapNet.h
		${BUGTRAP_INCLUDE_DIR}/BugTrapUI.cpp
		${BUGTRAP_INCLUDE_DIR}/BugTrapUI.h
		${BUGTRAP_INCLUDE_DIR}/BugTrapUtils.cpp
		${BUGTRAP_INCLUDE_DIR}/BugTrapUtils.h
		${BUGTRAP_INCLUDE_DIR}/CMapi.cpp
		${BUGTRAP_INCLUDE_DIR}/CMapi.h
		${BUGTRAP_INCLUDE_DIR}/ColHelper.cpp
		${BUGTRAP_INCLUDE_DIR}/ColHelper.h
		${BUGTRAP_INCLUDE_DIR}/DescribeErrorDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/DescribeErrorDlg.h
		${BUGTRAP_INCLUDE_DIR}/Encoding.cpp
		${BUGTRAP_INCLUDE_DIR}/Encoding.h
		${BUGTRAP_INCLUDE_DIR}/EnumProcess.cpp
		${BUGTRAP_INCLUDE_DIR}/EnumProcess.h
		${BUGTRAP_INCLUDE_DIR}/FileStream.cpp
		${BUGTRAP_INCLUDE_DIR}/FileStream.h
		${BUGTRAP_INCLUDE_DIR}/Globals.cpp
		${BUGTRAP_INCLUDE_DIR}/Globals.h
		${BUGTRAP_INCLUDE_DIR}/Hash.h
		${BUGTRAP_INCLUDE_DIR}/HexView.cpp
		${BUGTRAP_INCLUDE_DIR}/HexView.h
		${BUGTRAP_INCLUDE_DIR}/HyperLink.cpp
		${BUGTRAP_INCLUDE_DIR}/HyperLink.h
		${BUGTRAP_INCLUDE_DIR}/ImageView.cpp
		${BUGTRAP_INCLUDE_DIR}/ImageView.h
		${BUGTRAP_INCLUDE_DIR}/InMemLogFile.cpp
		${BUGTRAP_INCLUDE_DIR}/InMemLogFile.h
		${BUGTRAP_INCLUDE_DIR}/InputStream.cpp
		${BUGTRAP_INCLUDE_DIR}/InputStream.h
		${BUGTRAP_INCLUDE_DIR}/InterfacePtr.h
		${BUGTRAP_INCLUDE_DIR}/LayoutManager.cpp
		${BUGTRAP_INCLUDE_DIR}/LayoutManager.h
		${BUGTRAP_INCLUDE_DIR}/LeakWatcher.h
		${BUGTRAP_INCLUDE_DIR}/List.h
		${BUGTRAP_INCLUDE_DIR}/LogFile.cpp
		${BUGTRAP_INCLUDE_DIR}/LogFile.h
		${BUGTRAP_INCLUDE_DIR}/LogLink.h
		${BUGTRAP_INCLUDE_DIR}/LogStream.cpp
		${BUGTRAP_INCLUDE_DIR}/LogStream.h
		${BUGTRAP_INCLUDE_DIR}/MachineInfoDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/MachineInfoDlg.h
		${BUGTRAP_INCLUDE_DIR}/MachineStateDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/MachineStateDlg.h
		${BUGTRAP_INCLUDE_DIR}/MainDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/MainDlg.h
		${BUGTRAP_INCLUDE_DIR}/MemStream.cpp
		${BUGTRAP_INCLUDE_DIR}/MemStream.h
		${BUGTRAP_INCLUDE_DIR}/ModuleImportTable.cpp
		${BUGTRAP_INCLUDE_DIR}/ModuleImportTable.h
		${BUGTRAP_INCLUDE_DIR}/NetThunks.cpp
		${BUGTRAP_INCLUDE_DIR}/NetThunks.h
		${BUGTRAP_INCLUDE_DIR}/OutputStream.cpp
		${BUGTRAP_INCLUDE_DIR}/OutputStream.h
		${BUGTRAP_INCLUDE_DIR}/PreviewDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/PreviewDlg.h
		${BUGTRAP_INCLUDE_DIR}/ResManager.cpp
		${BUGTRAP_INCLUDE_DIR}/ResManager.h
		${BUGTRAP_INCLUDE_DIR}/resource.h
		${BUGTRAP_INCLUDE_DIR}/SendMailDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/SendMailDlg.h
		${BUGTRAP_INCLUDE_DIR}/SimpleDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/SimpleDlg.h
		${BUGTRAP_INCLUDE_DIR}/SmartPtr.h
		${BUGTRAP_INCLUDE_DIR}/Splitter.cpp
		${BUGTRAP_INCLUDE_DIR}/Splitter.h
		${BUGTRAP_INCLUDE_DIR}/Stream.h
		${BUGTRAP_INCLUDE_DIR}/StrHolder.cpp
		${BUGTRAP_INCLUDE_DIR}/StrHolder.h
		${BUGTRAP_INCLUDE_DIR}/StrStream.cpp
		${BUGTRAP_INCLUDE_DIR}/StrStream.h
		${BUGTRAP_INCLUDE_DIR}/SymEngine.cpp
		${BUGTRAP_INCLUDE_DIR}/SymEngine.h
		${BUGTRAP_INCLUDE_DIR}/SymEngineNet.cpp
		${BUGTRAP_INCLUDE_DIR}/SymEngineNet.h
		${BUGTRAP_INCLUDE_DIR}/TextFormat.cpp
		${BUGTRAP_INCLUDE_DIR}/TextFormat.h
		${BUGTRAP_INCLUDE_DIR}/TextLogFile.cpp
		${BUGTRAP_INCLUDE_DIR}/TextLogFile.h
		${BUGTRAP_INCLUDE_DIR}/TextView.cpp
		${BUGTRAP_INCLUDE_DIR}/TextView.h
		${BUGTRAP_INCLUDE_DIR}/ThemeXP.cpp
		${BUGTRAP_INCLUDE_DIR}/ThemeXP.h
		${BUGTRAP_INCLUDE_DIR}/TransferProgressDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/TransferProgressDlg.h
		${BUGTRAP_INCLUDE_DIR}/VersionInfo.h
		${BUGTRAP_INCLUDE_DIR}/WaitCursor.cpp
		${BUGTRAP_INCLUDE_DIR}/WaitCursor.h
		${BUGTRAP_INCLUDE_DIR}/WaitDlg.cpp
		${BUGTRAP_INCLUDE_DIR}/WaitDlg.h
		${BUGTRAP_INCLUDE_DIR}/XmlLogFile.cpp
		${BUGTRAP_INCLUDE_DIR}/XmlLogFile.h
		${BUGTRAP_INCLUDE_DIR}/XmlReader.cpp
		${BUGTRAP_INCLUDE_DIR}/XmlReader.h
		${BUGTRAP_INCLUDE_DIR}/XmlWriter.cpp
		${BUGTRAP_INCLUDE_DIR}/XmlWriter.h

		${ZLIB_INCLUDE_DIR}/src/adler32.c
		${ZLIB_INCLUDE_DIR}/src/compress.c
		${ZLIB_INCLUDE_DIR}/src/crc32.c
		${ZLIB_INCLUDE_DIR}/src/deflate.c
		${ZLIB_INCLUDE_DIR}/src/gzio.c
		${ZLIB_INCLUDE_DIR}/src/infback.c
		${ZLIB_INCLUDE_DIR}/src/inffast.c
		${ZLIB_INCLUDE_DIR}/src/inflate.c
		${ZLIB_INCLUDE_DIR}/src/inftrees.c
		${ZLIB_INCLUDE_DIR}/src/ioapi.c
		${ZLIB_INCLUDE_DIR}/src/iowin32.c
		${ZLIB_INCLUDE_DIR}/src/mztools.c
		${ZLIB_INCLUDE_DIR}/src/trees.c
		${ZLIB_INCLUDE_DIR}/src/uncompr.c
		${ZLIB_INCLUDE_DIR}/src/unzip.c
		${ZLIB_INCLUDE_DIR}/src/zip.c
		${ZLIB_INCLUDE_DIR}/src/zutil.c
	)

include_directories (
	${BUGTRAP_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}/Include
	)

set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:zlibSD.lib" )
set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:zlibS.lib" )
set( CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /NODEFAULTLIB:zlibS.lib" )

add_library ( BUGTRAP SHARED
		${SRCS}
	)

target_link_libraries( BUGTRAP
		ws2_32
		comctl32
		shlwapi
		version
		wininet
	)

install ( TARGETS BUGTRAP RUNTIME DESTINATION bin )
