#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "logindlg.h"

#include "account.h"
#include <SMC.h> //comms
#include "file_ver.h"
#include "thread.h"
#include "PreferenceDlg.h"
#include "EclCC.h"
#include <CustomMessages.h>
#include <AutoUpdate.h>
#include <UtilFilesystem.h>

static const SectionLabelDefault GLOBAL_LASTCONFIG(SectionLabel(_T("General"), _T("LastConfig")), _T("default"));
static const SectionLabelDefault GLOBAL_LASTCONFIG_LHS(SectionLabel(_T("General"), _T("LastConfigLHS")), _T("default"));
static const SectionLabelDefault GLOBAL_LASTCONFIG_RHS(SectionLabel(_T("General"), _T("LastConfigRHS")), _T("default"));
static const SectionLabelDefault GLOBAL_LASTFOLDER_LHS(SectionLabel(_T("General"), _T("LastPathLHS")), _T(""));
static const SectionLabelDefault GLOBAL_LASTFOLDER_RHS(SectionLabel(_T("General"), _T("LastPathRHS")), _T(""));
static const SectionLabelDefault GLOBAL_LASTMODFILE_LHS(SectionLabel(_T("General"), _T("LastModFileLHS")), _T(""));
static const SectionLabelDefault GLOBAL_LASTMODFILE_RHS(SectionLabel(_T("General"), _T("LastModFileRHS")), _T(""));
static const SectionLabelDefault GLOBAL_LASTMODE_LHS(SectionLabel(_T("General"), _T("LastModeLHS")), 1);
static const SectionLabelDefault GLOBAL_LASTMODE_RHS(SectionLabel(_T("General"), _T("LastModeRHS")), 1);

enum UM
{
	UM_FIRST = CWM_LAST + 1,
	UM_AUTOUPDATEFINISHED,
	UM_LAST
};

class CLoginDlg : public CDialogImpl<CLoginDlg>, 
	public CWinDataExchange<CLoginDlg>
{
	typedef CLoginDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	CString m_User;
	CString m_Password;
	CComboBox m_Combo;
	IConfigAdapt m_IniFile;
	IConfigAdapt m_config;
	CString m_DefaultConfig;
	ILoginConfigPreferences *m_configPrefs;
	CString m_Msg;
	CString m_Title;
	bool m_verifyUser;
	CComPtr<IAutoUpdate> m_autoupdate;
	CHyperLink m_autoUpdateLink;

public:
	CLoginDlg(bool verifyUser) : m_verifyUser(verifyUser)
	{
		m_configPrefs = 0;
		m_autoupdate = CreateIAutoUpdate();//, _T("/download/getting-started"));//_T("/eclide/version.txt"));
	}

	CString m_ConfigLabel;

	enum { IDD = IDD_LOGIN };

	void SetIni(IConfig *iniFile) 
	{ 
		m_IniFile = iniFile; 
	}
	void SetConfig(IConfig *config) 
	{ 
		m_config = config; 
	}
	void SetTitle(const TCHAR *pszTitle) 
	{ 
		m_Title = pszTitle; 
	}
	void SetLoginConfigPreferences(ILoginConfigPreferences *configPrefs )
	{ 
		m_configPrefs = configPrefs; 
	}

	void RefreshTitle()
	{
		std::_tstring version;
		GetAboutVersion(version);
		CString title = m_Title + _T(" ") + version.c_str();
		SetWindowText(title);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_HANDLER_EX(UM_AUTOUPDATEFINISHED, OnAutoUpdateFinished)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG, CBN_CLOSEUP, OnCbnSelendokComboConfig)
		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG, CBN_SELENDOK, OnCbnSelendokComboConfig)
		COMMAND_HANDLER(IDC_LOGIN_PREFS, BN_CLICKED, OnBnClickedLoginPrefs)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_USER, m_User)
		DDX_TEXT(IDC_EDIT_PASSWORD, m_Password)
		DDX_TEXT(IDC_STATIC_MSG, m_Msg)
	END_DDX_MAP()

	static void thread_CheckForUpdates(CLoginDlg * self, CComPtr<IAutoUpdate> autoupdate)
	{
		bool hasUpdate = autoupdate->HasUpdate(_T("hpccsystems.com"), _T("/sites/default/files/eclide/version.txt"), (const TCHAR *)GetApplicationVersion());
		self->PostMessageW(UM_AUTOUPDATEFINISHED, (WPARAM) hasUpdate);
	}

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;

		if ( m_IniFile.isLinked() )
		{
			m_DefaultConfig = m_IniFile->Get(GLOBAL_LASTCONFIG);
		}

		if (m_Title.IsEmpty())
		{
			GetWindowText(m_Title);
		}

		RefreshTitle();

		CenterWindow(GetParent());

		m_Combo = GetDlgItem(IDC_COMBO_CONFIG);
		m_ConfigLabel = m_DefaultConfig;
		if (!::PopulateConfigCombo(m_Combo, (const TCHAR *)m_DefaultConfig))
		{
			m_configPrefs->ShowConfigPreference(m_config);
			//stuff could have changed, so reup
			m_ConfigLabel = m_config->GetLabel();
			::PopulateConfigCombo(m_Combo, (const TCHAR *)m_ConfigLabel);
		}

		m_autoUpdateLink.SubclassWindow(GetDlgItem(IDC_STATIC_UPGRADE));

		RefreshGlobal();

		return TRUE;
	}
	void OnClose()
	{
		SetMsgHandled(false);
	}

	LRESULT OnAutoUpdateFinished(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
	{
		if (IsWindow() && wParam)
		{
			m_autoUpdateLink.SetLabel((boost::_tformat(_T("New ECL IDE available (%1%), click here to download!")) % m_autoupdate->GetResponse()).str().c_str());
			m_autoUpdateLink.SetHyperLink(_T("http://hpccsystems.com/download/eclide/setup.html"));
			m_autoUpdateLink.ShowWindow(SW_SHOW);
			m_autoUpdateLink.Invalidate();
		}
		return 0;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoDataExchange(true);

		if (m_IniFile.isLinked() && m_DefaultConfig != m_ConfigLabel)
		{
			::SetDefaultConfig(m_IniFile, static_cast<const TCHAR *>(m_ConfigLabel));
		}

		m_config->Set(GLOBAL_USER, m_User);
		m_config->Set(GLOBAL_PASSWORD, m_Password);
		//if ( !VerifyCommsServerConfig() )
		//{
		//	MessageBox(_T("You still need to configure one or more servers. Click on Preferences to configure your servers."), m_Title);
		//	return;
		//}

		int retCode = 0;
		CString retMsg;

		m_autoUpdateLink.ShowWindow(SW_HIDE);
		CString accountServer = m_config->Get(GLOBAL_SERVER_ACCOUNT);
		if (!m_verifyUser || accountServer.IsEmpty() || VerifyUser(m_config, m_User, m_Password, retCode, retMsg))
		{
			ReleaseAllSingletons();
			//prime the repository cache with the new credentials
			CString server(m_config->Get(GLOBAL_SERVER_ATTRIBUTE));
			CComPtr<IRepository> rep = ::AttachRepository(server, m_User, m_Password, m_ConfigLabel);
			EndDialog(nID);
		}
		else
		{
			MessageBeep(MB_ICONEXCLAMATION);
			m_Msg.Format(_T("%i:  %s"), retCode, retMsg);
			DoDataExchange();
		}
	}
	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}

	BOOL EndDialog(int nRetCode)
	{
		return baseClass::EndDialog(nRetCode);
	}
	void OnCbnSelendokComboConfig(UINT /*nId*/, int /*wID*/, HWND /*hWndCtl*/)
	{
		//get name of selected config
		m_Combo.GetLBText(m_Combo.GetCurSel(), m_ConfigLabel);
		RefreshGlobal();
	}

	void RefreshGlobal()
	{
		DoDataExchange(true);
		boost::filesystem::path configFile;
		GetConfigPath(static_cast<const TCHAR *>(m_ConfigLabel), configFile);
		CComPtr<IConfig> config = CreateIConfig(QUERYBUILDER_CFG, configFile);
		CString accountServer = config->Get(GLOBAL_SERVER_ACCOUNT);
		if (accountServer.IsEmpty())
		{
			GetDlgItem(IDC_EDIT_USER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_PASSWORD).EnableWindow(false);
			m_User = _T("");
			m_Password = _T("");
		}
		else
		{
			GetDlgItem(IDC_EDIT_USER).EnableWindow(true);
			GetDlgItem(IDC_EDIT_PASSWORD).EnableWindow(true);
			m_User = config->Get(GLOBAL_USER);
			m_Password = config->Get(GLOBAL_PASSWORD);
		}
		if (m_configPrefs)
			GetDlgItem(IDC_LOGIN_PREFS).EnableWindow(m_ConfigLabel.IsEmpty()==false);
		DoDataExchange();
		m_autoUpdateLink.ShowWindow(SW_HIDE);
		if (!(bool)m_IniFile->Get(GLOBAL_DISABLEAUTOUPDATE))
			clib::thread run(__FUNCTION__, boost::bind(&thread_CheckForUpdates, this, m_autoupdate));

	}
	LRESULT OnBnClickedLoginPrefs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(true);
		m_configPrefs->ShowConfigPreference(m_config);
		//stuff could have changed, so reup
		m_ConfigLabel = m_config->GetLabel();
		::PopulateConfigCombo(m_Combo, (const TCHAR *)m_ConfigLabel);
		RefreshGlobal();
		return 0;
	}
};

INT_PTR DoModalLogin(ILoginConfigPreferences *prefs, IConfig* configIni, IConfig* config, bool verifyUser, const TCHAR *title)
{
	CLoginDlg dlg(verifyUser);
	if ( prefs )
		dlg.SetLoginConfigPreferences(prefs);
	if ( configIni )
		dlg.SetIni(configIni);
	if ( config )
		dlg.SetConfig(config);
	if ( title )
		dlg.SetTitle(title);
	return dlg.DoModal();
}

enum MODE
{
	MODE_REPOSITORY = 0,
	MODE_FOLDER,
	MODE_MODULE_FILE,
	MODE_LAST
};

class CLoginDlgAMT : public CDialogImpl<CLoginDlgAMT>, 
	public CWinDataExchange<CLoginDlgAMT>
{
	typedef CLoginDlgAMT thisClass;
	typedef CDialogImpl<thisClass> baseClass;

protected:
	IConfigAdapt m_IniFile;
	ILoginConfigPreferences *m_configPrefs;

	MODE m_modeLHS; 
	CComboBox m_ComboLHSMode;
	CComboBox m_ComboLHS;
	CString m_UserLHS;
	CString m_PathLHS;
	CString m_PasswordLHS;
	CString m_DefaultConfigLHS;
	IConfig *m_configLHS;
	CString m_MsgLHS;
	StlLinked<IRepository> m_repositoryLHS;

	MODE m_modeRHS; 
	CComboBox m_ComboRHSMode;
	CComboBox m_ComboRHS;
	CString m_UserRHS;
	CString m_PathRHS;
	CString m_PasswordRHS;
	CString m_DefaultConfigRHS;
	IConfig *m_configRHS;
	CString m_MsgRHS;
	StlLinked<IRepository> m_repositoryRHS;

public:

	CLoginDlgAMT()
	{
		m_configPrefs = 0;
	}

	CString m_ConfigLabelLHS;
	CString m_ConfigLabelRHS;

	enum { IDD = IDD_LOGIN_AMT };

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG, CBN_CLOSEUP, OnCbnSelendokComboConfig)
		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG, CBN_SELENDOK, OnCbnSelendokComboConfig)
		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG_RHS, CBN_CLOSEUP, OnCbnSelendokComboConfigRHS)
		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG_RHS, CBN_SELENDOK, OnCbnSelendokComboConfigRHS)
		COMMAND_HANDLER(IDC_LOGIN_PREFS, BN_CLICKED, OnBnClickedLoginPrefs)
		COMMAND_HANDLER(IDC_LOGIN_PREFS_RHS, BN_CLICKED, OnBnClickedLoginPrefsRHS)
		COMMAND_HANDLER(IDC_COMBO_SOURCEMODE, CBN_CLOSEUP, OnCbnSelendokComboSourceMode)
		COMMAND_HANDLER(IDC_COMBO_SOURCEMODE, CBN_SELENDOK, OnCbnSelendokComboSourceMode)
		COMMAND_HANDLER(IDC_COMBO_TARGETMODE, CBN_CLOSEUP, OnCbnSelendokComboTargetMode)
		COMMAND_HANDLER(IDC_COMBO_TARGETMODE, CBN_SELENDOK, OnCbnSelendokComboTargetMode)
		COMMAND_HANDLER(IDC_LOCAL_PATH_BROWSE, BN_CLICKED, OnBnClickedLocalPathBrowse)
		COMMAND_HANDLER(IDC_LOCAL_PATH_BROWSE_RHS, BN_CLICKED, OnBnClickedLocalPathBrowseRhs)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_USER, m_UserLHS)
		DDX_TEXT(IDC_LOCAL_PATH, m_PathLHS)
		DDX_TEXT(IDC_EDIT_PASSWORD, m_PasswordLHS)
		DDX_TEXT(IDC_STATIC_MSG, m_MsgLHS)

		DDX_TEXT(IDC_EDIT_USER_RHS, m_UserRHS)
		DDX_TEXT(IDC_LOCAL_PATH_RHS, m_PathRHS)
		DDX_TEXT(IDC_EDIT_PASSWORD_RHS, m_PasswordRHS)
		DDX_TEXT(IDC_STATIC_MSG_RHS, m_MsgRHS)
	END_DDX_MAP()

	MODE GetMode(bool lhs = true)
	{
		if (lhs) 
			return m_modeLHS;
		else
			return m_modeRHS;
	}

	IConfig * GetConfig(bool lhs = true)
	{
		if (lhs) 
			return m_configLHS;
		else
			return m_configRHS;
	}

	IRepository * GetRepository(bool lhs = true)
	{
		if (lhs) 
			return m_repositoryLHS;
		else
			return m_repositoryRHS;
	}

	void SetLoginConfigPreferences(ILoginConfigPreferences *configPrefs )
	{ 
		m_configPrefs = configPrefs; 
	}
	void SetIni(IConfig *iniFile) 
	{ 
		m_IniFile = iniFile; 
		m_configLHS = CreateIConfig(AMT_CFG_LHS); 
		m_configRHS = CreateIConfig(AMT_CFG_RHS); 
	}
	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;

		if ( m_IniFile.isLinked() )
		{
			m_DefaultConfigLHS = m_IniFile->Get(GLOBAL_LASTCONFIG_LHS);
			m_DefaultConfigRHS = m_IniFile->Get(GLOBAL_LASTCONFIG_RHS);
		}

		CenterWindow(GetParent());

		m_ComboLHSMode = GetDlgItem(IDC_COMBO_SOURCEMODE);
		m_ComboLHSMode.AddString(_T("Repository"));
		m_ComboLHSMode.AddString(_T("Folder"));
		m_ComboLHSMode.AddString(_T("Module File"));

		m_ComboLHS = GetDlgItem(IDC_COMBO_CONFIG);
		m_ConfigLabelLHS = m_DefaultConfigLHS;
		::PopulateConfigCombo(m_ComboLHS, (const TCHAR *)m_DefaultConfigLHS);

		m_ComboRHSMode = GetDlgItem(IDC_COMBO_TARGETMODE);
		m_ComboRHSMode.AddString(_T("Repository"));
		m_ComboRHSMode.AddString(_T("Folder"));
		m_ComboRHSMode.AddString(_T("Module File"));

		m_ComboRHS = GetDlgItem(IDC_COMBO_CONFIG_RHS);
		m_ConfigLabelRHS = m_DefaultConfigRHS;
		::PopulateConfigCombo(m_ComboRHS, (const TCHAR *)m_DefaultConfigRHS);

		RefreshGlobal(true, true);
		m_ComboLHSMode.SetCurSel(m_modeLHS);
		m_ComboRHSMode.SetCurSel(m_modeRHS);

		BOOL handled = FALSE;
		OnCbnSelendokComboSourceMode(0, IDC_COMBO_SOURCEMODE, 0, handled);
		OnCbnSelendokComboTargetMode(0, IDC_COMBO_TARGETMODE, 0, handled);

		return TRUE;
	}
	void OnClose()
	{
		SetMsgHandled(false);
	}
	static void thread_VerifyUser(IConfig * config, CString user, CString password, int *pRetCode, CString *pRetMsg, bool *result)
	{
		*result = VerifyUser(config, user, password, *pRetCode, *pRetMsg);
	}
	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoDataExchange(true);
		m_modeLHS = (MODE)m_ComboLHSMode.GetCurSel();
		m_modeRHS = (MODE)m_ComboRHSMode.GetCurSel();

		m_MsgLHS = _T("");
		m_MsgRHS = _T("");

		bool success = true;

		if (m_IniFile.isLinked() && ((m_DefaultConfigLHS != m_ConfigLabelLHS) || (m_DefaultConfigRHS != m_ConfigLabelRHS)))
		{
			m_IniFile->Set(GLOBAL_LASTCONFIG_LHS, m_ConfigLabelLHS);
			m_IniFile->Set(GLOBAL_LASTCONFIG_RHS, m_ConfigLabelRHS);
		}

		if (m_IniFile.isLinked())
		{
			m_IniFile->Set(GLOBAL_LASTMODE_LHS, m_modeLHS);
			m_IniFile->Set(GLOBAL_LASTMODE_RHS, m_modeRHS);
			SavePathLHS(m_modeLHS);
			SavePathRHS(m_modeRHS);
		}

		m_configLHS->Set(GLOBAL_USER, m_UserLHS);
		m_configRHS->Set(GLOBAL_USER, m_UserRHS);
		m_configLHS->Set(GLOBAL_PASSWORD, m_PasswordLHS);
		m_configRHS->Set(GLOBAL_PASSWORD, m_PasswordRHS);

		::ReleaseAllSingletons();
		if (m_modeLHS == MODE_REPOSITORY || m_modeRHS == MODE_REPOSITORY)
		{
			if ( !VerifyCommsServerConfig() )
			{
				MessageBox(_T("You still need to configure one or more servers. Click on Preferences to configure your servers."), _T("AMT Login"));
				return;
			}
		}
		if (m_modeLHS == MODE_FOLDER)
		{
			if (!clib::filesystem::exists((const char *)CT2A(m_PathLHS)))
			{
				MessageBox(_T("Source folder does not exist."), _T("AMT Login"));
				return;
			}
		}
		if (m_modeRHS == MODE_FOLDER)
		{
			if (!clib::filesystem::exists((const char *)CT2A(m_PathRHS)))
			{
				MessageBox(_T("Target folder does not exist."), _T("AMT Login"));
				return;
			}
		}

		int LHSRetCode = 0;
		CString LHSRetMsg;
		bool LHSVerify = false;
		int RHSRetCode = 0;
		CString RHSRetMsg;
		bool RHSVerify = false;
		if (m_modeLHS == MODE_REPOSITORY && m_modeRHS == MODE_REPOSITORY)
		{
			clib::thread lhs_run(__FUNCTION__, boost::bind(&thread_VerifyUser, m_configLHS, m_UserLHS, m_PasswordLHS, &LHSRetCode, &LHSRetMsg, &LHSVerify));
			clib::thread rhs_run(__FUNCTION__, boost::bind(&thread_VerifyUser, m_configRHS, m_UserRHS, m_PasswordRHS, &RHSRetCode, &RHSRetMsg, &RHSVerify));
			lhs_run.join();
			rhs_run.join();
		}
		else if (m_modeLHS == MODE_REPOSITORY)
		{
			LHSVerify = VerifyUser(m_configLHS, m_UserLHS, m_PasswordLHS, LHSRetCode, LHSRetMsg);
		}
		else if (m_modeRHS == MODE_REPOSITORY)
		{
			RHSVerify = VerifyUser(m_configRHS, m_UserRHS, m_PasswordRHS, RHSRetCode, RHSRetMsg);
		}

		switch(m_modeLHS)
		{
		case MODE_REPOSITORY:
			if (LHSVerify)
			{
				m_repositoryLHS = ::AttachRepository(CString(m_configLHS->Get(GLOBAL_SERVER_ATTRIBUTE)), m_UserLHS, m_PasswordLHS, m_ConfigLabelLHS, _T("LHS"));
			}
			else
			{
				m_MsgLHS.Format(_T("%i:  %s"), LHSRetCode, LHSRetMsg);
				success = false;
			}
			break;
		case MODE_FOLDER:
			m_repositoryLHS = ::AttachDiskRepository(m_PathLHS, m_PathLHS, true);
			success = success && true;
			break;
		case MODE_MODULE_FILE:
			m_repositoryLHS = ::AttachModFileRepository(m_PathLHS, true);
			success = success && true;
			break;
		}

		switch(m_modeRHS)
		{
		case MODE_REPOSITORY:
			if (RHSVerify)
			{
				m_repositoryRHS = ::AttachRepository(CString(m_configRHS->Get(GLOBAL_SERVER_ATTRIBUTE)), m_UserRHS, m_PasswordRHS, m_ConfigLabelRHS, _T("RHS"));
				success = success && true;
			}
			else
			{
				m_MsgRHS.Format(_T("%i:  %s"), RHSRetCode, RHSRetMsg);
				success = false;
			}
			break;
		case MODE_FOLDER:
			m_repositoryRHS = ::AttachDiskRepository(m_PathRHS, m_PathRHS, true);
			success = success && true;
			break;
		case MODE_MODULE_FILE:
			m_repositoryRHS = ::AttachModFileRepository(m_PathRHS, true);
			success = success && true;
		}

		if (success)
		{
			EndDialog(nID);
		}
		else
		{
			MessageBeep(MB_ICONEXCLAMATION);
			DoDataExchange();
		}
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}

	BOOL EndDialog(int nRetCode)
	{
		return baseClass::EndDialog(nRetCode);
	}
	void OnCbnSelendokComboConfig(UINT /*nId*/, int /*wID*/, HWND /*hWndCtl*/)
	{
		//get name of selected config
		m_ComboLHS.GetLBText(m_ComboLHS.GetCurSel(), m_ConfigLabelLHS);
		RefreshGlobal(true, false);
	}

	void OnCbnSelendokComboConfigRHS(UINT /*nId*/, int /*wID*/, HWND /*hWndCtl*/)
	{
		//get name of selected config
		m_ComboRHS.GetLBText(m_ComboRHS.GetCurSel(), m_ConfigLabelRHS);
		RefreshGlobal(false, true);
	}

	void SavePathLHS(MODE mode)
	{
		DoDataExchange(TRUE, IDC_LOCAL_PATH);
		switch (mode)
		{
		case MODE_FOLDER:
			m_IniFile->Set(GLOBAL_LASTFOLDER_LHS, m_PathLHS);
			break;
		case MODE_MODULE_FILE:
			m_IniFile->Set(GLOBAL_LASTMODFILE_LHS, m_PathLHS);
			break;
		}
	}

	void LoadPathLHS(MODE mode)
	{
		switch (mode)
		{
		case MODE_FOLDER:
			m_PathLHS = m_IniFile->Get(GLOBAL_LASTFOLDER_LHS);
			break;
		case MODE_MODULE_FILE:
			m_PathLHS = m_IniFile->Get(GLOBAL_LASTMODFILE_LHS);
			break;
		}
		DoDataExchange(FALSE, IDC_LOCAL_PATH);
	}

	void SavePathRHS(MODE mode)
	{
		DoDataExchange(TRUE, IDC_LOCAL_PATH_RHS);
		switch (mode)
		{
		case MODE_FOLDER:
			m_IniFile->Set(GLOBAL_LASTFOLDER_RHS, m_PathRHS);
			break;
		case MODE_MODULE_FILE:
			m_IniFile->Set(GLOBAL_LASTMODFILE_RHS, m_PathRHS);
			break;
		}
	}

	void LoadPathRHS(MODE mode)
	{
		switch (mode)
		{
		case MODE_FOLDER:
			m_PathRHS = m_IniFile->Get(GLOBAL_LASTFOLDER_RHS);
			break;
		case MODE_MODULE_FILE:
			m_PathRHS = m_IniFile->Get(GLOBAL_LASTMODFILE_RHS);
			break;
		}
		DoDataExchange(FALSE, IDC_LOCAL_PATH_RHS);
	}

	void RefreshGlobal(bool LHS, bool RHS)
	{
		DoDataExchange(true);
		if (LHS)
		{
			boost::filesystem::path configFileLHS;
			GetConfigPath(static_cast<const TCHAR *>(m_ConfigLabelLHS), configFileLHS);
			CComPtr<IConfig> configLHS = CreateIConfig(AMT_CFG_LHS, configFileLHS);
			m_UserLHS = configLHS->Get(GLOBAL_USER);
			m_PasswordLHS = configLHS->Get(GLOBAL_PASSWORD);
			m_modeLHS = (MODE)(UINT)m_IniFile->Get(GLOBAL_LASTMODE_LHS);

			LoadPathLHS(m_modeLHS);
		}
		if (RHS)
		{
			boost::filesystem::path configFileRHS;
			GetConfigPath(static_cast<const TCHAR *>(m_ConfigLabelRHS), configFileRHS);
			CComPtr<IConfig> configRHS = CreateIConfig(AMT_CFG_RHS, configFileRHS);
			m_UserRHS = configRHS->Get(GLOBAL_USER);
			m_PasswordRHS = configRHS->Get(GLOBAL_PASSWORD);
			m_modeRHS = (MODE)(UINT)m_IniFile->Get(GLOBAL_LASTMODE_RHS);

			LoadPathRHS(m_modeRHS);
		}
		DoDataExchange();
	}

	LRESULT OnBnClickedLoginPrefs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(true);
		m_configPrefs->ShowConfigPreference(m_configLHS);
		//stuff could have changed, so reup
		m_ConfigLabelLHS = m_configLHS->GetLabel();
		::PopulateConfigCombo(m_ComboLHS, (const TCHAR *)m_ConfigLabelLHS);
		RefreshGlobal(true, false);
		return 0;
	}

	LRESULT OnBnClickedLoginPrefsRHS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(true);
		m_configPrefs->ShowConfigPreference(m_configRHS);
		//stuff could have changed, so reup
		m_ConfigLabelRHS = m_configRHS->GetLabel();
		::PopulateConfigCombo(m_ComboRHS, (const TCHAR *)m_ConfigLabelRHS);
		RefreshGlobal(false, true);
		return 0;
	}

	LRESULT CLoginDlgAMT::OnCbnSelendokComboSourceMode(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_modeLHS = (MODE)m_ComboLHSMode.GetCurSel();

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_PATH), (m_modeLHS != MODE_REPOSITORY) ? SW_SHOW : SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_LOCAL_PATH), (m_modeLHS != MODE_REPOSITORY) ? SW_SHOW : SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_LOCAL_PATH_BROWSE), (m_modeLHS != MODE_REPOSITORY) ? SW_SHOW : SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_COMBO_CONFIG), (m_modeLHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_USER), (m_modeLHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_PASSWORD), (m_modeLHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_CONFIGURATION), (m_modeLHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_USER), (m_modeLHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_PASSWORD), (m_modeLHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);

		LoadPathLHS(m_modeLHS);

		return 0;
	}

	LRESULT CLoginDlgAMT::OnCbnSelendokComboTargetMode(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_modeRHS = (MODE)m_ComboRHSMode.GetCurSel();

		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_PATH_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_SHOW : SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_LOCAL_PATH_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_SHOW : SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_LOCAL_PATH_BROWSE_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_SHOW : SW_HIDE);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_COMBO_CONFIG_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_USER_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_EDIT_PASSWORD_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_CONFIGURATION_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_USER_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);
		::ShowWindow(::GetDlgItem(m_hWnd, IDC_STATIC_PASSWORD_RHS), (m_modeRHS != MODE_REPOSITORY) ? SW_HIDE : SW_SHOW);

		LoadPathRHS(m_modeRHS);

		return 0;
	}

	void CheckForModuleFolders(std::_tstring & path)
	{
		bool hasChildFolder = false;
		boost::filesystem::path folder = stringToPath(path);
		try
		{
			boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
			for ( boost::filesystem::directory_iterator folder_itr(folder); folder_itr != end_itr; ++folder_itr)
			{
				if (clib::filesystem::is_directory(*folder_itr))
				{
					hasChildFolder = true;
					break;
				}
			}
		}
		catch (const std::exception &)
		{
		}

		if (!hasChildFolder)
		{
			if (MessageBox((boost::_tformat(_T("The selected folder contains no \"Child\" folders, use %1% instead?")) % pathToWString(folder.parent_path())).str().c_str(), _T("AMT"), MB_YESNO) == IDYES)
				path = pathToWString(folder.parent_path());
		}
	}

	LRESULT CLoginDlgAMT::OnBnClickedLocalPathBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_modeLHS == MODE_FOLDER)
		{
			std::_tstring path;
			path.resize(_MAX_PATH);
			::GetWindowText(::GetDlgItem(m_hWnd,IDC_LOCAL_PATH), &path[0], _MAX_PATH);
			CFolderDialog dlg;
			dlg.m_lpstrInitialFolder = path.c_str();
			if (IDOK == dlg.DoModal())
			{
				path = dlg.GetFolderPath();
				CheckForModuleFolders(path);
				::SetWindowText(::GetDlgItem(m_hWnd,IDC_LOCAL_PATH), path.c_str());
			}
		}
		else
		{
			static const TCHAR szEclFilter[] = _T("Module Files (*.mod)\0*.mod\0All Files (*.*)\0*.*\0\0");
			CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szEclFilter, m_hWnd);	
			if (IDOK == dlg.DoModal())
			{
				::SetWindowText(::GetDlgItem(m_hWnd,IDC_LOCAL_PATH), dlg.m_szFileName);
			}
		}
		return 0;
	}

	LRESULT CLoginDlgAMT::OnBnClickedLocalPathBrowseRhs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_modeRHS == MODE_FOLDER)
		{
			std::_tstring path;
			path.resize(_MAX_PATH);
			::GetWindowText(::GetDlgItem(m_hWnd,IDC_LOCAL_PATH_RHS), &path[0], _MAX_PATH);
			CFolderDialog dlg;
			dlg.m_lpstrInitialFolder = path.c_str();
			if (IDOK == dlg.DoModal())
			{
				path = dlg.GetFolderPath();
				CheckForModuleFolders(path);
				::SetWindowText(::GetDlgItem(m_hWnd,IDC_LOCAL_PATH_RHS), path.c_str());
			}
		}
		else
		{
			static const TCHAR szEclFilter[] = _T("Module Files (*.mod)\0*.mod\0All Files (*.*)\0*.*\0\0");
			CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szEclFilter, m_hWnd);	
			if (IDOK == dlg.DoModal())
			{
				::SetWindowText(::GetDlgItem(m_hWnd,IDC_LOCAL_PATH_RHS), dlg.m_szFileName);
			}
		}
		return 0;
	}
};

INT_PTR DoModalLoginAMT(ILoginConfigPreferences *prefs, IConfig* configIni, StlLinked<IRepository> & repositoryLHS, StlLinked<IRepository> & repositoryRHS, STRUCT_DALI & daliLHS, STRUCT_DALI & daliRHS)
{
	repositoryLHS = NULL;
	repositoryRHS = NULL;

	CLoginDlgAMT dlg;
	dlg.SetLoginConfigPreferences(prefs);
	dlg.SetIni(configIni);
	INT_PTR result = dlg.DoModal();
	if (IDOK == result)
	{
		repositoryLHS = dlg.GetRepository(true);
		repositoryRHS = dlg.GetRepository(false);
		if (dlg.GetMode(true) == MODE_REPOSITORY)
		{
			daliLHS.m_dali = Dali::AttachDali(CString(dlg.GetConfig(true)->Get(GLOBAL_SERVER_WORKUNIT)), dlg.GetConfig(true)->GetLabel());
			daliLHS.cluster = CString(dlg.GetConfig(true)->Get(GLOBAL_CLUSTER));
			daliLHS.queue = CString(dlg.GetConfig(true)->Get(GLOBAL_QUEUE));
		}
		if (dlg.GetMode(false) == MODE_REPOSITORY)
		{
			daliRHS.m_dali = Dali::AttachDali(CString(dlg.GetConfig(false)->Get(GLOBAL_SERVER_WORKUNIT)), dlg.GetConfig(false)->GetLabel());
			daliRHS.cluster = CString(dlg.GetConfig(false)->Get(GLOBAL_CLUSTER));
			daliRHS.queue = CString(dlg.GetConfig(false)->Get(GLOBAL_QUEUE));
		}
	}
	return result;
}

bool PopulateConfigCombo(CComboBox &configCombo, const std::_tstring & defaultValue)
{
	bool retVal = true;
	int matchID = -1;
	configCombo.ResetContent();
	boost::filesystem::path folder;
	GetApplicationFolder(folder);
	boost::filesystem::directory_iterator end_itr; 
	for (boost::filesystem::directory_iterator itr(folder); itr != end_itr; ++itr)
	{
		if (!clib::filesystem::is_directory(*itr))
		{
			if (boost::algorithm::iequals(pathToString(itr->path().extension()), _T(".cfg")))
			{
				std::string s = pathToString(itr->path().leaf());
				std::_tstring label = CA2T(s.substr(0, s.length() - 4).c_str());
				int id = configCombo.AddString(label.c_str());
				if (defaultValue.length() && defaultValue == label)
				{
					matchID = id;
					configCombo.SetCurSel(id);
				}
			}
		}
	}
	if(configCombo.GetCount() == 0)
	{
		configCombo.AddString(_T("default"));
		retVal = false;
	}
	if ( matchID < 0 )
		configCombo.SetCurSel(0);
	return retVal;
}

void SetDefaultConfig(IConfig *iniFile, const std::_tstring & defaultConfig)
{
	iniFile->Set(GLOBAL_LASTCONFIG, defaultConfig);
}

//  ===========================================================================
const TCHAR * GetAboutVersion(std::_tstring &version)
{
	std::_tstring buildVer;
	version += GetBuildVersion(buildVer);
	return version.c_str();
}

