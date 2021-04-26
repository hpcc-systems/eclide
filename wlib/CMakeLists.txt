PROJECT( WLIB )

SET ( SRCS
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
    CefClientHandler.h
    CefClientHandler.cpp
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
    HListBox.h
    HtmlView.h
    HtmlIEView.cpp
    HtmlChromiumView.cpp
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

INCLUDE_DIRECTORIES (
    ../clib
    ../ecllib
    ../sgrid
    ../comms
    ../wtlex
    ../dockingwindows
    ../EclEditor
    
    ${WTL_INCLUDE_DIR}
    ${SCINTILLA_INCLUDE_DIR}
    ${CEF_ROOT_DIR}
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_LIBRARY ( WLIB SHARED
    ${SRCS}
    wlib.rc
)

ADD_DEPENDENCIES ( WLIB
    CLIB
    COMMS
    SGRID
)

TARGET_LINK_LIBRARIES( WLIB
    CLIB
    COMMS
    SGRID
)

TARGET_LINK_LIBRARIES( WLIB
    debug ${CEF_ROOT_DIR}/Debug/lib/libcef_dll_wrapper.lib
    optimized ${CEF_ROOT_DIR}/lib/libcef_dll_wrapper.lib
)

TARGET_LINK_LIBRARIES( WLIB
    debug ${CEF_ROOT_DIR}/Debug/libcef.lib
    optimized ${CEF_ROOT_DIR}/Release/libcef.lib
)

INSTALL ( TARGETS WLIB RUNTIME DESTINATION bin )

INSTALL ( FILES 
    ${CEF_ROOT_DIR}/Release/chrome_elf.dll
    ${CEF_ROOT_DIR}/Release/d3dcompiler_47.dll
    ${CEF_ROOT_DIR}/Release/libcef.dll
    ${CEF_ROOT_DIR}/Release/libEGL.dll
    ${CEF_ROOT_DIR}/Release/libGLESv2.dll
    ${CEF_ROOT_DIR}/Release/natives_blob.bin
    ${CEF_ROOT_DIR}/Release/snapshot_blob.bin
    ${CEF_ROOT_DIR}/Release/v8_context_snapshot.bin
    ${CEF_ROOT_DIR}/Resources/icudtl.dat
    ${CEF_ROOT_DIR}/Resources/cef.pak
    ${CEF_ROOT_DIR}/Resources/cef_100_percent.pak
    ${CEF_ROOT_DIR}/Resources/cef_200_percent.pak
    ${CEF_ROOT_DIR}/Resources/cef_extensions.pak
    ${CEF_ROOT_DIR}/Resources/devtools_resources.pak
    DESTINATION bin 
)

INSTALL ( FILES 
    ${CEF_ROOT_DIR}/Release/swiftshader/libEGL.dll
    ${CEF_ROOT_DIR}/Release/swiftshader/libGLESv2.dll
    DESTINATION bin/swiftshader
)

INSTALL ( FILES 
    ${CEF_ROOT_DIR}/Resources/locales/en-US.pak
    DESTINATION bin/locales
)
