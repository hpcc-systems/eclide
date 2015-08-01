project( ECLLIB )

set ( SRCS
		DefinitionParser.cpp
		DefinitionParser.h
		EclErrorParser.cpp
		EclErrorParser.h
		EclLib.rc
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

	${WTL_INCLUDE_DIR}/include
	)

if (Boost_FOUND)
	include_directories ( ${Boost_INCLUDE_DIRS} )
	link_directories ( ${Boost_LIBRARY_DIRS} )
endif (Boost_FOUND)


ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)
add_library ( ECLLIB SHARED
		${SRCS}
	)

add_dependencies ( ECLLIB
		CLIB
	)

target_link_libraries( ECLLIB
		CLIB
	)

install ( TARGETS ECLLIB RUNTIME DESTINATION bin )