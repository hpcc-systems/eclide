#pragma once

#include "resource.h"		// main symbols

#ifdef SEISINT_LIBEXPORTS
#  define SGRID_API
#else
#  ifdef SGRID_EXPORTS
#    define SGRID_API __declspec(dllexport)
#  else
#    define SGRID_API __declspec(dllimport)
#  endif
#endif





