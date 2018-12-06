PROJECT( ECLLIB )

SET ( SRCS
    DefinitionParser.cpp
    DefinitionParser.h
    EclErrorParser.cpp
    EclErrorParser.h
    EclLib.cpp
    EclLib.h
    EclParser.cpp
    EclParser.h
    ecl_grammar.cpp
    ecl_grammar.h
    resource.h
    VersionParser.cpp
    VersionParser.h
)

include_directories (
    ../clib
    ../ecllib

    ${Boost_INCLUDE_DIRS}
    ${WTL_INCLUDE_DIR}
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_LIBRARY ( ECLLIB SHARED
    ${SRCS}
    EclLib.rc
)

ADD_DEPENDENCIES ( ECLLIB
    CLIB
)

TARGET_LINK_LIBRARIES( ECLLIB
    CLIB
    ${Boost_LIBRARIES}
)

INSTALL ( TARGETS ECLLIB RUNTIME DESTINATION bin )
