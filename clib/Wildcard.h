#pragma once

#include "clib.h"

CLIB_API bool WildMatch(const std::string & src, const std::string & pat, bool nocase = false);
CLIB_API bool WildMatch(const std::_tstring & src, const std::_tstring & pat, bool nocase = false);

