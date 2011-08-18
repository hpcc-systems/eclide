#pragma once

#include "clib.h"

typedef std::vector<std::_tstring> StdStringVector;
CLIB_API const TCHAR * Serialize(StdStringVector & source, std::_tstring & target);	//source will be sorted

