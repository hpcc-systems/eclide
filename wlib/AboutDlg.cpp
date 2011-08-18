#include "stdafx.h"
#include "..\en_us\resource.h"

#include "AboutDlg.h"
//#include "file_ver.h"
#include "comms.h"
#include "smc.h"
#include "LoginDlg.h"

const TCHAR * const warning = _T("Warning: This computer program is protected by copyright law and international treaties. Unauthorized reproduction or distrubution of this program, or any protion of it, may result in severe civil and criminal penalties, and will be prosecuted to the maximum extent possible under the law.");

//  ===========================================================================
class CAboutDlg : public CDialogImpl<CAboutDlg>, 
	public CWinDataExchange<CAboutDlg>
{
protected:
	CString m_warning;
	CBevelLine m_ctlBevel;
	std::_tstring m_title;
	CString m_product;
	CIcon m_icon;

public:
	enum { IDD = IDD_ABOUTBOX };

	CAboutDlg(const std::_tstring & title, _U_STRINGorID icon_id) 
	{
		m_title = _T("About ");
		m_title += title;
		m_product = CString(title.c_str()) + _T(" by HPCC Systems.");
		m_warning = warning;
		m_icon.LoadIcon(icon_id, 128, 128, LR_COPYFROMRESOURCE);
	}

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAboutDlg)
		DDX_TEXT(IDC_STATIC_PRODUCT, m_product)
		DDX_TEXT(IDC_STATIC_WARNING, m_warning)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		SetWindowText(m_title.c_str());
		CStatic logo = GetDlgItem(IDC_STATIC_ICON);
		logo.SetWindowPos(HWND_BOTTOM, 5, 5, 128, 128, SWP_SHOWWINDOW); 
		logo.SetIcon(m_icon);

		m_ctlBevel.SubclassWindow(GetDlgItem(IDC_STATIC_BEVEL));

		DoDataExchange();

		CenterWindow(GetParent());

		std::_tstring version = _T("Version:\t");
		GetAboutVersion(version);
		SetDlgItemText(IDC_STATIC_VERSION, version.c_str());

		CComPtr<SMC::ISMC> smc = SMC::AttachSMC(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC), _T("SMC"));

		std::_tstring server = _T("Server:\t");
		CComPtr<SMC::IVersion> serverVersion = smc->GetBuild();
		server += serverVersion->GetString();
		SetDlgItemText(IDC_STATIC_SERVER, server.c_str());

		CenterWindow(GetParent());
		return TRUE;
	}
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
//  ===========================================================================
void DoModalAbout(const std::_tstring & title, _U_STRINGorID icon_id)
{
	CAboutDlg dlg(title, icon_id);
	dlg.DoModal();
}
//  ===========================================================================
