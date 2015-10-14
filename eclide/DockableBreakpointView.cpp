#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "DockableBreakpointView.h"
#include "DialogAddBreakpoint.h"

const TCHAR * const GLOBAL_LABEL = _T("Global");

class CDockableBreakpointViewMenuButton : public CMFCToolBarMenuButton
{
    friend class CDockableBreakpointView;

    DECLARE_SERIAL(CDockableBreakpointViewMenuButton)

public:
    CDockableBreakpointViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
    {
    }

    virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
        BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
    {
        pImages = CMFCToolBar::GetImages();

        CAfxDrawState ds;
        pImages->PrepareDrawImage(ds);

        CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

        pImages->EndDrawImage(ds);
    }
};

IMPLEMENT_SERIAL(CDockableBreakpointViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDockableBreakpointView::CDockableBreakpointView()
{
    m_owner = NULL;
}

CDockableBreakpointView::~CDockableBreakpointView()
{
}

void CDockableBreakpointView::SetOwner(IGraphViewSlot * owner)
{
    if (m_owner != owner)
    {
        m_owner = owner;
        if (m_owner)
        {
            m_owner->NewSelection(NULL, m_owner->GetCurrentID(), true, true);	//  Hack to re-sync the data and properties windows.
            m_wndBreakpointList.DeleteAllItems();
            m_owner->GetAllBreakpoints();
        }
    }
}

void CDockableBreakpointView::LoadBreakpoints(const roxie::CBreakpointMap & bpmap)
{
    m_wndBreakpointList.DeleteAllItems();
    int col = 0;
    for(roxie::CBreakpointMap::const_iterator itr = bpmap.begin(); itr != bpmap.end(); ++itr)
    {
        for(roxie::CBreakpointVector::const_iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
        {
            col = 0;
            int row = m_wndBreakpointList.InsertItem(col++, itr2->m_idx.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_id.size() ? itr2->m_id.c_str() : GLOBAL_LABEL);
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_action.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_fieldName.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_condition.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_value.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_caseSensitive.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_rowCountMode.c_str());
            m_wndBreakpointList.SetItemText(row, col++, itr2->m_rowCount.c_str());
        }
    }
    for (int i = 0; i < col; ++i)
        m_wndBreakpointList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
}

void CDockableBreakpointView::Send_Clear()
{
    m_wndBreakpointList.DeleteAllItems();
}

void CDockableBreakpointView::Post_Reset()
{
    Send_Clear();
}

BEGIN_MESSAGE_MAP(CDockableBreakpointView, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    //ON_COMMAND(ID_DEBUGGER_SETBREAKPOINT, OnButtonSetBreakpoint)
    //ON_UPDATE_COMMAND_UI(ID_DEBUGGER_SETBREAKPOINT, OnUpdateButtonSetBreakpoint)
    //ON_COMMAND(ID_DEBUGGER_REMOVEBREAKPOINT, OnButtonRemoveBreakpoint)
    //ON_UPDATE_COMMAND_UI(ID_DEBUGGER_REMOVEBREAKPOINT, OnUpdateButtonRemoveBreakpoint)
    ON_WM_PAINT()
    ON_WM_SETFOCUS()
    ON_COMMAND(ID_SORTING_DELETE, OnDelete)
    ON_UPDATE_COMMAND_UI(ID_SORTING_DELETE, OnUpdateDelete)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SYNTAX, &CDockableBreakpointView::OnLvnItemchangedListResults)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SYNTAX, OnNmDblClk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDockableBreakpointView message handlers

int CDockableBreakpointView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create views:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_SORTASCENDING | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    if (!m_wndBreakpointList.Create(dwViewStyle, rectDummy, this, IDC_LIST_SYNTAX))
    {
        TRACE0("Failed to create Class View\n");
        return -1;      // fail to create
    }
    m_wndBreakpointList.SetExtendedStyle(m_wndBreakpointList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    m_wndBreakpointList.EnableMultipleSort(false);
    int col = 0;
    m_wndBreakpointList.InsertColumn(col++, _T("##"));
    m_wndBreakpointList.InsertColumn(col++, _T("Edge ID"));
    m_wndBreakpointList.InsertColumn(col++, _T("Mode"));
    m_wndBreakpointList.InsertColumn(col++, _T("Field"));
    m_wndBreakpointList.InsertColumn(col++, _T("Condition"));
    m_wndBreakpointList.InsertColumn(col++, _T("Value"));
    m_wndBreakpointList.InsertColumn(col++, _T("Case Sensitive"));
    m_wndBreakpointList.InsertColumn(col++, _T("Row Count Mode"));
    m_wndBreakpointList.InsertColumn(col++, _T("Row Count"));
    for (int i = 0; i < col; ++i)
        m_wndBreakpointList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

    // Load images:
    m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_BREAKPOINTS);
    m_wndToolBar.LoadToolBar(IDR_BREAKPOINTS, 0, 0, TRUE /* Is locked */);

    OnChangeVisualStyle();

    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

    m_wndToolBar.SetOwner(this);

    // All commands will be routed via this control , not via the parent frame:
    m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

    //CMenu menuSort;
    //menuSort.LoadMenu(IDR_POPUP_SORT);
    //m_wndToolBar.ReplaceButton(ID_SORT_MENU, CDockableBreakpointViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));
    //CDockableBreakpointViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CDockableBreakpointViewMenuButton, m_wndToolBar.GetButton(0));

    //if (pButton != NULL)
    //{
    //	pButton->m_bText = FALSE;
    //	pButton->m_bImage = TRUE;
    //	pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
    //	pButton->SetMessageWnd(this);
    //}

    // Fill in some static tree view data (dummy code, nothing magic here)
    return 0;
}

void CDockableBreakpointView::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

void CDockableBreakpointView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMFCListCtrl* pWndTree = (CMFCListCtrl*)&m_wndBreakpointList;
    ASSERT_VALID(pWndTree);

    if (pWnd != pWndTree)
    {
        CDockablePane::OnContextMenu(pWnd, point);
        return;
    }

    if (point != CPoint(-1, -1))
    {
        // Select clicked item:
        CPoint ptTree = point;
        pWndTree->ScreenToClient(&ptTree);

        UINT flags = 0;
        int hTreeItem = pWndTree->HitTest(ptTree, &flags);
        if (hTreeItem != NULL)
        {
            pWndTree->SetItemState(hTreeItem, true, LVIS_SELECTED);
        }
    }

    pWndTree->SetFocus();
    CMenu menu;
    menu.LoadMenu(IDR_POPUP_SORT);

    CMenu* pSumMenu = menu.GetSubMenu(0);

    if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
    {
        CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

        if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
            return;

        ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
        UpdateDialogControls(this, FALSE);
    }
}

void CDockableBreakpointView::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

    m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndBreakpointList.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CDockableBreakpointView::PreTranslateMessage(MSG* pMsg)
{
    return CDockablePane::PreTranslateMessage(pMsg);
}

void CDockableBreakpointView::OnButtonSetBreakpoint()
{
}

void CDockableBreakpointView::OnUpdateButtonSetBreakpoint(CCmdUI* pCmdUI)
{
}

void CDockableBreakpointView::OnButtonRemoveBreakpoint()
{
}

void CDockableBreakpointView::OnUpdateButtonRemoveBreakpoint(CCmdUI* pCmdUI)
{
}

void CDockableBreakpointView::OnDelete()
{
    int sel = m_wndBreakpointList.GetSelectionMark();
    if (m_owner && sel >= 0)
        m_owner->DeleteBreakpoint(m_wndBreakpointList.GetItemText(sel, 0));
}

void CDockableBreakpointView::OnUpdateDelete(CCmdUI* pCmdUI)
{
    if (m_wndBreakpointList.GetSelectionMark() >= 0)
        pCmdUI->Enable();
}

void CDockableBreakpointView::OnClassAddMemberFunction()
{
    AfxMessageBox(_T("Add member function..."));
}

void CDockableBreakpointView::OnClassAddMemberVariable()
{
    // TODO: Add your command handler code here
}

void CDockableBreakpointView::OnClassDefinition()
{
    // TODO: Add your command handler code here
}

void CDockableBreakpointView::OnClassProperties()
{
    // TODO: Add your command handler code here
}

void CDockableBreakpointView::OnNewFolder()
{
    AfxMessageBox(_T("New Folder..."));
}

void CDockableBreakpointView::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect rectTree;
    m_wndBreakpointList.GetWindowRect(rectTree);
    ScreenToClient(rectTree);

    rectTree.InflateRect(1, 1);
    dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CDockableBreakpointView::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);

    m_wndBreakpointList.SetFocus();
}

void CDockableBreakpointView::OnChangeVisualStyle()
{
    m_BreakpointListImages.DeleteImageList();

    UINT uiBmpId = IDB_CLASS_VIEW_24;

    CBitmap bmp;
    if (!bmp.LoadBitmap(uiBmpId))
    {
        TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
        ASSERT(FALSE);
        return;
    }

    BITMAP bmpObj;
    bmp.GetBitmap(&bmpObj);

    UINT nFlags = ILC_MASK;

    nFlags |= ILC_COLOR24;

    m_BreakpointListImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
    m_BreakpointListImages.Add(&bmp, RGB(255, 0, 0));

    m_wndBreakpointList.SetImageList(&m_BreakpointListImages, TVSIL_NORMAL);

    m_wndToolBar.CleanUpLockedImages();
    m_wndToolBar.LoadBitmap(IDB_SORT_24, 0, 0, TRUE /* Locked */);
}

void CDockableBreakpointView::OnLvnItemchangedListResults(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
    {
        CString edge_id = m_wndBreakpointList.GetItemText(pNMLV->iItem, 1);
        if (edge_id.Compare(GLOBAL_LABEL))
        {
            CUniqueID id(guidDefault, XGMML_CAT_EDGE, (const TCHAR *)edge_id);
            m_owner->NewSelection(NULL, id, true, false);
        }
    }
    *pResult = 0;
}

void CDockableBreakpointView::OnNmDblClk(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    CDialogAddBreakpoint dlg;
    dlg.m_stringMode = m_wndBreakpointList.GetItemText(pNMIA->iItem, 2);
    dlg.m_stringConditionMode = m_wndBreakpointList.GetItemText(pNMIA->iItem, 4);
    if (dlg.m_stringConditionMode.empty())
        dlg.m_stringConditionMode = _T("Always");
    dlg.m_stringField = m_wndBreakpointList.GetItemText(pNMIA->iItem, 3);
    dlg.m_stringCondition = m_wndBreakpointList.GetItemText(pNMIA->iItem, 5);
    dlg.m_stringCountMode = m_wndBreakpointList.GetItemText(pNMIA->iItem, 7);
    if (dlg.m_stringCountMode.empty())
        dlg.m_stringCountMode = _T("Always");
    dlg.m_stringCount = m_wndBreakpointList.GetItemText(pNMIA->iItem, 8);
    dlg.m_caseSensitive = m_wndBreakpointList.GetItemText(pNMIA->iItem, 6).Compare(_T("true")) == 0;

    if (dlg.DoModal() == IDOK)
    {
        std::_tstring id = m_wndBreakpointList.GetItemText(pNMIA->iItem, 0);
        std::_tstring edgeID = m_wndBreakpointList.GetItemText(pNMIA->iItem, 1).Compare(_T("Global")) == 0 ? _T("") : m_wndBreakpointList.GetItemText(pNMIA->iItem, 1);
        m_owner->GetDebugSession()->DeleteBreakpoint(id, false);
        m_owner->GetDebugSession()->AddBreakpoint(edgeID, dlg.m_stringMode, dlg.m_stringConditionMode, dlg.m_stringField, dlg.m_stringCondition, dlg.m_stringCountMode, dlg.m_stringCount, dlg.m_caseSensitive != 0);
    }
    *pResult = 0;
}
