#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "PropertyGridView.h"

//  ===========================================================================
CMyTableGridCtrl::CMyTableGridCtrl()
{
}

CMyTableGridCtrl::CMyTableGridCtrl(XGMML_CAT cat) : m_cat(cat)
{
}

void CMyTableGridCtrl::SetOwner(IGraphViewSlot * owner)
{
    if (m_owner != owner)
    {
        m_owner = owner;
    }
}

BOOL CMyTableGridCtrl::CallbackCellClick(SGRID::GV_CELL *pCell)
{
    if (pCell->cell.row > 0)
    {
        if (m_owner)
        {
            CUniqueID newSel(guidDefault, m_cat, m_data->GetCell(pCell->cell.row - 1, 0));
            m_owner->NewSelection(NULL, newSel, true, false, true);
        }
    }
    return CTableGridCtrl::CallbackCellClick(pCell);
}
//  ===========================================================================
CPropertyGridView::CPropertyGridView()
{
}

CPropertyGridView::CPropertyGridView(XGMML_CAT cat) : m_grid(cat)
{
}

CPropertyGridView::~CPropertyGridView()
{
}

void CPropertyGridView::SetOwner(IGraphViewSlot * owner)
{
    if (m_owner != owner)
    {
        m_owner = owner;
        m_grid.SetOwner(owner);
    }
}

void CPropertyGridView::SetData(const ITable * data)
{
    m_data = data == NULL ? NULL : const_cast<ITable *>(data);
    m_data->SortAgain();
    m_grid.SetData(m_data, false);
    //SetTabSelection(searchResults);
}

void CPropertyGridView::SetTabSelection(const roxie::CSearchRowVector & searchResults)
{
    m_grid.SelectNone();
    if (searchResults.empty())
        return;

    int offset = 0;
    try
    {
        offset = boost::lexical_cast<int>(m_data->GetCell(0,0));
    }
    catch(boost::bad_lexical_cast &)
    {
        ATLASSERT(false);
        return;
    }
    for(roxie::CSearchRowVector::const_iterator itr = searchResults.begin(); itr != searchResults.end(); ++itr)
    {
        m_grid.SelectRow(itr->m_count - offset, false);
    }
}

BOOL CPropertyGridView::PreTranslateMessage(MSG* pMsg)
{
    //if (m_tabbedChildWindow.PreTranslateMessage(pMsg))
    //	return TRUE;

    return CWnd::PreTranslateMessage(pMsg);
}

IMPLEMENT_DYNCREATE(CPropertyGridView, CWnd)

BEGIN_MESSAGE_MAP(CPropertyGridView, CWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


BOOL CPropertyGridView::PreCreateWindow(CREATESTRUCT& cs) 
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    //cs.dwExStyle |= WS_EX_CLIENTEDGE;
    //cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

    return TRUE;
}

//void CPropertyGridView::SetWU(Dali::IWorkunit * wu)
//{
//	m_wu = wu;
//}
//
//bool CPropertyGridView::WorkunitDeleted(Dali::IWorkunit * wu)
//{
//	m_wu = NULL;
//	return true;
//}
//
int CPropertyGridView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_grid.Create(m_hWnd, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE | WS_TABSTOP | LVS_OWNERDATA | LVS_SHOWSELALWAYS | LVS_REPORT, WS_EX_CLIENTEDGE);
    m_grid.SetCallbackFunc(m_grid.GridCallback, (LPARAM)&m_grid);
    m_grid.SetCallbackFuncPrepCache(m_grid.GridCallbackPrepCache, (LPARAM)&m_grid);
    m_grid.SetCallbackFuncCellClick(m_grid.GridCallbackCellClick, (LPARAM)&m_grid);
    m_grid.SetFixedRowCount(1);
    m_grid.SetFixedColumnCount(1);
    //m_grid.SetTrim(true);

    return 0;
}

BOOL CPropertyGridView::OnEraseBkgnd(CDC* dc)
{
    return 1;
}

void CPropertyGridView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    CWnd::OnSize(nType, cx, cy);

    CRect rectClient;
    GetClientRect(rectClient);
    m_grid.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertyGridView::OnLButtonUp(UINT /*hWnd*/, CPoint pt)
{
    POINT pointList = pt;
    ::MapWindowPoints(0, m_grid.m_hWnd, &pointList, 1);
    int col = -1;
    __int64 row = m_grid.HitTestForColumn(pointList, &col);
    int d = 0;
}
//  ===========================================================================
