#include "stdafx.h"

#include "Util.h"
#include "global.h"
#include <HtmlHelp.h>

static ULONG COMCTL_VERSION=0; 

void SetClipboard(const std::_tstring & txt)
{
	std::string s = CT2A(txt.c_str(), CP_UTF8);  //No CP_UTF8 needed
	::OpenClipboard(NULL);
	::EmptyClipboard();

	int chars = s.length() + 1;
	HGLOBAL hand = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, chars);
	if (hand) 
	{
		char *ptr = static_cast<char *>(::GlobalLock(hand));
		memcpy(ptr, s.c_str(), chars);
		::GlobalUnlock(hand);
	}
	::SetClipboardData(CF_TEXT, hand);

#ifdef _UNICODE
	int uchars = txt.length() + 1;
	HGLOBAL uhand = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(TCHAR) * (uchars));
	if (uhand) 
	{
		wchar_t *uptr = static_cast<wchar_t *>(::GlobalLock(uhand));
		wmemcpy(uptr, txt.c_str(), uchars);
		::GlobalUnlock(uhand);
	}
	::SetClipboardData(CF_UNICODETEXT, uhand);
#endif

	::CloseClipboard();
}

void CopyBitmapToClipboard(HBITMAP hBitmap, HPALETTE hPal)
{
	::OpenClipboard(NULL);
	::EmptyClipboard() ;
	if(hPal)
		::SetClipboardData(CF_PALETTE, hPal);
	::SetClipboardData(CF_BITMAP, hBitmap);
	::CloseClipboard();
}

bool HasClipboardText()
{
	bool retVal = false;
	if(::OpenClipboard(0))
	{
		if(::IsClipboardFormatAvailable(CF_UNICODETEXT) || ::IsClipboardFormatAvailable(CF_TEXT))
			retVal = true;
		::CloseClipboard();
	}
	return retVal;
}

unsigned GetClipboardText(std::_tstring& cbtext)
{
	unsigned len=0;

	if(::OpenClipboard(0))
	{
		if(::IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			HANDLE hText=::GetClipboardData(CF_UNICODETEXT);
			if(hText)
			{
				const WCHAR* pwt=static_cast<const WCHAR*>(::GlobalLock(hText));
				if(pwt)
				{
					CW2T cv(pwt);
					cbtext.assign(cv);
					len=cbtext.length();
				}
				::GlobalUnlock(hText);
			}
		}
		else if(::IsClipboardFormatAvailable(CF_TEXT))
		{
			HANDLE hText=::GetClipboardData(CF_TEXT); // ASCII
			if(hText)
			{
				const char* pat=static_cast<const char*>(::GlobalLock(hText));
				if(pat)
				{
					CA2T cv(pat);
					cbtext.assign(cv);
					len=cbtext.length();
				}
				::GlobalUnlock(hText);
			}
		}

		::CloseClipboard();
	}

	return len;
}

const TCHAR * CheckExtension(const std::_tstring & path, const std::string & defaultExt, std::_tstring & ext)
{
	try
	{
		boost::filesystem::path p(CT2A(path.c_str()), boost::filesystem::native);
		std::string d = extension(p);
		if (extension(p).length() == 0)
			p = change_extension(p, defaultExt);
		ext = CA2T(p.native_file_string().c_str());
		return ext.c_str();
	}
	catch (std::exception const& /*e*/)
	{
	}
	return path.c_str();
}

const TCHAR * EncodeForCSV(const TCHAR *csv, std::_tstring &encoded)
{
	static const TCHAR doubleQuote[] = _T("\"");

	encoded = csv;
	boost::algorithm::trim(encoded);

	//if the value has a comma, quote, or CR or LF then fix it
	bool found = false;
	for (std::_tstring::size_type i = 0; i < encoded.length() && found == false; ++i)
	{
		switch(encoded[i])
		{
		case _T(','):
		case _T('\"'):
		case _T('\r'):
		case _T('\n'):
			found = true;
			break;
		}
	}
	if (found)
	{
		//fix it by changing double quotes into two double quotes and
		//wrapping the value in double quotes
		boost::algorithm::replace_all(encoded, doubleQuote, _T("\"\""));
		encoded = doubleQuote + encoded + doubleQuote;
	}
	return encoded.c_str();
}

int DecodeCSV(const std::_tstring & _line, std::vector<std::_tstring> &decoded)
{
	std::_tstring line = _line;
	boost::algorithm::trim_right_if(line, boost::algorithm::is_any_of(_T("\r\n")));
	line += _T(",,,");
	bool inQuote = false;
	bool inField = false;
	std::_tstring field;
	for(unsigned int i = 0; i < line.size() - 1; ++i)
	{
		TCHAR thisChar = line[i];
		TCHAR nextChar = line[i + 1];
		if (!inField)
		{
			if (i > 0)
				decoded.push_back(field);

			inField = true;
			field.clear();
			if (thisChar == _T('"'))
			{
				inQuote = true;
				continue;
			}
		}
		//  Now we must be in a field
		if (inQuote)
		{
			if (thisChar == _T('"'))
			{
				if (nextChar == _T('"'))
				{
					field += _T('"');
				}
				else if(nextChar == _T(','))
				{
					inQuote = false;
					inField = false;
				}
				else
					ATLASSERT(!"Found Single Quote in Double Quotes");
				++i;	//Eat the actual quote or comma.
				continue;
			}
		}
		else
		{
			if(thisChar == _T(','))
			{
				inField = false;
				continue;
			}
		}
		field += thisChar;
	}
	return decoded.size();
}

const TCHAR * EncodeForEclNoQuote(const TCHAR *rawString, std::_tstring &encoded, bool ucescape/*=false*/)
{
	//add escapes for Ecl string constants
	int len = _tcslen(rawString);
	for (int i=0; i<len; i++ )
	{
		switch (rawString[i] )
		{
		case '\'':
			encoded += _T("\\'");
			break;
		case '\n':
			encoded += _T("\\n");
			break;
		case '\r':
			encoded += _T("\\r");
			break;
		case '\\':
			encoded += _T("\\\\");
			break;
		case '\t':
			encoded += _T("\\t"); //not listed in laguage reference, but seems to work
			break;
		default:
			if(ucescape)
			{
				if(_istupper(rawString[i]))
				{
					_TCHAR lower=_totlower(rawString[i]);

					encoded += _T('^');
					encoded += lower;
				}
				else
				{
					encoded += rawString[i];
				}
			}
			else
			{
				encoded += rawString[i];
			}
			break;
		}
	}
	return encoded.c_str();
}

const TCHAR * EncodeForEcl(const TCHAR *rawString, std::_tstring &encoded, bool ucescape/*=false*/)
{
	encoded = _T("'");
	EncodeForEclNoQuote(rawString, encoded, ucescape);
	encoded += _T("'");
	return encoded.c_str();
}

const TCHAR * DecodeFromEcl(const TCHAR *rawString, std::_tstring & decoded)
{
	decoded.clear();
	//remove escapes from ECL and surrounding quotes
	int len = _tcslen(rawString);
	if ( len > 2 )
	{
		decoded.reserve(len); //default buffer
		for (int i=1; i<len-1; i++ )
		{
			if ( rawString[i] == '\\' )
			{
				i++;
				switch (rawString[i] )
				{
				case '\'':
					decoded += '\'';
					break;
				case 'n':
					decoded += '\n';
					break;
				case 'r':
					decoded += '\r';
					break;
				case 't': //not listed in laguage reference, but seems to work
					decoded += '\t';
					break;
				case '\\':
					decoded += '\\';
					break;
				default:
					//could be OCTAL set, but not likely
					break;
				}
			}
			else
			{
				decoded += rawString[i];
			}

		}
	}
	return decoded.c_str();
}

const TCHAR *IntToString(int i, CString &str)
{
	str = boost::lexical_cast<std::_tstring>(i).c_str();
	return str;
}

const TCHAR *IntToString(int i, std::_tstring &str)
{
	str = boost::lexical_cast<std::_tstring>(i);
	return str.c_str();
}

bool StringToInt(const TCHAR* from, int &to)
{
	bool good = true;
	try
	{
		to = boost::lexical_cast<int>(from);
	}
	catch(const boost::bad_lexical_cast &)
	{
		good = false;
	}
	return good;
}


bool StringToInt(const TCHAR* from, __int64 &to)
{
	to = ::_tstoi64(from);
	return true;
	//boost lib won't handle a __int64
	//try
	//{
	//	to = boost::lexical_cast<__int64>(from);
	//}
	//catch(const boost::bad_lexical_cast &)
	//{
	//	good = false;
	//}
}

void TidyCRLF(std::_tstring & source)
{
	std::_tstring search = _T("\r\n");
	std::_tstring replace = _T("\r");
	for(std::string::size_type idx = source.find(search); idx != std::string::npos; idx = source.find(search, idx + 1))
		source.replace(idx, search.length(), replace);

	search = _T("\n");
	replace = _T("\r");
	for(std::string::size_type idx = source.find(search); idx != std::string::npos; idx = source.find(search, idx + 1))
		source.replace(idx, search.length(), replace);

	search = _T("\r");
	replace = _T("\r\n");
	for(std::string::size_type idx = source.find(search); idx != std::string::npos; idx = source.find(search, idx + 1))
		source.replace(idx, search.length(), replace);
}

void TidyCRLF(CString & source)
{
	std::_tstring tmp = source;
	TidyCRLF(tmp);
	source = tmp.c_str();
}

static void vformat(std::_tstring& str, const TCHAR *fmt, va_list argPtr) {
	// We draw the line at a 1MB string.
	const int maxSize = 1000000;

	// If the string is less than 512 characters,
	// allocate it on the stack because this saves
	// the malloc/free time.
	const int bufSize = 512;
	TCHAR stackBuffer[bufSize];

	int attemptedSize = bufSize - 1;

	int numChars = ::_vsntprintf(stackBuffer, attemptedSize, fmt, argPtr);

	if (numChars >= 0) 
	{
		// Got it on the first try.
		str = stackBuffer;
	}
	else
	{
		// Now use the heap.
		TCHAR* heapBuffer = NULL;

		while ((numChars == -1) && (attemptedSize < maxSize)) {
			// Try a bigger size
			attemptedSize *= 2;
			heapBuffer = (TCHAR*)::realloc(heapBuffer, attemptedSize + 1);
			numChars = ::_vsntprintf(heapBuffer, attemptedSize, fmt, argPtr);
		}
		str = heapBuffer;

		::free(heapBuffer);
	}
}

const TCHAR* Format(std::_tstring& str, const TCHAR* fmt ...) 
{
	va_list argList;
	va_start(argList, fmt);
	vformat(str, fmt, argList);
	va_end(argList);

	return str.c_str();
}

std::_tstring& MakeLower(std::_tstring& str)
{
	std::transform(str.begin(),str.end(),str.begin(), std::tolower);
	return str;
}

bool AddSeparator(std::_tstring& path) 
{
	unsigned int notSeparator = path.find_last_not_of(_T("/\\"));
	if ( notSeparator + 1 == path.length() )
	{
		unsigned int separator = path.find_first_of(_T("/\\"));
		if ( separator == std::string::npos )
			path += '/';
		else
			path += path[separator];
		return true;
	}
	return false;
}


void Replace(std::_tstring& str, TCHAR replace, const std::_tstring& with)
{
	unsigned int pos = 0;
	while ( (pos = str.find(replace,pos)) != std::_tstring::npos )
	{
		str.replace(pos,1,with);
		pos += with.length();
	}
}


void Replace(std::_tstring& str, const TCHAR* replace, const std::_tstring& with)
{
	unsigned int pos = 0;
	int len = _tcslen(replace);
	int withLen = with.length();
	while ( (pos = str.find(replace,pos)) != std::_tstring::npos )
	{
		str.replace(pos,len,with);
		pos += withLen;
	}
}

int MoveToRecycleBin(const std::_tstring & path)
{
	TCHAR buff[MAX_PATH + 3];
	ZeroMemory(buff, sizeof(buff));
	_tcscpy(buff, path.c_str());
	SHFILEOPSTRUCT fileop = {0}; 
	fileop.wFunc = FO_DELETE; 
	fileop.pFrom = buff; 
	fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION; 
	return SHFileOperation(&fileop);
}

void ShowHelp(const std::_tstring word)
{
	if (word.empty())
		return;

	boost::filesystem::path appFolder;
	GetProgramFolder(appFolder);
#ifdef _DEBUG
	appFolder /= "ECLReference_debug.chm";
#else
	appFolder /= "ECLReference.chm";
#endif
	std::_tstring helpPath = CA2T(appFolder.native_file_string().c_str());

	HtmlHelp(GetDesktopWindow(), helpPath.c_str(), HH_DISPLAY_TOPIC, NULL);
	HH_AKLINK link = {0};
	link.cbStruct =     sizeof(HH_AKLINK) ;
	link.fReserved =    FALSE ;
	link.pszKeywords =  word.c_str();
	link.pszUrl =       NULL ;
	link.pszMsgText =   NULL ;
	link.pszMsgTitle =  NULL ;
	link.pszWindow =    NULL ;
	link.fIndexOnFail = TRUE ;
	HtmlHelp(GetDesktopWindow(), helpPath.c_str(), HH_KEYWORD_LOOKUP, (DWORD)&link);
}

#ifndef SEISINT_LIBEXPORTS
namespace _com_util 
{
	BSTR __stdcall ConvertStringToBSTR(const char* pSrc)
	{
		return ::SysAllocString(CA2W(pSrc));
	}
}
#endif

int round_int(double x)
{
	ATLASSERT(x > static_cast<double> (INT_MIN / 2) - 1.0);
	ATLASSERT(x < static_cast<double> (INT_MAX / 2) + 1.0);

	const static float round_to_nearest = 0.5f;
#if _WIN64
	int i = x;
#elif _WIN32_WCE
	int i = x;
#else
	int i;
	__asm
	{
		fld x
		fadd st, st (0)
		fadd round_to_nearest
		fistp i
		sar i, 1
	}
#endif
	return i;
}

int floor_int (double x)
{
	//assert (x > static_cast <double> (INT_MIN / 2) - 1.0);
	//assert (x < static_cast <double> (INT_MAX / 2) + 1.0);

	const static float round_towards_m_i = -0.5f;
#if _WIN64
	int i = x;
#elif _WIN32_WCE
	int i = x;
#else
	int i;
	__asm
	{
		fld x
		fadd st, st (0)
		fadd round_towards_m_i
		fistp i
		sar i, 1
	}
#endif
	return (i);
}

int ceil_int (double x)
{
	assert (x > static_cast <double> (INT_MIN / 2) - 1.0);
	assert (x < static_cast <double> (INT_MAX / 2) + 1.0);
	const float round_towards_p_i = -0.5f;
#if _WIN64
	int i = x + 1.0f;
#elif _WIN32_WCE
	int i = x + 1.0f;
#else
	int i;
	__asm
	{
		fld x
			fadd st, st (0)
			fsubr round_towards_p_i
			fistp i
			sar i, 1
	}
#endif
	return (-i);
}

/*
int truncate_int (double x)
{
	assert (x > static_cast <double> (INT_MIN / 2) - 1.0);
	assert (x < static_cast <double> (INT_MAX / 2) + 1.0);
	const float round_towards_p_i = -0.5f;
	int i;
	__asm
	{
		fld x
			fadd st, st (0)
			fabs st (0)
			fadd round_towards_p_i
			fistp i
			sar i, 1
	}
	if (x < 0)
	{
		i = -i;
	}
	return (i);
}
*/
//-----------------------------------------------------------------------------

const TCHAR * GetVersion(const std::_tstring & dllLabel, std::_tstring & version)
{
	version = dllLabel + _T(":  ");
	HMODULE dll=GetModuleHandle(dllLabel.c_str()); 
	if (dll) 
	{ 
		DLLGETVERSIONPROC DllGetVersion=(DLLGETVERSIONPROC)GetProcAddress(dll,"DllGetVersion"); 
		if (DllGetVersion) 
		{ 
			DLLVERSIONINFO vinfo; 
			memset(&vinfo,0,sizeof(vinfo)); 
			vinfo.cbSize=sizeof(vinfo); 
			DllGetVersion(&vinfo); 
			version = (boost::_tformat(_T("%1%.%2%.%3%.%4%")) % vinfo.dwMajorVersion % vinfo.dwMinorVersion % vinfo.dwBuildNumber % vinfo.dwPlatformID).str();
		}
	} 

	return version.c_str();
}

ULONG GetComCtl32Version()
{
	if(COMCTL_VERSION==0)
	{
		DLLVERSIONINFO vinfo; 
		memset(&vinfo,0,sizeof(vinfo)); 
		vinfo.cbSize=sizeof(vinfo); 
		if (AtlGetCommCtrlVersion(&vinfo.dwMajorVersion, &vinfo.dwMinorVersion) == S_OK)
		{
			COMCTL_VERSION = MAKEVERSION(vinfo.dwMajorVersion,vinfo.dwMinorVersion); 
		}
	} 

	return COMCTL_VERSION;
}

#ifndef SEISINT_LIBEXPORTS

const TCHAR * GetMyIP(std::_tstring & ip)
{
	char szLclHost[120]; 
	SOCKADDR_IN stLclAddr; 
	SOCKADDR_IN stRmtAddr; 
	int nAddrSize = sizeof(SOCKADDR); 
	int nRet; 

	/* Init local address (to zero) */ 
	stLclAddr.sin_addr.s_addr = INADDR_ANY; 

	/* Get the local hostname */ 
	nRet = gethostname(szLclHost, sizeof(szLclHost)); 
	if (nRet != SOCKET_ERROR) 
	{ 
		/* Resolve hostname for local address */ 
		LPHOSTENT lpstHostent = gethostbyname((LPSTR)szLclHost); 
		if (lpstHostent) 
			stLclAddr.sin_addr.s_addr = *((u_long FAR*)(lpstHostent->h_addr)); 
	} 


	/* If still not resolved, then try second strategy */ 
	if (stLclAddr.sin_addr.s_addr == INADDR_ANY) 
	{ 
		/* Get a UDP socket */ 
		SOCKET hSock = socket(AF_INET, SOCK_DGRAM, 0); 
		if (hSock != INVALID_SOCKET) 
		{ 
			/* Connect to arbitrary port and address (NOT loopback) */ 
			stRmtAddr.sin_family = AF_INET; 
			stRmtAddr.sin_port   = htons(IPPORT_ECHO); 
			stRmtAddr.sin_addr.s_addr = inet_addr("128.127.50.1"); 
			nRet = connect(hSock, (LPSOCKADDR)&stRmtAddr, sizeof(SOCKADDR)); 
			if (nRet != SOCKET_ERROR) 
			{ 
				/* Get local address */ 
				getsockname(hSock, (LPSOCKADDR)&stLclAddr, (int FAR*)&nAddrSize); 
			} 
			closesocket(hSock);   /* we're done with the socket */ 
		} 
	} 

	struct in_addr {
		union {
			struct {
				unsigned char	s_b1,
								s_b2,
								s_b3,
								s_b4;
			}  S_un_b;
			struct  {
				unsigned short	s_w1,
								s_w2;
			}  S_un_w;
			unsigned long  S_addr;
		} S_un;
	} addrCracker;
	addrCracker.S_un.S_addr = stLclAddr.sin_addr.s_addr;
	ip = (boost::_tformat(_T("%1%.%2%.%3%.%4%")) % addrCracker.S_un.S_un_b.s_b1 % addrCracker.S_un.S_un_b.s_b2 % addrCracker.S_un.S_un_b.s_b3 % addrCracker.S_un.S_un_b.s_b4).str();
	return ip.c_str();
}

#endif

const TCHAR * UnescapeXML(const std::_tstring & _source, std::_tstring & _target)
{
	_target = _source;
	boost::algorithm::replace_all(_target, _T("&lt;"), _T("<"));
	boost::algorithm::replace_all(_target, _T("&gt;"), _T(">"));
	boost::algorithm::replace_all(_target, _T("&apos;"), _T("'"));
	boost::algorithm::replace_all(_target, _T("&quot;"), _T("\""));
	boost::algorithm::replace_all(_target, _T("&amp;"), _T("&"));
	return _target.c_str();
}

const TCHAR * EscapeXML(const std::_tstring & _source, std::_tstring & _target, bool escapeQuotes)
{
	if (!_source.empty())
	{
		std::wstring source = CT2W(_source.c_str(), CP_UTF8);
		std::wstring target;
		int escapedLen = EscapeXML(source.c_str(), source.length(), NULL, 0, escapeQuotes ? ATL_ESC_FLAG_ATTR : ATL_ESC_FLAG_NONE);
		target.resize(escapedLen);
		EscapeXML(source.c_str(), source.length(), &target[0], escapedLen, escapeQuotes ? ATL_ESC_FLAG_ATTR : ATL_ESC_FLAG_NONE);
		_target = CW2T(target.c_str(), CP_UTF8);
	}
	return _target.c_str();
}

const TCHAR * GetActiveXDLLPath(const std::_tstring & clsID, std::_tstring & path)
{
	HKEY hKey = NULL; 
	LPTSTR pBuff = new TCHAR[MAX_PATH]; 
	LONG szAccount = MAX_PATH; 
	if (ERROR_SUCCESS == RegCreateKey(HKEY_CLASSES_ROOT, (boost::_tformat(_T("CLSID\\%1%\\InprocServer32")) % clsID.c_str()).str().c_str(), &hKey))
	{
		szAccount = MAX_PATH; 
		RegQueryValue(hKey, NULL, pBuff, &szAccount); 
		RegCloseKey(hKey); 
		path = pBuff;
	}
	delete pBuff;
	return path.c_str();
}

const TCHAR * GetActiveXDLLFolder(const std::_tstring & clsID, std::_tstring & folder)
{
	boost::filesystem::path dllPath(CT2A(GetActiveXDLLPath(clsID, folder)), boost::filesystem::native), dllFolder;
	dllFolder = dllPath.branch_path();
	folder = CA2T(dllFolder.native_file_string().c_str());
	return folder.c_str();
}

bool GetPluginECL(const std::_tstring & pluginPath, std::_tstring & ecl)
{
	struct ECLPluginDefinitionBlock
	{
		unsigned size;			  // Size of passed in structure, filled in by caller
		unsigned magicVersion;	  // Filled in by plugin - must be PLUGIN_VERSION
		const char *moduleName;
		const char *ECL;
		unsigned flags;
		const char *version;
		const char *description;
	};
	typedef bool (*EclPluginDefinition) (ECLPluginDefinitionBlock *);

	//	DONT_RESOLVE_DLL_REFERENCES:  dllmain is not called and dependent dlls are not loaded.
	//	This is dangerous and will crash if getECLPluginDefinition does anything fancy.
	//	Note:	We can't use LoadLibrary as there is no way to specify additional dll folders (like bin)
	//			without requiring XP + SP1 (and even then you can only call SetDllDirectory, which is no 
	//			help if dependent DLLs are in bin and plugin folder).
	HMODULE hMod = LoadLibraryEx(pluginPath.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);  
	if (hMod != NULL) 
	{
		bool retVal = false;
		try
		{
			EclPluginDefinition p = (EclPluginDefinition) GetProcAddress(hMod,"getECLPluginDefinition");
			if (p)
			{
				ECLPluginDefinitionBlock info = { 0 };
				info.size = sizeof(ECLPluginDefinitionBlock);
				if (p(&info))
				{
					ecl = CA2T(info.ECL, CP_UTF8);
					retVal = true;
				}
			}
		}
		catch (...)
		{
		}
		FreeLibrary(hMod);
		return retVal;
	}
	return false;
}

void InsertSeparator(CMenu &menu)
{
	CMenuItemInfo item;
	item.fMask = MIIM_TYPE;
	item.fType = MFT_SEPARATOR;
	menu.InsertMenuItem((unsigned)-1,FALSE,&item);
}

BOOL InsertMenuItem(CMenu &menu, int id, const TCHAR *itemText)
{
	return InsertMenuItem(menu,id,id,itemText);
}

BOOL InsertMenuItem(CMenu &menu, int pos, int id, const TCHAR *itemText)
{
	CMenuItemInfo item;
	item.fMask = MIIM_TYPE | MIIM_ID;
	item.wID = id;
	item.fType = MFT_STRING;
	item.dwTypeData = const_cast<LPTSTR>(itemText);
	return menu.InsertMenuItem(pos,FALSE,&item);
}

