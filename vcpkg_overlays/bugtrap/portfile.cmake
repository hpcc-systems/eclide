# Common Ambient Variables:
#   CURRENT_BUILDTREES_DIR    = ${VCPKG_ROOT_DIR}\buildtrees\${PORT}
#   CURRENT_PACKAGES_DIR      = ${VCPKG_ROOT_DIR}\packages\${PORT}_${TARGET_TRIPLET}
#   CURRENT_PORT_DIR          = ${VCPKG_ROOT_DIR}\ports\${PORT}
#   PORT                      = current port name (zlib, etc)
#   TARGET_TRIPLET            = current triplet (x86-windows, x64-windows-static, etc)
#   VCPKG_CRT_LINKAGE         = C runtime linkage type (static, dynamic)
#   VCPKG_LIBRARY_LINKAGE     = target library linkage type (static, dynamic)
#   VCPKG_ROOT_DIR            = <C:\path\to\current\vcpkg>
#   VCPKG_TARGET_ARCHITECTURE = target architecture (x64, x86, arm)
#

include(vcpkg_common_functions)
set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src/BugTrap-${VERSION})
vcpkg_download_distfile(ARCHIVE
    URLS "https://github.com/bchavez/BugTrap/archive/v${VERSION}.tar.gz"
    FILENAME "BugTrap-${VERSION}.tar.gz"
    SHA512 67df0f45a0c0078a749483fdd6cde9851fb22437da353b3727c7194210637863f118c67caef31d3450ca4462b008297e552fcf2cb6e1bef45a89edff1f71cb28
)
vcpkg_extract_source_archive(${ARCHIVE})

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/include ${CURRENT_PACKAGES_DIR}/src)

file(COPY ${SOURCE_PATH}/source/Client/BugTrap.h DESTINATION ${CURRENT_PACKAGES_DIR}/include/bugtrap)

file(INSTALL ${SOURCE_PATH}/source DESTINATION ${CURRENT_PACKAGES_DIR}/share/bugtrap)
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/bugtrap RENAME copyright)

