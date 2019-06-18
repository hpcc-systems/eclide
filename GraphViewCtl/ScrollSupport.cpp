#include "stdafx.h"

#include "AggDC.h"
#include "ScrollSupport.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

CScrollSupport::CScrollSupport(CCanvas* canvas, CScaledWindow* parent/*=0*/)
:	m_pcanvas(canvas),
    m_vSB(canvas, parent),
    m_hSB(canvas, parent, false),
    m_rcClient(0,0,0,0),
    m_rcWorld(0,0,0,0)
{
}

CScrollSupport::~CScrollSupport()
{
}
//-----------------------------------------------------------------------------

bool CScrollSupport::DispatchToScrollbars(
    UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    bool handled=false;
    PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

    if(m_vSB.IsVisible() && m_vSB.HitTest(p))
    {
        lResult=m_vSB.SendMessage(uMsg, wParam, lParam);
        handled=true;
    }

    if(!handled && m_hSB.IsVisible() && m_hSB.HitTest(p))
    {
        lResult=m_hSB.SendMessage(uMsg, wParam, lParam);
        handled=true;
    }

    return handled;
}

//-----------------------------------------------------------------------------

void CScrollSupport::UpdateScrollbars(const CRect& rcextent)
{
    int whScrollbar=GetSystemMetrics(SM_CYHSCROLL);

    CRect rcunion(m_rcClient);
    CRect rcvp(m_rcClient);
    CRect rcintersect;

    if(!rcextent.IsRectEmpty())
    {
        if(rcintersect.IntersectRect(&rcextent, &rcvp)!=0)	
        {
            if(rcintersect==rcextent) // everything inside, no scrollbars
            {
            }
            else
            {
                rcunion.UnionRect(&rcextent, &rcvp);
            }
        }
        else // entire graph extent is outside the viewport
        {
            rcunion.UnionRect(&rcextent, &rcvp);
        }
    }

    bool bHSB=rcvp.Width()<rcunion.Width();
    bool bVSB=rcvp.Height()<rcunion.Height();
    if(bHSB)
        rcvp.DeflateRect(0,0,0,whScrollbar);
    if(bVSB)
        rcvp.DeflateRect(0,0,whScrollbar, 0);

    // now with viewport excluding scrollbars calc scroll extent again

    if(bHSB || bVSB)
        rcunion.UnionRect(&rcextent, &rcvp);

    m_rcWorld=rcunion;

    // +1 to prevent displaying scrollbars when page equals extent
    CSize page(rcvp.Width()+1,rcvp.Height()+1);  


    SCROLLINFO si;
    si.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;

    m_hSB.GetScrollInfo(&si);

    si.nMin=0;
    si.nMax=m_rcWorld.Width();
    si.nPage=page.cx;
    si.nPos=0;
    if(m_rcWorld.left < rcvp.left)
        si.nPos=rcvp.left-m_rcWorld.left;

    //ATLASSERT(si.nPos>=0 && si.nPos<=(si.nMax-page.cx+1));

    m_hSB.SetScrollInfo(&si, false);

    m_vSB.GetScrollInfo(&si);
    si.nMin=0;
    si.nMax=m_rcWorld.Height();
    si.nPage=page.cy;
    si.nPos=0;
    if(m_rcWorld.top < rcvp.top)
        si.nPos=rcvp.top-m_rcWorld.top;

    //ATLASSERT(si.nPos>=0 && si.nPos<=(si.nMax-page.cy+1));

    m_vSB.SetScrollInfo(&si, false);

    CRect vrcsb=m_rcClient;
    vrcsb.left=vrcsb.right-whScrollbar;
    CRect hrcsb=m_rcClient;
    hrcsb.top=hrcsb.bottom-whScrollbar;

    if(m_hSB.IsVisible())
        vrcsb.bottom-=whScrollbar;
    m_vSB.SetWindowPos(vrcsb, SWP_NOREDRAW);

    if(m_vSB.IsVisible())
        hrcsb.right-=whScrollbar;
    m_hSB.SetWindowPos(hrcsb, SWP_NOREDRAW);
}

//-----------------------------------------------------------------------------

void CScrollSupport::PaintScrollbars(WPARAM wParam)
{
    CAggDC& dc=*reinterpret_cast<CAggDC*>(wParam);
    if(m_vSB.IsVisible())
    {
        m_vSB.SendMessage(WM_PAINT, wParam, 0);
    }
    if(m_hSB.IsVisible())
    {
        m_hSB.SendMessage(WM_PAINT, wParam, 0);
    }

    if(m_vSB.IsVisible() && m_hSB.IsVisible())
    {
        int whScrollbar=GetSystemMetrics(SM_CYHSCROLL);
        CRect rc(m_rcClient);
        rc.top=rc.bottom-whScrollbar;
        rc.left=rc.right-whScrollbar;
        rc.bottom++;
        rc.right++;
        dc.FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));
    }
}


//-----------------------------------------------------------------------------

void CScrollSupport::SetScrollRect(const CRect& wndrc)
{
    m_rcClient=wndrc;
    ATLASSERT(m_rcClient.left==0 && m_rcClient.top==0);
}

//-----------------------------------------------------------------------------

void CScrollSupport::ModifyWindowRect(CRect& rc)
{
    int whScrollbar=GetSystemMetrics(SM_CYHSCROLL);

    if(m_vSB.IsVisible())
        rc.right-=whScrollbar;	
    if(m_hSB.IsVisible())
        rc.bottom-=whScrollbar;	
}

//-----------------------------------------------------------------------------
//	Message handlers
//-----------------------------------------------------------------------------

LRESULT CScrollSupport::OnVScroll(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    int code=(int)LOWORD(wParam);
    REAL change=0;

    SCROLLINFO si;

    si.fMask=SIF_RANGE|SIF_POS|SIF_PAGE;
    m_vSB.GetScrollInfo(&si);
    int maxpos=si.nMax-si.nPage+1;

    switch(code)
    {
    case SB_LINEDOWN:
        if(si.nPos<maxpos)
        {
            change=static_cast<REAL>(si.nPage/10);
        }
        break;

    case SB_LINEUP:
        if(si.nPos>si.nMin)
        {
            change=-static_cast<REAL>(si.nPage/10);
        }
        break;

    case SB_PAGEDOWN:
        change=static_cast<REAL>(si.nPage);
        break;

    case SB_PAGEUP:
        change=-static_cast<REAL>(si.nPage);
        break;

    case SB_TOP:
    case SB_BOTTOM:
    case SB_THUMBPOSITION:
    case SB_ENDSCROLL:
        break;

    case SB_THUMBTRACK:
        change=static_cast<REAL>(lParam);
        break;
    }

    if(abs(change)>maxpos)
        change=static_cast<REAL>(change>0?maxpos:-maxpos);

    if(change!=0)
    {
        PointF offset=m_pcanvas->GetOffset();
        offset.y+=change;
        m_pcanvas->SetOffset(offset);
        CPoint pt(0,round_int(-change));
        m_pcanvas->Scroll(pt);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScrollSupport::OnHScroll(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    int code=(int)LOWORD(wParam);
    REAL change=0;

    SCROLLINFO si;

    si.fMask=SIF_RANGE|SIF_POS|SIF_PAGE;
    m_hSB.GetScrollInfo(&si);
    int maxpos=si.nMax-si.nPage+1;

    switch(code)
    {
    case SB_LINEDOWN:
        if(si.nPos<maxpos)
        {
            change=static_cast<REAL>(si.nPage/10);
        }
        break;

    case SB_LINEUP:
        if(si.nPos>si.nMin)
        {
            change=-static_cast<REAL>(si.nPage/10);
        }
        break;

    case SB_PAGEDOWN:
        change=static_cast<REAL>(si.nPage);
        break;

    case SB_PAGEUP:
        change=-static_cast<REAL>(si.nPage);
        break;

    case SB_TOP:
    case SB_BOTTOM:
    case SB_THUMBPOSITION:
    case SB_ENDSCROLL:
        break;

    case SB_THUMBTRACK:
        change=static_cast<REAL>(lParam);
        break;
    }

    if(abs(change)>maxpos)
        change=static_cast<REAL>(change>0?maxpos:-maxpos);

    if(change!=0)
    {
        PointF offset=m_pcanvas->GetOffset();
        offset.x+=change;
        m_pcanvas->SetOffset(offset);
        CPoint pt(round_int(-change),0);
        m_pcanvas->Scroll(pt);
    }

    return 0;
}
