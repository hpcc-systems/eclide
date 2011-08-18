#pragma once

#include "resource.h"		// main symbols

#ifdef EN_US_EXPORTS
#  define EN_US_API __declspec(dllexport)
#else
#  define EN_US_API __declspec(dllimport)
#endif

