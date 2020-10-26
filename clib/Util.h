#pragma once

#include "clib.h"

CLIB_API void SetClipboard(const std::_tstring & txt);
CLIB_API void CopyBitmapToClipboard(HBITMAP hBitmap, HPALETTE hPal = 0);
CLIB_API const TCHAR * CheckExtension(const std::_tstring & path, const std::string & defaultExt, std::_tstring & ext);
CLIB_API bool HasClipboardText();
CLIB_API unsigned GetClipboardText(std::_tstring& cbtext);
CLIB_API const TCHAR * EncodeForCSV(const TCHAR *csv, std::_tstring &encoded);
CLIB_API int DecodeCSV(const std::_tstring & line, std::vector<std::_tstring> &decoded);
CLIB_API const TCHAR * EncodeForEclNoQuote(const TCHAR *rawString, std::_tstring &encoded, bool ucescape=false);
CLIB_API const TCHAR * EncodeForEcl(const TCHAR *rawString, std::_tstring &encoded, bool ucescape=false);
CLIB_API const TCHAR * DecodeFromEcl(const TCHAR *rawString, std::_tstring & decoded);
CLIB_API const TCHAR *IntToString(int i, std::_tstring &str); 
CLIB_API bool StringToInt(const TCHAR* from, int &to);
CLIB_API bool StringToInt(const TCHAR* from, __int64 &to);
CLIB_API void TidyCRLF(std::_tstring & source);
CLIB_API const TCHAR* Format(std::_tstring& str, const TCHAR* fmt ...);
CLIB_API bool AddSeparator(std::_tstring& path);
CLIB_API std::_tstring& MakeLower(std::_tstring& str);
CLIB_API void Replace(std::_tstring& str, TCHAR replace, const std::_tstring& with);
CLIB_API void Replace(std::_tstring& str, const TCHAR* replace, const std::_tstring& with);
CLIB_API const TCHAR * EscapeXML(const std::_tstring & source, std::_tstring & target, bool escapeQuotes = false);
CLIB_API const TCHAR * UnescapeXML(const std::_tstring & source, std::_tstring & target);
CLIB_API const TCHAR * GetActiveXDLLPath(const std::_tstring & clsID, std::_tstring & path);
CLIB_API const TCHAR * GetActiveXDLLFolder(const std::_tstring & clsID, std::_tstring & folder);
CLIB_API int MoveToRecycleBin(const std::_tstring & path);
CLIB_API const TCHAR * GetLocale(std::_tstring & locale);
CLIB_API void ShowHelp(const std::_tstring word);
CLIB_API bool GetPluginECL(const std::_tstring & pluginPath, std::_tstring & ecl);

//  Should be in wlib?
void CLIB_API InsertSeparator(WTL::CMenu &menu);
BOOL CLIB_API InsertMenuItem(WTL::CMenu &menu, int id, const TCHAR *item);
BOOL CLIB_API InsertMenuItem(WTL::CMenu &menu, int pos, int id, const TCHAR *item);

CLIB_API int round_int(double x);
CLIB_API int floor_int (double x);
CLIB_API int ceil_int (double x);

#define MAKEVERSION(major, minor) (ULONG)(MAKELONG(minor,major))

CLIB_API const TCHAR * GetVersion(const std::_tstring & dllLabel, std::_tstring & version);
CLIB_API ULONG GetComCtl32Version();

#ifndef SEISINT_LIBEXPORTS
CLIB_API const TCHAR * GetMyIP(std::_tstring & ip);
#endif

//  comsupp.lib does not have this function in MBCS builds...
#ifndef SEISINT_LIBEXPORTS
#ifndef UNICODE
namespace _com_util 
{
    CLIB_API BSTR __stdcall ConvertStringToBSTR(const char* pSrc);
}
#endif
#endif