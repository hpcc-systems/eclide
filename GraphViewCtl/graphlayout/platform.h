#pragma once

#pragma warning(disable:4251)
//#pragma warning(disable:4275)
#pragma warning(disable:4503)
//#pragma warning(disable:4127)

//  Leak Checking ---
#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif

#ifdef WIN32
#include "targetver.h"
#endif

#include <stdio.h>
#ifdef WIN32
#include <tchar.h>
#else
#define TCHAR wchar_t
#endif

#include <iostream>
#include <string>
#include <map>

#if defined(_DEBUGXXX)
# include <stdlib.h>
# include <crtdbg.h>
# define GJS_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
# define new GJS_DEBUG_NEW
#endif
