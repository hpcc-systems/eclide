#pragma once

#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4503)
#pragma warning(disable:4127)

#define _COMMS_VER								300800

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
#define _CRTDBG_MAP_ALLOC
#if _COMMS_VER < 68200
#ifdef _DEBUG
#define _ATLSOAP_TRACE_XML
#endif
#endif
#endif

//  Target  ---
#include "stdversion.h"

// Platform SDK  ---
#include "stdplatform.h"

//  ATL  ---
#include "stdatl.h"

//  WTL  ---
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#include <atlapp.h>
#define _Module (*_pModule)
#include "stdwtl.h"

//  WTLEX  ---
#undef _Module

//  STL  ---
#include "stdstl.h"

//  BOOST  ---
#include "stdboost.h"
#include <boost/asio.hpp>

//  Services  ---
const TCHAR * const _COMMS_VER_STR = _T("build_3_08");
#if _COMMS_VER == 471
#  include "471/WebService.h"
#elif _COMMS_VER == 491
#  include "491/WebService.h"
#elif _COMMS_VER == 499
#  include "499/WebService.h"
#elif _COMMS_VER == 505
#  include "505/WebService.h"
#elif _COMMS_VER == 50509
#  include "50509/WebService.h"
#elif _COMMS_VER == 50514
#  include "50514/WebService.h"
#elif _COMMS_VER == 50515
#  include "50515/WebService.h"
#elif _COMMS_VER == 50520
#  include "50520/WebService.h"
#elif _COMMS_VER == 50523
#  include "50523/WebService.h"
#elif _COMMS_VER == 51100
#  include "51100/WebService.h"
#elif _COMMS_VER == 51300
#  include "51300/WebService.h"
#elif _COMMS_VER == 51400
#  include "51400/WebService.h"
#elif _COMMS_VER == 52501
#  include "52501/WebService.h"
#elif _COMMS_VER == 53300
#  include "53300/WebService.h"
#elif _COMMS_VER == 53306
#  include "53306/WebService.h"
#elif _COMMS_VER == 53313
#  include "53313/WebService.h"
#elif _COMMS_VER == 53317		//17 is the same as 13 just adding for SR5 release
#  include "53313/WebService.h"
#elif _COMMS_VER == 53600
#  include "53600/WebService.h"
#elif _COMMS_VER == 54400
#  include "54400/WebService.h"
#elif _COMMS_VER == 54700
#  include "54700/WebService.h"
#elif _COMMS_VER == 54800
#  include "54800/WebService.h"
#elif _COMMS_VER == 55000
#  include "55000/WebService.h"
#elif _COMMS_VER == 55100
#  include "55100/WebService.h"
#elif _COMMS_VER == 55800
#  include "55800/WebService.h"
#elif _COMMS_VER == 56400
#  include "56400/WebService.h"
#elif _COMMS_VER == 57600
#  include "57600/WebService.h"
#elif _COMMS_VER == 57700
#  include "57700/WebService.h"
#elif _COMMS_VER == 58300
#  include "58300/WebService.h"
#elif _COMMS_VER == 58600
#  include "58600/WebService.h"
#elif _COMMS_VER == 58800
#  include "58800/WebService.h"
#elif _COMMS_VER == 60600
#  include "60600/WebService.h"
#elif _COMMS_VER == 61200
#  include "61200/WebService.h"
#elif _COMMS_VER == 62808
#  include "62808/WebService.h"
#elif _COMMS_VER == 65101
#  include "65101/WebService.h"
#elif _COMMS_VER == 67205
#  include "67205/WebService.h"
#elif _COMMS_VER == 67300
#  include "67300/WebService.h"
#elif _COMMS_VER == 68200
#  include "68200/WebService.h"
#elif _COMMS_VER == 68201
#  include "68201/WebService.h"
#elif _COMMS_VER == 300800
#  include "3_08/WebService.h"
#else
#  include "WebService.h"
#endif

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
# include <stdlib.h>
# include <crtdbg.h>
# define GJS_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
# define new GJS_DEBUG_NEW 
#endif

#pragma warning(3:4702; error:4702)	 //warning C4702: unreachable code
