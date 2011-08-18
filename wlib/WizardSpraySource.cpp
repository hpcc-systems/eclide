#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WizardSpraySource.h"

#include "FileSPray.h"
#include "DropZone.h"
#include "global.h"
#include "EclParser.h"

CWizardSpraySource::CWizardSpraySource(CWizardSprayData *data, _U_STRINGorID title) : baseClass(title)
{
	m_data = data;

	baseClass::SetHeaderTitle(_T("Source File Path"));
	baseClass::SetHeaderSubTitle(_T("Select the path of source file on landingzone."));
}

CWizardSpraySource::~CWizardSpraySource()
{
}


LRESULT CWizardSpraySource::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CWaitCursor waitCursor;

	InitializeControls();
	InitializeValues();

	return 1;
}

LRESULT CWizardSpraySource::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Be sure the base gets the message too
	bHandled = FALSE;

	UninitializeControls();	
	return 0;
}

void CWizardSpraySource::InitializeControls(void)
{
	m_ctrl_listview.Attach(GetDlgItem(IDC_LIST_FILEBROWSER));
	SetView(m_ctrl_listview, 3);

	CImageList img, state;
	img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	img.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_ctrl_listview.SetImageList(img, LVSIL_SMALL);

	m_ctrl_upbutton.Attach(GetDlgItem(IDC_BUTTON_UP));
	CIcon upIcon;
	upIcon.LoadIcon(IDI_ICON_UPFOLDER);
	m_ctrl_upbutton.SetIcon(upIcon);

	m_ctrl_gobutton.Attach(GetDlgItem(IDC_BUTTON_GO));
	CIcon goIcon;
	goIcon.LoadIcon(IDI_ICON_REFRESH);
	m_ctrl_gobutton.SetIcon(goIcon);
}

void CWizardSpraySource::UninitializeControls(void)
{
}

void CWizardSpraySource::InitializeValues(void)
{
	m_data->m_lz_path.SetEclPath(m_data->GetSource());
	LoadFolder();
}

bool CWizardSpraySource::StoreValues(void)
{
	std::_tstring tmp_path;
	m_data->SetSource(m_data->m_lz_path.GetEclPath(tmp_path));
	return true;
}

// Overrides from base class
int CWizardSpraySource::OnSetActive()
{
	if(m_data->GetFileSize()>0)
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		SetWizardButtons(PSWIZB_BACK);

	// 0 = allow activate
	// -1 = go back to page that was active
	// page ID = jump to page
	return 0;
}

int CWizardSpraySource::OnWizardNext()
{
	bool success = this->StoreValues();
	if(!success)
	{
		// Any errors are already reported, and if appropriate,
		// the control that needs attention has been given focus.
		return -1;
	}

	if(m_data->GetFileSize()<=0)
		return -1;

	// 0  = goto next page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID

	return m_data->FindNextPage(IDD);
}

int CWizardSpraySource::OnWizardBack()
{
	return m_data->FindPreviousPage(IDD);
}

void CWizardSpraySource::OnHelp()
{
	// NOTE: Several controls on this dialog have been given
	//  context sensitive help descriptions, and the HtmlHelp
	//  file is setup to recognize their help IDs.  Please
	//  look at resource.hm, help\Context.h, help\Context.txt
	//  and the help project Wizard97Test.hhp.
	//
	// It's also important to note that context help doesn't
	//  come through this route, but rather goes to the page and
	//  then the sheet (if not handled) as WM_HELP with dwContextId
	//  in the HELPINFO structure set to a non-zero value.
	//  See the sheet for how it deals with context help.
	//  We get to this point if the user clicks on the help button
	//  at the bottom.

	m_data->ShowHelp(IDD);
}

void CWizardSpraySource::SetView(HWND hWndListView, DWORD dwView) 
{ 
	// Retrieve the current window style. 
	DWORD dwStyle = ::GetWindowLong(hWndListView, GWL_STYLE); 
	// Only set the window style if the view bits have
	// changed. 
	if ((dwStyle & LVS_TYPEMASK) != dwView) 
	{
		::SetWindowLong(hWndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView); 
	}
} 	

void CWizardSpraySource::RefreshPath()
{
	m_ddx_ip = m_data->m_lz_path.GetIP();
	m_ddx_path = m_data->m_lz_path.GetPath();
	DoDataExchange();
}

void CWizardSpraySource::LoadRoot()
{
	CWaitCursor wait;
	m_ctrl_listview.DeleteAllItems();
	Topology::IDropZoneVector dropZones;
	StlLinked<Topology::ITopology> server = Topology::AttachTopology(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY));
	server->GetDropZones(dropZones);

	int id = 0;
	for (Topology::IDropZoneVector::iterator itr = dropZones.begin(); itr != dropZones.end(); ++itr)
	{
		std::_tstring name;
		itr->get()->GetName(name);
		std::_tstring folder;
		itr->get()->GetMachine()->GetDirectory(folder);
		Topology::IDropZone * dropzone = itr->get();
		dropzone->AddRef();
		std::_tstring label = name + _T(" (") + folder + _T(")");
		m_ctrl_listview.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, id, label.c_str(), 0, 0, 13, reinterpret_cast<LPARAM>(dropzone));
		++id;
	}
	m_data->m_lz_path.Set(Topology::OS_UNKNOWN, _T(""), _T(""), true);
	RefreshPath();
}

void CWizardSpraySource::LoadFolder()
{
	SetWizardButtons(PSWIZB_BACK);

	if (m_data->m_lz_path.IsEmpty())
	{
		LoadRoot();
		return;
	}
	CWaitCursor wait;
	m_ctrl_listview.DeleteAllItems();
	CComPtr<WsFileSpray::IFileSpray> server = WsFileSpray::AttachFileSpray(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_FILESPRAY) ,_T("FileSpray"));
	WsFileSpray::IFileSprayPathVector files;
	if (server->GetFiles(m_data->m_lz_path.GetIP(), m_data->m_lz_path.GetCurrentFolder(), _T(""), &files))
	{
		int id = 0;
		for(WsFileSpray::IFileSprayPathVector::iterator itr = files.begin(); itr != files.end(); ++itr)
		{
			WsFileSpray::IFileSprayPath * filespec = *itr;
			filespec->AddRef();
			m_ctrl_listview.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, id, filespec->GetName(), 0, 0, filespec->IsDir() ? 0 : 2, reinterpret_cast<LPARAM>(filespec));
			++id;
		}
	}
	RefreshPath();
}

LRESULT CWizardSpraySource::OnEnChangeEditFilepath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CWizardSpraySource::OnBnClickedButtonGo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(true);
	m_data->m_lz_path.Set(m_data->m_lz_path.GetOS(), static_cast<const TCHAR *>(m_ddx_ip), static_cast<const TCHAR *>(m_ddx_path), true);
	LoadFolder();
	return 0;
}

LRESULT CWizardSpraySource::OnBnClickedButtonUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_data->m_lz_path.Up();
	LoadFolder();
	return 0;
}

LRESULT CWizardSpraySource::OnNmDblCLickListBrowser(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) pNMHDR;

	CComQIPtr<Topology::IDropZone> dz = reinterpret_cast<IUnknown *>(m_ctrl_listview.GetItemData(lpnmitem->iItem));
	if (dz)
	{
		std::_tstring ip;
		std::_tstring folder;
		dz->GetMachine()->GetNetaddress(ip);
		dz->GetMachine()->GetDirectory(folder);
		m_data->m_lz_path.Set((Topology::OS)dz->GetMachine()->GetOSType(), ip, folder, true);
		LoadFolder();
	}
	else
	{
		CComPtr<WsFileSpray::IFileSprayPath> fs = reinterpret_cast<WsFileSpray::IFileSprayPath *>(m_ctrl_listview.GetItemData(lpnmitem->iItem));
		if (fs)
		{
			m_data->SetFileSize(fs->GetFileSize());
			m_data->m_lz_path.Down(fs->GetName(), fs->IsDir());
			if (fs->IsDir())
			{
				LoadFolder();
			}
			else
			{
				if(m_data->GetFileSize()>0)
					SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
				RefreshPath();
			}
		}
	}
	return 0;
}

LRESULT CWizardSpraySource::OnNotifyDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	switch (idCtrl)
	{
	case IDC_LIST_FILEBROWSER:
		{
			LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
			IUnknown * unknown = reinterpret_cast<IUnknown *>(pnmv->lParam);
			ATLASSERT(unknown);
			unknown->Release();
		}

		break;
	default:
		break;
	}
	return 0;
}	
