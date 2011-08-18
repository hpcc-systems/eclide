#pragma once
#include "clib.h"

CLIB_API bool ShellExecOpen(const TCHAR *fileName);
CLIB_API bool ShellExecExplore(const TCHAR *fileName);
CLIB_API const TCHAR * FindExecutableForDocument(const TCHAR *fileName, std::_tstring & result);
CLIB_API const TCHAR * DocumentTypeName(const TCHAR *fileName, std::_tstring & result);
