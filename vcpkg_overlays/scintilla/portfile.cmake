include(vcpkg_common_functions)

vcpkg_check_linkage(ONLY_DYNAMIC_LIBRARY ONLY_DYNAMIC_CRT)

vcpkg_download_distfile(ARCHIVE
  URLS "http://www.scintilla.org/scintilla376.zip"
  FILENAME "scintilla376.zip"
  SHA512 618a50405eede3277d7696ac58122aeeb490d10ae392c60c7f78baaa96c965a8e1a599948e0ebd61bed7f75894b01bdf4574a0e5d0e20996bfdfb2e1bdb33203
)

vcpkg_extract_source_archive_ex(
  OUT_SOURCE_PATH SOURCE_PATH
  ARCHIVE ${ARCHIVE}
  REF 3.7.6
  PATCHES 
    "use-string-in-platwin.patch"  
)

file(INSTALL ${SOURCE_PATH}/include DESTINATION ${CURRENT_PACKAGES_DIR}/include/scintilla)
file(INSTALL ${SOURCE_PATH}/src DESTINATION ${CURRENT_PACKAGES_DIR}/share/scintilla)
file(INSTALL ${SOURCE_PATH}/LexLib DESTINATION ${CURRENT_PACKAGES_DIR}/share/scintilla)
file(INSTALL ${SOURCE_PATH}/Win32 DESTINATION ${CURRENT_PACKAGES_DIR}/share/scintilla)
file(INSTALL ${SOURCE_PATH}/License.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/scintilla RENAME copyright)
