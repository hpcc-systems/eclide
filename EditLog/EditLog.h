/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
\ editlog.h,  Fast asynchronous text logging using a Windows Edit-Control
/
\ Version:	1.0, created 2000-04-02
/           1.1, 2000-07-11: Now *really* supports Win9x, uses V2.0 of SubclassWnd
\
/ Author:	Daniel Lohmann (daniel@uni-koblenz.de)
\
/ Please send comments and bugfixes to the above email address.
\
/ Nick - Changed to use WTL subclassing - much better now
/
\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#pragma once

///////////////////////////////////
// CEditLog class 
// Implemets a fast, asynchronous and threadsave text logging for Edit-Controls
//
//

class CEditLog : public CWindowImpl<CEditLog>
{
	typedef CWindowImpl<CEditLog> baseClass;
public:

	// Constructs the CEditLog. You can pass the edit controls handle
	// here or set it later using the SetEditCtrl() function.
	// To increase performance CEditLog repaints the edit-control
	// only if the caret resides in the last line. Otherwise the
	// control will be refreshed only every nMaxRefreshDelay msec.
	CEditLog( HWND hEdit = NULL, UINT nMaxRefreshDelay = 500 );
	
	virtual ~CEditLog();

	#define UM_ADDTEXT (WM_USER + 500)

	BEGIN_MSG_MAP(CEditLog)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(UM_ADDTEXT, OnAddText)
	END_MSG_MAP()


	// Adds some text to the end of the edit control. Works asynchronously
	// (using PostMessage()) and is save to be called by multiple threads.
	// If you pass true for bLFtoCRLF every LF (ASCII 10) (as it is used in
	// most cases for end-of-line) will be converted to a CR/LF (ASCII 10/13)
	// sequence as it is needed by the windows edit control. 
	virtual void AddText( LPCWSTR pwszAdd, bool bLFtoCRLF = false );
	
	// Converts pszAdd to UNICODE and calls the above
	void AddText( LPCSTR pszAdd, bool bLFtoCRLF = false );

	// Sets the edit-control to be used for logging.
	// Pass NULL to stop text logging.
	virtual void SetEditCtrl( HWND hEdit );

	HWND GetEditCtrl() const
	{
		return GetHandle();
	}

protected:
	void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnAddText(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	bool DoAddText( bool bForce = false);
	
	// This members are used by the GUI thread only
	UINT			m_nTimerID;
	UINT			m_nTimerDelay;

	HWND GetHandle() const
	{
		return m_hWnd;
	}

	// This members are used by the GUI thread and 
	// every thread calling AddText, so we protect them with
	// a Critical Section.
	CRITICAL_SECTION	m_csLock;
	bool				m_bMessagePending;
	std::wstring		m_wsStore;
};

