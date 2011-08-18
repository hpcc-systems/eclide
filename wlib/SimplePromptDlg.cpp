#include "StdAfx.h"
#include "..\en_us\resource.h"

#include ".\simplepromptdlg.h"

class CSimplePromptDlg : 
	public CDialogImpl<CSimplePromptDlg>, 
	public CWinDataExchange<CSimplePromptDlg>
{
protected:
	CString m_Title;
	CString m_Label;

public:
	CString m_Prompt;

	enum { IDD = IDD_SIMPLE_PROMPT };

	CSimplePromptDlg(const CString &title, const CString &label, const TCHAR *initPrompt)
		: m_Title(title), m_Label(label)
	{
		if ( initPrompt )
			m_Prompt = initPrompt;
	}

	~CSimplePromptDlg(void)
	{
	}

	BEGIN_MSG_MAP(CSimplePromptDlg)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CSimplePromptDlg)
		DDX_TEXT(IDC_EDIT, m_Prompt)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		SetWindowText(m_Title);
		GetDlgItem(IDC_STATIC).SetWindowText(m_Label);
		DoDataExchange();
		CenterWindow(GetParent());
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
};

bool ShowSimplePrompt(HWND hwnd, const TCHAR *title, const TCHAR *label, CString &promptValue)
{
	CSimplePromptDlg dlg(title,label,promptValue);
	if ( IDOK == dlg.DoModal(hwnd) )
	{
		promptValue = dlg.m_Prompt;
		return true;
	}
	return false;
}
