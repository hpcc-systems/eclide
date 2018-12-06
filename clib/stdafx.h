#pragma once

#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4503)
#pragma warning(disable:4127)

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
#define _CRTDBG_MAP_ALLOC
#endif

//  Target  ---
#include "stdversion.h"

// Platform SDK  ---
#include "stdplatform.h"
#include "wininet.h"

//  ATL  ---
#include "stdatl.h"
#include "atltime.h"

//  WTL  ---
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#include <wtl/atlapp.h>
#define _Module (*_pModule)
#include "stdwtl.h"

//  WTLEX  ---
#include "atlwinmisc.h"
#undef _Module

//  ATLSERVER  ---
//#include <atlhtml.h>
//#include <atlsoap.h>

//  STL  ---
#include "stdstl.h"

//  BOOST  ---
#include "stdboost.h"

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
# include <stdlib.h>
# include <crtdbg.h>
# define GJS_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
# define new GJS_DEBUG_NEW 
#endif
