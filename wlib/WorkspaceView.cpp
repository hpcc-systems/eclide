#include "stdafx.h"
#include "..\en_us\resource.h"

#include "WorkspaceView.h"
#include "global.h"
#include "TreeNode.h"
#include "Util.h"
#include <utilDateTime.h> //clib
#include <RecursiveMutex.h> //clib

#define MAX_ASYNC_SYNTAXCHECK 15

//  ===========================================================================
CWorkspaceView::CWorkspaceView()
{
}

void CWorkspaceView::Init(IWorkspaceSlot *pMenuManager, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner)
{
    m_owner = pMenuManager;
    m_repOwner = pRepOwner;
    m_daliOwner = pDaliOwner;
}

CWorkspaceView::~CWorkspaceView()
{
}

BOOL CWorkspaceView::PreTranslateMessage(MSG* pMsg)
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
            RefreshProject();
            break;
        default:
            break;
        }
    }

    return IsDialogMessage(pMsg);
}

void CWorkspaceView::DoClear()
{
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    m_Root.Release();
    m_currWorkspace = NULL;
}

void CWorkspaceView::DoReset(IWorkspace * currWorkspace)
{
    DoClear();
    m_currWorkspace = currWorkspace;
    Refresh(false);
}

LRESULT CWorkspaceView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
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

void CWorkspaceView::HandleNmClick(LPNMTREEVIEW pNmHdr)
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

void CWorkspaceView::OnItemSelectionChanged(CTreeNode *node, bool bSelected )
{
    //m_Owner->OnItemSelectionChanged(node,bSelected);
}

void CWorkspaceView::OnItemExpanding(CTreeNode *node, bool bExpanding)
{
    //m_Owner->OnExpandItem(node, bExpanding);
}

LRESULT CWorkspaceView::OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //m_Owner->OnItemSelectionChanged(0,true);
    return 0;
}

LRESULT CWorkspaceView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoClear();
    return 0;
}

LRESULT CWorkspaceView::OnReset(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    DoReset((IWorkspace *)wParam);
    return 0;
}

LRESULT CWorkspaceView::OnRefresh(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    Refresh(wParam != 0);
    return 0;
}

LRESULT CWorkspaceView::OnInitialize(UINT /*uMsg*/, WPARAM /*wNoCreate*/, LPARAM /*lParam*/)
{
    Refresh(false);
    return 0;
}

void CWorkspaceView::OnPaint(HDC /*hDc*/)
{
    SetMsgHandled(false);
}

//void CWorkspaceView::OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
//{
//	Refresh();
//}

void CWorkspaceView::Refresh(bool activeOnly)
{
    if (activeOnly)
    {
        WTL::CLockWindowUpdate lock(m_hWnd);
        m_Tree.PushState();
        for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextItem(pos, TVGN_EX_ALL))
        {
            if (CComQIPtr<CWorkspaceNode> node = (CUnknown *)pos.GetData())
            {
                if (node->GetWorkspace() == GetCurrentWorkspace())
                {
                    m_Tree.Expand(pos, TVE_COLLAPSE | TVE_COLLAPSERESET);
                    //break;  Need to do all as it could be in tree twice.
                }
            }
        }
        m_Tree.PopState();
    }
    else
    {
        m_Tree.PushState();
        WTL::CLockWindowUpdate lock(m_hWnd);
        m_Tree.DeleteAllItems();
        m_Root = new CWorkspaceRootNode(m_owner, m_repOwner, m_daliOwner);
        m_Root->InsertBelow(m_Tree, TVI_ROOT);
        m_Root->Expand();
        //  GJS:  m_Tree.PopState() -> Pop is done after threaded call ends
    }
}

IAttribute * CWorkspaceView::GetFirstSelectedAttribute()
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

IModule * CWorkspaceView::GetFirstSelectedModule()
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

HWND CWorkspaceView::GetTreeHandle()
{
    return m_Tree;
}

//  IWorkspaceSlot
void CWorkspaceView::OnContextMenu(HWND phWnd, CPoint pt)
{
    SetMsgHandled(true);
    if (phWnd == (HWND)m_Tree)
    {
        CTreeNodeVector s;

        GetSelected(s);

        m_owner->OnContext(s, pt);
    }
}
//  ---------------------------------------------------------------------------
CProjectVectorView::CProjectVectorView(IWorkspaceSlot * pOwner, IRepositorySlot * pRepOwner, IWorkspaceVector & workspaces, IAttributeHistoryVector & attrs) : CVectorView(pRepOwner), m_workspaces(workspaces), m_attrHistories(attrs), m_threads(MAX_ASYNC_SYNTAXCHECK)
{
    m_pOwner = pOwner;
}

void CProjectVectorView::Refresh()
{
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    for(IWorkspaceVector::iterator itr = m_workspaces.begin(); itr != m_workspaces.end(); ++itr)
    {
        CComPtr<CWorkspaceNode> item = new CWorkspaceNode(m_pOwner, m_Owner, NULL, itr->get(), false);
        item->InsertBelow(m_Tree, TVI_ROOT);
        item->operator ()(itr->get());
    }
    for(IAttributeHistoryVector::iterator itr = m_attrHistories.begin(); itr != m_attrHistories.end(); ++itr)
    {
        CComPtr<CAttributeHistoryNode> item = new CAttributeHistoryNode(m_Owner, itr->get(), true);
        item->InsertBelow(m_Tree, TVI_ROOT);
        item->operator ()(itr->get()->GetAttribute(), false, NULL, false);
    }
}

void CheckSyntax3(CComPtr<CAttributeHistoryNode> node)
{
    CheckSyntaxT<CAttributeHistoryNode>(node);
}

void CProjectVectorView::CheckSyntax()
{
    CWaitCursor wait;
    for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextTreeItem(pos))
    {
        WTL::CTreeItem item(pos, &m_Tree);

        if (CComQIPtr<CAttributeHistoryNode> node = reinterpret_cast<CTreeNode *>(item.GetData()))
        {
            node->SetSyntaxState(SYNTAX_CHECK_PENDING);
            m_threads.Append(__FUNCTION__, boost::bind(&::CheckSyntax3, node));
        }
    }
    m_Tree.Invalidate();
}
//  ===========================================================================
