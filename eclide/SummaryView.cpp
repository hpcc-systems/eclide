#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "SummaryView.h"
#include "EclCC.h"

CSummaryView::CSummaryView(IResultSlot *resultSlot)
{
    m_resultSlot = resultSlot;
}

CSummaryView::~CSummaryView()
{
}

void CSummaryView::Unhide()
{
    if ( !m_view.IsLoaded() )
    {
        m_view.CreateHTMLControl();
        //either one of these should work
        //TODO:  Gordon to retest
        m_view.put_ProfferService(IID_IAuthenticate, GetIServiceProvider());
        m_view.put_ServiceProvider(GetIServiceProvider());
        PostMessage(CWM_INITIALIZE);
    }
}

LRESULT CSummaryView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    m_view.Create( *this, rcDefault, _T(""), WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE );
    return 0;
}

LRESULT CSummaryView::OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    Refresh();
    return 0;
}

LRESULT CSummaryView::OnPaint(HDC /*dc*/)
{
    //this little trick is used to keep from loading the
    //browser control when the tab is hidden
    SetMsgHandled(FALSE);
    Unhide();
    return 1;
}

void CSummaryView::OnSize(UINT wParam, const CSize & size)
{
    if(wParam != SIZE_MINIMIZED)
    {
        UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
        m_view.SetWindowPos(NULL, 0, 0, size.cx, size.cy, flags);
    }
}

LRESULT CSummaryView::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    LPMSG pMsg = (LPMSG)lParam;
    //return 0;
    return m_view.PreTranslateMessage(pMsg);
}

void CSummaryView::DoRefresh()
{
    //No refreshing of ECL Watch on Polling  
}

LRESULT CSummaryView::OnRefresh(UINT /*uMsg*/, WPARAM /*bCreated*/, LPARAM /*lParam*/)
{
    DoRefresh();
    return 0;
}

LRESULT CSummaryView::OnBrowserNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DWORD_PTR dwRet = (DWORD_PTR)::ShellExecute(0, _T("open"), m_FramedUrl, 0, 0, SW_SHOWNORMAL);
    bool bRet = (dwRet > 32);
    ATLASSERT(bRet);
    return 0;
}

LRESULT CSummaryView::OnBrowserIE(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString password(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD));
    CString user(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER));
    m_view.Navigate(m_FramedUrl, user, password, true);
    return 0;
}

LRESULT CSummaryView::OnBrowserBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CWaitCursor cursor;
    m_view.BrowserBack();
    return 0;
}

LRESULT CSummaryView::OnBrowserForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CWaitCursor cursor;
    m_view.BrowserForward();
    return 0;
}

LRESULT CSummaryView::OnBrowserStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CWaitCursor cursor;
    m_view.BrowserStop();
    return 0;
}

LRESULT CSummaryView::OnBrowserRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (m_view.IsLoaded())
    {
        m_view.BrowserRefresh();
    }
    return 0;
}

void CSummaryView::OnEditCopy(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    CWaitCursor cursor;
    m_view.BrowserCmdCopy();
}

void CSummaryView::OnEditSelectAll(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    CWaitCursor cursor;
    m_view.BrowserCmdSelectAll();
}

void CSummaryView::OnContextMenu(HWND /*hWnd*/, CPoint pt)
{
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_RESULTS_SUMMARY);
    CResultContext context(this, m,pt);
    if ( !m_resultSlot || !m_resultSlot->OnContextResult(context) )
    {
        //if nobody handled it, then do it ourselves
        BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
        if (id)
            PostMessage(WM_COMMAND, id);
    }
}

//CTabPane
void CSummaryView::Refresh()
{
}

HWND CSummaryView::GetHWND()
{
    return m_hWnd;
}

bool CSummaryView::CanCopy()
{
    return true;
}
bool CSummaryView::HasResults() const
{
    return false;
}

bool CSummaryView::UpdateUI(CCmdUI * cui)
{
    if ( m_view.IsLoaded() )
    {
        UPDATEUI(cui, ID_BROWSER_NEW, true);
        UPDATEUI(cui, ID_BROWSER_IE, true);
        UPDATEUI(cui, ID_BROWSER_STOP, true);
        UPDATEUI(cui, ID_BROWSER_BACK, m_view.BackEnabled());
        UPDATEUI(cui, ID_BROWSER_FORWARD, m_view.ForwardEnabled());
        UPDATEUI(cui, ID_BROWSER_REFRESH, true);
    }
    return false;
}

//ISearchable
bool CSummaryView::CanBeSearched() const 
{
    return HasResults();
}

bool CSummaryView::Search(const CSearchableInfo & /*info*/, CSearchableResult &/*result*/) const
{
    return false;
}
//  --  IResultView  ---
HWND CSummaryView::GetHwnd() const 
{
    return m_hWnd;
}

// --  CWUSummaryView ---
CWUSummaryView::CWUSummaryView(Dali::IWorkunit * wu, IResultSlot *resultSlot) : CSummaryView(resultSlot), m_wu(wu)
{
    GetWorkUnitEclWatchURL(m_wu, m_Url);
    GetFramedWorkUnitEclWatchURL(m_wu, m_FramedUrl);
}

LRESULT CWUSummaryView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (boost::algorithm::istarts_with(m_wu->GetWuid(), _T("W")))
        SetWindowText(_T("ECL Watch"));
    else
        SetWindowText(_T("Workunit XML"));
    return CSummaryView::OnCreate(uMsg, wParam, lParam, bHandled);
}

void CWUSummaryView::Refresh()
{
    CSummaryView::Refresh();
    if (m_view.IsLoaded())
    {
        if (IsRemoteDaliEnabled() == TRI_BOOL_TRUE && boost::algorithm::istarts_with(m_wu->GetWuid(), _T("W")))
        {
            CString password(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD));
            CString user(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER));
            m_view.Navigate(m_Url, user, password, false);
        }
        else if (CComPtr<IEclCC> eclcc = CreateIEclCC())
        {
            std::_tstring xmlPath;
            eclcc->SaveWorkunitXML(m_wu->GetWuid(), xmlPath);
            m_view.Navigate(xmlPath.c_str());
        }
    }
}

bool CWUSummaryView::CanCopy()
{
    return true;
}

// --  CDesdlSummaryView  ---
CDesdlSummaryView::CDesdlSummaryView(const std::_tstring & desdlID, IResultSlot *resultSlot) : CSummaryView(resultSlot), m_desdlID(desdlID)
{
    GetDesdlEclWatchURL(m_desdlID, m_Url);
    GetFramedDesdlEclWatchURL(m_desdlID, m_FramedUrl);
}


LRESULT CDesdlSummaryView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    SetWindowText(_T("ECL Watch"));
    return CSummaryView::OnCreate(uMsg, wParam, lParam, bHandled);
}

void CDesdlSummaryView::Refresh()
{
    CSummaryView::Refresh();
    if (m_view.IsLoaded())
    {
        if (IsRemoteDaliEnabled() == TRI_BOOL_TRUE)
        {
            CString password(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD));
            CString user(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER));
            m_view.Navigate(m_Url, user, password, false);
        }
    }
}

bool CDesdlSummaryView::CanCopy()
{
    return false;
}
