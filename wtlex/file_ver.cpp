// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
//		File:		file_ver.cpp
//					------------
//
//		Content:	Implementation of class CFileVersionInfo.
//					Manages the retrieval of various information from a
//					Win32 file image.
//
//
//
//		Created:		01/27/2002
//						Thomas Weller
//
//		Last modified:	02/02/2002
//						Thomas Weller
//
//
// ----------------------------------------------------------------------------
//
//	Remarks
//	-------
//
//	- The "version.lib" library module must be added to project settings.
//
//	- The GetCharSetName member function will only return a value 
//	  on Win98/2K or higher.
//	- Remove the _WIN98_OR_WIN2K_ define below if this code is to be 
//	  compiled on a more recent version. 
//	  
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//		includes
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "file_ver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

// Target OS version (used in GetCharSetName)
// *****************
#define _WIN98_OR_WIN2K_

CString GetApplicationName()
{
	TCHAR pcFile[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0), pcFile, sizeof(pcFile)); 
	return pcFile;
}

CString	GetApplicationVersion()
{
	CFileVersionInfo info;
	info.ReadVersionInfo(GetApplicationName());
	return info.GetFileVersionString();
}

CString	GetApplicationMajorVersion()
{
	CFileVersionInfo info;
	info.ReadVersionInfo(GetApplicationName());
	return info.GetMajorFileVersionString();
}

BOOL AfxExtractSubString(CString& rString, LPCTSTR lpszFullString, int iSubString, TCHAR chSep = '\n')
{
	if (lpszFullString == NULL)
		return FALSE;

	while (iSubString--)
	{
		lpszFullString = _tcschr(lpszFullString, chSep);
		if (lpszFullString == NULL)
		{
			rString.Empty();        // return empty string as well
			return FALSE;
		}
		lpszFullString++;       // point past the separator
	}
	LPCTSTR lpchEnd = _tcschr(lpszFullString, chSep);
	int nLen = (lpchEnd == NULL) ?
		lstrlen(lpszFullString) : (int)(lpchEnd - lpszFullString);
//	ASSERT(nLen >= 0);
	memcpy(rString.GetBufferSetLength(nLen), lpszFullString, nLen*sizeof(TCHAR));
	rString.ReleaseBuffer();	// Need to call ReleaseBuffer 
								// after calling GetBufferSetLength
	return TRUE;
};

//*****************************************************************************
//*****************************************************************************
//
//				CFileVersionInfo private static data members
//				--------------------------------------------
//
//  (used by the "GetVersion" member function)
//
//*****************************************************************************
//*****************************************************************************

// Format string for browsing the variable resource info block
// ***********************************************************
const LPCTSTR CFileVersionInfo::m_lpszFmtStringFileInfo = 
										_T("\\StringFileInfo\\%04x%04x\\%s");

// Specific pieces of version information (corresponding to SFI_xxx constants)
// ***************************************************************************
const LPCTSTR CFileVersionInfo::m_lpszStringFileInfos[] =
{ 
	_T("CompanyName"),			// SFI_COMPANYNAME
	_T("FileDescription"),		// SFI_FILEDESCRIPTION
	_T("FileVersion"),			// SFI_FILEVERSION
	_T("InternalName"),			// SFI_INTERNALNAME
	_T("LegalCopyRight"),		// SFI_LEGALCOPYRIGHT
	_T("OriginalFileName"),		// SFI_ORIGINALFILENAME
	_T("ProductName"),			// SFI_PRODUCTNAME
	_T("ProductVersion"),		// SFI_PRODUCTVERSION
	_T("Comments"),				// SFI_COMMENTS
	_T("LegalTrademarks"),		// SFI_LEGALTRADEMARKS
	_T("PrivateBuild"),			// SFI_PRIVATEBUILD
	_T("SpecialBuild")			// SFI_SPECIALBUILD
};




//*****************************************************************************
//*****************************************************************************
//
//					CFileVersionInfo Construction
//					-----------------------------
//
//*****************************************************************************
//*****************************************************************************

// standard constructor
CFileVersionInfo::CFileVersionInfo()
{
	m_pVersionInfo = NULL;
	Clear();	// initialize all to zero
}

// copy constructor
CFileVersionInfo::CFileVersionInfo(const CFileVersionInfo& fvi)
{
	CString strFile = fvi.GetFilename();
	if (!strFile.IsEmpty())
	{
		ReadVersionInfo(strFile);
	}
	else
	{
		m_pVersionInfo = NULL;
		Clear();	// initialize all to zero
	}
}

// initialize from a specific file (by name)
CFileVersionInfo::CFileVersionInfo(LPCTSTR lpszFile)
{
	if (lpszFile && _tcslen(lpszFile))
	{
		ReadVersionInfo(lpszFile);
	}
	else
	{
		m_pVersionInfo = NULL;
		Clear();	// initialize all to zero
	}
}



//*****************************************************************************
//*****************************************************************************
//
//					CFileVersionInfo Destruction
//					----------------------------
//
//*****************************************************************************
//*****************************************************************************

// perform clean up as necessary
CFileVersionInfo::~CFileVersionInfo()
{
	Clear();
}



//*****************************************************************************
//*****************************************************************************
//
//					CFileVersionInfo Operations
//					---------------------------
//
//*****************************************************************************
//*****************************************************************************

// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::Clear
//					-----------------------
//
//					Clear all data, free memory if necessary.
//
//  Access level:	public
//
//	Declaration:	void Clear()
//
//	Remarks:
//	-------
//	Called by destructor for clean up.
//
// ----------------------------------------------------------------------------

// clear all data
void CFileVersionInfo::Clear()
{
	m_strFilename.Empty();
	m_dwTransInfo = 0L;
	memset(&m_fixedFileInfo, 0, sizeof(VS_FIXEDFILEINFO));
	if (m_pVersionInfo)
	{
		delete m_pVersionInfo;
		m_pVersionInfo = NULL;
	}
}	// Clear


// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::ReadVersionInfo
//					---------------------------------
//
//					Read a file's version resource information block.
//
//  Access level:	public
//
//	Declaration:	BOOL ReadVersionInfo(const CString& strFile)
//
//	Returns:		TRUE if the version information was read successfully.
//
//	Parameters:
//		In:			strFile
//					Name of file for which information should be retrieved.
//		Out:
//		In/Out:
//
//	Remarks:
//	-------
//	If the function succeeds, a subsequent call to GetFileName will return
//	strFile, IsValid will return TRUE, and version information for the
//	specified file will be available using the various attribute member
//	functions.
//	If an error occured, IsValid will return FALSE, and GetFilename will
//	return an empty string.
//
//	A file's version resource may contain more than one version info block.
//	If so, only the first one will be retrieved.
//
//	(The number of available blocks can be determined from the value of the
//	nLen variable after a call to VerQueryValue with the lpSubBlock parameter
//	set to "\\VarFileInfo\\Translation". There will be 4 Bytes for each block.
//	If one would like to handle more than one block simultaneously,
//	this would be the starting point for modifications.)
//
// ----------------------------------------------------------------------------

// read the file's version information
BOOL CFileVersionInfo::ReadVersionInfo(const CString& strFile)
{
	DWORD	dwDummy;
	UINT	nSize;
	LPVOID	lpVoid;

	// clear previous data first
	// *************************
	Clear();
	
	// determine size (in bytes) of version information block
	// ******************************************************
	nSize = ::GetFileVersionInfoSize((LPTSTR) (LPCTSTR) strFile, &dwDummy);
	if (!nSize) return FALSE;	// no version information available

	// allocate memory for the information block
	// *****************************************
	m_pVersionInfo = new BYTE[nSize];

	// retrieve version information block
	// **********************************
	if (!::GetFileVersionInfo ((LPTSTR) (LPCTSTR) strFile, 
							   0L, nSize, m_pVersionInfo)
	   ) 
	{
		delete m_pVersionInfo;
		m_pVersionInfo = NULL;
		return FALSE;
	}

	// Extract VS_FIXEDFILEINFO part from memory block. This part contains
	// language independant version information.
	// See SDK documentation for details.
	// ********************************************************************
	if (!::VerQueryValue(m_pVersionInfo, _T ("\\"), &lpVoid, &nSize))
	{
		delete m_pVersionInfo;
		m_pVersionInfo = NULL;
		return FALSE;
	}
	memcpy(&m_fixedFileInfo, lpVoid, nSize);

	// Extract translation "table" (Contains two WORD entries:
	//								Language Id and Charset).
	// This information is used to retrieve language dependant parts
	// of version info (see "GetVersionInfo" member function).
	// Parameter nSize indicates the number of available version info blocks.
	// (4 bytes for each, see Remarks above).
	// **********************************************************************
	if (!::VerQueryValue (m_pVersionInfo, _T("\\VarFileInfo\\Translation"), 
						 &lpVoid, &nSize)
	   ) 
	{
		Clear();
		return FALSE;
	}
	memcpy(&m_dwTransInfo, lpVoid, sizeof(DWORD));

	// Everything is OK, now save filename
	// ***********************************
	m_strFilename = strFile;

	return TRUE;

}	// ReadVersionInfo



// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::GetCharSetName
//					--------------------------------
//
//					Get description of character set (code page) which
//					is used in version info resource.
//
//  Access level:	public
//
//	Declaration:	CString GetCharSetName() const
//
//	Returns:		The name of the charset, empty string if the name
//					could not be retrieved.
//
//	Remarks:
//	-------
//	This function only returns a value for Win98/2000 (NT 5) or higher.
//	This is because the API "GetCPInfoEx" is used which is not present
//	in more recent versions of Windows. If running on a previous version,
//	the function will return an empty CString.
//	Workaround for previous versions: Use the GetCharSet() member function
//	to check the value of the char set and create an appropriate description.
//
//  To compile on a previous version, undef the _WIN98_OR_WIN2K_ #define above.
//
// ----------------------------------------------------------------------------

// get name of charset of version info block (98/NT5 or higher)
CString CFileVersionInfo::GetCharSetName() const
{ 
// CPINFOEX struct and ::GetCPInfoEx API not available in older
// versions of Windows ...
// ************************************************************
#ifdef _WIN98_OR_WIN2K_

	// Check Win version (must be at least 98/NT5)
	// The GetCPInfoEx API is not supported in versions before these.
	// **************************************************************
	OSVERSIONINFO osvi;
	BOOL bWinVerOK;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	bWinVerOK = (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
				((osvi.dwMajorVersion > 4) ||
				 ((osvi.dwMajorVersion == 4) && 
				  (osvi.dwMinorVersion > 0))
				);
	if (!bWinVerOK) bWinVerOK = 
		((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
		 (osvi.dwMajorVersion >= 5));

	if (!bWinVerOK) return CString(_T(""));

	// get information about code page
	// *******************************
	CPINFOEX info;
	if (::GetCPInfoEx(GetCharSet(), 0, &info)) 
		return CString(info.CodePageName);

#endif 	// _WIN98_OR_WIN2K_

	return CString(_T(""));

}	// GetCharSetName



//*****************************************************************************
//*****************************************************************************
//
//					CFileVersionInfo Attributes
//					---------------------------
//
//*****************************************************************************
//*****************************************************************************


// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::GetVersionInfo
//					--------------------------------
//
//					Retrieve a specific piece of information from a file's
//					version resource information block.
//
//  Access level:	public
//
//	Declaration:	CString GetVersionInfo(int nIndex) const
//
//	Returns:		One of the version resource information pieces 
//					(see Remarks section) or an empty CString.
//
//	Parameters:
//		In:			nIndex
//					Index of information requested (see Remarks section).
//		Out:
//		In/Out:
//
//	Remarks:
//	-------
//
//	The nIndex Parameter can be one of the following values:
//
//	SFI_COMPANYNAME			CompanyName
//							Identifies the company that produced the file.
//
//	SFI_FILEDESCRIPTION		FileDescription
//							Describes the file in such a way that it can be
//							presented to users. This string may be presented 
//							in a list box when the user is choosing files to 
//							install. For example, "Keyboard driver for AT-style
//							keyboards" or "Microsoft Word for Windows".
//
//	SFI_FILEVERSION			FileVersion
//							Identifies the version of this file. For example
//							"3.00A" or "5.00.RC2".
//
//	SFI_INTERNALNAME		InternalName
//							Identifies the file's internal name, if one exists.
//							For example, this string could contain the module 
//							name for a dynamic-link library (DLL), a virtual
//							device name for a Windows virtual device, or a 
//							device name for an MS-DOS device driver.
//
//	SFI_LEGALCOPYRIGHT		LegalCopyright
//							Describes all copyright notices, trademarks, and 
//							registered trademarks that apply to the file. This
//							should include the full text of all notices, legal
//							symbols, copyright dates, trademark numbers, and so
//							on. In English, this string should be in the format
//							"Copyright Microsoft Corp. 1990-1994".
//
//	SFI_ORIGINALFILENAME	OriginalFilename
//							Identifies the original name of the file, not
//							including a path. This enables an application to 
//							determine whether a file has been renamed by a 
//							user. This name may not be MS-DOS 8.3-format if 
//							the file is specific to a non-FAT file system.
//
//	SFI_PRODUCTNAME			ProductName
//							Identifies the name of the product with which
//							this file is distributed. For example, this string
//							could be "Microsoft Windows".
//
//	SFI_PRODUCTVERSION		ProductVersion
//							Identifies the version of the product (identified
//							by "Productname") with which this file is
//							distributed.
//
//	SFI_COMMENTS			Comments
//							Contains any additional information that should be 
//							displayed for diagnostic purposes. 
//							This string can be an arbitrary length.
//
//	SFI_LEGALTRADEMARKS		LegalTrademarks
//							Describes all trademarks and registered trademarks
//							that apply to the file. This should include the 
//							full text of all notices, legal symbols, trademark
//							numbers, and so on. In English, this string should
//							be in the format "Windows is a trademark of 
//							Microsoft Corporation".
//
//	SFI_PRIVATEBUILD		PrivateBuild
//							Describes by whom, where, and why this private 
//							version of the file was built. This string should
//							only be present if the VS_FF_PRIVATEBUILD flag is
//							set in the dwFileFlags member of the 
//							VS_FIXEDFILEINFO structure. For example, Value 
//							could be "Built by OSCAR on \OSCAR2".
//
//	SFI_SPECIALBUILD		SpecialBuild
//							Describes how this version of the file differs from
//							he normal version. This entry should only be
//							present if the VS_FF_SPECIALBUILD flag is set in
//							the dwFileFlags member of the VS_FIXEDFILEINFO
//							structure. For example, Value could be "Private 
//							build for Olivetti solving mouse problems on M250
//							and M250E computers".
//
//
//	It does not necessarily indicate an error, if the function returns an 
//	empty CString, since the requested resource string may really be empty.
//	To see whether the requested information is present use 
//	"IsVersionInfoAvailable".
//
//
// ----------------------------------------------------------------------------

// Get a specific aspect of the file version resource (see above)
CString CFileVersionInfo::GetVersionInfo(int nIndex) const
{
	// First check if object data and index range are valid
	// ****************************************************
	if ((nIndex < SFI_FIRST)	|| 
		(nIndex > SFI_LAST)		||
		(!m_pVersionInfo)
	   ) return CString(_T(""));

	CString strRes,
			strFmt;
	LPVOID	lpInfo;
	UINT	nLen;

	// Create a search string for the VerQueryValue function 
	// (See SDK documentation for details)
	// *****************************************************
	strFmt.Format(m_lpszFmtStringFileInfo, 
				  GetLanguageId(),
				  GetCharSet(),
				  m_lpszStringFileInfos[nIndex]
				 );

	// Get the requested version information string
	// ********************************************
	if (::VerQueryValue(m_pVersionInfo, strFmt.GetBuffer(_MAX_PATH), 
						&lpInfo, &nLen)
	   ) strRes = (const TCHAR*) lpInfo;

	return strRes;

}	// GetVersionInfo


// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::IsVersionInfoAvailable
//					----------------------------------------
//
//					Check if a file's version resource information block
//					contains a specific piece of information.
//
//  Access level:	public
//
//	Declaration:	BOOL IsVersionInfoAvailable(int nIndex) const
//
//	Returns:		TRUE, if the information described by the nIndex
//					parameter is contained within the file's version info
//					block.
//
//	Parameters:
//		In:			nIndex
//					Index of information requested (see Remarks section).
//		Out:
//		In/Out:
//
//	Remarks:
//	-------
//	See Remarks section of "GetVersionInfo" above for details.
//	
//	[Function is almost the same as "GetVersionInfo"]
//
// ----------------------------------------------------------------------------

// Determine, if a specific aspect of the file version info is present
BOOL CFileVersionInfo::IsVersionInfoAvailable(int nIndex) const
{
	// First check if object data and index range are valid
	// ****************************************************
	if ((nIndex < SFI_FIRST)	|| 
		(nIndex > SFI_LAST)		||
		(!m_pVersionInfo)
	   ) return FALSE;

	CString strRes,
			strFmt;
	LPVOID	lpInfo;
	UINT	nLen;

	// Create a search string for the VerQueryValue function 
	// (See SDK documentation for details)
	// *****************************************************
	strFmt.Format(m_lpszFmtStringFileInfo, 
				  GetLanguageId(),
				  GetCharSet(),
				  m_lpszStringFileInfos[nIndex]
				 );

	// See if requested version information string can be found
	// ********************************************************
	return ::VerQueryValue(m_pVersionInfo, strFmt.GetBuffer(_MAX_PATH), 
						   &lpInfo, &nLen
						  );

}	// IsVersionInfoAvailable



// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::GetTargetOs
//					-----------------------------
//
//					Returns a string naming the operating system this
//					file was built for.
//
//  Access level:	public
//
//	Declaration:	CString GetTargetOs(UINT nID) const
//
//	Returns:		If successful, the name of operating system which this 
//					file was built for, otherwise an empty string.
//
//	Parameters:
//		In:			nID
//					ID of a resource string which contains a list of names
//					for the various operating systems. See remarks for details.
//		Out:
//		In/Out:
//
//	Remarks:
//	-------
//	The string resource identified by the nID parameter must be supplied from
//	the calling applications string table. This is to provide a higher degree
//	of flexibility. If different builts for different languages are made, this
//	can be done without changing source code.
//
//	The string must be a '\n'-separated list of OS names. The sequence and 
//	number of substrings is determined by the OS_xxx constants in file_ver.h.
//
//	The parameterless version of this function (returning a DWORD value)
//	may be used as an alternative.
//
//	Example (German):
//	----------------
//	MS-DOS\nWin16\nWin32\nWin16 (MS-DOS)\nWin32 API (MS-DOS)\n16-bit OS/2\n
//	32-bit OS/2\nWindows NT\nWin32 API (Windows NT)\nNicht bekannt
//
// ----------------------------------------------------------------------------

// get name of targwet operating system
CString CFileVersionInfo::GetTargetOs(UINT nID) const
{
	CString str, 
			strRes;
	short   n = -1;
	if (!str.LoadString(nID)) return CString(_T(""));

	switch (m_fixedFileInfo.dwFileOS)
	{
	case VOS_DOS:			n = OS_DOS;			break;
	case VOS_DOS_WINDOWS16:	n = OS_WIN16_DOS;	break;
	case VOS_DOS_WINDOWS32:	n = OS_WIN32_DOS;	break;
	case VOS__WINDOWS16:	n = OS_WIN16;		break;
	case VOS__WINDOWS32:	n = OS_WIN32;		break;
	case VOS_OS216:			n = OS_OS216;		break;
	case VOS_OS232:			n = OS_OS232;		break;
	case VOS_NT:			n = OS_WINNT;		break;
	case VOS_NT_WINDOWS32:	n = OS_WIN32_NT;	break;
	default:				n = OS_UNKNOWN;		break;
	}

	return (AfxExtractSubString(strRes, str, n)) ? strRes : CString(_T(""));
}	// GetTargetOs


// ----------------------------------------------------------------------------
//
//	Function:		CFileVersionInfo::GetFileType
//					-----------------------------
//
//					Returns a string describing the type of the current file.
//
//  Access level:	public
//
//	Declaration:	CString GetFileType(UINT nID) const
//
//	Returns:		If successful, a string describing the file type 
//					of this file, otherwise an empty string.
//
//	Parameters:
//		In:			nID
//					ID of a resource string which contains a list of names
//					for the various file types. See remarks for details.
//		Out:
//		In/Out:
//
//	Remarks:
//	-------
//	The string resource identified by the nID parameter must be supplied from
//	the calling applications string table. This is to provide a higher degree
//	of flexibility. If different builts for different languages are made, this
//	can be done without changing source code.
//
//	The string must be a '\n'-separated list of file type descriptions. The 
//	sequence and number of substrings is determined by the FT_xxx constants 
//	in file_ver.h.
//
//	The parameterless attribute functions "GetFileType"/"GetFileSubType"
//	(both returning the appropriate DWORD value) may be used as an alternative.
//	
//	Example (German):
//	----------------
//	Bildschirmtreiber\nInstallierbarer Treiber\nTastaturtreiber\n
//	Sprachentreiber\nMaustreiber\nNetzwerktreiber\nDruckertreiber\n
//	Soundtreiber\nSystemgerätetreiber\nUnbekannter Treibertyp\n
//	Raster-Schriftart\nTruetype-Schriftart\nVector-Schriftart\nSchriftart\n
//	Anwendung\nDynamische Linkbibliothek\nStatische Linkbibliothek\n
//	Virtuelles Gerät\nUnbekannter Dateityp
//
// ----------------------------------------------------------------------------

// get file type description
CString CFileVersionInfo::GetFileType(UINT nID) const
{
	CString str, 
			strRes;
	short   n = -1;
	if (!str.LoadString(nID)) return CString(_T(""));

	switch (m_fixedFileInfo.dwFileType)
	{
	case VFT_DRV:
		switch (m_fixedFileInfo.dwFileSubtype)
		{
		case VFT2_DRV_DISPLAY:		n = FT_DRV_DISPLAY;		break;
		case VFT2_DRV_INSTALLABLE:	n = FT_DRV_INSTALLABLE;	break;
		case VFT2_DRV_KEYBOARD:		n = FT_DRV_KEYBOARD;	break;
		case VFT2_DRV_LANGUAGE:		n = FT_DRV_LANGUAGE;	break;
		case VFT2_DRV_MOUSE:		n = FT_DRV_MOUSE;		break;
		case VFT2_DRV_NETWORK:		n = FT_DRV_NETWORK;		break;
		case VFT2_DRV_PRINTER:		n = FT_DRV_PRINTER;		break;
		case VFT2_DRV_SOUND:		n = FT_DRV_SOUND;		break;
		case VFT2_DRV_SYSTEM:		n = FT_DRV_SYSTEM;		break;
		case VFT2_UNKNOWN:
		default:					n = FT_DRV_UNKNOWN;		break;
		}
	case VFT_FONT:
		switch (m_fixedFileInfo.dwFileSubtype)
		{
		case VFT2_FONT_RASTER:		n = FT_FONT_RASTER;		break;
		case VFT2_FONT_TRUETYPE:	n = FT_FONT_TRUETYPE;	break;
		case VFT2_FONT_VECTOR:		n = FT_FONT_VECTOR;		break;
		case VFT2_UNKNOWN:
		default:					n = FT_FONT_UNKNOWN;	break;
		}
	case VFT_APP:					n = FT_APP;				break;
	case VFT_DLL:					n = FT_DLL;				break;
	case VFT_STATIC_LIB:			n = FT_STATIC_LIB;		break;
	case VFT_VXD:					n = FT_VXD;				break;
	case VFT_UNKNOWN:
	default:						n = FT_UNKNOWN;			break;
	}	
	return (AfxExtractSubString(strRes, str, n)) ? strRes :
												   CString(_T(""));
}	// GetFileType

