#include "stdafx.h"
#include "..\en_us\resource.h"

#include "RepositoryView.h"
#include "global.h"
#include "TreeNode.h"
#include "Util.h"
#include <utilDateTime.h> //clib
#include <RecursiveMutex.h> //clib

#define MAX_ASYNC_SYNTAXCHECK 2

//  ===========================================================================
CRepositoryView::CRepositoryView()
{
}

void CRepositoryView::Init(IRepositorySlot *pMenuManager)
{
	m_Owner = pMenuManager;
}

CRepositoryView::~CRepositoryView()
{
}

BOOL CRepositoryView::PreTranslateMessage(MSG* pMsg)
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
		case _T('C'):
		case _T('c'):
			{
				UINT ctrl = ::GetKeyState(VK_CONTROL);
				UINT alt = ::GetKeyState(VK_MENU);
				if (ctrl != 0 && alt != 0)
				{
					CRepositorySelections s;
					GetSelected(s);
					if (!s.attrs.empty())
						DoCopy(s.attrs, _T("\r\n"));

					return 0;
				}
			}
			break;
		case VK_F5:
			RefreshRepository();
			break;
		default:
			break;
		}
	}

	return IsDialogMessage(pMsg);
}

void CRepositoryView::DoClear()
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	m_Root.Release();
}

void CRepositoryView::DoReset()
{
	Refresh(NULL);
	m_Root->Expand();
}

LRESULT CRepositoryView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false,false);

	WTL::CImageList img, state;
	img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

	img.SetBkColor(GetSysColor(COLOR_WINDOW));
	ATLTRACE(_T("IDB_REP images=%d\n"), img.GetImageCount());

	state.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	state.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_Tree = GetDlgItem(IDC_TREE_REPOSITORY);
	m_Tree.SetImageLists(img, state);

	//::SetWindowTheme(m_Tree, L"explorer", 0);
	//m_Tree.ModifyStyle(TVS_HASLINES, 0);
	//m_Tree.ModifyStyleEx(0, TVS_EX_DOUBLEBUFFER | TVS_EX_FADEINOUTEXPANDOS | TVS_EX_AUTOHSCROLL);

	DoDataExchange();
	//Refresh();

	return 0;
}

void CRepositoryView::HandleNmClick(LPNMTREEVIEW pNmHdr)
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

void CRepositoryView::OnItemSelectionChanged(CTreeNode *node, bool bSelected )
{
	m_Owner->OnItemSelectionChanged(node,bSelected);
}


void CRepositoryView::OnItemExpanding(CTreeNode *node, bool bExpanding)
{
	m_Owner->OnExpandItem(node, bExpanding);
}

LRESULT CRepositoryView::OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_Owner->OnItemSelectionChanged(0,true);
	return 0;
}

LRESULT CRepositoryView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	DoClear();
	return 0;
}

LRESULT CRepositoryView::OnReset(UINT /*uMsg*/, WPARAM, LPARAM /*lParam*/)
{
	DoReset();
	return 0;
}

LRESULT CRepositoryView::OnRefresh(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	Refresh((IModule *)wParam);
	return 0;
}
LRESULT CRepositoryView::OnInitialize(UINT /*uMsg*/, WPARAM /*wNoCreate*/, LPARAM /*lParam*/)
{
	Refresh(NULL);
	return 0;
}

void CRepositoryView::OnPaint(HDC /*hDc*/)
{
	SetMsgHandled(false);
}

//void CRepositoryView::OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
//{
//	Refresh();
//}
//
void CRepositoryView::Refresh(IModule * module)
{
	CComPtr<IRepository> rep = m_Owner->GetRepository();
	rep->ClearShortTermCache();
	if (module == NULL)
	{
		WTL::CLockWindowUpdate lock(m_hWnd);
		m_Tree.DeleteAllItems();
		m_Root = new CRepositoryNode(m_Owner, true); //  No attach as it is "in" the tree control also.
		m_Root->InsertBelow(m_Tree, TVI_ROOT);
		m_Root->Expand();
	}
	else
		TreeNode::RefreshChildren(module->GetQualifiedLabel(), m_Root);
}

IAttribute * CRepositoryView::GetFirstSelectedAttribute()
{
	for(HTREEITEM pos = m_Tree.GetFirstSelectedItem(); pos != (HTREEITEM)0; pos = m_Tree.GetNextSelectedItem(pos))
	{
		WTL::CTreeItem item(pos, &m_Tree);

		CComQIPtr<CAttributeNode> attrNode = reinterpret_cast<CTreeNode *>(item.GetData());
		if (attrNode)
			return attrNode->GetAttribute();
	}
	return NULL;
}

IModule * CRepositoryView::GetFirstSelectedModule()
{
	for(HTREEITEM pos = m_Tree.GetFirstSelectedItem(); pos != (HTREEITEM)0; pos = m_Tree.GetNextSelectedItem(pos))
	{
		WTL::CTreeItem item(pos, &m_Tree);

		CComQIPtr<CModuleNode> node = reinterpret_cast<CTreeNode *>(item.GetData());
		if (node)
			return node->GetModule();
	}
	return NULL;
}

HWND CRepositoryView::GetTreeHandle()
{
	return m_Tree;
}

void CRepositoryView::OnContextMenu(HWND phWnd, CPoint pt)
{
	SetMsgHandled(true);
	if (phWnd == (HWND)m_Tree)
	{
		CRepositorySelections s;

		GetSelected(s);

		m_Owner->OnContext(s, pt);
	}
}
//  ===========================================================================
CVectorView::CVectorView(IRepositorySlot * owner)
{
	m_Owner = owner;
}

CVectorView::~CVectorView()
{
}

BOOL CVectorView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_RETURN) 
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
	}

	return IsDialogMessage(pMsg);
}

LRESULT CVectorView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false,false);

	WTL::CImageList img, state;
	img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	img.SetBkColor(GetSysColor(COLOR_WINDOW));
	ATLTRACE(_T("IDB_REP images=%d\n"), img.GetImageCount() );

	state.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	state.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_Tree = GetDlgItem(IDC_TREE_REPOSITORY);
	m_Tree.SetImageLists(img, state);

	DoDataExchange();
	Refresh();

	return 0;
}

void CVectorView::HandleNmClick(LPNMTREEVIEW pNmHdr)
{
	//this is only needed so that we can tell if they clicked on
	//an item that is already selected
	if ( pNmHdr->action == TVC_BYMOUSE )
	{
		unsigned nFlags;
		HTREEITEM hItem = m_Tree.HitTest(pNmHdr->ptDrag, &nFlags);
		ATLTRACE(_T("NM_CLICK(%0x)\n"), hItem );

		if (hItem && (nFlags & TVHT_ONITEM) > 0 )
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

void CVectorView::OnItemSelectionChanged(CTreeNode *node, bool bSelected )
{
	m_Owner->OnItemSelectionChanged(node,bSelected);
}

void CVectorView::OnItemExpanding(CTreeNode *node, bool bExpanding)
{
	m_Owner->OnExpandItem(node, bExpanding);
}

LRESULT CVectorView::OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_Owner->OnItemSelectionChanged(0,true);
	return 0;
}

LRESULT CVectorView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	return 0;
}

LRESULT CVectorView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	//GJS Todo m_Tree.PushState();
	m_Tree.DeleteAllItems();
	m_Root = new CRepositoryNode(m_Owner, false); //  No attach as it is "in" the tree control also.
	m_Root->InsertBelow(m_Tree, TVI_ROOT);
	m_Root->Expand();
	return 0;
}

void CVectorView::OnPaint(HDC /*hDc*/)
{
	SetMsgHandled(false);
}

void CVectorView::OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
	Refresh();
}

IAttribute * CVectorView::GetFirstSelectedAttribute()
{
	for(HTREEITEM pos = m_Tree.GetFirstSelectedItem(); pos != (HTREEITEM)0; pos = m_Tree.GetNextSelectedItem(pos))
	{
		WTL::CTreeItem item(pos, &m_Tree);

		CComQIPtr<CAttributeNode> attrNode = reinterpret_cast<CTreeNode *>(item.GetData());
		if (attrNode)
			return attrNode->GetAttribute();
	}
	return NULL;
}

HWND CVectorView::GetTreeHandle()
{
	return m_Tree;
}

void CVectorView::OnContextMenu(HWND phWnd, CPoint pt)
{
	SetMsgHandled(true);
	if (phWnd == (HWND)m_Tree)
	{
		CRepositorySelections s;

		GetSelected(s);

		m_Owner->OnContext(s, pt);
	}
}
//  ---------------------------------------------------------------------------
CAttributeVectorView::CAttributeVectorView(IRepositorySlot * owner, IAttributeVector & attrs) : CVectorView(owner), m_attrs(attrs), m_threads(MAX_ASYNC_SYNTAXCHECK)
{
}

void CAttributeVectorView::Refresh()
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	for(IAttributeVector::iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
	{
		CComPtr<CAttributeNode> item = new CAttributeNode(m_Owner, itr->get(), true);
		item->InsertBelow(m_Tree, TVI_ROOT);
		item->operator ()(item->GetAttribute(), false, NULL, false);
	}
}

void CheckSyntax1(CComPtr<CAttributeNode> node)
{
	CheckSyntaxT<CAttributeNode>(node);
}

void CAttributeVectorView::CheckSyntax()
{
	CWaitCursor wait;
	for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextTreeItem(pos))
	{
		WTL::CTreeItem item(pos, &m_Tree);

		CComQIPtr<CAttributeNode> node = reinterpret_cast<CTreeNode *>(item.GetData());
		if (node)
		{
			node->SetSyntaxState(SYNTAX_CHECK_PENDING);
			m_threads.Append(__FUNCTION__, boost::bind(&::CheckSyntax1, node));
		}
	}
	m_Tree.Invalidate();
}
void CAttributeVectorView::DoCopyAll(const std::_tstring & delim)
{
	__super::DoCopy(m_attrs, delim);
}
//  ---------------------------------------------------------------------------
CAttributeHistoryVectorView::CAttributeHistoryVectorView(IRepositorySlot * owner, IAttributeHistoryVector & attrs) : CVectorView(owner), m_attrHistories(attrs), m_threads(MAX_ASYNC_SYNTAXCHECK)
{
}

void CAttributeHistoryVectorView::Refresh()
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	for(IAttributeHistoryVector::iterator itr = m_attrHistories.begin(); itr != m_attrHistories.end(); ++itr)
	{
		CComPtr<CAttributeHistoryNode> item = new CAttributeHistoryNode(m_Owner, itr->get(), false);
		item->InsertBelow(m_Tree, TVI_ROOT);
		item->operator ()(itr->get()->GetAttribute(), false, NULL, false);
	}
}

void CheckSyntax2(CComPtr<CAttributeHistoryNode> node)
{
	CheckSyntaxT<CAttributeHistoryNode>(node);
}

void CAttributeHistoryVectorView::CheckSyntax()
{
	CWaitCursor wait;
	for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextTreeItem(pos))
	{
		WTL::CTreeItem item(pos, &m_Tree);

		CComQIPtr<CAttributeHistoryNode> node = reinterpret_cast<CTreeNode *>(item.GetData());
		if (node)
		{
			node->SetSyntaxState(SYNTAX_CHECK_PENDING);
			m_threads.Append(__FUNCTION__, boost::bind(&::CheckSyntax2, node));
		}
	}
	m_Tree.Invalidate();
}
//  ---------------------------------------------------------------------------
CSingleAttributeHistoryVectorView::CSingleAttributeHistoryVectorView(IRepositorySlot * owner, IAttributeHistoryVector & attrs) : CVectorView(owner), m_attrHistories(attrs), m_threads(MAX_ASYNC_SYNTAXCHECK)
{
}

void CSingleAttributeHistoryVectorView::Refresh()
{
	WTL::CLockWindowUpdate lock(m_hWnd);
	m_Tree.DeleteAllItems();
	for(IAttributeHistoryVector::iterator itr = m_attrHistories.begin(); itr != m_attrHistories.end(); ++itr)
	{
		CComPtr<CAttributeHistoryNode> item = new CAttributeHistoryNode(m_Owner, itr->get(), true);
		item->InsertBelow(m_Tree, TVI_ROOT);
		item->operator ()(itr->get()->GetAttribute(), false, NULL, false);
	}
}

void CSingleAttributeHistoryVectorView::CheckSyntax()
{
	CWaitCursor wait;
	for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextTreeItem(pos))
	{
		WTL::CTreeItem item(pos, &m_Tree);

		CComQIPtr<CAttributeHistoryNode> node = reinterpret_cast<CTreeNode *>(item.GetData());
		if (node)
		{
			node->SetSyntaxState(SYNTAX_CHECK_PENDING);
			m_threads.Append(__FUNCTION__, boost::bind(&::CheckSyntax2, node));
		}
	}
	m_Tree.Invalidate();
}
//  ===========================================================================
