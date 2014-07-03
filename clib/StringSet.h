#pragma once

#include "clib.h"

typedef std::vector<std::_tstring> StdStringVector;
CLIB_API const TCHAR * SortedSerialize(StdStringVector & source, std::_tstring & target, const TCHAR * delim = _T(" "));                        //source will be sorted before serialize
CLIB_API StdStringVector::size_type SortedDeserialize(const std::_tstring & source, StdStringVector & target, const TCHAR * delim = _T(" "));   //target will be sorted

