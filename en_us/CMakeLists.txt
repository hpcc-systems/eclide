PROJECT( EN_US )

SET ( SRCS
    en_us.cpp
    en_us.h
    resource.h
)

INCLUDE_DIRECTORIES (
../clib
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)

ADD_LIBRARY ( EN_US SHARED
    ${SRCS}
    en_us.rc
)

INSTALL ( TARGETS EN_US RUNTIME DESTINATION bin )
