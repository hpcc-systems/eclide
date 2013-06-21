#pragma once

#ifdef SEISINT_LIBEXPORTS
#  define WLIB_API
#else
#  ifdef WLIB_EXPORTS
#    define WLIB_API __declspec(dllexport)
#  else
#    define WLIB_API __declspec(dllimport)
#  endif
#endif

WLIB_API const TCHAR *GetRegPathQB(const std::_tstring & overideVersion = _T(""));
WLIB_API const TCHAR *GetAppVersion(std::_tstring & version);
WLIB_API const TCHAR *GetAppMajorVersion(std::_tstring & version);
WLIB_API const TCHAR *GetBuildVersion(std::_tstring & version);
