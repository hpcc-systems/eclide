project( EN_US )

set ( SRCS
		en_us.rc
		en_us.cpp
		en_us.h
		resource.h
	)

include_directories (
	../clib
	)

if (Boost_FOUND)
	include_directories ( ${Boost_INCLUDE_DIRS} )
	link_directories ( ${Boost_LIBRARY_DIRS} )
endif (Boost_FOUND)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)
add_library ( EN_US SHARED
		${SRCS}
	)

install ( TARGETS EN_US RUNTIME DESTINATION bin )
