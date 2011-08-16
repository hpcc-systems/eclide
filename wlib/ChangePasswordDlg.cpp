#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "changepassworddlg.h"
#include "global.h"
#include "account.h"

class CChangePasswordDlg : public CDialogImpl<CChangePasswordDlg>, 
	public CWinDataExchange<CChangePasswordDlg>
{
public:
	CString m_userID;
	CString m_oldPW;
	CString m_newPW;
	CString m_confirmPW;

	enum { IDD = IDD_CHANGEPASSWORD };

	BEGIN_MSG_MAP(CChangePasswordDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CChangePasswordDlg)
		DDX_TEXT(IDC_EDIT_USER, m_userID)
		DDX_TEXT(IDC_EDIT_OLD, m_oldPW)
		DDX_TEXT(IDC_EDIT_NEW, m_newPW)
		DDX_TEXT(IDC_EDIT_CONFIRM, m_confirmPW)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		m_userID = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		DoDataExchange();
		CenterWindow(GetParent());
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		int retCode;
		CString retMsg;
		ChangePassword(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ACCOUNT), m_userID, m_oldPW, m_newPW, m_confirmPW, retCode, retMsg);
		if (retCode)
		{
			MessageBox(retMsg, _T("Change Password"), MB_ICONASTERISK);
		}
		else
		{
			GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_PASSWORD, m_newPW);
			EndDialog(nID);
		}
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}

};

INT_PTR DoModalChangePassword()
{
	CChangePasswordDlg dlg;
	int nButton = dlg.DoModal();
	return nButton;
}

