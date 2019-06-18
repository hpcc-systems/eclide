PROJECT( BUGTRAP )

SET ( SRCS
    ${BUGTRAP_DIR}/AboutDlg.cpp
    ${BUGTRAP_DIR}/AboutDlg.h
    ${BUGTRAP_DIR}/AnimProgressBar.cpp
    ${BUGTRAP_DIR}/AnimProgressBar.h
    ${BUGTRAP_DIR}/Array.h
    ${BUGTRAP_DIR}/AssemblyInfo.cpp
    ${BUGTRAP_DIR}/BaseStream.h
    ${BUGTRAP_DIR}/BTAtlWindow.h
    ${BUGTRAP_DIR}/BTMfcWindow.h
    ${BUGTRAP_DIR}/BTTrace.h
    ${BUGTRAP_DIR}/Buffer.h
    ${BUGTRAP_DIR}/BugTrap.rc
    ${BUGTRAP_DIR}/BugTrap.cpp
    ${BUGTRAP_DIR}/BugTrap.h
    ${BUGTRAP_DIR}/BugTrapNet.cpp
    ${BUGTRAP_DIR}/BugTrapNet.h
    ${BUGTRAP_DIR}/BugTrapUI.cpp
    ${BUGTRAP_DIR}/BugTrapUI.h
    ${BUGTRAP_DIR}/BugTrapUtils.cpp
    ${BUGTRAP_DIR}/BugTrapUtils.h
    ${BUGTRAP_DIR}/CMapi.cpp
    ${BUGTRAP_DIR}/CMapi.h
    ${BUGTRAP_DIR}/ColHelper.cpp
    ${BUGTRAP_DIR}/ColHelper.h
    ${BUGTRAP_DIR}/DescribeErrorDlg.cpp
    ${BUGTRAP_DIR}/DescribeErrorDlg.h
    ${BUGTRAP_DIR}/Encoding.cpp
    ${BUGTRAP_DIR}/Encoding.h
    ${BUGTRAP_DIR}/EnumProcess.cpp
    ${BUGTRAP_DIR}/EnumProcess.h
    ${BUGTRAP_DIR}/FileStream.cpp
    ${BUGTRAP_DIR}/FileStream.h
    ${BUGTRAP_DIR}/Globals.cpp
    ${BUGTRAP_DIR}/Globals.h
    ${BUGTRAP_DIR}/Hash.h
    ${BUGTRAP_DIR}/HexView.cpp
    ${BUGTRAP_DIR}/HexView.h
    ${BUGTRAP_DIR}/HyperLink.cpp
    ${BUGTRAP_DIR}/HyperLink.h
    ${BUGTRAP_DIR}/ImageView.cpp
    ${BUGTRAP_DIR}/ImageView.h
    ${BUGTRAP_DIR}/InMemLogFile.cpp
    ${BUGTRAP_DIR}/InMemLogFile.h
    ${BUGTRAP_DIR}/InputStream.cpp
    ${BUGTRAP_DIR}/InputStream.h
    ${BUGTRAP_DIR}/InterfacePtr.h
    ${BUGTRAP_DIR}/LayoutManager.cpp
    ${BUGTRAP_DIR}/LayoutManager.h
    ${BUGTRAP_DIR}/LeakWatcher.h
    ${BUGTRAP_DIR}/List.h
    ${BUGTRAP_DIR}/LogFile.cpp
    ${BUGTRAP_DIR}/LogFile.h
    ${BUGTRAP_DIR}/LogLink.h
    ${BUGTRAP_DIR}/LogStream.cpp
    ${BUGTRAP_DIR}/LogStream.h
    ${BUGTRAP_DIR}/MachineInfoDlg.cpp
    ${BUGTRAP_DIR}/MachineInfoDlg.h
    ${BUGTRAP_DIR}/MachineStateDlg.cpp
    ${BUGTRAP_DIR}/MachineStateDlg.h
    ${BUGTRAP_DIR}/MainDlg.cpp
    ${BUGTRAP_DIR}/MainDlg.h
    ${BUGTRAP_DIR}/MemStream.cpp
    ${BUGTRAP_DIR}/MemStream.h
    ${BUGTRAP_DIR}/ModuleImportTable.cpp
    ${BUGTRAP_DIR}/ModuleImportTable.h
    ${BUGTRAP_DIR}/NetThunks.cpp
    ${BUGTRAP_DIR}/NetThunks.h
    ${BUGTRAP_DIR}/OutputStream.cpp
    ${BUGTRAP_DIR}/OutputStream.h
    ${BUGTRAP_DIR}/PreviewDlg.cpp
    ${BUGTRAP_DIR}/PreviewDlg.h
    ${BUGTRAP_DIR}/ResManager.cpp
    ${BUGTRAP_DIR}/ResManager.h
    ${BUGTRAP_DIR}/resource.h
    ${BUGTRAP_DIR}/SendMailDlg.cpp
    ${BUGTRAP_DIR}/SendMailDlg.h
    ${BUGTRAP_DIR}/SimpleDlg.cpp
    ${BUGTRAP_DIR}/SimpleDlg.h
    ${BUGTRAP_DIR}/SmartPtr.h
    ${BUGTRAP_DIR}/Splitter.cpp
    ${BUGTRAP_DIR}/Splitter.h
    ${BUGTRAP_DIR}/Stream.h
    ${BUGTRAP_DIR}/StrHolder.cpp
    ${BUGTRAP_DIR}/StrHolder.h
    ${BUGTRAP_DIR}/StrStream.cpp
    ${BUGTRAP_DIR}/StrStream.h
    ${BUGTRAP_DIR}/SymEngine.cpp
    ${BUGTRAP_DIR}/SymEngine.h
    ${BUGTRAP_DIR}/SymEngineNet.cpp
    ${BUGTRAP_DIR}/SymEngineNet.h
    ${BUGTRAP_DIR}/TextFormat.cpp
    ${BUGTRAP_DIR}/TextFormat.h
    ${BUGTRAP_DIR}/TextLogFile.cpp
    ${BUGTRAP_DIR}/TextLogFile.h
    ${BUGTRAP_DIR}/TextView.cpp
    ${BUGTRAP_DIR}/TextView.h
    ${BUGTRAP_DIR}/ThemeXP.cpp
    ${BUGTRAP_DIR}/ThemeXP.h
    ${BUGTRAP_DIR}/TransferProgressDlg.cpp
    ${BUGTRAP_DIR}/TransferProgressDlg.h
    ${BUGTRAP_DIR}/VersionInfo.h
    ${BUGTRAP_DIR}/WaitCursor.cpp
    ${BUGTRAP_DIR}/WaitCursor.h
    ${BUGTRAP_DIR}/WaitDlg.cpp
    ${BUGTRAP_DIR}/WaitDlg.h
    ${BUGTRAP_DIR}/XmlLogFile.cpp
    ${BUGTRAP_DIR}/XmlLogFile.h
    ${BUGTRAP_DIR}/XmlReader.cpp
    ${BUGTRAP_DIR}/XmlReader.h
    ${BUGTRAP_DIR}/XmlWriter.cpp
    ${BUGTRAP_DIR}/XmlWriter.h

    ${BUGTRAP_ZLIB_DIR}/src/adler32.c
    ${BUGTRAP_ZLIB_DIR}/src/compress.c
    ${BUGTRAP_ZLIB_DIR}/src/crc32.c
    ${BUGTRAP_ZLIB_DIR}/src/deflate.c
    ${BUGTRAP_ZLIB_DIR}/src/gzio.c
    ${BUGTRAP_ZLIB_DIR}/src/infback.c
    ${BUGTRAP_ZLIB_DIR}/src/inffast.c
    ${BUGTRAP_ZLIB_DIR}/src/inflate.c
    ${BUGTRAP_ZLIB_DIR}/src/inftrees.c
    ${BUGTRAP_ZLIB_DIR}/src/ioapi.c
    ${BUGTRAP_ZLIB_DIR}/src/iowin32.c
    ${BUGTRAP_ZLIB_DIR}/src/mztools.c
    ${BUGTRAP_ZLIB_DIR}/src/trees.c
    ${BUGTRAP_ZLIB_DIR}/src/uncompr.c
    ${BUGTRAP_ZLIB_DIR}/src/unzip.c
    ${BUGTRAP_ZLIB_DIR}/src/zip.c
    ${BUGTRAP_ZLIB_DIR}/src/zutil.c
)

INCLUDE_DIRECTORIES (
    source
    ${BUGTRAP_ZLIB_DIR}/include
)

SET( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:zlibSD.lib" )
SET( CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:zlibS.lib" )
SET( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:zlibS.lib" )
SET( CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /NODEFAULTLIB:zlibS.lib" )

ADD_LIBRARY ( BUGTRAP SHARED
    ${SRCS}
)

TARGET_LINK_LIBRARIES( BUGTRAP
    ws2_32
    comctl32
    shlwapi
    version
    wininet
)

INSTALL ( TARGETS BUGTRAP RUNTIME DESTINATION bin )
