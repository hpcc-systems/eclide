//////////////////////////////////////////////////////////////////////
//
//	dialogex.h - Windows Common Dialogs implementation
//
//////////////////////////////////////////////////////////////////////
//
//	 Version:	1.0
//	 Created:	2001/2/16 - 2:00 pm
//	Filename: 	dialogex.h
//	  Author:	Ben Burnett
//	   Email:	ben.burnett@quaggait.com
//				
//	 Purpose:	An extention to WTL's Common Dialogs implementation
//
//////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000 - 2001, Quagga IT - All rights reserved.
//
// The code and information is provided "as-is" without warranty of 
//	any kind, either expressed or implied.
//
// Send any bug fixes and/or suggestions to the e-mail listed above.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( _dialogex_h_included_ )
#define _dialogex_h_included_

#if _MSC_VER >= 1000
#	pragma once
#endif // _MSC_VER >= 1000

#ifndef __cplusplus
#	error ATL requires C++ compilation (use a .cpp suffix)
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDialogEx - used for File Open or File Save As

class CFileDialogEx : public WTL::CFileDialogImpl < CFileDialogEx > 
{

// Constructors / Destructors
public:

	CFileDialogEx ( BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
					LPCTSTR lpszDefExt = NULL,
					LPCTSTR lpszFileName = NULL,
					DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					LPCTSTR lpszFilter = NULL,
					HWND hWndParent = NULL)
					: CFileDialogImpl < CFileDialogEx > ( bOpenFileDialog, lpszDefExt, 
							lpszFileName, dwFlags, lpszFilter, hWndParent )
	{ /**/ }
	
// Message map
public:

	BEGIN_MSG_MAP ( CFileDialogEx )	

		//
		// Message handlers
		//
		
		MESSAGE_HANDLER ( WM_INITDIALOG, OnInitDialog )

	END_MSG_MAP ()	

// Overides
public:
	
	LRESULT OnInitDialog ( UINT /* uMsg */, WPARAM /* wParam */, LPARAM /* lParam */, BOOL & /* bHandled */ )
	{
		// center the dialog
		CWindow wndDlgWindow = GetFileDialogWindow ();
		wndDlgWindow.CenterWindow ();		

		return FALSE;
	}

// Attributes
public:
	
	// Returns the full path of the selected file.
	_CSTRING_NS::CString GetPathName () const
	{
		return _CSTRING_NS::CString ( m_ofn.lpstrFile );
	}

	// Returns the filename of the selected file.
	_CSTRING_NS::CString GetFileName () const
	{
		TCHAR szFileName [ _MAX_FNAME ], szExt [ _MAX_FNAME ];
		_tsplitpath_s(m_ofn.lpstrFile, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, szExt, _MAX_FNAME);		
		lstrcat ( szFileName, szExt );
		return _CSTRING_NS::CString ( szFileName );
	}

	// Returns the file extension of the selected file.
	_CSTRING_NS::CString GetFileExt () const
	{
		TCHAR szExt [ _MAX_FNAME ];
		_tsplitpath_s(m_ofn.lpstrFile, NULL, 0, NULL, 0, NULL, 0, szExt, _MAX_FNAME);		
		return _CSTRING_NS::CString ( szExt );
	}

	// Returns the title of the selected file. 
	_CSTRING_NS::CString GetFileTitle () const
	{
		return _CSTRING_NS::CString ( m_ofn.lpstrFileTitle );
	}
	
	// Returns the position of the first file pathname in the list, 
	//	if m_ofn.Flags has the OFN_ALLOWMULTISELECT flag set.
	//
	// From MFC CFileDialog
	//
	DWORD GetStartPosition () const
	{
		return reinterpret_cast < DWORD > ( m_ofn.lpstrFile );
	}

	// Returns the next filename from the group selected in the dialog box. 
	//	path of the filename includes the file’s title plus the entire 
	//	directory path. 
	//
	// You can use GetNextPathName in a forward iteration loop if you establish 
	//	the initial position with a call to GetStartPosition.
	//
	// From MFC CFileDialog
	//
	_CSTRING_NS::CString GetNextPathName ( DWORD & dwPos ) const
	{
		BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
		TCHAR chDelimiter;
		if (bExplorer)
			chDelimiter = '\0';
		else
			chDelimiter = ' ';
		
		LPTSTR lpsz = reinterpret_cast < LPTSTR > ( dwPos );
		if (lpsz == m_ofn.lpstrFile) // first time
		{
			if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
			{
				dwPos = NULL;
				return m_ofn.lpstrFile;
			}
			
			// find char dwPos after first Delimiter
			while(*lpsz != chDelimiter && *lpsz != '\0')
				lpsz = _tcsinc(lpsz);
			lpsz = _tcsinc(lpsz);
			
			// if single selection then return only selection
			if (*lpsz == 0)
			{
				dwPos = NULL;
				return m_ofn.lpstrFile;
			}
		}
		
		_CSTRING_NS::CString strPath = m_ofn.lpstrFile;
		if (!bExplorer)
		{
			LPTSTR lpszPath = m_ofn.lpstrFile;
			while(*lpszPath != chDelimiter)
				lpszPath = _tcsinc(lpszPath);
			strPath = strPath.Left(lpszPath - m_ofn.lpstrFile);
		}
		
		LPTSTR lpszFileName = lpsz;
		_CSTRING_NS::CString strFileName = lpsz;
		
		// find char dwPos at next Delimiter
		while(*lpsz != chDelimiter && *lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		
		if (!bExplorer && *lpsz == '\0')
			dwPos = NULL;
		else
		{
			if (!bExplorer)
				strFileName = strFileName.Left(lpsz - lpszFileName);
			
			lpsz = _tcsinc(lpsz);
			if (*lpsz == '\0') // if double terminated then done
				dwPos = NULL;
			else
				dwPos = reinterpret_cast < DWORD > ( lpsz );
		}
		
		// only add '\\' if it is needed
		if (!strPath.IsEmpty())
		{
			// check for last back-slash or forward slash (handles DBCS)
			LPCTSTR lpsz = _tcsrchr(strPath, '\\');
			if (lpsz == NULL)
				lpsz = _tcsrchr(strPath, '/');
			// if it is also the last character, then we don't need an extra
			if (lpsz != NULL &&
				(lpsz - (LPCTSTR)strPath) == strPath.GetLength()-1)
			{
				ATLASSERT(*lpsz == '\\' || *lpsz == '/');
				return strPath + strFileName;
			}
		}
		return strPath + '\\' + strFileName;
	}

};

//////////////////////////////////////////////////////////////////////

#endif // !defined ( _dialogex_h_included_ )

//////////////////////////////////////////////////////////////////////
// EOF ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////