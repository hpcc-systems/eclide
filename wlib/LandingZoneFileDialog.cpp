#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "LandingZoneFileDialog.h"
#include "Combo.h"
#include "Topology.h"
#include "LZoneFolderTree.h"

const SectionLabelDefault GLOBAL_LANDINGZONE(SectionLabel(_T("FileSyncAndSpray"), _T("Options")), _T(""));

class CLandingZoneFileDialog : public CDialogImpl<CLandingZoneFileDialog>, 
	public CWinDataExchange<CLandingZoneFileDialog>
{
	typedef CLandingZoneFileDialog thisClass;
	typedef CDialogImpl<thisClass> baseClass;
	
protected:
	CComPtr<CComboDropZone> m_comboLandingZone;
	std::_tstring m_ip;
	CComPtr<WsFileSpray::IFileSprayPath> m_currSel;

public:
	CLZoneFolderTree m_tree;

	CLandingZoneFileDialog(WsFileSpray::IFileSprayPath * currSel)
	{
		m_comboLandingZone = new CComboDropZone();
		m_ip = currSel->GetIP();
		m_currSel = currSel;
	}

	enum { IDD = IDD_LANDINGZONE };

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_COMBO_LANDINGZONE, CBN_SELCHANGE, OnCbnSelchangeComboLandingzone)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_COMBO_LANDINGZONE, m_comboLandingZone->m_value)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		m_comboLandingZone->Attach(GetDlgItem(IDC_COMBO_LANDINGZONE));
		m_comboLandingZone->Load();

		std::_tstring topoIP = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY));
		CComPtr<Topology::ITopology> topology = AttachTopology(topoIP, topoIP);
		CComPtr<Topology::IDropZone> dz = topology->GetDropZone(m_ip);
		if (dz)
		{
			std::_tstring name;
			m_comboLandingZone->m_value = dz->GetName(name);
			m_comboLandingZone->SelectDefault(m_comboLandingZone->m_value);
		}
		else
		{
			LoadFromDropCombo();
		}

		m_tree.SubclassWindow(GetDlgItem(IDC_TREE_LANDINGZONE));
		m_tree.SetupImageList();

		DoDataExchange(false);

		RefreshTree();
		return 0;
	}

	void OnClose()
	{
		SetMsgHandled(false);
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoDataExchange(true);
		HTREEITEM selected=m_tree.GetSelectedItem();
		if(selected)
		{
			DWORD_PTR data=m_tree.GetItemData(selected);
			WsFileSpray::IFileSprayPath* path=reinterpret_cast<WsFileSpray::IFileSprayPath*>(data);
			// store selected path somewhere
			if(path)
				path;
		}

		GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_LANDINGZONE, m_comboLandingZone->m_value);
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}

	void RefreshTree()
	{
		CWaitCursor cursor;
		m_tree.DeleteAllItems();
		m_tree.SetFocus();
		m_tree.InsertRootNode(m_ip, m_currSel);
	}

	void LoadFromDropCombo()
	{
		CComPtr<Topology::IDropZone> dz = m_comboLandingZone->GetSelectedDropZone();
		if (dz)
		{
			CComPtr<Topology::IMachine> machine = dz->GetMachine();
			CComPtr<WsFileSpray::IFileSpray> server = WsFileSpray::AttachFileSpray(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_FILESPRAY) ,_T("FileSpray"));
			machine->GetNetaddress(m_ip);
			std::_tstring path;
			machine->GetDirectory(path);
			m_currSel = server->GetFolder(m_ip, path);
		}
	}

	LRESULT OnCbnSelchangeComboLandingzone(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DoDataExchange(true);
		LoadFromDropCombo();
		RefreshTree();

		return 0;
	}
};

INT_PTR DoModalLandingZoneDialog(CComPtr<WsFileSpray::IFileSprayPath> & path)
{
	CLandingZoneFileDialog dlg(path);
	//dlg.m_tree.m_currSel = path;
	INT_PTR result = dlg.DoModal();
	if (IDOK == result)
	{
		path = dlg.m_tree.GetCurrSel();
	}
	return result;
}

