project( SGRID )

set ( SRCS
		atlsgrid.h
		atlstooltip.h
		ImageHelper.cpp
		ImageHelper.h
		PageLoadedView.cpp
		PageLoadedView.h
		resource.h
		sdksgrid.h
		sdkstooltip.h
		sgrid.rc
		sgrid.cpp
		sgrid.h
		sgridbase.cpp
		sgridbase.h

		GridCtrl_src/CellRange.h
		GridCtrl_src/GridCell.cpp
		GridCtrl_src/GridCell.h
		GridCtrl_src/GridCellBase.cpp
		GridCtrl_src/GridCellBase.h
		GridCtrl_src/GridCtrl.cpp
		GridCtrl_src/GridCtrl.h
		GridCtrl_src/GridDropTarget.cpp
		GridCtrl_src/GridDropTarget.h
		GridCtrl_src/InPlaceEdit.cpp
		GridCtrl_src/InPlaceEdit.h
		GridCtrl_src/MemDC.h
		GridCtrl_src/TitleTip.cpp
		GridCtrl_src/TitleTip.h
	)

include_directories (
	../sgrid
	../clib
	../wtlex

	${WTL_INCLUDE_DIR}/include
	)

add_definitions( -D_AFXDLL -D_USRDLL )
#ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)
add_library ( SGRID SHARED
		${SRCS}

		CxImage/tif_xfile.cpp
		CxImage/xfile.h
		CxImage/ximabmp.cpp
		CxImage/ximabmp.h
		CxImage/ximacfg.h
		CxImage/ximadef.h
		CxImage/ximadsp.cpp
		CxImage/ximaenc.cpp
		CxImage/ximaexif.cpp
		CxImage/ximage.cpp
		CxImage/ximage.h
		CxImage/ximagif.cpp
		CxImage/ximagif.h
		CxImage/ximahist.cpp
		CxImage/ximaico.cpp
		CxImage/ximaico.h
		CxImage/ximainfo.cpp
		CxImage/ximaint.cpp
		CxImage/ximaiter.h
		CxImage/ximaj2k.cpp
		CxImage/ximaj2k.h
		CxImage/ximajas.cpp
		CxImage/ximajas.h
		CxImage/ximajbg.cpp
		CxImage/ximajbg.h
		CxImage/ximajpg.cpp
		CxImage/ximajpg.h
		CxImage/ximalpha.cpp
		CxImage/ximalyr.cpp
		CxImage/ximamng.cpp
		CxImage/ximamng.h
		CxImage/ximapal.cpp
		CxImage/ximapcx.cpp
		CxImage/ximapcx.h
		CxImage/ximapng.cpp
		CxImage/ximapng.h
		CxImage/ximasel.cpp
		CxImage/ximatga.cpp
		CxImage/ximatga.h
		CxImage/ximath.cpp
		CxImage/ximath.h
		CxImage/ximatif.cpp
		CxImage/ximatif.h
		CxImage/ximatran.cpp
		CxImage/ximawbmp.cpp
		CxImage/ximawbmp.h
		CxImage/ximawmf.cpp
		CxImage/ximawmf.h
		CxImage/ximawnd.cpp
		CxImage/xiofile.h
		CxImage/xmemfile.cpp
		CxImage/xmemfile.h
	)

install ( TARGETS SGRID RUNTIME DESTINATION bin )