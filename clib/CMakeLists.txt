PROJECT( CLIB )

find_package(boost_filesystem CONFIG REQUIRED)
find_package(boost_serialization CONFIG REQUIRED)

SET ( SRCS
    Cache.h
    clib.cpp
    clib.h
    CmdLine.cpp
    CmdLine.h
    cmdProcess.cpp
    cmdProcess.h
    ControlScroll.h
    CustomMessages.h
    DaFileSrv.cpp
    DaFileSrv.h
    DLLVersion.cpp
    DLLVersion.h
    global.cpp
    global.h
    GridFocus.cpp
    GridFocus.h
    GridSelection.cpp
    GridSelection.h
    #IEFileCache.cpp
    #IEFileCache.h
    #iface.cpp
    #iface.h
    Iterators.h
    LimitSingleInstance.cpp
    LimitSingleInstance.h
    LockableEventQueue.h
    logger.cpp
    Logger.h
    md5.cpp
    PersistMap.cpp
    PersistMap.h
    RecursiveMutex.cpp
    RecursiveMutex.h
    resource.h
    ResultFormat.cpp
    ResultFormat.h
    RWMonitor.h
    SAXHandlers.cpp
    SAXHandlers.h
    SAXHandlersBase.cpp
    SAXHandlersBase.h
    Searchable.h
    SeisintDatasetSAXHandler.h
    ShellHelper.cpp
    ShellHelper.h
    Singleton.h
    SoapUtil.cpp
    SoapUtil.h
    stdatl.h
    stdboost.h
    stdplatform.h
    stdstl.h
    stdversion.h
    stdwtl.h
    StringSet.cpp
    StringSet.h
    Table.cpp
    Table.h
    TextCompare.cpp
    TextCompare.h
    Thread.cpp
    Thread.h
    Tree.cpp
    Tree.h
    UnicodeFile.cpp
    UnicodeFile.h
    UniqueID.cpp
    UniqueID.h
    Util.cpp
    Util.h
    utilDateTime.cpp
    utilDateTime.h
    UtilFilesystem.cpp
    UtilFilesystem.h
    Wildcard.cpp
    Wildcard.h
    XMLWriter.cpp
    XMLWriter.h

    ../wtlex/membm.cpp
    ../wtlex/membm.h
)

INCLUDE_DIRECTORIES (
    ../clib
    ../wtlex
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_LIBRARY ( CLIB SHARED
    ${SRCS}
    clib.rc
)

TARGET_LINK_LIBRARIES( CLIB PRIVATE 
    Wininet
    Ws2_32
    msxml2
    comsupp
    version
    Htmlhelp
    Boost::filesystem
    Boost::serialization
)

INSTALL ( TARGETS CLIB RUNTIME DESTINATION bin )
