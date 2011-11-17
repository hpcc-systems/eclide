project( CLIB )

set ( SRCS
		Cache.h
		clib.rc
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
		GraphContainer.cpp
		GraphContainer.h
		GraphContainerImpl.cpp
		GraphContainerImpl.h
		GraphEdgeImpl.cpp
		GraphEdgeImpl.h
		GraphIteratorImpl.h
		GraphSortedIndexContainer.cpp
		GraphSortedIndexContainer.h
		GraphSubgraphImpl.cpp
		GraphSubgraphImpl.h
		GraphTypes.cpp
		GraphTypes.h
		GraphVertexImpl.cpp
		GraphVertexImpl.h
		GridFocus.cpp
		GridFocus.h
		GridSelection.cpp
		GridSelection.h
		Hwprof.cpp
		Hwprof.h
		#IEFileCache.cpp
		#IEFileCache.h
		#iface.cpp
		#iface.h
		IPictureHelper.cpp
		IPictureHelper.h
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
		Wildcard.cpp
		Wildcard.h
		XGMMLSaxHandler.cpp
		XGMMLSaxHandler.h
		XMLWriter.cpp
		XMLWriter.h

		../wtlex/membm.cpp
		../wtlex/membm.h
	)

include_directories (
	../clib
	../wtlex
	
	${WTL_INCLUDE_DIR}/include
	)

if (Boost_FOUND)
	include_directories ( ${Boost_INCLUDE_DIRS} )
	link_directories ( ${Boost_LIBRARY_DIRS} )
endif (Boost_FOUND)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" SRCS)
add_library ( CLIB SHARED
		${SRCS}
	)

target_link_libraries( CLIB
		Wininet
		Ws2_32
		msxml2
		comsupp
		version
		Htmlhelp
	)

install ( TARGETS CLIB RUNTIME DESTINATION bin )