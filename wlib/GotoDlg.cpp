#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "gotodlg.h"
#include "global.h"

//  ===========================================================================
class CGotoDlg : public CDialogImpl<CGotoDlg>, 
	public CWinDataExchange<CGotoDlg>
{
public:
	CString m_Prompt;
	int m_Curr;
	int m_Len;
	int m_Result;

public:
	CGotoDlg(int curr, int len)
	{
		m_Curr = curr;
		m_Len = len;
		m_Result = 0;
	}

public:
	enum { IDD = IDD_GOTO };

	BEGIN_MSG_MAP(CGotoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CGotoDlg)
		DDX_TEXT(IDC_STATIC_PROMPT, m_Prompt)
		DDX_INT(IDC_EDIT_CURR, m_Curr)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		m_Prompt = _T("&Line number (1 - ");
		m_Prompt.AppendFormat(_T("%i"), m_Len);
		m_Prompt += _T("):");
		DoDataExchange();
		CenterWindow(GetParent());
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_Result = m_Curr;
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
};

INT_PTR DoModalGoto(int curr, int len, int &newLine)
{
	CGotoDlg dlg(curr, len);
	int nButton = dlg.DoModal();
	newLine = dlg.m_Result-1; //zero based
	return nButton;
}
//  ===========================================================================
class CSetTabSizeDlg : public CDialogImpl<CSetTabSizeDlg>, 
	public CWinDataExchange<CSetTabSizeDlg>
{
public:
	CString m_Prompt;
	int m_Curr;
	int m_Len;
	int m_Result;

public:
	CSetTabSizeDlg(int curr, int len)
	{
		m_Curr = curr;
		m_Len = len;
		m_Result = 0;
	}

public:
	enum { IDD = IDD_GOTO };

	BEGIN_MSG_MAP(CSetTabSizeDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CSetTabSizeDlg)
		DDX_TEXT(IDC_STATIC_PROMPT, m_Prompt)
		DDX_INT(IDC_EDIT_CURR, m_Curr)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		m_Prompt = _T("&Tab Size (1 - ");
		m_Prompt.AppendFormat(_T("%i"), m_Len);
		m_Prompt += _T("):");
		DoDataExchange();
		CenterWindow(GetParent());
		SetWindowText(_T("Set Tab Width"));
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_Result = m_Curr;
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
};

INT_PTR DoModalSetTabSize(int curr, int max, int &tabSize)
{
	CSetTabSizeDlg dlg(curr, max);
	int nButton = dlg.DoModal();
	tabSize = dlg.m_Result; //zero based
	return nButton;
}
//  ===========================================================================
class CGotoIDDlg : public CDialogImpl<CGotoDlg>, 
	public CWinDataExchange<CGotoDlg>
{
public:
	CString m_Prompt;
	CString m_Curr;
	CString m_Result;

public:
	CGotoIDDlg()
	{
	}

public:
	enum { IDD = IDD_GOTO };

	BEGIN_MSG_MAP(CGotoIDDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CGotoDlg)
		DDX_TEXT(IDC_STATIC_PROMPT, m_Prompt)
		DDX_TEXT(IDC_EDIT_CURR, m_Curr)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		m_Prompt = _T("&Subgraph/Vertex/Edge ID:");
		DoDataExchange();
		CenterWindow(GetParent());
		SetWindowText(_T("Goto Item"));
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_Result = m_Curr;
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
};

INT_PTR DoModalGotoID(std::_tstring &id)
{
	CGotoIDDlg dlg;
	int nButton = dlg.DoModal();
	id = dlg.m_Result;
	return nButton;
}
