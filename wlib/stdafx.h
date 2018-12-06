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

//  ATL  ---
#include "stdatl.h"

//  WTL  ---
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#define _WTL_NEW_PAGE_NOTIFY_HANDLERS
#include <wtl/atlapp.h>
#define _Module (*_pModule)
#include "stdwtl.h"

//  WTLEX  ---
#include "atlwinmisc.h"
#include "CustomAutoComplete.h"
#include "atlgdix.h"
#include "DockMisc.h"
#include "BevelLine.h"
#include "dialogex.h"

#undef _Module

// Scintilla  --
#define SCI_NAMESPACE
#include "platform.h"
#include "SciLexer.h"
#include "atlscintilla.h"

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

#pragma warning(3:4702;error:4702)	  //warning C4702: unreachable code - make level 3 warning
