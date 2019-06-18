#include "StdAfx.h"
#include "resource.h"

#include "GraphViewCanvas.h"
#include "AggDC.h"
#include "OverviewWnd.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

COverviewWnd::COverviewWnd() 
    : m_or(0),
    m_canvas(0),
    m_layered(false),
    m_opacity(255)
{
    OSVERSIONINFO osv;
    memset(&osv, 0, sizeof osv);
    osv.dwOSVersionInfoSize=sizeof(osv);
    GetVersionEx(&osv);
    if(osv.dwMajorVersion>=5)
        m_layered=true;					
}

COverviewWnd::~COverviewWnd()
{
}

//-----------------------------------------------------------------------------

void COverviewWnd::Init(CGraphViewCanvas* canvas, IGraph* graph)
{
    m_canvas=canvas;
    m_or=CreateOverviewRenderer(*(m_canvas->GetCanvas().get()), graph);
    ATLASSERT(m_or);
    m_or->SetInWindow(true); 
    //if(m_canvas->GetOverview()) // init stage may be too early to get visibility
    //	ShowWindow(SW_SHOW);
}

//-----------------------------------------------------------------------------

BYTE COverviewWnd::SetOverviewOpacity(BYTE opacity)
{
    BYTE old=m_opacity;
    if(opacity!=m_opacity)
    {
        m_opacity=opacity;
        SetLayeredWindowAttributes(0, m_opacity, LWA_ALPHA);
    }
    return old;
}

//-----------------------------------------------------------------------------

void COverviewWnd::SetOverviewPosition(const CPoint& pos)
{
    CPoint spt(pos);
    m_canvas->ClientToScreen(&spt);
    SetWindowPos(0, spt.x, spt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

//-----------------------------------------------------------------------------

void COverviewWnd::SetOverviewSize(const CSize& size)
{
    CRect rc(0,0,size.cx,size.cy);
    ::AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
    SetWindowPos(0, 0, 0, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_NOMOVE);
}

//-----------------------------------------------------------------------------

void COverviewWnd::Refresh()
{
    Invalidate();
}

//-----------------------------------------------------------------------------
//
//	Messages
//

void COverviewWnd::OnSize(UINT wParam, const CSize & size)
{
    RectF orgrc;
    if(m_or)
    {
        orgrc.Width=static_cast<REAL>(size.cx);
        orgrc.Height=static_cast<REAL>(size.cy);
        m_or->SetBoundsBox(orgrc,SWP_NOMOVE);
    }
}

//-----------------------------------------------------------------------------

void COverviewWnd::OnClose()
{ 
    m_canvas->ShowOverview(false);
}

//-----------------------------------------------------------------------------

int COverviewWnd::OnNcHitTest(CPoint pt)
{
    LRESULT lRes=DefWindowProc();
    if(lRes==HTCLIENT)
    {
        ScreenToClient(&pt);
        if(!m_or->CursorInViewport(pt))
            lRes=HTNOWHERE;
    }
    return lRes;
}

//-----------------------------------------------------------------------------

bool COverviewWnd::OnSetCursor(HWND hWnd, UINT htcode, UINT uMsg)
{
    if(htcode==HTCLIENT)
    {
        bool mdown=(GetAsyncKeyState(VK_LBUTTON) & 0x8000)!=0;
        LPTSTR cursortype=MAKEINTRESOURCE(mdown?IDC_HAND_CLOSED:IDC_HAND_OPEN);
        SetCursor(LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(),cursortype));
        return true;
    }
    SetMsgHandled(false);
    return false;
}

//-----------------------------------------------------------------------------

void COverviewWnd::OnLButtonDown(UINT code, CPoint pt)
{
    if(m_or->CursorInViewport(pt))
    {
        SetCapture();
        m_capturePos=pt;
        m_savePos=m_canvas->GetCanvas().get()->GetOffset();
    }
}

//-----------------------------------------------------------------------------

void COverviewWnd::OnLButtonUp(UINT code, CPoint pt)
{
    if(GetCapture()==*this)
    {
        ReleaseCapture();
    }
}

//-----------------------------------------------------------------------------

void COverviewWnd::OnMouseMove(UINT code, CPoint pt)
{
    if(GetCapture()==*this)
    {
        PointF offset(pt.x-m_capturePos.x,pt.y-m_capturePos.y);
        m_or->RSizeTransform(offset);
        m_canvas->GetCanvas().get()->SizeTransform(offset);

        offset.x+=m_savePos.x;
        offset.y+=m_savePos.y;
        m_canvas->GetCanvas().get()->SetOffset(offset);
        m_canvas->Invalidate();
    }
}

//-----------------------------------------------------------------------------

LRESULT COverviewWnd::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CPaintDC dc(m_hWnd);
    CRect rcclient;
    GetClientRect(&rcclient);

    boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateGDIDC(dc, m_canvas->GetCanvas().get()->m_fonts, &rcclient));
    m_or->SendMessage(WM_PAINT, (WPARAM)aggdc.get(), 0);
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT COverviewWnd::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 1;
}
