PROJECT( COMMS )

find_package(boost_thread CONFIG REQUIRED)
find_package(boost_serialization CONFIG REQUIRED)
find_package(boost_filesystem CONFIG REQUIRED)
find_path(GSOAP_DIR stdsoap2.cpp HINTS ${VCPKG_ROOT_DIR}/share/gsoap)
find_path(GSOAP_BIN wsdl2h.exe HINTS ${VCPKG_ROOT_DIR}/share/gsoap/bin/win32)

SET (GEN_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapws_USCOREaccountServiceSoapProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapWsAttributesServiceSoapProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapWsDfuServiceSoapProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapWsSMCServiceSoapProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapWsTopologyServiceSoapProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/soapWsWorkunitsServiceSoapProxy.cpp
)

SET (GEN_DEPS
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/Ws_Account.wsdl
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/WsAttributes.wsdl
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/WsDfu.wsdl
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/WsSMC.wsdl
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/WsStore.wsdl 
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/WsTopology.wsdl
    ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0/WsWorkunits.wsdl
)

ADD_CUSTOM_COMMAND(
    OUTPUT ${GEN_SRCS}
    DEPENDS ${GEN_DEPS}
    COMMAND ${GSOAP_BIN}/wsdl2h.exe -I${GSOAP_DIR} -I./hpcc -o Services.h Ws_Account.wsdl WsAttributes.wsdl WsDfu.wsdl WsSMC.wsdl WsTopology.wsdl WsWorkunits.wsdl
    COMMAND ${GSOAP_BIN}/soapcpp2.exe -C -x -i -I${GSOAP_DIR}/import Services.h
    COMMAND ${GSOAP_BIN}/soapcpp2.exe -C -x -i -I${GSOAP_DIR}/import -penv ./hpcc/env.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/6_4_0
)

SET ( SRCS
    Account.cpp
    Account.h
    Attribute.cpp
    Attribute.h
    AttributeImpl.h
    AttributeHistory.cpp
    AttributeHistory.h
    AttributeType.cpp
    AttributeType.h
    Autoupdate.cpp
    Autoupdate.h
    Cluster.cpp
    Cluster.h
    comms.cpp
    comms.h
    Dali.cpp
    Dali.h
    Depository.h
    dfu.cpp
    dfu.h
    dfuFile.cpp
    dfuFile.h
    dfuServer.cpp
    dfuServer.h
    DiskAttribute.h
    DiskAttribute.cpp
    DiskAttributeHistory.cpp
    DiskModule.h
    DiskModule.cpp
    DiskMonitor.h
    DiskMonitor.cpp
    DiskRepository.cpp
    DiskRepository.h
    DropZone.cpp
    DropZone.h
    EclCC.cpp
    EclCC.h
    EclMeta.cpp
    EclMeta.h
    EclServer.cpp
    EclServer.h
    Graph.cpp
    Graph.h
    Group.cpp
    Group.h
    GSoapUtil.cpp
    GSoapUtil.h
    LocalFile.cpp
    LocalFile.h
    LocalResult.cpp
    LocalResult.h
    LocalWorkunit.cpp
    LocalWorkunit.h
    machine.h
    meta.h
    Migration.cpp
    Migration.h
    ModFileAttribute.cpp
    ModFileAttributeHistory.cpp
    ModFileModule.cpp
    ModFileRepository.cpp
    ModFileRepository.h
    Module.cpp
    Module.h
    ModuleHelper.cpp
    ModuleHelper.h
    Repository.cpp
    Repository.h
    RepositoryImpl.h
    RequestQueue.h
    resource.h
    Result.cpp
    Result.h
    ResultRequestQueue.cpp
    ResultRequestQueue.h
    SecureSoapSocketClient.cpp
    SecureSoapSocketClient.h
    SMC.cpp
    SMC.h
    SMCCluster.cpp
    SMCCluster.h
    SMCVersion.cpp
    SMCVersion.h
    Topology.cpp
    Topology.h
    WebService.cpp
    WebService.h
    Workspace.cpp
    Workspace.h
    WorkspaceItem.cpp
    WorkspaceItem.h
    Workunit.cpp
    Workunit.h
    XGMMLWorkunit.cpp
    XGMMLWorkunit.h
)

INCLUDE_DIRECTORIES (
    ../clib
    ../ecllib

    ${GSOAP_DIR}
    ${GSOAP_DIR}/extras
    ${GSOAP_DIR}/mod_gsoap/gsoap_win/wininet
)

ADD_DEFINITIONS( -DWITH_SOAPDEFS_H )

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_LIBRARY ( COMMS SHARED 
    ${SRCS}
    ${GEN_SRCS}
    comms.rc

    ${GSOAP_DIR}/stdsoap2.cpp
    ${GSOAP_DIR}/mod_gsoap/gsoap_win/wininet/gsoapWinInet.cpp
    ${GSOAP_DIR}/mod_gsoap/gsoap_win/wininet/gsoapWinInet.h
)

ADD_DEPENDENCIES ( COMMS
    CLIB
)

TARGET_LINK_LIBRARIES( COMMS PRIVATE
    CLIB
    ECLLIB
    msxml2
    comsupp
    authz

    Boost::thread
    Boost::serialization
    Boost::filesystem
)

INSTALL ( TARGETS COMMS RUNTIME DESTINATION bin )
