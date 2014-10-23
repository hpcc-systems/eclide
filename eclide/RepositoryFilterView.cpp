#include "stdafx.h"
#include "..\en_us\resource.h"

#include "RepositoryFilterView.h"
#include <SMC.h>

//  ==========================================================================
CRepositoryFilterView::CRepositoryFilterView() 
{
	m_comboModuleCtrl = new CComboModule();
}

CRepositoryFilterView::~CRepositoryFilterView()
{
}

void CRepositoryFilterView::Init(IRepositorySlot *owner)
{
	m_Owner = owner;
}

BOOL CRepositoryFilterView::PreTranslateMessage(MSG* pMsg)
{
	BOOL retVal = IsDialogMessage(pMsg);
	return retVal;
}

const TCHAR * CRepositoryFilterView::GetSearchText()
{
	return m_searchText;
}

FINDMODE CRepositoryFilterView::GetFindMode()
{
	switch (m_mode)
	{
	case SEARCHMODE_PLAIN:
		return FINDMODE_NONE;
	case SEARCHMODE_WILDCARD:
		return FINDMODE_WILDCARD;
	case SEARCHMODE_REGEXP:
		return FINDMODE_REGEX;
	}
	return FINDMODE_NONE;
}

LRESULT CRepositoryFilterView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false,false);

	*m_comboModuleCtrl = GetDlgItem(IDC_COMBO_MODULE);

	m_modeCtrl = GetDlgItem(IDC_COMBO_MODE);

	m_dateCtrl = GetDlgItem(IDC_DATETIMEPICKER_DATE);
	m_timeCtrl = GetDlgItem(IDC_DATETIMEPICKER_TIME);
	m_timeCtrl.ModifyStyle(0, DTS_TIMEFORMAT);
	m_results = GetDlgItem(IDC_TREE_REPOSITORY);

	WTL::CImageList img, state;
	img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	img.SetBkColor(GetSysColor(COLOR_WINDOW));

	state.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	state.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_search = GetDlgItem(IDC_BUTTON_SEARCH);

	m_Tree = GetDlgItem(IDC_TREE_REPOSITORY);
	m_Tree.SetImageLists(img, state);

	return 0;
}

//LRESULT CRepositoryFilterView::OnTreeNotify(LPNMHDR pNMHDR)
//{
//	switch(pNMHDR->code)
//	{
//	case TVN_GETDISPINFO:
//		{
//			LPNMTVDISPINFO pTVDispInfo = (LPNMTVDISPINFO)pNMHDR;
//			if (pTVDispInfo->item.lParam)
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)pTVDispInfo->item.lParam;
//				tn->GetDispInfo(pTVDispInfo->item);
//			}
//		}
//		break;
//	case TVN_ITEMEXPANDING:
//		{
//			CWaitCursor w;
//			LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
//			ATLASSERT(pNMTreeView->itemNew.lParam);
//			CComPtr<CTreeNode> tn = (CTreeNode *)pNMTreeView->itemNew.lParam;
//			if (pNMTreeView->action == TVE_EXPAND)
//			{
//				if (!(pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
//					tn->ItemExpanding();
//			}
//			else
//				m_Tree.Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
//			//OnItemExpanding(tn, pNMTreeView->action == TVE_EXPAND);
//		}
//		break;
//	case TVN_DELETEITEM:
//		{
//			LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
//			ATLASSERT(pNMTreeView->itemOld.lParam);
//			CTreeNode * tn = (CTreeNode *)pNMTreeView->itemOld.lParam;
//			tn->SetData(NULL);
//			tn->Release();
//		}
//		break;
//	case TVN_GETINFOTIP:
//		SetMsgHandled(TRUE);
//		{
//			//LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);
//			//_tcscpy(pGetInfoTip->pszText, _T("GordonZZZ"));
//		}
//		break;
//	case TVN_BEGINLABELEDIT:
//		SetMsgHandled(TRUE);
//		{
//			LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)pNMHDR;
//			ATLASSERT(dispInfo->item.lParam);
//			CComPtr<CTreeNode> tn = (CTreeNode *)dispInfo->item.lParam;
//			if (tn->CanEdit())
//			{
//				return false;
//			}
//			else
//			{
//				return true;
//			}
//		}
//		break;
//	case TVN_ENDLABELEDIT:
//		SetMsgHandled(TRUE);
//		{
//			LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)pNMHDR;
//			ATLASSERT(dispInfo->item.lParam);
//			if(dispInfo->item.pszText != NULL)
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)dispInfo->item.lParam;
//				return tn->Rename(dispInfo->item.pszText);
//			}
//		}
//		break;
//	case TVN_SELCHANGED :
//		SetMsgHandled(TRUE);
//		{
//			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pNMHDR; 
//			ATLTRACE(_T("TVN_SELCHANGED(%0x,%0x)\n"),  pnmtv->itemNew.hItem, pnmtv->itemOld.hItem );
//			if ( pnmtv->itemNew.hItem && pnmtv->itemNew.lParam )
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemNew.lParam;
//				//OnItemSelectionChanged(tn, true);
//			}
//			else if ( pnmtv->itemOld.hItem && pnmtv->itemOld.lParam )
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemOld.lParam;
//				//OnItemSelectionChanged(tn, false);
//			}
//		}
//		break;
//	case TVN_LAST :
//		SetMsgHandled(TRUE);
//		{
//			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pNMHDR; 
//			CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemNew.lParam;
//			//OnItemSelectionChanged(tn, true);
//		}
//		break;
//
//	case NM_CLICK:
//		SetMsgHandled(TRUE);
//		//HandleNmClick((LPNMTREEVIEW) pNMHDR);
//		break;
//
//	case NM_DBLCLK:
//		SetMsgHandled(TRUE);
//		{
//			TVITEM item;
//			item.mask = TVIF_HANDLE;
//			item.hItem = m_Tree.GetSelectedItem();
//			if (m_Tree.GetItem(&item))
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
//				tn->OnDblclk(item.hItem);
//			}
//		}
//		break;
//	}
//	return 0;
//}

void CRepositoryFilterView::OnContextMenu(HWND phWnd, CPoint pt)
{
	SetMsgHandled(true);
	if (phWnd == (HWND)m_Tree)
	{
		CRepositorySelections s;

		GetSelected(s);

		m_Owner->OnContext(s, pt);
	}
}

LRESULT CRepositoryFilterView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_comboModuleCtrl->ResetContent();
	m_searchText = GLOBAL_REPOSITORYFIND_TEXT.second;
	m_searchModule = _T("");
	m_searchUser = GLOBAL_REPOSITORYFIND_USER.second;
	m_mode = static_cast<SEARCHMODE>((int)GLOBAL_REPOSITORYFIND_MODE.second);
	m_sandboxed = GLOBAL_REPOSITORYFIND_SANDBOXED.second;
	m_checkedout = GLOBAL_REPOSITORYFIND_CHECKEDOUT.second;
	m_locked = GLOBAL_REPOSITORYFIND_LOCKED.second;
	m_orphaned = GLOBAL_REPOSITORYFIND_ORPHANED.second;
	m_modifiedSince = GLOBAL_REPOSITORYFIND_MODIFIEDSINCE.second;
	m_dateTime = CString(GLOBAL_REPOSITORYFIND_DATETIME.second);
	DoDataExchange();
	DoRestoreState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	m_modeCtrl.ResetContent();
	return 0;
}

LRESULT CRepositoryFilterView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_searchText = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_TEXT);
	m_comboModuleCtrl->Load(_T(""));
	m_comboModuleCtrl->SelectDefault(m_searchModule);
	m_searchUser = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_USER);
	m_mode = static_cast<SEARCHMODE>((int)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_MODE));
	m_sandboxed = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_SANDBOXED);
	m_checkedout = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_CHECKEDOUT);
	m_locked = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_LOCKED);
	m_orphaned = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_ORPHANED);
	m_modifiedSince = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_MODIFIEDSINCE);
	m_dateTime = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_REPOSITORYFIND_DATETIME));
	DoDataExchange();
	DoRestoreState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();

	CComPtr<SMC::ISMC> smc = SMC::AttachSMC(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC), _T("SMC"));
	CComPtr<SMC::IVersion> serverVersion = smc->GetBuild();

	m_modeCtrl.ResetContent();
	int row = 0;
	if (serverVersion->GetMajor() >= 5)
	{
		row = m_modeCtrl.AddString(_T("Plain"));
		m_modeCtrl.SetItemData(row, SEARCHMODE_PLAIN);
	}
	row = m_modeCtrl.AddString(_T("Wildcard"));
	m_modeCtrl.SetItemData(row, SEARCHMODE_WILDCARD);
	row = m_modeCtrl.AddString(_T("Regular Expression"));
	m_modeCtrl.SetItemData(row, SEARCHMODE_REGEXP);

	return 0;
}

LRESULT CRepositoryFilterView::OnSubmitDone(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	ATLASSERT(wParam);
	StlLinked<IAttributeVectorPtr> results = reinterpret_cast<IAttributeVectorPtr *>(wParam);
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	if (results->size())
	{
		m_Root = new CRepositoryFilterNode(m_Owner, _T("Results"), results.get()); //  No attach as it is "in" the tree control also.
		m_Root->InsertBelow(m_Tree, TVI_ROOT);
		m_Root->Expand();
	}
	else if (lParam == 0)
	{
		CLoadingNode * loading = new CLoadingNode(NULL, _T("...No Matches Found..."));
		loading->InsertBelow(m_Tree, TVI_ROOT);
	}
	else
	{
		CLoadingNode * loading = new CLoadingNode(NULL, _T("...Search Failed (see error window)..."));
		loading->InsertBelow(m_Tree, TVI_ROOT);
	}
	m_search.EnableWindow(true);
	return 0;
}

LRESULT CRepositoryFilterView::OnRefreshState(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	DoDataExchange(true);
	DoRefreshState();
	return 0;
}

LRESULT CRepositoryFilterView::OnEnChangeEditSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}

LRESULT CRepositoryFilterView::OnEnChangeUserSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}

LRESULT CRepositoryFilterView::OnCbnSelendokComboModule(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_searchModule = m_comboModuleCtrl->GetSelectedModule();
	return 0;
}

LRESULT CRepositoryFilterView::OnCbnSelendokComboMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_mode = SEARCHMODE_UNKNOWN;
	int sel = m_modeCtrl.GetCurSel();
	if (sel >=0)
		m_mode = static_cast<SEARCHMODE>(m_modeCtrl.GetItemData(sel));
	return 0;
}

LRESULT CRepositoryFilterView::OnBnClickedCheckSandboxed(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}
LRESULT CRepositoryFilterView::OnBnClickedCheckCheckedout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}
LRESULT CRepositoryFilterView::OnBnClickedCheckLocked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}
LRESULT CRepositoryFilterView::OnBnClickedCheckOrphaned(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}

LRESULT CRepositoryFilterView::OnBnClickedCheckModifiedsince(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(RFUM_REFRESHSTATE);
	return 0;
}
LRESULT CRepositoryFilterView::OnBnClickedButtonSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_search.EnableWindow(false);
	DoSaveState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	CLoadingNode * loading = new CLoadingNode(NULL, _T("...Searching..."));
	loading->InsertBelow(m_Tree, TVI_ROOT);
	DoSearchOptions options;
	options.mode = m_mode;
	options.sandboxed = m_sandboxed;
	options.checkedout = m_checkedout;
	options.locked = m_locked;
	options.orphaned = m_orphaned;
	options.modifiedSince = m_modifiedSince;
	clib::thread run(__FUNCTION__, boost::bind(&DoSearch, this, static_cast<const TCHAR *>(m_searchText), static_cast<const TCHAR *>(m_searchModule), static_cast<const TCHAR *>(m_searchUser), options, m_dateTime.c_str()));
	return 0;
}

LRESULT CRepositoryFilterView::OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoSaveState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	return 0;
}

LRESULT CRepositoryFilterView::OnBnClickedButtonCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_Root)
	{
		std::_tstring attrs;
		WTL::CTreeItem item = *m_Root;
		while(item)
		{
			if (CComQIPtr<CAttributeNode> node = reinterpret_cast<CTreeNode *>(item.GetData()))
			{
				if (!attrs.empty())
					attrs += _T("\r\n");
				attrs += node->GetAttribute()->GetQualifiedLabel();
			}
			item = m_Tree.GetNextItem(item, TVGN_EX_ALL);
		}
		SetClipboard(attrs);
	}
	return 0;
}

bool CRepositoryFilterView::UpdateUI(CCmdUI * cui)
{
	return false;
}

void CRepositoryFilterView::DoSaveState()
{
	DoDataExchange(true);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_TEXT, (const TCHAR *)m_searchText);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_USER, (const TCHAR *)m_searchUser);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_MODE, m_mode);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_SANDBOXED, m_sandboxed);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_CHECKEDOUT, m_checkedout);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_LOCKED, m_locked);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_ORPHANED, m_orphaned);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_MODIFIEDSINCE, m_modifiedSince);
	SYSTEMTIME date, time;
	m_dateCtrl.GetSystemTime(&date);
	m_timeCtrl.GetSystemTime(&time);
	DateTimeToUTC(date, time, m_dateTime);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_REPOSITORYFIND_DATETIME, m_dateTime);
}

void CRepositoryFilterView::DoRestoreState()
{
	if (m_mode == SEARCHMODE_UNKNOWN)
		m_mode = SEARCHMODE_WILDCARD;

	m_modeCtrl.SetCurSel(m_mode);

	SYSTEMTIME dateTime;
	memset(&dateTime, 0, sizeof(dateTime));
	UTCToSystemTime(m_dateTime, dateTime, true);
	m_dateCtrl.SetSystemTime(GDT_VALID, &dateTime);
	m_timeCtrl.SetSystemTime(GDT_VALID, &dateTime);
	DoRefreshState();
}

void CRepositoryFilterView::DoRefreshState()
{
	if(m_modifiedSince)
	{
		m_dateCtrl.EnableWindow(true);
		m_timeCtrl.EnableWindow(true);
	}
	else
	{
		m_dateCtrl.EnableWindow(false);
		m_timeCtrl.EnableWindow(false);
	}

	if (m_search.IsWindowEnabled() && m_searchText.IsEmpty() && m_searchUser.IsEmpty() && !m_sandboxed && !m_checkedout && !m_locked && !m_orphaned && !m_modifiedSince)
	{
		m_search.EnableWindow(false);
	}
	else if (!m_search.IsWindowEnabled() && (!m_searchText.IsEmpty() || !m_searchUser.IsEmpty() || m_sandboxed || m_checkedout || m_locked || m_orphaned || m_modifiedSince))
	{
		m_search.EnableWindow(true);
	}
}

void CRepositoryFilterView::OnItemSelectionChanged(CTreeNode *node, bool bSelected )
{
	m_Owner->OnItemSelectionChanged(node,bSelected);
}

void CRepositoryFilterView::OnItemExpanding(CTreeNode *node, bool bExpanding)
{
	m_Owner->OnExpandItem(node, bExpanding);
}

void CRepositoryFilterView::HandleNmClick(LPNMTREEVIEW pNmHdr)
{
	//this is only needed so that we can tell if they clicked on
	//an item that is already selected
	if ( pNmHdr->action == TVC_BYMOUSE )
	{
		unsigned nFlags;
		HTREEITEM hItem = m_Tree.HitTest(pNmHdr->ptDrag, &nFlags);
		ATLTRACE(_T("NM_CLICK(%0x)\n"), hItem );

		if (hItem && (nFlags & TVHT_ONITEMINDENT|TVHT_ONITEMSTATEICON|TVHT_ONITEM|TVHT_ONITEMRIGHT) > 0 )
		{
			TVITEM item;
			item.mask = TVIF_HANDLE;
			item.hItem = hItem;
			if ( m_Tree.GetItem(&item))
			{
				CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
				OnItemSelectionChanged(tn, true );
			}
		}
	}
}

void CRepositoryFilterView::DoSearch(CRepositoryFilterView *self, std::_tstring searchText, std::_tstring searchModule, std::_tstring searchUser, DoSearchOptions searchOptions, std::_tstring dateTime)
{
	CComPtr<IRepository> rep = AttachRepository();
	IAttributeVectorPtr * results = new IAttributeVectorPtr();	//This is released in the submit done.
	int foundCount = rep->FindAttributes(searchText, searchModule, searchUser, searchOptions.mode, searchOptions.sandboxed, searchOptions.checkedout, searchOptions.locked, searchOptions.orphaned, searchOptions.modifiedSince ? dateTime.c_str() : _T(""), *results);
	::PostMessage(self->m_hWnd, CWM_SUBMITDONE, (WPARAM)(results), foundCount);
}
//  ===========================================================================
