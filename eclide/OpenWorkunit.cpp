#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "OpenWorkunit.h"
#include "repository.h"

class COpenWorkunit : public WTL::CDialogImpl<COpenWorkunit>, public WTL::CWinDataExchange<COpenWorkunit>
{
protected:
	CString &m_Label;

public:
	bool m_ok;
	enum { IDD = IDD_OPEN_WORKUNIT };

	COpenWorkunit(CString &label) : m_Label(label)
	{
		m_ok = false;
	}

	BEGIN_MSG_MAP(COpenWorkunit)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(COpenWorkunit)
		DDX_TEXT(IDC_EDIT_LABEL, m_Label)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		DoDataExchange();
		CenterWindow(GetParent());
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_ok = true;
		m_Label.Trim();
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_ok = false;
		EndDialog(nID);
	}
};

bool GetWorkunit(CString & label, HWND hWndParent)
{
	COpenWorkunit dlg(label);
	dlg.DoModal(hWndParent);
	return dlg.m_ok;
}

