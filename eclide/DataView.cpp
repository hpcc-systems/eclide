#include "StdAfx.h"

#include "DataView.h"

//  ===========================================================================
CDataView::CDataView() //: m_dlgview(NULL, NULL)
{
}

CDataView::~CDataView()
{
}

void CDataView::SetData(const ITable * data, bool hasRowNum, const roxie::CSearchRowVector & searchResults)
{
    m_data = data == NULL ? NULL : const_cast<ITable *>(data);
    m_grid.SetData(m_data, hasRowNum);
    SetTabSelection(searchResults);
}

void CDataView::SetTabSelection(const roxie::CSearchRowVector & searchResults)
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

BOOL CDataView::PreTranslateMessage(MSG* pMsg)
{
    //if (m_tabbedChildWindow.PreTranslateMessage(pMsg))
    //	return TRUE;

    return CWnd::PreTranslateMessage(pMsg);
}

IMPLEMENT_DYNCREATE(CDataView, CWnd)

BEGIN_MESSAGE_MAP(CDataView, CWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CDataView::PreCreateWindow(CREATESTRUCT& cs) 
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    //cs.dwExStyle |= WS_EX_CLIENTEDGE;
    //cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

    return TRUE;
}

//void CDataView::SetWU(Dali::IWorkunit * wu)
//{
//	m_wu = wu;
//}
//
//bool CDataView::WorkunitDeleted(Dali::IWorkunit * wu)
//{
//	m_wu = NULL;
//	return true;
//}
//
int CDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

BOOL CDataView::OnEraseBkgnd(CDC* dc)
{
    return 1;
}

void CDataView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    CWnd::OnSize(nType, cx, cy);

    CRect rectClient;
    GetClientRect(rectClient);
    m_grid.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}
//  ===========================================================================
