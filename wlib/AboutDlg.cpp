#include "stdafx.h"
#include "..\en_us\resource.h"

#include "AboutDlg.h"
//#include "file_ver.h"
#include "comms.h"
#include "smc.h"
#include "LoginDlg.h"
#include <EclCC.h>

const TCHAR * const warning = _T("HPCC SYSTEMS software Copyright (C) 2022 HPCC Systems®.\r\n"
"\r\n"
"Licensed under the Apache License, Version 2.0 (the \"License\");\r\n"
"you may not use this file except in compliance with the License.\r\n"
"You may obtain a copy of the License at\r\n"
"\r\n"
"http ://www.apache.org/licenses/LICENSE-2.0\r\n"
"\r\n"
"Unless required by applicable law or agreed to in writing, software\r\n"
"distributed under the License is distributed on an \"AS IS\" BASIS,\r\n"
"WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\r\n"
"See the License for the specific language governing permissions and\r\n"
"limitations under the License.");

//  ===========================================================================
class CAboutDlg : public CDialogImpl<CAboutDlg>, 
	public CWinDataExchange<CAboutDlg>
{
protected:
	CBevelLine m_ctlBevel;
	std::_tstring m_title;
	std::_tstring m_version;
	std::_tstring m_server;
	std::_tstring m_compiler;
	CString m_product;
	CString m_warning;
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
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnCopy)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CAboutDlg)
		DDX_TEXT(IDC_STATIC_PRODUCT, m_product)
		DDX_TEXT(IDC_EDIT_WARNING, m_warning)
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

		m_version = _T("Version:\t\t");
		GetAboutVersion(m_version);
		SetDlgItemText(IDC_STATIC_VERSION, m_version.c_str());

		CComPtr<SMC::ISMC> smc = SMC::AttachSMC(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC), _T("SMC"));

		m_server = _T("Server:\t\t");
		CComPtr<SMC::IVersion> serverVersion = smc->GetBuild();
		std::_tstring version;
		m_server += serverVersion->GetString(version);
		SetDlgItemText(IDC_STATIC_SERVER, m_server.c_str());

		m_compiler = _T("Compiler:\t");
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
			m_compiler += eclcc->GetVersionString(version);
		else
			m_compiler += _T("Unknown");
		SetDlgItemText(IDC_STATIC_COMPILER, m_compiler.c_str());

		CenterWindow(GetParent());
		return TRUE;
	}

	LRESULT OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SetClipboard((boost::_tformat(_T("%1%\r\n%2%\r\n%3%\r\n")) % m_version % m_server % m_compiler).str());
		return 0;
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
