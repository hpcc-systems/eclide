#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "preferencedlg.h"

#include "global.h"
#include "cmdProcess.h"
#include "loginDlg.h"
#include "SimplePromptDlg.h"
#include "Combo.h"
#include "Dali.h"
#include "ShellHelper.h"
#include "ColorButton.h"
#include "LangRef.h"
#include "EclCommand.h"
#include "EclParser.h"
//#include "atlGraphView.h"
//#include "GraphViewCtl.h"
#include "cmdProcess.h"
#include <EclCC.h>
#include "npHPCCSystemsGraphViewControl.h"
#include "HListBox.h"
//  ===========================================================================
#define GLYPH_WIDTH 15

class CFontComboBox : public CWindowImpl<CFontComboBox, CComboBox>, public COwnerDraw<CFontComboBox>
{
	typedef CWindowImpl<CFontComboBox, CComboBox> thisClass;
	typedef COwnerDraw<CFontComboBox> baseClass;
protected:
	CImageList m_img;	
	int m_cyItem;

public:
	CFontComboBox()
	{
		m_cyItem = -1;
	}
	void Init()
	{
		HFONT hFont = ((HFONT)GetStockObject( DEFAULT_GUI_FONT ));
		SetFont(hFont);

		m_img.CreateFromImage(IDB_TRUETYPE_FONTTYPE, GLYPH_WIDTH, 1, RGB(255,255,255), IMAGE_BITMAP);
		CClientDC dc(m_hWnd);		
		EnumFonts(dc, 0,(FONTENUMPROC) EnumFontProc,(LPARAM)this); //Enumerate font
	}

	static BOOL CALLBACK EnumFontProc (LPLOGFONT lplf, LPTEXTMETRIC /*lptm*/, DWORD dwType, LPARAM lpData)	
	{	
		CFontComboBox * pThis = reinterpret_cast<CFontComboBox *>(lpData);		
		int index = pThis->AddString(lplf->lfFaceName);
		pThis->SetItemData(index, dwType); 
		return TRUE;
	}

	void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		ATLASSERT(lpDIS->CtlType == ODT_COMBOBOX); 
	
		ATLASSERT(lpDIS->CtlType == ODT_COMBOBOX); 
	
		CDCHandle dc = lpDIS->hDC;		
		RECT rc = lpDIS->rcItem;
	

		if (lpDIS->itemState & ODS_FOCUS)
			dc.DrawFocusRect(&rc);
			
		int nIndexDC = dc.SaveDC();

		CBrush br;
		
		if (lpDIS->itemState & ODS_SELECTED)
		{
			br.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{
			br.CreateSolidBrush(dc.GetBkColor());
		}

		dc.SetBkMode(TRANSPARENT);
		dc.FillRect(&rc, br);

		int nLen = GetLBTextLen(lpDIS->itemID);
		TCHAR* psFont = (TCHAR *)_alloca(sizeof TCHAR * (nLen + 1));
		GetLBText(lpDIS->itemID, psFont);
		
		DWORD dwData = GetItemData(lpDIS->itemID);

		if (dwData & TRUETYPE_FONTTYPE)
			m_img.Draw(dc, 0, rc.left+5, rc.top+4,ILD_TRANSPARENT);
	
		rc.left += GLYPH_WIDTH + 2;

		CFont cf;

		cf.CreateFont(20,0,0,0,FW_NORMAL,FALSE, FALSE, FALSE,DEFAULT_CHARSET ,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,psFont);
		HFONT hf = dc.SelectFont(cf);
		dc.TextOut(rc.left+10, rc.top, psFont);
		dc.SelectFont(hf);

		dc.RestoreDC(nIndexDC);
	}

	void MeasureItem(LPMEASUREITEMSTRUCT pmis)
	{
		ATLASSERT(pmis->CtlType == ODT_COMBOBOX);

		if (m_cyItem == -1)
		{
			// calculate height
			CClientDC dc(m_hWnd);
			HFONT hFont = ((HFONT)GetStockObject( DEFAULT_GUI_FONT ));
			dc.SelectFont( hFont ); //GetFont()
			TEXTMETRIC tm;
			dc.GetTextMetrics(&tm); 

			m_cyItem = tm.tmHeight + tm.tmInternalLeading;
		}else
			m_cyItem = 20;

		pmis->itemHeight = m_cyItem;
	}

	BEGIN_MSG_MAP(thisClass)
		CHAIN_MSG_MAP_ALT(baseClass, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
};
//  ===========================================================================
__interface IOwner
{
	void SetChanged(bool bChanged=true);
	bool GetChanged();
};

//  ===========================================================================
const TCHAR g_DefaultConfig[] = _T("Default");

class CPrefServerDlg : public CDialogImpl<CPrefServerDlg>, 
	public CWinDataExchange<CPrefServerDlg>
{
	typedef CPrefServerDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	IOwner * m_owner;
	IConfigAdapt m_config;

	CString m_clusterIP;

	CComPtr<CCustomAutoComplete> m_TopologyServerAC;
	CComPtr<CCustomAutoComplete> m_WorkunitServerAC;
	CComPtr<CCustomAutoComplete> m_AttributeServerAC;
	CComPtr<CCustomAutoComplete> m_AccountServerAC;
	CComPtr<CCustomAutoComplete> m_SMCServerAC;
	CComPtr<CCustomAutoComplete> m_SprayServerAC;
	CComPtr<CCustomAutoComplete> m_DfuServerAC;

	CString m_ServerIP;
	CString m_TopologyServer;
	CString m_WorkunitServer;
	CString m_AttributeServer;
	CString m_AccountServer;
	CString m_SMCServer;
	CString m_SprayServer;
	CString m_DfuServer;

	bool m_Advanced;
	bool m_SSL;
	bool m_disableEnvironmentSettings;

public:
	CPrefServerDlg(IOwner * owner, IConfig * config, bool disableEnvironmentSettings) : m_owner(owner), m_config(config)
	{
		m_Advanced = false;
		m_SSL = false;
		m_disableEnvironmentSettings = disableEnvironmentSettings;
	}

	enum { IDD = IDD_PREF_SERVER };

	void LoadFromConfig(IConfig * config)
	{
		m_config = config;
		PopulateControls();
		DoChanged(false);
	}

	void PopulateControls()
	{
		m_ServerIP = m_config->Get(GLOBAL_SERVER_IP);
		m_Advanced = m_config->Get(GLOBAL_SERVER_ADVANCED);
		m_TopologyServer = m_config->Get(GLOBAL_SERVER_TOPOLOGY);

		//to preserve "old" configs, go to advanced if they had a topo server defined
		if (!m_Advanced)
		{
			CString defaultServer(GLOBAL_SERVER_TOPOLOGY.second);
			m_Advanced = (m_TopologyServer != defaultServer && m_ServerIP.IsEmpty());
			if (m_Advanced)
			{
				CUrl url;
				if (url.CrackUrl(m_TopologyServer))
					m_ServerIP = url.GetHostName();
			}
		}

		m_WorkunitServer = m_config->Get(GLOBAL_SERVER_WORKUNIT);
		m_AttributeServer = m_config->Get(GLOBAL_SERVER_ATTRIBUTE);
		m_AccountServer = m_config->Get(GLOBAL_SERVER_ACCOUNT);
		m_SMCServer = m_config->Get(GLOBAL_SERVER_SMC);
		m_SprayServer = m_config->Get(GLOBAL_SERVER_FILESPRAY);
		m_DfuServer = m_config->Get(GLOBAL_SERVER_DFU);
		m_SSL = m_config->Get(GLOBAL_SERVER_SSL);

		EnableServerSettings();

		DoDataExchange();

		DoChanged(false);
	}

	void UpdateConfig()
	{
		DoDataExchange(true);
		m_config->Set(GLOBAL_SERVER_TOPOLOGY, m_TopologyServer);
		m_config->Set(GLOBAL_SERVER_WORKUNIT, m_WorkunitServer);
		m_config->Set(GLOBAL_SERVER_ATTRIBUTE, m_AttributeServer);
		m_config->Set(GLOBAL_SERVER_ACCOUNT, m_AccountServer);
		m_config->Set(GLOBAL_SERVER_SMC, m_SMCServer);
		m_config->Set(GLOBAL_SERVER_FILESPRAY, m_SprayServer);
		m_config->Set(GLOBAL_SERVER_DFU, m_DfuServer);
		m_config->Set(GLOBAL_SERVER_SSL, m_SSL);
		m_config->Set(GLOBAL_SERVER_ADVANCED, m_Advanced);
		m_config->Set(GLOBAL_SERVER_IP, m_ServerIP);
	}

	void DoApply(bool bMakeGlobal)
	{
		DoDataExchange(true);

		m_TopologyServerAC->AddItem(m_TopologyServer);
		m_WorkunitServerAC->AddItem(m_WorkunitServer);
		m_AttributeServerAC->AddItem(m_AttributeServer);
		m_AccountServerAC->AddItem(m_AccountServer);
		m_SMCServerAC->AddItem(m_SMCServer);
		m_SprayServerAC->AddItem(m_SprayServer);
		m_DfuServerAC->AddItem(m_DfuServer);

		UpdateConfig();

		DoChanged(false);
	}

	void DoChanged(bool bChanged=true)
	{
		m_owner->SetChanged(bChanged);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER(IDC_EDIT_IPADDRESS, EN_CHANGE, OnIpFieldChanged)
		COMMAND_HANDLER(IDC_CHECK_SSL, BN_CLICKED, OnClickedSSL)
		COMMAND_HANDLER(IDC_CHECK_ADVANCED, BN_CLICKED, OnClickedAdvanced)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnChangedEdit)
		NOTIFY_CODE_HANDLER(UDN_DELTAPOS, OnSpinChange)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_TOPOLOGYSERVER, m_TopologyServer)
		DDX_TEXT(IDC_EDIT_WORKUNITSERVER, m_WorkunitServer)
		DDX_TEXT(IDC_EDIT_ATTRIBUTESERVER, m_AttributeServer)
		DDX_TEXT(IDC_EDIT_ACCOUNTSERVER, m_AccountServer)
		DDX_TEXT(IDC_EDIT_SMCSERVER, m_SMCServer)
		DDX_TEXT(IDC_EDIT_SPRAYSERVER, m_SprayServer)
		DDX_TEXT(IDC_EDIT_DFUSERVER, m_DfuServer)
		DDX_CHECK(IDC_CHECK_SSL, m_SSL)
		DDX_CHECK(IDC_CHECK_ADVANCED, m_Advanced)
		DDX_TEXT(IDC_EDIT_IPADDRESS, m_ServerIP)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor wait;

		boost::filesystem::path iniPath;
		IConfigAdapt ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));

		CString regPath(::GetRegPathQB());
		regPath += _T("\\Preferences\\");
		m_TopologyServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("TopologyServer"));
		m_WorkunitServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("WorkunitServer"));
		m_AttributeServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("AttributeServer"));
		m_AccountServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("AccountServer"));
		m_SMCServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("SMCServer"));
		m_SprayServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("SprayServer"));
		m_DfuServerAC = new CCustomAutoComplete(HKEY_CURRENT_USER, regPath+_T("DfuServer"));

		m_TopologyServerAC->Bind(GetDlgItem(IDC_EDIT_TOPOLOGYSERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		m_WorkunitServerAC->Bind(GetDlgItem(IDC_EDIT_WORKUNITSERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		m_AttributeServerAC->Bind(GetDlgItem(IDC_EDIT_ATTRIBUTESERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		m_AccountServerAC->Bind(GetDlgItem(IDC_EDIT_ACCOUNTSERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		m_SMCServerAC->Bind(GetDlgItem(IDC_EDIT_SMCSERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		m_SprayServerAC->Bind(GetDlgItem(IDC_EDIT_SPRAYSERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);
		m_DfuServerAC->Bind(GetDlgItem(IDC_EDIT_DFUSERVER), ACO_AUTOSUGGEST | ACO_AUTOAPPEND | ACO_UPDOWNKEYDROPSLIST);

		LoadFromConfig(m_config); //this calls PopulateControls

		DoChanged(false);

		return TRUE;
	}
	void OnDestroy()
	{
		SetMsgHandled(false);
		m_TopologyServerAC->Unbind();
		m_WorkunitServerAC->Unbind();
		m_AttributeServerAC->Unbind();
		m_AccountServerAC->Unbind();
		m_SMCServerAC->Unbind();
		m_SprayServerAC->Unbind();
		m_DfuServerAC->Unbind();
	}
	LRESULT OnChangedEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnSpinChange(int idCtrl, LPNMHDR pnmHdr, BOOL &bHandled)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnCheckClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		EnableServerSettings();
		return 0;
	}
	void SetLimit(int nId, int lower, int upper)
	{
		CUpDownCtrl ctrl = GetDlgItem(nId);
		ctrl.SetRange(lower, upper);
	}
	LRESULT OnClickedAdvanced(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(true);
		//if switching to non-advanced, update the server fields, with whatever is
		//in the IP field
		if ( !m_Advanced )
		{
			UpdateServerFields();
		}
		DoChanged();
		EnableServerSettings();
		if ( m_Advanced )
			GetDlgItem(IDC_EDIT_TOPOLOGYSERVER).SetFocus();
		return 0;
	}
	LRESULT OnClickedSSL(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(true);
		DoChanged();
		if ( !m_Advanced )
		{
			UpdateServerFields();
		}
		return 0;
	}
	void SerializeIPAddress(CString &addr, DWORD nAddr)
	{
		//0 24 through 31 
		//1 16 through 23 
		//2 8 through 15 
		//3 0 through 7 
		BYTE *bytes = (BYTE *)&nAddr;
		addr.Format(_T("%u.%u.%u.%u"), bytes[3], bytes[2], bytes[1], bytes[0] );
	}
	LRESULT OnIpFieldChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString serverIP = m_ServerIP;
		DoDataExchange(true);
		if (boost::algorithm::contains(static_cast<const TCHAR *>(m_ServerIP), _T(":")) || boost::algorithm::contains(static_cast<const TCHAR *>(m_ServerIP), _T("/")))
		{
			::MessageBox(NULL, _T("Server should not contain ':' or '/'."), _T("ECL IDE"), MB_ICONEXCLAMATION);
			m_ServerIP = serverIP;
			DoDataExchange(false);
		}
		else
		{
			DoChanged();
			if ( !m_Advanced )
			{
				UpdateServerFields();
			}
		}
		return 0;
	}
	void UpdateServerFields()
	{
		if (m_ServerIP.IsEmpty())
		{
			SetDlgItemText(IDC_EDIT_TOPOLOGYSERVER, _T(""));
			SetDlgItemText(IDC_EDIT_WORKUNITSERVER, _T(""));
			SetDlgItemText(IDC_EDIT_ATTRIBUTESERVER, _T(""));
			SetDlgItemText(IDC_EDIT_ACCOUNTSERVER, _T(""));
			SetDlgItemText(IDC_EDIT_SMCSERVER, _T(""));
			SetDlgItemText(IDC_EDIT_SPRAYSERVER, _T(""));
			SetDlgItemText(IDC_EDIT_DFUSERVER, _T(""));
		}
		else
		{
			CString url = m_SSL ? _T("https://") : _T("http://");
			url += m_ServerIP;
			url += (m_SSL ? _T(":1") : _T(":"));

			SetDlgItemText(IDC_EDIT_TOPOLOGYSERVER,url+_T("8010/WsTopology"));
			SetDlgItemText(IDC_EDIT_WORKUNITSERVER,url+_T("8010/WsWorkunits"));
			SetDlgItemText(IDC_EDIT_ATTRIBUTESERVER,url+_T("8145/WsAttributes"));
			SetDlgItemText(IDC_EDIT_ACCOUNTSERVER,url+_T("8010/Ws_Account"));
			SetDlgItemText(IDC_EDIT_SMCSERVER,url+_T("8010/WsSMC"));
			SetDlgItemText(IDC_EDIT_SPRAYSERVER,url+_T("8010/FileSpray"));
			SetDlgItemText(IDC_EDIT_DFUSERVER,url+_T("8010/WsDfu"));
		}
	}
	void EnableServerSettings()
	{
		if (m_disableEnvironmentSettings)
		{
			GetDlgItem(IDC_STATIC_ONLYVIALOGIN).ShowWindow(SW_SHOW);
			GetDlgItem(IDC_EDIT_IPADDRESS).EnableWindow(false);
			GetDlgItem(IDC_CHECK_SSL).EnableWindow(false);
			GetDlgItem(IDC_CHECK_ADVANCED).EnableWindow(false);
			GetDlgItem(IDC_EDIT_TOPOLOGYSERVER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_WORKUNITSERVER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_ATTRIBUTESERVER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_ACCOUNTSERVER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_SMCSERVER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_SPRAYSERVER).EnableWindow(false);
			GetDlgItem(IDC_EDIT_DFUSERVER).EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_EDIT_IPADDRESS).EnableWindow(!m_Advanced);
			GetDlgItem(IDC_CHECK_SSL).EnableWindow(!m_Advanced);
			GetDlgItem(IDC_EDIT_TOPOLOGYSERVER).EnableWindow(m_Advanced);
			GetDlgItem(IDC_EDIT_WORKUNITSERVER).EnableWindow(m_Advanced);
			GetDlgItem(IDC_EDIT_ATTRIBUTESERVER).EnableWindow(m_Advanced);
			GetDlgItem(IDC_EDIT_ACCOUNTSERVER).EnableWindow(m_Advanced);
			GetDlgItem(IDC_EDIT_SMCSERVER).EnableWindow(m_Advanced);
			GetDlgItem(IDC_EDIT_SPRAYSERVER).EnableWindow(m_Advanced);
			GetDlgItem(IDC_EDIT_DFUSERVER).EnableWindow(m_Advanced);
		}
	}
	DWORD ParseAddress(const CString &addr)
	{
		DWORD nAddr = 0;
		CString token;
		int curPos = 0;
		int count = 0;
		do 
		{
			token = addr.Tokenize(_T("."),curPos);
			if (!token.IsEmpty())
			{
				nAddr = (nAddr << 8);
				unsigned x = _ttoi(token);
				nAddr |= x;
				count++;
			}
		}
		while ( !token.IsEmpty() && count < 4 );

		return nAddr;
	}
};
//  ===========================================================================
class CPrefEditorDlg : public CDialogImpl<CPrefEditorDlg>, 
	public CWinDataExchange<CPrefEditorDlg>
{
	typedef CPrefEditorDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	IOwner * m_owner;
	int  m_TabWidth;
	int  m_TabUseSpaces;

	IConfigAdapt m_config;
	IConfigAdapt m_ini;
	CString m_ConfigLabel;
	int m_MaintainIndent;
	int m_OpenMDIChildMax;
	int m_AutoSaveFreq;
	int m_LineNo;
	int m_Tree;
	int m_Tooltip;
	int m_SyncRepository;
	int m_autoCompleteOnDot;
	int m_doubleClickSelQual;

public:
	CPrefEditorDlg(IOwner * owner, IConfig * config) : m_config(config), m_owner(owner)
	{
		m_ConfigLabel = m_config->GetLabel();
		if (m_ConfigLabel.IsEmpty())
		{
			m_ConfigLabel = g_DefaultConfig;
		}
	}

	enum { IDD = IDD_PREF_EDITOR };

	void LoadFromConfig(IConfig * config)
	{
		m_config = config;
		m_TabWidth = m_config->Get(GLOBAL_TAB_WIDTH);
		m_TabUseSpaces = m_config->Get(GLOBAL_TAB_USESPACES);
		m_MaintainIndent = m_config->Get(GLOBAL_MAINTAININDENT);
		m_OpenMDIChildMax = m_config->Get(GLOBAL_OPENMDICHILDMAX);
		m_AutoSaveFreq = m_config->Get(GLOBAL_AUTOSAVEFREQ);
		m_LineNo = m_config->Get(GLOBAL_LINENO);
		m_Tree = m_config->Get(GLOBAL_TREE);
		m_Tooltip = m_config->Get(GLOBAL_TOOLTIP);
		m_SyncRepository = m_config->Get(GLOBAL_SYNCREPOSITORY);
		m_autoCompleteOnDot = m_config->Get(GLOBAL_AUTOCOMPLETEONDOT);
		m_doubleClickSelQual = m_config->Get(GLOBAL_DOUBLECLICKSELQUAL);
		DoDataExchange();
	}

	void SaveToConfig()
	{
		DoDataExchange(true);
		m_config->Set(GLOBAL_TAB_WIDTH, m_TabWidth);
		m_config->Set(GLOBAL_TAB_USESPACES, m_TabUseSpaces);

		m_config->Set(GLOBAL_MAINTAININDENT, m_MaintainIndent);
		m_config->Set(GLOBAL_OPENMDICHILDMAX, m_OpenMDIChildMax);
		m_config->Set(GLOBAL_AUTOSAVEFREQ, m_AutoSaveFreq);

		m_config->Set(GLOBAL_LINENO, m_LineNo);
		m_config->Set(GLOBAL_TREE, m_Tree);
		m_config->Set(GLOBAL_TOOLTIP, m_Tooltip);
		m_config->Set(GLOBAL_SYNCREPOSITORY, m_SyncRepository);
		m_config->Set(GLOBAL_AUTOCOMPLETEONDOT, m_autoCompleteOnDot);
		m_config->Set(GLOBAL_DOUBLECLICKSELQUAL, m_doubleClickSelQual);
	}

	void DoApply(bool bMakeGlobal)
	{
		DoDataExchange(true);

		SaveToConfig();
		DoChanged(false);
	}

	void DoChanged(bool bChanged=true)
	{
		m_owner->SetChanged(bChanged);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER(IDC_CHECK_TABUSESPACES, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_LINENO, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_TREE, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_MAINTAININDENT, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_OPENMDICHILDMAX, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_TOOLTIP, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_SYNCREPOSITORY, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_AUTOCOMPLETEONDOT, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_DOUBLECLICKSEL, BN_CLICKED, OnCheckClicked)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnChangedEdit)
		NOTIFY_CODE_HANDLER(UDN_DELTAPOS, OnSpinChange)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_INT(IDC_EDIT_TABWIDTH, m_TabWidth)
		DDX_CHECK(IDC_CHECK_TABUSESPACES, m_TabUseSpaces)
		DDX_CHECK(IDC_CHECK_MAINTAININDENT, m_MaintainIndent)
		DDX_CHECK(IDC_CHECK_OPENMDICHILDMAX, m_OpenMDIChildMax)
		DDX_INT(IDC_EDIT_AUTOSAVEFREQ, m_AutoSaveFreq)
		DDX_CHECK(IDC_CHECK_LINENO, m_LineNo)
		DDX_CHECK(IDC_CHECK_TREE, m_Tree)
		DDX_CHECK(IDC_CHECK_TOOLTIP, m_Tooltip)
		DDX_CHECK(IDC_CHECK_SYNCREPOSITORY, m_SyncRepository)
		DDX_CHECK(IDC_CHECK_AUTOCOMPLETEONDOT, m_autoCompleteOnDot)
		DDX_CHECK(IDC_CHECK_DOUBLECLICKSEL, m_doubleClickSelQual)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor wait;

		boost::filesystem::path iniPath;
		m_ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));

		CString regPath(::GetRegPathQB());
		regPath += _T("\\Preferences\\");

		SetLimit(IDC_SPIN_AUTOSAVEFREQ, 0, 60);

		LoadFromConfig(m_config);
		DoChanged(false);

		return TRUE;
	}

	void OnDestroy()
	{
		SetMsgHandled(false);
	}

	LRESULT OnChangedEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnSpinChange(int idCtrl, LPNMHDR pnmHdr, BOOL &bHandled)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnCheckClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}
	void SetLimit(int nId, int lower, int upper)
	{
		CUpDownCtrl ctrl = GetDlgItem(nId);
		ctrl.SetRange(lower, upper);
	}
};
//  ===========================================================================
class CPrefCompilerDlg : public CDialogImpl<CPrefCompilerDlg>, 
	public CWinDataExchange<CPrefCompilerDlg>
{
	typedef CPrefCompilerDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	IOwner * m_owner;
	IConfigAdapt m_config;
	IConfigAdapt m_ini;
	CString m_ConfigLabel;

	CString m_Location;
	CString m_Arguments;
	CString m_WUArguments;
	CString m_EclWorkingFolder;

	CHListBox m_listFolders;

	CButton m_moveUp;
	CButton m_moveDown;

public:
	CPrefCompilerDlg(IOwner * owner, IConfig * config) : m_config(config), m_owner(owner)
	{
		m_ConfigLabel = m_config->GetLabel();
		if (m_ConfigLabel.IsEmpty())
		{
			m_ConfigLabel = g_DefaultConfig;
		}
	}

	enum { IDD = IDD_PREF_COMPILER };

	void LoadDefaults()
	{
		m_Arguments = _T("");
		m_WUArguments = _T("");
		const TCHAR * hpccbin = _tgetenv(_T("HPCCBIN"));
		if (hpccbin)
		{
			boost::filesystem::wpath eclccPath = hpccbin;
			eclccPath /= _T("eclcc.exe");
			if (boost::filesystem::exists(eclccPath))
				m_Location = eclccPath.native_file_string().c_str();
		}

		const TCHAR * hpccEcl = _tgetenv(_T("HPCCECL"));
		if (hpccEcl)
		{
			boost::filesystem::wpath wuFolder = hpccEcl;
			wuFolder /= _T("wu");
			boost::filesystem::create_directories(wuFolder);
			m_EclWorkingFolder = wuFolder.native_file_string().c_str();

			m_listFolders.ResetContent();
			boost::filesystem::wpath samplesPath = hpccEcl;
			samplesPath = samplesPath.parent_path();
			samplesPath /= _T("Samples");
			if (boost::filesystem::exists(samplesPath))
				m_listFolders.AddString(samplesPath.native_file_string().c_str());

			boost::filesystem::wpath repositoryPath = hpccEcl;
			repositoryPath = repositoryPath .parent_path();
			repositoryPath /= _T("My Files");
			boost::filesystem::create_directories(repositoryPath);
			m_listFolders.AddString(repositoryPath.native_file_string().c_str());
		}
		DoDataExchange();
		SaveToConfig();
	}

	void LoadFromConfig(IConfig * config)
	{
		m_config = config;
		m_Location = m_config->Get(GLOBAL_COMPILER_LOCATION);
		m_Arguments = m_config->Get(GLOBAL_COMPILER_ARGUMENTS);
		m_WUArguments = m_config->Get(GLOBAL_COMPILER_WUARGUMENTS);
		m_EclWorkingFolder = m_config->Get(GLOBAL_COMPILER_ECLWORKINGFOLDER);
		m_listFolders.ResetContent();
		for (int i = 0; i < 10; ++i)
		{
			CString text;
			switch (i)
			{
			case 0:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER00);
				break;
			case 1:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER01);
				break;
			case 2:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER02);
				break;
			case 3:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER03);
				break;
			case 4:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER04);
				break;
			case 5:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER05);
				break;
			case 6:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER06);
				break;
			case 7:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER07);
				break;
			case 8:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER08);
				break;
			case 9:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER09);
				break;
			}
			if (text.GetLength() > 0)
				m_listFolders.InsertString(i, text);
		}
		DoDataExchange();
	}

	void SaveToConfig()
	{
		DoDataExchange(true);
		m_config->Set(GLOBAL_COMPILER_LOCATION, m_Location);
		m_config->Set(GLOBAL_COMPILER_ARGUMENTS, m_Arguments);
		m_config->Set(GLOBAL_COMPILER_WUARGUMENTS, m_WUArguments);
		m_config->Set(GLOBAL_COMPILER_ECLWORKINGFOLDER, m_EclWorkingFolder);
		for (int i = 0; i < 10; ++i)
		{
			CString text;
			m_listFolders.GetText(i, text);
			switch (i)
			{
			case 0:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER00, text);
				break;
			case 1:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER01, text);
				break;
			case 2:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER02, text);
				break;
			case 3:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER03, text);
				break;
			case 4:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER04, text);
				break;
			case 5:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER05, text);
				break;
			case 6:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER06, text);
				break;
			case 7:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER07, text);
				break;
			case 8:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER08, text);
				break;
			case 9:
				m_config->Set(GLOBAL_COMPILER_ECLFOLDER09, text);
				break;
			}
		}
	}

	void DoApply(bool bMakeGlobal)
	{
		DoDataExchange(true);

		SaveToConfig();
		DoChanged(false);
	}

	void DoChanged(bool bChanged=true)
	{
		m_owner->SetChanged(bChanged);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_CODE_HANDLER(EN_CHANGE, OnChangedEdit)

		COMMAND_HANDLER(IDC_BUTTON_ECLCOMPILER, BN_CLICKED, OnEclCompilerClicked)
		COMMAND_HANDLER(IDC_BUTTON_ECLWORKINGFOLDER, BN_CLICKED, OnEclWorkingFolderClicked)
		COMMAND_HANDLER(IDC_BUTTON_MOVEUP, BN_CLICKED, OnEclFolderMoveUpClicked)
		COMMAND_HANDLER(IDC_BUTTON_MOVEDOWN, BN_CLICKED, OnEclFolderMoveDownClicked)
		COMMAND_HANDLER(IDC_BUTTON_DEFAULTS, BN_CLICKED, OnEclFolderDefaults)
		COMMAND_HANDLER(IDC_BUTTON_ECLFOLDERADD, BN_CLICKED, OnEclFolderAddClicked)
		COMMAND_HANDLER(IDC_BUTTON_ECLFOLDERDELETE, BN_CLICKED, OnEclFolderDeleteClicked)

		NOTIFY_HANDLER(IDC_LIST_ECLFOLDERS, LBN_SELCHANGE, OnSelChange)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_LOCATION, m_Location)
		DDX_TEXT(IDC_EDIT_ARGUMENTS, m_Arguments)
		DDX_TEXT(IDC_EDIT_WUARGUMENTS, m_WUArguments)
		DDX_TEXT(IDC_EDIT_ECLWORKINGFOLDER, m_EclWorkingFolder)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor wait;

		boost::filesystem::path iniPath;
		m_ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));

		m_listFolders = GetDlgItem(IDC_LIST_ECLFOLDERS);
		ATLASSERT(m_listFolders);
		m_moveUp = GetDlgItem(IDC_BUTTON_MOVEUP);
		m_moveDown = GetDlgItem(IDC_BUTTON_MOVEDOWN);

		LoadFromConfig(m_config);
		DoChanged(false);

		return TRUE;
	}

	void OnDestroy()
	{
		SetMsgHandled(false);
	}

	LRESULT OnChangedEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}

	LRESULT OnEclCompilerClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static const TCHAR szEclFilter[] = _T("ECL Compiler (eclcc.exe)\0eclcc.exe\0All Files (*.*)\0*.*\0\0");
		CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, szEclFilter, m_hWnd);	
		if (IDOK == dlg.DoModal())
		{
			m_Location = dlg.m_szFileName;
			::SetWindowText(::GetDlgItem(m_hWnd, IDC_EDIT_LOCATION), m_Location);
			DoChanged();
		}
		return 0;
	}

	LRESULT OnEclWorkingFolderClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog dlg(NULL, _T("Select working folder for the compiler:"), BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_NEWDIALOGSTYLE);
		dlg.SetInitialFolder(m_EclWorkingFolder);
		if (IDOK == dlg.DoModal())
		{
			m_EclWorkingFolder = dlg.GetFolderPath();
			::SetWindowText(::GetDlgItem(m_hWnd, IDC_EDIT_ECLWORKINGFOLDER), m_EclWorkingFolder);
			DoChanged();
		}
		return 0;
	}

	LRESULT OnEclFolderDefaults(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		LoadDefaults();
		DoChanged();
		return 0;
	}

	LRESULT OnEclFolderAddClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog dlg(NULL, _T("Select folder that contains ECL:"), BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_NEWDIALOGSTYLE);
		dlg.SetInitialFolder(m_EclWorkingFolder);
		if (IDOK == dlg.DoModal())
		{
			std::_tstring folder = dlg.GetFolderPath();
			m_listFolders.AddString(folder.c_str());
			DoChanged();
		}
		return 0;
	}

	LRESULT OnEclFolderDeleteClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_listFolders.DeleteString(m_listFolders.GetCurSel());
		DoChanged();
		return 0;
	}

	LRESULT OnEclFolderMoveUpClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int idx = m_listFolders.GetCurSel();
		if(idx > 0 && idx <= m_listFolders.GetCount() - 1) 
		{
			CString prevLine;
			m_listFolders.GetText(idx - 1, prevLine);
			m_listFolders.DeleteString(idx - 1);
			m_listFolders.InsertString(idx, prevLine);
			DoChanged();
		}
		else
			::MessageBeep(MB_ICONEXCLAMATION);
		return 0;
	}

	LRESULT OnEclFolderMoveDownClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int idx = m_listFolders.GetCurSel();
		if(idx >= 0 && idx < m_listFolders.GetCount() - 1) 
		{
			CString nextLine;
			m_listFolders.GetText(idx + 1, nextLine);
			m_listFolders.DeleteString(idx + 1);
			m_listFolders.InsertString(idx, nextLine);
			DoChanged();
		}
		else
			::MessageBeep(MB_ICONEXCLAMATION);
		return 0;
	}

   LRESULT OnSelChange(int wParam, LPNMHDR lParam, BOOL &/*bHandled*/)
   {
	   return 0;
   }
};
//  ===========================================================================
class CPrefColorDlg : public CDialogImpl<CPrefColorDlg>, 
	public CWinDataExchange<CPrefColorDlg>,
	public CSourceSlotImpl,
	public CEclCommandMixin<CPrefColorDlg>

{
	typedef CPrefColorDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	IOwner * m_owner;
	IConfigAdapt m_config;
	IConfigAdapt m_ini;
	CString m_ConfigLabel;
	CFontComboBox m_comboFont;
	CComboBox m_comboFontSize;
	std::_tstring m_Font;
	std::_tstring m_FontSize;	
	CColorButton m_buttForeground;
	CColorButton m_buttBackground;
	CComPtr<ILangRef> m_langRef;
	CComboBox m_comboElement;
	CSourceCtrl m_ecl;
	CButton m_checkBold;

public:
	CPrefColorDlg(IOwner * owner, IConfig * config) : m_config(config), m_owner(owner), m_ecl(this)
	{
		m_ConfigLabel = m_config->GetLabel();
		if (m_ConfigLabel.IsEmpty())
		{
			m_ConfigLabel = g_DefaultConfig;
		}
	}

	enum { IDD = IDD_PREF_COLOR };

	void LoadFromConfig(IConfig * config)
	{
		m_config = config;
		m_Font = CString(m_config->Get(GLOBAL_FONT));
		m_FontSize = CString(m_config->Get(GLOBAL_FONTSIZE));
		SetComboText(m_comboFont, m_Font);
		SetComboText(m_comboFontSize, m_FontSize);
	}

	void SetComboText(CComboBox & combo, const std::_tstring & text)
	{
		int nItem = combo.FindStringExact(-1,text.c_str());
		if ( nItem >= 0 )
		{
			combo.SetCurSel(nItem);
		}
		else
		{
			combo.SetWindowText(text.c_str());
		}
	}

	void SaveToConfig()
	{
		//DoDataExchange(true);
		//CString font, fontSize;
		//m_comboFont.GetLBText(m_comboFont.GetCurSel(), font);
		//m_comboFontSize.GetLBText(m_comboFontSize.GetCurSel(), fontSize);
		//m_Font = font;
		//m_FontSize = fontSize;
		//m_config->Set(GLOBAL_FONT, m_Font);
		//m_config->Set(GLOBAL_FONTSIZE, m_FontSize);
		m_langRef->Save();
	}

	void DoApply(bool bMakeGlobal)
	{
		DoDataExchange(true);

		SaveToConfig();
		DoChanged(false);
	}

	void DoRestore()
	{
		m_langRef->Restore();
	}

	void DoChanged(bool bChanged=true)
	{
		m_owner->SetChanged(bChanged);
	}

	void GetTitle(CString & title)
	{
	}

	IAttribute * GetAttribute() 
	{
		return NULL;
	}

	void DoLoadElement()
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return;
		int catID = m_langRef->GetColorCatID(row);
		
		m_buttBackground.SetColor(m_langRef->GetColorBack(catID));
		m_buttForeground.SetColor(m_langRef->GetColorFore(catID));
		std::_tstring font = CString(m_config->Get(GLOBAL_FONT));
		font = m_comboFont.SetWindowText(m_langRef->GetFontName(catID));
		m_comboFontSize.SetWindowText(boost::lexical_cast<std::_tstring>(m_langRef->GetFontSize(catID)).c_str());
		m_checkBold.SetCheck(m_langRef->GetFontBold(catID));
	}

	void DoSaveElement()
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return;
		int catID = m_langRef->GetColorCatID(row);

		m_langRef->SetColorFore(catID, m_buttBackground.GetColor());
		m_langRef->SetColorBack(catID, m_buttForeground.GetColor());

		CString font;
		m_comboFont.GetWindowText(font);
		m_langRef->SetFontName(catID, static_cast<const TCHAR *>(font));

		CString fontSize;
		m_comboFontSize.GetWindowText(fontSize);
		m_langRef->SetFontSize(catID, boost::lexical_cast<int>(static_cast<const TCHAR *>(fontSize)));
		m_langRef->SetFontBold(catID, m_checkBold.GetCheck() != 0);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER(IDC_CHECK_BOLD, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_COMBO_ELEMENT, CBN_SELCHANGE, OnCbnSelchangeComboElement)
		COMMAND_HANDLER(IDC_COMBO_ELEMENT, CBN_SELENDOK, OnCbnSelendokComboElement)
		COMMAND_HANDLER(IDC_COMBO_FONT, CBN_SELCHANGE, OnCbnSelendokComboFont)
		COMMAND_HANDLER(IDC_COMBO_FONT, CBN_SELENDOK, OnCbnSelendokComboFont)
		COMMAND_HANDLER(IDC_COMBO_FONTSIZE, CBN_SELCHANGE, OnCbnSelendokComboFontSize)
		COMMAND_HANDLER(IDC_COMBO_FONTSIZE, CBN_SELENDOK, OnCbnSelendokComboFontSize)
		NOTIFY_HANDLER(IDC_BUTTON_FOREGROUND, CPN_SELENDOK, OnBnSelendokForeground)
		NOTIFY_HANDLER(IDC_BUTTON_BACKGROUND, CPN_SELENDOK, OnBnSelendokBackground)
		COMMAND_HANDLER(IDC_BUTTON_DEFAULTS, BN_CLICKED, OnBnClickedButtonDefaults)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnChangedEdit)
		NOTIFY_CODE_HANDLER(UDN_DELTAPOS, OnSpinChange)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		//DDX_TEXT(IDC_EDIT_FONT, m_Font)
		//DDX_INT(IDC_EDIT_FONTSIZE, m_FontSize)
	END_DDX_MAP()

	void InitComboFont()
	{
		CWindow wndPlaceholder = GetDlgItem(IDC_COMBO_FONT);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_comboFont.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | CBS_DROPDOWN | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_SORT , 0, IDC_COMBO_FONT);
		m_comboFont.Init();
	}

	void InitComboFontSize()
	{
		m_comboFontSize = GetDlgItem(IDC_COMBO_FONTSIZE);
		m_comboFontSize.AddString(_T("8"));
		m_comboFontSize.AddString(_T("9"));
		m_comboFontSize.AddString(_T("10"));
		m_comboFontSize.AddString(_T("11"));
		m_comboFontSize.AddString(_T("12"));
		m_comboFontSize.AddString(_T("14"));
		m_comboFontSize.AddString(_T("16"));
		m_comboFontSize.AddString(_T("18"));
		m_comboFontSize.AddString(_T("20"));
		m_comboFontSize.AddString(_T("22"));
		m_comboFontSize.AddString(_T("24"));
		m_comboFontSize.AddString(_T("26"));
		m_comboFontSize.AddString(_T("28"));
		m_comboFontSize.AddString(_T("36"));
		m_comboFontSize.AddString(_T("48"));
		m_comboFontSize.AddString(_T("78"));
	}

	void InitComboElement()
	{
		m_comboElement = GetDlgItem(IDC_COMBO_ELEMENT);
		for(int row = 0; row < m_langRef->GetColorRowCount(); ++row)
		{
			int catID = m_langRef->GetColorCatID(row);
			m_comboElement.AddString(m_langRef->GetColorName(catID));
		}
	}

	void InitEditEcl()
	{
		CWindow wndPlaceholder = GetDlgItem(IDC_ECL_PLACEHOLDER);
		CRect rc;
		wndPlaceholder.GetWindowRect ( rc );
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_ecl.Create(*this, rc, _T(""), WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, WS_EX_CLIENTEDGE, IDC_ECL_PLACEHOLDER);
		m_ecl.DoInit();
		m_ecl.SetText(
			_T("//  Click the ECL to select color scheme.")		_T("\r\n")
			_T("/*  Edit the ECL by typing here.	*/")		_T("\r\n")
			_T("import ut;")									_T("\r\n")
			_T("r := ")											_T("\r\n")
			_T("  record")										_T("\r\n")
			_T("   string22 s1 := '123';")						_T("\r\n")
			_T("   integer4 i1 := 123;")						_T("\r\n")
			_T("  end;")										_T("\r\n")
			_T("#option('tmp', true);")							_T("\r\n")
			_T("d := dataset('tmp::qb', r, thor);")				_T("\r\n")
			_T("output(d);")									_T("\r\n")
			_T("Compare:  Added")								_T("\r\n")
			_T("Compare:  Deleted")								_T("\r\n")
			_T("Compare:  Changed")								_T("\r\n")
			_T("Compare:  Moved"));
		m_ecl.SetLineState(11, SCE_ECL_ADDED);
		m_ecl.SetLineState(12, SCE_ECL_DELETED);
		m_ecl.SetLineState(13, SCE_ECL_CHANGED);
		m_ecl.SetLineState(14, SCE_ECL_MOVED);
	}

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor wait;

		boost::filesystem::path iniPath;
		m_ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));
		m_langRef = CreateEclLangRef();

		CString regPath(::GetRegPathQB());
		regPath += _T("\\Preferences\\");

		InitComboFont();
		InitComboFontSize();
		InitComboElement();
		InitEditEcl();

		m_buttForeground.SubclassWindow(GetDlgItem(IDC_BUTTON_FOREGROUND));
		m_buttForeground.SetDefaultColor(GetSysColor(COLOR_WINDOWTEXT));
		m_buttBackground.SubclassWindow(GetDlgItem(IDC_BUTTON_BACKGROUND));
		m_buttBackground.SetDefaultColor(GetSysColor(COLOR_WINDOW));

		m_checkBold = GetDlgItem(IDC_CHECK_BOLD);

		LoadFromConfig(m_config);
		DoLoadElement();
		DoChanged(false);

		return TRUE;
	}

	void OnDestroy()
	{
		SetMsgHandled(false);
	}

	LRESULT OnChangedEdit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (wID != IDC_ECL_PLACEHOLDER)
			DoChanged();
		return 0;
	}

	LRESULT OnSpinChange(int idCtrl, LPNMHDR pnmHdr, BOOL &bHandled)
	{
		DoChanged();
		return 0;
	}

	LRESULT OnCheckClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return 0;
		int catID = m_langRef->GetColorCatID(row);

		m_langRef->SetFontBold(catID, m_checkBold.GetCheck() != 0);
		m_ecl.InitColors(m_langRef);
		return 0;
	}

	void SetLimit(int nId, int lower, int upper)
	{
		CUpDownCtrl ctrl = GetDlgItem(nId);
		ctrl.SetRange(lower, upper);
	}

	LRESULT OnCbnSelchangeComboElement(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoLoadElement();
		return 0;
	}

	LRESULT OnCbnSelendokComboElement(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoLoadElement();
		return 0;
	}

	LRESULT OnCbnSelendokComboFont(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return 0;
		int catID = m_langRef->GetColorCatID(row);

		CString font;
		m_comboFont.GetLBText(m_comboFont.GetCurSel(), font);
		m_langRef->SetFontName(catID, static_cast<const TCHAR *>(font));

		m_ecl.InitColors(m_langRef);
		return 0;
	}

	LRESULT OnCbnSelendokComboFontSize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return 0;
		int catID = m_langRef->GetColorCatID(row);

		CString fontSize;
		m_comboFontSize.GetLBText(m_comboFontSize.GetCurSel(), fontSize);
		try
		{
			m_langRef->SetFontSize(catID, boost::lexical_cast<int>(static_cast<const TCHAR *>(fontSize)));
		}
		catch(boost::bad_lexical_cast &)
		{
			return 0;
		}

		m_ecl.InitColors(m_langRef);
		return 0;
	}

	LRESULT OnBnSelendokForeground(int /*wID*/, LPNMHDR /*nmhdr*/, BOOL& /*bHandled*/)
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return 0;
		int catID = m_langRef->GetColorCatID(row);

		m_langRef->SetColorFore(catID, m_buttForeground.GetColor());

		m_ecl.InitColors(m_langRef);
		return 0;
	}

	LRESULT OnBnSelendokBackground(int /*wID*/, LPNMHDR /*nmhdr*/, BOOL& /*bHandled*/)
	{
		int row = m_comboElement.GetCurSel();
		if (row < 0 || row >= m_langRef->GetColorRowCount())
			return 0;
		int catID = m_langRef->GetColorCatID(row);

		m_langRef->SetColorBack(catID, m_buttBackground.GetColor());

		m_ecl.InitColors(m_langRef);
		return 0;
	}
	
	//  IEclSlot 
	void NewSel() 
	{
		int catID = m_ecl.GetStyleAt(m_ecl.GetCurrentPos());
		if (catID == 0)
			catID = 32;  //The true default...
		int row = m_langRef->GetColorRow(catID);
		m_comboElement.SetCurSel(row);
		DoLoadElement();
	}

	LRESULT OnBnClickedButtonDefaults(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (MessageBox(_T("Restore default color scheme.  Are you sure you want to continue?"), CString(MAKEINTRESOURCE(IDR_MAINFRAME)), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
		{
			m_langRef->RestoreDefaults();
			m_ecl.InitColors(m_langRef);
		}
		return 0;
	}
};
//  ===========================================================================
class CPrefResultDlg : public CDialogImpl<CPrefResultDlg>, 
	public CWinDataExchange<CPrefResultDlg>
{
	typedef CPrefResultDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	IOwner * m_owner;
	IConfigAdapt m_config;
	IConfigAdapt m_ini;
	CString m_ConfigLabel;
	int m_ResultLimit;
	int m_AutoOpenResult;
	std::_tstring m_Font;
	int m_FontSizeResult;
	CFontComboBox m_comboFont;

public:
	CPrefResultDlg(IOwner *owner, IConfig * config) : m_config(config), m_owner(owner)
	{
		m_ConfigLabel = m_config->GetLabel();
		if (m_ConfigLabel.IsEmpty())
		{
			m_ConfigLabel = g_DefaultConfig;
		}
	}

	enum { IDD = IDD_PREF_RESULTS };

	void LoadFromConfig(IConfig * config)
	{
		m_config = config;
		PopulateControls();
		DoChanged(false);
	}

	void PopulateControls()
	{
		m_ResultLimit = m_config->Get(GLOBAL_WORKUNIT_RESULTLIMIT);
		m_AutoOpenResult = m_config->Get(GLOBAL_AUTOOPENRESULT);
		m_Font = CString(m_config->Get(GLOBAL_FONT_RESULT));
		m_FontSizeResult = m_config->Get(GLOBAL_FONTSIZE_RESULT);
		SetComboText(m_comboFont, m_Font);

		DoDataExchange();

		DoChanged(false);
	}

	void SetComboText(CComboBox & combo, const std::_tstring & text)
	{
		int nItem = combo.FindStringExact(-1,text.c_str());
		if ( nItem >= 0 )
		{
			combo.SetCurSel(nItem);
		}
		else
		{
			combo.SetWindowText(text.c_str());
		}
	}

	void UpdateConfig()
	{
		m_config->Set(GLOBAL_WORKUNIT_RESULTLIMIT, m_ResultLimit);
		m_config->Set(GLOBAL_AUTOOPENRESULT, m_AutoOpenResult);
		CString font;
		m_comboFont.GetWindowText(font);
		m_config->Set(GLOBAL_FONT_RESULT, font);
		m_config->Set(GLOBAL_FONTSIZE_RESULT, m_FontSizeResult);
	}

	void DoApply(bool bMakeGlobal)
	{
		DoDataExchange(true);

		UpdateConfig();

		DoChanged(false);
	}

	void DoChanged(bool bChanged=true)
	{
		m_owner->SetChanged(bChanged);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_CODE_HANDLER(EN_CHANGE, OnChangedEdit)
		COMMAND_HANDLER(IDC_COMBO_FONT, CBN_SELCHANGE, OnCbnSelendokComboFont)
		COMMAND_HANDLER(IDC_COMBO_FONT, CBN_SELENDOK, OnCbnSelendokComboFont)
		NOTIFY_CODE_HANDLER(UDN_DELTAPOS, OnSpinChange)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_INT(IDC_EDIT_LIMITRESULT, m_ResultLimit)
		DDX_INT(IDC_EDIT_RESULT_FONTSIZE, m_FontSizeResult)
	END_DDX_MAP()

	void InitComboFont()
	{
		CWindow wndPlaceholder = GetDlgItem(IDC_COMBO_FONT);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_comboFont.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | CBS_DROPDOWN | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_SORT , 0, IDC_COMBO_FONT);
		m_comboFont.Init();
	}

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor wait;

		boost::filesystem::path iniPath;
		m_ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));

		CString regPath(::GetRegPathQB());
		regPath += _T("\\Preferences\\");

		InitComboFont();

		SetLimit(IDC_SPIN_LIMITRESULT, 0, 9999);
		SetLimit(IDC_SPIN_RESULT_FONTSIZE, 2, 32);

		LoadFromConfig(m_config); //this calls PopulateControls

		DoChanged(false);

		return TRUE;
	}
	void OnDestroy()
	{
		SetMsgHandled(false);
	}
	LRESULT OnChangedEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnCbnSelendokComboFont(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		CString font;
		m_Font = m_comboFont.GetLBText(m_comboFont.GetCurSel(), font);
		return 0;
	}

	LRESULT OnSpinChange(int idCtrl, LPNMHDR pnmHdr, BOOL &bHandled)
	{
		DoChanged();
		return 0;
	}
	void SetLimit(int nId, int lower, int upper)
	{
		CUpDownCtrl ctrl = GetDlgItem(nId);
		ctrl.SetRange(lower, upper);
	}
};
//  ===========================================================================
class CPrefOtherDlg : public CDialogImpl<CPrefOtherDlg>, 
	public CWinDataExchange<CPrefOtherDlg>
{
	typedef CPrefOtherDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	IOwner * m_owner;
	IConfigAdapt m_config;
	IConfigAdapt m_ini;
	CString m_ConfigLabel;
	int m_DisableInvoke;
	int m_IgnoreWhitespace;
	int m_IgnoreServerVersion;
	int m_ShowCRLF;
	int m_DisableAutoUpdate;
	int m_WorkUnitPollFreq;
	int m_WorkUnitFetchLimit;

	WTL::CButton m_buttInstall;
	WTL::CButton m_buttUninstall;

public:
	CPrefOtherDlg(IOwner * owner, IConfig * config) : m_config(config), m_owner(owner)
	{
		m_ConfigLabel = m_config->GetLabel();
		if (m_ConfigLabel.IsEmpty())
		{
			m_ConfigLabel = g_DefaultConfig;
		}
	}

	enum { IDD = IDD_PREF_OTHER };

	void LoadFromConfig(IConfig * config)
	{
		m_config = config;
		PopulateControls();
		DoChanged(false);
	}

	void PopulateControls()
	{
		SetDefaultConfig(m_ini, static_cast<const TCHAR *>(m_ConfigLabel));

		m_DisableInvoke = m_config->Get(GLOBAL_DISABLEINVOKE);
		m_IgnoreWhitespace = m_config->Get(GLOBAL_IGNOREWHITESPACE);
		m_IgnoreServerVersion = m_config->Get(GLOBAL_IGNORESERVERVERSION);
		m_ShowCRLF = m_config->Get(GLOBAL_SHOWCRLF);
		m_DisableAutoUpdate = m_ini->Get(GLOBAL_DISABLEAUTOUPDATE);
		m_WorkUnitPollFreq = m_config->Get(GLOBAL_ACTIVEWORKUNIT_REFRESH);
		m_WorkUnitFetchLimit = m_config->Get(GLOBAL_WORKUNIT_FETCHLIMIT);

		DoDataExchange();

		DoChanged(false);
	}

	void UpdateConfig()
	{
		m_config->Set(GLOBAL_DISABLEINVOKE, m_DisableInvoke);
		m_config->Set(GLOBAL_IGNOREWHITESPACE, m_IgnoreWhitespace);
		m_config->Set(GLOBAL_IGNORESERVERVERSION, m_IgnoreServerVersion);
		m_config->Set(GLOBAL_SHOWCRLF, m_ShowCRLF);
		m_ini->Set(GLOBAL_DISABLEAUTOUPDATE, m_DisableAutoUpdate);
		m_config->Set(GLOBAL_ACTIVEWORKUNIT_REFRESH, m_WorkUnitPollFreq);
		m_config->Set(GLOBAL_WORKUNIT_FETCHLIMIT, m_WorkUnitFetchLimit);
	}

	void DoApply(bool bMakeGlobal)
	{
		DoDataExchange(true);

		UpdateConfig();

		DoChanged(false);
	}

	void DoChanged(bool bChanged=true)
	{
		m_owner->SetChanged(bChanged);
	}

	void CalcVersions()
	{
		std::_tstring currentVersion;
		if (GetCurrentVersionString(currentVersion))
		{
			m_buttUninstall.SetWindowText((boost::_tformat(_T("Uninstall Version:  %1%")) % currentVersion).str().c_str());
			m_buttUninstall.EnableWindow();
		}
		else
		{
			m_buttUninstall.SetWindowText(_T("Uninstall Version"));
			m_buttUninstall.EnableWindow(false);
		}

		std::_tstring myVersion;
		if(GetMyVersionString(myVersion))
		{
			m_buttInstall.SetWindowText((boost::_tformat(_T("Install Version:  %1%")) % myVersion).str().c_str());
			m_buttInstall.EnableWindow(true);
		}
		else
		{
			m_buttInstall.SetWindowText(_T("Install Version"));
			m_buttInstall.EnableWindow(false);
		}
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_HANDLER(IDC_CHECK_DISABLEINVOKE, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_IGNOREWHITESPACE, BN_CLICKED, OnCheckClicked)
		COMMAND_HANDLER(IDC_CHECK_IGNORESERVERVERSION, BN_CLICKED, OnCheckClicked)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnChangedEdit)
		COMMAND_HANDLER(IDC_CHECK_DISABLEAUTOUPDATE, BN_CLICKED, OnCheckClicked)
		NOTIFY_CODE_HANDLER(UDN_DELTAPOS, OnSpinChange)
		COMMAND_HANDLER(IDC_BUTTON_INSTALLVERSION, BN_CLICKED, OnBnClickedInstall)
		COMMAND_HANDLER(IDC_BUTTON_UNINSTALLVERSION, BN_CLICKED, OnBnClickedUninstall)
		COMMAND_HANDLER(IDC_BUTTON_GPF, BN_CLICKED, OnBnClickedButtonGpf)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_CHECK(IDC_CHECK_DISABLEINVOKE, m_DisableInvoke)
		DDX_CHECK(IDC_CHECK_IGNOREWHITESPACE, m_IgnoreWhitespace)
		DDX_CHECK(IDC_CHECK_IGNORESERVERVERSION, m_IgnoreServerVersion)
		DDX_CHECK(IDC_CHECK_SHOWCRLF, m_ShowCRLF)
		DDX_CHECK(IDC_CHECK_DISABLEAUTOUPDATE, m_DisableAutoUpdate)
		DDX_INT(IDC_EDIT_WORKUNITPOLL, m_WorkUnitPollFreq)
		DDX_INT(IDC_EDIT_WORKUNITFETCHLIMIT, m_WorkUnitFetchLimit)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor wait;

		boost::filesystem::path iniPath;
		m_ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));

		SetLimit(IDC_SPIN_WORKUNITPOLL, 5, 999);
		SetLimit(IDC_SPIN_WORKUNITFETCHLIMIT, 100, 999999);

		LoadFromConfig(m_config); //this calls PopulateControls

		DoChanged(false);

		m_buttInstall = GetDlgItem(IDC_BUTTON_INSTALLVERSION);
		m_buttUninstall = GetDlgItem(IDC_BUTTON_UNINSTALLVERSION);

		CalcVersions();

		return TRUE;
	}
	LRESULT OnChangedEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnSpinChange(int idCtrl, LPNMHDR pnmHdr, BOOL &bHandled)
	{
		DoChanged();
		return 0;
	}
	LRESULT OnCheckClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoChanged();
		return 0;
	}
	void SetLimit(int nId, int lower, int upper)
	{
		CUpDownCtrl ctrl = GetDlgItem(nId);
		ctrl.SetRange(lower, upper);
	}
	LRESULT OnBnClickedInstall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (RegisterMyVersion())
			CalcVersions();
		return 0;
	}
	LRESULT OnBnClickedUninstall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (UnregisterCurrentVersion())
			CalcVersions();
		return 0;
	}
	LRESULT OnBnClickedButtonGpf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//std::_tstring strOut;
		//runProcess(_T("cmd /c dot"), _T("c:\\temp"), _T("123"), strOut);
		int * ptr = NULL;
		*ptr = 0;
		return 0;
	}
};
//  ===========================================================================
class CMyTabView : public CTabViewImpl<CMyTabView>
{
public:
	DECLARE_WND_CLASS(_T("WTL_MyTabView"))

	void UpdateLayout()
	{
		RECT rect;
		GetClientRect(&rect);

		if (m_tab.IsWindow() && ((m_tab.GetStyle() & WS_VISIBLE) != 0))
			m_tab.SetWindowPos(NULL, 0, 0, rect.right - rect.left, m_cyTabHeight, SWP_NOZORDER);

		if (m_nActivePage != -1)
			::SetWindowPos(GetPageHWND(m_nActivePage), NULL, 0, m_cyTabHeight, rect.right - rect.left, rect.bottom - rect.top - m_cyTabHeight, SWP_NOZORDER);
	}
};

class CPrefDlg : public CDialogImpl<CPrefDlg>, public CWinDataExchange<CPrefDlg>, public IOwner
{
	typedef CPrefDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
protected:
	bool m_changed;

	IConfigAdapt m_config;
	IConfigAdapt m_ini;
	CString m_ConfigLabel;
	CComboBox m_cbConfig;
	int  m_ConfigId;

	CMyTabView m_tabView;
	CPrefServerDlg m_serverPref;
	CPrefEditorDlg m_editorPref;
	CPrefColorDlg m_colorPref;
	CPrefResultDlg m_resultPref;
	CPrefCompilerDlg m_compilerPref;
	CPrefOtherDlg m_otherPref;

	bool m_disableEnvironmentSettings;

public:
	CPrefDlg(IConfig * config, bool disableEnvironmentSettings) : m_serverPref(this, config, disableEnvironmentSettings), m_editorPref(this, config), m_colorPref(this, config), m_resultPref(this, config), m_compilerPref(this, config), m_otherPref(this, config)
	{
		m_disableEnvironmentSettings = disableEnvironmentSettings;
		m_changed = false;

		m_config = config;
		m_ConfigLabel = m_config->GetLabel();
		if (m_ConfigLabel.IsEmpty())
		{
			m_ConfigLabel = g_DefaultConfig;
		}
	}

	enum { IDD = IDD_PREF };

	void SetChanged(bool bChanged=true)
	{
		if ( bChanged )
		{
			if (!m_changed)
			{
				m_changed = true;
			}
		}
		else
		{
			m_changed = false;
		}

		CWindow apply = GetDlgItem(ID_APPLY);
		if (apply.IsWindow())
		{
			apply.EnableWindow(m_changed);
		}
	}

	bool GetChanged()
	{
		return m_changed;
	}

	void InitConfig()
	{
		//update our private config
		if (m_ConfigLabel.CompareNoCase(m_config->GetLabel()) != 0)
		{
			m_config->SetLabel(static_cast<const TCHAR *>(m_ConfigLabel));
		}
		SetDefaultConfig(m_ini, static_cast<const TCHAR *>(m_ConfigLabel));
		m_config = GetIConfig(QUERYBUILDER_CFG);
	}

	void DoLoadFromConfig(bool changed)
	{
		m_serverPref.LoadFromConfig(m_config);
		m_serverPref.DoChanged(changed);
		m_editorPref.LoadFromConfig(m_config);
		m_editorPref.DoChanged(changed);
		m_colorPref.LoadFromConfig(m_config);
		m_colorPref.DoChanged(changed);
		m_resultPref.LoadFromConfig(m_config);
		m_resultPref.DoChanged(changed);
		m_compilerPref.LoadFromConfig(m_config);
		m_compilerPref.DoChanged(changed);
		m_otherPref.LoadFromConfig(m_config);
		m_otherPref.DoChanged(changed);
	}

	void DoApply(bool bMakeGlobal)
	{
		if ( !bMakeGlobal )
		{
			//if this is the same as the global one, then update the global one
			bMakeGlobal = (m_ConfigLabel.CompareNoCase(m_config->GetLabel()) != 0);
		}

		//we are setting this config as the global config
		//TODO Gordon needs to retest this...
		if ( bMakeGlobal )
		{
			m_config->SetLabel(static_cast<const TCHAR *>(m_ConfigLabel));
		}

		m_serverPref.DoApply(bMakeGlobal);
		m_editorPref.DoApply(bMakeGlobal);
		m_colorPref.DoApply(bMakeGlobal);
		m_resultPref.DoApply(bMakeGlobal);
		m_compilerPref.DoApply(bMakeGlobal);
		m_otherPref.DoApply(bMakeGlobal);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_HANDLER_EX(IDC_COMBO_CONFIG, CBN_SELENDOK, OnSelEndOkConfig)
		COMMAND_HANDLER(IDC_LOCATE, BN_CLICKED, OnBnLocate)
		COMMAND_HANDLER(IDC_SAVE_AS, BN_CLICKED, OnBnSaveAs)
		COMMAND_HANDLER(IDC_CONFIG_DELETE, BN_CLICKED, OnBnDelete)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(ID_APPLY, OnApply)
//		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		//update our private config
		if (m_ConfigLabel.CompareNoCase(m_config->GetLabel()) != 0)
		{
			m_config->SetLabel(static_cast<const TCHAR *>(m_ConfigLabel));
		}
		boost::filesystem::path iniPath;
		m_ini = CreateIConfig(QUERYBUILDER_INI, GetIniPath(iniPath));
		SetDefaultConfig(m_ini, static_cast<const TCHAR *>(m_ConfigLabel));

		CWindow wndPlaceholder = GetDlgItem(IDC_SHEET);
		CRect rc;
		wndPlaceholder.GetWindowRect ( rc );
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_tabView.Create(*this, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, NULL, IDC_SHEET);

		m_serverPref.Create(m_tabView);
		m_tabView.AddPage(m_serverPref, _T("Server"));

		m_editorPref.Create(m_tabView);
		m_tabView.AddPage(m_editorPref, _T("Editor"));

		m_colorPref.Create(m_tabView);
		m_tabView.AddPage(m_colorPref, _T("Colors"));

		m_resultPref.Create(m_tabView);
		m_tabView.AddPage(m_resultPref, _T("Results"));

		m_compilerPref.Create(m_tabView);
		m_tabView.AddPage(m_compilerPref, _T("Compiler"));

		m_otherPref.Create(m_tabView);
		m_tabView.AddPage(m_otherPref, _T("Other"));

		m_tabView.SetActivePage(0);

		m_cbConfig = GetDlgItem(IDC_COMBO_CONFIG);
		if (!::PopulateConfigCombo(m_cbConfig, (const TCHAR *)m_ConfigLabel))	//  No existing configurations
		{
			DoLoadFromConfig(false);
			m_compilerPref.LoadDefaults();
			DoApply(true);
		}
		m_ConfigId = m_cbConfig.GetCurSel();
		GetDlgItem(IDC_LOCATE).EnableWindow(true);
		if (m_disableEnvironmentSettings)
		{
			m_cbConfig.EnableWindow(false);
			GetDlgItem(IDC_SAVE_AS).EnableWindow(false);
			GetDlgItem(IDC_CONFIG_DELETE).EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_SAVE_AS).EnableWindow(true);
			GetDlgItem(IDC_CONFIG_DELETE).EnableWindow(m_ConfigLabel != g_DefaultConfig);
		}
		return 0;
	}

	void OnSelEndOkConfig(UINT /*nId*/, int /*wID*/, HWND /*hWndCtl*/)
	{
		CString title;
		GetWindowText(title);
		if ( GetChanged() && IDNO == MessageBox(_T("Lose Changes?"), title, MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) )
		{
			//set it back 
			m_cbConfig.SetCurSel( m_ConfigId );
			return;
		}
		CWaitCursor wait;

		//get name of selected config
		m_ConfigId = m_cbConfig.GetCurSel();
		m_cbConfig.GetLBText(m_ConfigId, m_ConfigLabel);

		InitConfig();
		DoLoadFromConfig(false);
	}

	LRESULT OnBnLocate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		boost::filesystem::path path;
		GetApplicationFolder(path);
		std::_tstring native_path = CA2T(path.native_file_string().c_str());
		::ShellExecute(m_hWnd, _T("open"), native_path.c_str(), _T(""), native_path.c_str(), SW_SHOW);
		return 0;
	}

	LRESULT OnBnSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString newLabel(_T("New"));
tryagain:
		if ( ::ShowSimplePrompt(m_hWnd,_T("New"),_T("Config:"), newLabel) )
		{
			CWaitCursor wait;
			if (IsValidIdentifier(static_cast<const TCHAR *>(newLabel)))
			{
				m_ConfigLabel = newLabel;
				m_cbConfig.SetCurSel(m_cbConfig.AddString(m_ConfigLabel));
				InitConfig();
				DoLoadFromConfig(true);
				SetChanged(true);
				m_compilerPref.LoadDefaults();
				//don't need to create the file, it will get created when the hit OK
				//m_iConfig->SaveAs(newLabel);
			}
			else
			{
				MessageBox(_T("Invalid configuration name..."), CString(MAKEINTRESOURCE(IDR_MAINFRAME)), MB_ICONEXCLAMATION);
				goto tryagain;
			}
		}
		return 0;
	}

	LRESULT OnBnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString title;
		GetWindowText(title);
		CString prompt;
		prompt.FormatMessage(_T("Delete Configuration '%1'?"), m_ConfigLabel);
		if ( IDYES == MessageBox(prompt,title,MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) )
		{
			CWaitCursor wait;
			SetChanged(false);
			boost::filesystem::path path;
			GetConfigPath(static_cast<const TCHAR *>(m_ConfigLabel), path);
			boost::filesystem::remove(path);
			//delete the current selection
			m_cbConfig.DeleteString(m_cbConfig.GetCurSel());
			//pick the next one
			if(m_cbConfig.GetCount() == 0)
			{
				m_cbConfig.AddString(_T("default"));
			}
			m_cbConfig.SetCurSel(0);
			m_cbConfig.GetLBText(0,m_ConfigLabel);
			//if this is the global one reset it
			//TODO Gordon needs to retest this...
			m_config->SetLabel(static_cast<const TCHAR *>(m_ConfigLabel));
			DoLoadFromConfig(false);
		}
		return 0;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoApply(true);
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		if ( m_changed )
		{
			CString title;
			GetWindowText(title);
			if ( IDNO == MessageBox(_T("Lose Changes?"),title,MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) )
			{
				return;
			}
		}
		m_colorPref.DoRestore();
		EndDialog(nID);
	}

	void OnApply(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoApply(false);
	}
};
//  ===========================================================================
bool ShowPreferencesDlg(CComPtr<IConfig> config, bool disableEnvironmentSettings)
{
	CPrefDlg dlg(config, disableEnvironmentSettings);
	dlg.DoModal();
	return true; //true if changed, it may have been
}
