// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
# define _CRTDBG_MAP_ALLOC
#endif

#include <stdversion.h> //clib

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
# include <stdlib.h>
# include <crtdbg.h>
// # define GJS_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
// # define new GJS_DEBUG_NEW 
#endif