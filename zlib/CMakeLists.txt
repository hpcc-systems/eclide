project( ZLIB )

set ( SRCS
		${ZLIB_INCLUDE_DIR}/adler32.c
		${ZLIB_INCLUDE_DIR}/compress.c
		${ZLIB_INCLUDE_DIR}/crc32.c
		${ZLIB_INCLUDE_DIR}/deflate.c
		${ZLIB_INCLUDE_DIR}/gzio.c
		${ZLIB_INCLUDE_DIR}/infback.c
		${ZLIB_INCLUDE_DIR}/inffast.c
		${ZLIB_INCLUDE_DIR}/inflate.c
		${ZLIB_INCLUDE_DIR}/inftrees.c
		${ZLIB_INCLUDE_DIR}/contrib/minizip/ioapi.c
		${ZLIB_INCLUDE_DIR}/contrib/minizip/iowin32.c
		${ZLIB_INCLUDE_DIR}/contrib/minizip/mztools.c
		${ZLIB_INCLUDE_DIR}/trees.c
		${ZLIB_INCLUDE_DIR}/uncompr.c
		${ZLIB_INCLUDE_DIR}/contrib/minizip/unzip.c
		${ZLIB_INCLUDE_DIR}/contrib/minizip/zip.c
		${ZLIB_INCLUDE_DIR}/zutil.c

		${ZLIB_INCLUDE_DIR}/zlib.def
	)

include_directories (
		${ZLIB_INCLUDE_DIR}
		${ZLIB_INCLUDE_DIR}/contrib/minizip
	)

add_library ( ZLIB SHARED
		${SRCS}
	)

target_link_libraries( ZLIB
	)

install ( TARGETS ZLIB RUNTIME DESTINATION bin )