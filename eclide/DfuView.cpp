#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DfuView.h"
#include "global.h"
#include "TreeNode.h"
#include "Util.h"
#include <utilDateTime.h> //clib
#include <RecursiveMutex.h> //clib

#define MAX_ASYNC_SYNTAXCHECK 2

//  ===========================================================================
CDfuView::CDfuView()
{
}

CDfuView::~CDfuView()
{
}

void CDfuView::Init(IDfuSlot *pMenuManager)
{
	m_Owner = pMenuManager;
}

BOOL CDfuView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
		{
			TVITEM item;
			item.mask = TVIF_HANDLE;
			item.hItem = m_Tree.GetSelectedItem();
			if (m_Tree.GetItem(&item))
			{
				CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
				tn->OnDblclk(m_Tree.GetSelectedItem());
			}
		}
			break;
		case VK_F5:
			//RefreshRepository();
			break;
		default:
			break;
		}
	}

	return IsDialogMessage(pMsg);
}

void CDfuView::DoClear()
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	m_Root.Release();
}

void CDfuView::DoReset()
{
	Refresh();
	m_Root->Expand();
}

LRESULT CDfuView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false,false);

	WTL::CImageList img, state;
	img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

	img.SetBkColor(GetSysColor(COLOR_WINDOW));
	ATLTRACE(_T("IDB_REP images=%d\n"), img.GetImageCount());

	state.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	state.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_Tree = GetDlgItem(IDC_TREE_DFU);
	m_Tree.SetImageLists(img, state);

	DoDataExchange();
	//Refresh();

	return 0;
}

void CDfuView::HandleNmClick(LPNMTREEVIEW pNmHdr)
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

void CDfuView::OnItemSelectionChanged(CTreeNode *node, bool bSelected )
{
	m_Owner->OnItemSelectionChanged(node,bSelected);
}


void CDfuView::OnItemExpanding(CTreeNode *node, bool bExpanding)
{
	m_Owner->OnExpandItem(node, bExpanding);
}

LRESULT CDfuView::OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_Owner->OnItemSelectionChanged(0,true);
	return 0;
}

LRESULT CDfuView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	DoClear();
	return 0;
}

LRESULT CDfuView::OnReset(UINT /*uMsg*/, WPARAM, LPARAM /*lParam*/)
{
	DoReset();
	return 0;
}

LRESULT CDfuView::OnInitialize(UINT /*uMsg*/, WPARAM /*wNoCreate*/, LPARAM /*lParam*/)
{
	Refresh();
	return 0;
}

void CDfuView::OnPaint(HDC /*hDc*/)
{
	SetMsgHandled(false);
}

void CDfuView::OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
	Refresh();
}

void CDfuView::Refresh()
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	m_Root = new CDfuNode(m_Owner, m_label);
	m_Root->InsertBelow(m_Tree, TVI_ROOT);
}

HWND CDfuView::GetTreeHandle()
{
	return m_Tree;
}

void CDfuView::OnContextMenu(HWND phWnd, CPoint pt)
{
	if (phWnd == (HWND)m_Tree)
	{
		CDfuSelections s;

		GetSelected(s);

		m_Owner->OnContext(s, pt);
	}
}
//  ==========================================================================
CDfuFilterView::CDfuFilterView() 
{
}

CDfuFilterView::~CDfuFilterView()
{
}

void CDfuFilterView::Init(IDfuSlot *owner)
{
	m_Owner = owner;
}

BOOL CDfuFilterView::PreTranslateMessage(MSG* pMsg)
{
	BOOL retVal = IsDialogMessage(pMsg);
	return retVal;
}

LRESULT CDfuFilterView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false,false);

	m_results = GetDlgItem(IDC_TREE_DFU);

	WTL::CImageList img, state;
	img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	img.SetBkColor(GetSysColor(COLOR_WINDOW));

	state.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	state.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_search = GetDlgItem(IDC_BUTTON_SEARCH);

	m_Tree = GetDlgItem(IDC_TREE_DFU);
	m_Tree.SetImageLists(img, state);

	return 0;
}

void CDfuFilterView::OnContextMenu(HWND phWnd, CPoint pt)
{
	if (phWnd == (HWND)m_Tree)
	{
		CDfuSelections s;

		GetSelected(s);

		m_Owner->OnContext(s, pt);
	}
}

LRESULT CDfuFilterView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_searchPrefix = GLOBAL_DFUFIND_PREFIX.second;
	m_searchCluster = GLOBAL_DFUFIND_CLUSTER.second;
	m_searchLogicalName = GLOBAL_DFUFIND_LOGICALNAME.second;
	m_searchDescription = GLOBAL_DFUFIND_DESCRIPTION.second;
	m_searchOwner = GLOBAL_DFUFIND_OWNER.second;
	DoDataExchange();
	DoRestoreState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	return 0;
}

LRESULT CDfuFilterView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_searchPrefix = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DFUFIND_PREFIX);
	m_searchCluster = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DFUFIND_CLUSTER);
	m_searchLogicalName = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DFUFIND_LOGICALNAME);
	m_searchDescription = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DFUFIND_DESCRIPTION);
	m_searchOwner = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DFUFIND_OWNER);
	DoDataExchange();
	DoRestoreState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	return 0;
}

LRESULT CDfuFilterView::OnSubmitDone(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	ATLASSERT(wParam);
	StlLinked<Dfu::IFileVectorPtr> results = reinterpret_cast<Dfu::IFileVectorPtr *>(wParam);
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	m_Root = new CDfuFilterNode(m_Owner, _T("Results"), results.get()); //  No attach as it is "in" the tree control also.
	m_Root->InsertBelow(m_Tree, TVI_ROOT);
	m_Root->Expand();
	m_search.EnableWindow(true);
	return 0;
}

LRESULT CDfuFilterView::OnRefreshState(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	DoDataExchange(true);
	DoRefreshState();
	return 0;
}

LRESULT CDfuFilterView::OnEnChangeEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(DFUM_REFRESHSTATE);
	return 0;
}

LRESULT CDfuFilterView::OnBnClickedButtonSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_search.EnableWindow(false);
	DoSaveState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	clib::thread run(__FUNCTION__, boost::bind(&DoSearch, this, static_cast<const TCHAR *>(m_searchPrefix), static_cast<const TCHAR *>(m_searchCluster), static_cast<const TCHAR *>(m_searchLogicalName), static_cast<const TCHAR *>(m_searchDescription), static_cast<const TCHAR *>(m_searchOwner)));
	return 0;
}

LRESULT CDfuFilterView::OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoSaveState();
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	return 0;
}

void CDfuFilterView::DoSaveState()
{
	DoDataExchange(true);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_DFUFIND_PREFIX, (const TCHAR *)m_searchPrefix);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_DFUFIND_CLUSTER, (const TCHAR *)m_searchCluster);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_DFUFIND_LOGICALNAME, (const TCHAR *)m_searchLogicalName);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_DFUFIND_DESCRIPTION, (const TCHAR *)m_searchDescription);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_DFUFIND_OWNER, (const TCHAR *)m_searchOwner);
}

void CDfuFilterView::DoRestoreState()
{
	DoRefreshState();
}

void CDfuFilterView::DoRefreshState()
{
	if (m_search.IsWindowEnabled() && m_searchPrefix.IsEmpty() && m_searchCluster.IsEmpty() && m_searchLogicalName.IsEmpty() && m_searchDescription.IsEmpty() && m_searchOwner.IsEmpty())
	{
		m_search.EnableWindow(false);
	}
	else if (!m_search.IsWindowEnabled() && (!m_searchPrefix.IsEmpty() || !m_searchCluster.IsEmpty() || !m_searchLogicalName.IsEmpty() || !m_searchDescription.IsEmpty() || !m_searchOwner.IsEmpty()))
	{
		m_search.EnableWindow(true);
	}
}

void CDfuFilterView::OnItemSelectionChanged(CTreeNode *node, bool bSelected )
{
	m_Owner->OnItemSelectionChanged(node,bSelected);
}

void CDfuFilterView::OnItemExpanding(CTreeNode *node, bool bExpanding)
{
	m_Owner->OnExpandItem(node, bExpanding);
}

void CDfuFilterView::HandleNmClick(LPNMTREEVIEW pNmHdr)
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

void CDfuFilterView::DoSearch(CDfuFilterView *self, std::_tstring searchPrefix, std::_tstring searchCluster, std::_tstring searchLogicalName, std::_tstring searchDescription, std::_tstring searchOwner)
{
	CComPtr<Dfu::IServer> server = Dfu::AttachDfu();
	Dfu::IFileVectorPtr * results = new Dfu::IFileVectorPtr();	//This is released in the submit done.
	server->FindFiles(searchPrefix, searchCluster, searchLogicalName, searchDescription, searchOwner, results);
	::PostMessage(self->m_hWnd, CWM_SUBMITDONE, (WPARAM)(results), 0);
}


//  ===========================================================================
