#include "StdAfx.h"

#include "AggDC.h"
#include "ScaledScrollBar.h"

#define __POSITION_DIFFERENCE__

CBrushHandle CScaledScrollBar::m_lightTrackBrush;

//-----------------------------------------------------------------------------

CScaledScrollBar::CScaledScrollBar(
    CCanvas* canvas,
    CScaledWindow* parent,
    bool bVert)
    : m_Theme(canvas->GetPartTheme(_T("SCROLLBAR"))),
    m_stateULAB(canvas, this),
    m_stateDRAB(canvas, this),
    m_stateULTrack(canvas, this),
    m_stateDRTrack(canvas, this),
    m_stateThumb(canvas, this),
    m_bVisible(false),
    m_bVertical(bVert),
    m_bDragging(false),
    m_capturedPart(HT_UNKNOWN),
    CScaledWindow(canvas, parent),
    m_TimerCode(0)
{
    si.nMin=si.nMax=0;
    si.nPage=0;
    si.nPos=0;
    si.nTrackPos=0;
    si.fMask=bVert?SB_VERT:SB_HORZ;
    if(m_lightTrackBrush.IsNull())
        m_lightTrackBrush=CDCHandle::GetHalftoneBrush();
}

//-----------------------------------------------------------------------------

CScaledScrollBar::~CScaledScrollBar()
{
}

//-----------------------------------------------------------------------------

void CScaledScrollBar::SetWindowPos(const CRect& pos, UINT flags)
{
    if(flags&SWP_NOSIZE)
    {
        // calling us with no move and no size makes no sense but,
        // for now we have CalcDisplaySizes invoked before each paint
        // and it invokes us, so return for now
        if(flags&SWP_NOMOVE)
            return;

        ATLASSERT(!(flags&SWP_NOMOVE));

        CPoint offset=pos.TopLeft()-m_rcWnd.TopLeft();
        m_rcWnd.OffsetRect(offset);

        m_rcClient.OffsetRect(offset);
        m_rcULAB.OffsetRect(offset);
        m_rcDRAB.OffsetRect(offset);

        CalcWorkVars();
    }
    else
    {
        if((flags&SWP_NOMOVE)!=0 && m_bVisible)
        {
            m_rcWnd.right=m_rcWnd.left+pos.Width();
            m_rcWnd.bottom=m_rcWnd.top+pos.Height();			
        }
        else
        {
            m_rcWnd=pos;
        }

        m_rcClient=m_rcWnd;
        m_rcULAB=m_rcClient;
        m_rcDRAB=m_rcClient;

        if(m_bVertical)
        {
            int size=m_rcClient.Width();

            if(m_rcClient.Height()<=(size*2+2))
            {
                size=m_rcClient.Height()/2;
                if(size>1)
                    size-=1;
            }

            m_rcULAB.bottom=m_rcULAB.top+size;

            m_rcDRAB.top=m_rcDRAB.bottom-size;
        }
        else
        {
            int size=m_rcClient.Height();

            if(m_rcClient.Width()<=(size*2+2))
            {
                size=m_rcClient.Width()/2;
                if(size>1)
                    size-=1;
            }

            m_rcULAB.right=m_rcULAB.left+size;

            m_rcDRAB.left=m_rcDRAB.right-size;
        }

        CalcWorkVars();

        m_bVisible=si.nPage<(UINT)(si.nMax-si.nMin+1);
    }

    if((flags&SWP_NOREDRAW)==0)
        Invalidate(&m_rcWnd);
}

//-----------------------------------------------------------------------------

int CScaledScrollBar::DrawDecodeState(HIT_TEST_CODE htcode, bool bTheme)
{
    int state=0;

    if(bTheme)
    {
        switch(htcode)
        {
        case HT_SBUPBTN:
            {
                state=ABS_UPNORMAL-1+m_stateULAB.m_state;
            }
            break;
        case HT_SBLEFTBTN:
            {
                state=ABS_LEFTNORMAL-1+m_stateULAB.m_state;
            }
            break;
        case HT_SBDOWNBTN:
            {
                state=ABS_DOWNNORMAL-1+m_stateDRAB.m_state;
            }
            break;
        case HT_SBRIGHTBTN:
            {
                state=ABS_RIGHTNORMAL-1+m_stateDRAB.m_state;
            }
            break;
        case HT_SBUPTRACK:
        case HT_SBLEFTTRACK:
            {
                state=SCRBS_NORMAL-1+m_stateULTrack.m_state;
            }
            break;
        case HT_SBDOWNTRACK:
        case HT_SBRIGHTTRACK:
            {
                state=SCRBS_NORMAL-1+m_stateDRTrack.m_state;
            }
            break;
        case HT_SBVERTTHUMB:
        case HT_SBHORZTHUMB:
            {
                state=SCRBS_NORMAL-1+m_stateThumb.m_state;
            }
            break;
        default:
            ATLASSERT(0);
            break;
        }
    }
    else
    {
        switch(htcode)
        {
        case HT_SBUPBTN:
            {
                state=DFCS_SCROLLUP;
                switch(m_stateULAB.m_state)
                {
                case BS_HOT:
                    state|=DFCS_HOT;
                    break;

                case BS_PUSHED:
                    state|=DFCS_PUSHED;
                    break;

                case BS_DISABLED:
                    state|=DFCS_INACTIVE;
                    break;
                }
            }
            break;
        case HT_SBLEFTBTN:
            {
                state=DFCS_SCROLLLEFT;
                switch(m_stateULAB.m_state)
                {
                case BS_HOT:
                    state|=DFCS_HOT;
                    break;

                case BS_PUSHED:
                    state|=DFCS_PUSHED;
                    break;

                case BS_DISABLED:
                    state|=DFCS_INACTIVE;
                    break;
                }
            }
            break;
        case HT_SBDOWNBTN:
            {
                state=DFCS_SCROLLDOWN;
                switch(m_stateDRAB.m_state)
                {
                case BS_HOT:
                    state|=DFCS_HOT;
                    break;

                case BS_PUSHED:
                    state|=DFCS_PUSHED;
                    break;

                case BS_DISABLED:
                    state|=DFCS_INACTIVE;
                    break;
                }
            }
            break;
        case HT_SBRIGHTBTN:
            {
                state=DFCS_SCROLLRIGHT;
                switch(m_stateDRAB.m_state)
                {
                case BS_HOT:
                    state|=DFCS_HOT;
                    break;

                case BS_PUSHED:
                    state|=DFCS_PUSHED;
                    break;

                case BS_DISABLED:
                    state|=DFCS_INACTIVE;
                    break;
                }
            }
            break;
        case HT_SBUPTRACK:
        case HT_SBLEFTTRACK:
            {
                state=m_stateULTrack.m_state;
            }
            break;
        case HT_SBDOWNTRACK:
        case HT_SBRIGHTTRACK:
            {
                state=m_stateDRTrack.m_state;
            }
            break;
        case HT_SBVERTTHUMB:
        case HT_SBHORZTHUMB:
            {
                state=DFCS_BUTTONPUSH;

                switch(m_stateThumb.m_state)
                {
                case BS_HOT:
                    state|=DFCS_HOT;
                    break;

                case BS_DISABLED:
                    state|=DFCS_INACTIVE;
                    break;
                }
            }
            break;
        default:
            ATLASSERT(0);
            break;
        }
    }

    return state;
}

//-----------------------------------------------------------------------------

bool CScaledScrollBar::HitTest(const CPoint& p)
{
    bool bHit=false;

    if(m_rcWnd.PtInRect(p))
        bHit=true;

    return bHit;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CScaledScrollBar::HitTestParts(const CPoint& p)
{
    HIT_TEST_CODE code=HT_UNKNOWN;
    CalcWorkVars();
    int pix;

    pix=m_bVertical?p.y:p.x;

    if(m_bVertical)
    {
        if(pix<wtopArrow)
            code=HT_SBUPBTN;
        else if(pix<wtopThumb)
            code=HT_SBUPTRACK;
        else if(pix<wbottomThumb)
            code=HT_SBVERTTHUMB;
        else if(pix<wbottomArrow)
            code=HT_SBDOWNTRACK;
        else if(pix>=wbottomArrow)
            code=HT_SBDOWNBTN;
    }
    else
    {
        if(pix<wtopArrow)
            code=HT_SBLEFTBTN;
        else if(pix<wtopThumb)
            code=HT_SBLEFTTRACK;
        else if(pix<wbottomThumb)
            code=HT_SBHORZTHUMB;
        else if(pix<wbottomArrow)
            code=HT_SBRIGHTTRACK;
        else if(pix>=wbottomArrow)
            code=HT_SBRIGHTBTN;
    }
        
    ATLASSERT(code!=HT_UNKNOWN);

    return code;
}

//-----------------------------------------------------------------------------

CButtonState* CScaledScrollBar::Code2State(HIT_TEST_CODE code)
{
    CButtonState* hitstate=0;

    switch(code)
    {
    case HT_SBUPBTN:
    case HT_SBLEFTBTN:
        hitstate=&m_stateULAB;
        break;

    case HT_SBDOWNBTN:
    case HT_SBRIGHTBTN:
        hitstate=&m_stateDRAB;
        break;

    case HT_SBUPTRACK:
    case HT_SBLEFTTRACK:
        hitstate=&m_stateULTrack;
        break;

    case HT_SBDOWNTRACK:
    case HT_SBRIGHTTRACK:
        hitstate=&m_stateDRTrack;
        break;

    case HT_SBVERTTHUMB:
    case HT_SBHORZTHUMB:
        hitstate=&m_stateThumb;
        break;

    case HT_UNKNOWN:
        hitstate=0;
        break;

    default:
        ATLASSERT(0);
    }

    return hitstate;
}

//-----------------------------------------------------------------------------

void CScaledScrollBar::CalcWorkVars()
{
    int scrollsize;		// total size of scrollbar
    int workingsize;	// thumb sliding length
    int siMinMax;
    int butsize;
    int thumbpos;

    butsize=m_rcDRAB.Width();

    if(m_bVertical)
    {
        scrollsize=m_rcWnd.Height();
        wtop=m_rcWnd.top;
        wbottom=m_rcWnd.bottom;
    }
    else
    {
        scrollsize=m_rcWnd.Width();
        wtop=m_rcWnd.left;
        wbottom=m_rcWnd.right;
    }

    wtopArrow=wtop+m_rcDRAB.Height();
    wbottomArrow=wbottom-m_rcDRAB.Height();

    siMinMax=si.nMax-si.nMin+1;
    workingsize=scrollsize-butsize*2;

    if(si.nPage==0)
        wthumbsize=butsize;
    else if(siMinMax>0)
    {
        wthumbsize=MulDiv(si.nPage, workingsize, siMinMax);
        // adjust for some minimum allowed size
    }

    wminpix=wtopArrow;
    wmaxpix=wbottomArrow-wthumbsize;

    if(siMinMax>0)
    {
        int pagesize=__max(1,si.nPage);
        thumbpos=MulDiv(si.nPos-si.nMin, 
            workingsize-wthumbsize, 
            siMinMax-pagesize);

        if(thumbpos<0)
            thumbpos=0;
        if(thumbpos>=workingsize-wthumbsize)
            thumbpos=workingsize-wthumbsize;
    }
    else
    {
        thumbpos=0;
    }

    wtopThumb=wminpix+thumbpos;
    wbottomThumb=wtopThumb+wthumbsize;

    m_rcThumb=m_rcWnd;

    if(m_bVertical)
    {
        m_rcThumb.top=wtopThumb;
        m_rcThumb.bottom=wbottomThumb;
    }
    else
    {
        m_rcThumb.left=wtopThumb;
        m_rcThumb.right=wbottomThumb;
    }
}

//-----------------------------------------------------------------------------

int CScaledScrollBar::PosFromPix(int pix)
{
    CalcWorkVars();
    int pos=si.nMin-1;

    if(pix<wminpix)
        pos=si.nMin;
    else if(pix>=wmaxpix)
        pos=si.nMax-(si.nPage?(si.nPage-1):0);
    else if ((wmaxpix-wminpix)>0)
        pos=si.nMin+MulDiv(si.nMax-si.nMin-(si.nPage?(si.nPage-1):0),
            pix-wminpix, wmaxpix-wminpix);
    return pos;
}

//-----------------------------------------------------------------------------

int CScaledScrollBar::SetScrollInfo(
    SCROLLINFO* lpsi,
    bool fRedraw )
{
    // don't allow changes while dragging (requires update when dragging done)
    if(m_bDragging)
        return si.nPos;

    if(lpsi->fMask & SIF_RANGE)
    {
        si.nMin=lpsi->nMin;
        si.nMax=lpsi->nMax;
    }

    if(lpsi->fMask & SIF_PAGE)
    {
        UINT tot=(UINT)si.nMax-si.nMin+1;
        si.nPage=__min(__max(0,lpsi->nPage),tot);
    }

    if(lpsi->fMask & SIF_POS)
    {
        si.nPos=__max(lpsi->nPos, si.nMin);
        si.nPos=__min((UINT)lpsi->nPos, lpsi->nMax-__max(si.nPage-1,0));
    }

    if(fRedraw)
        Invalidate();

    return si.nPos;
}

//-----------------------------------------------------------------------------

bool CScaledScrollBar::GetScrollInfo(SCROLLINFO* lpsi)
{
    bool bCopied=false;

    if(lpsi->fMask & SIF_RANGE)
    {
        lpsi->nMin=si.nMin;
        lpsi->nMax=si.nMax;
        bCopied=true;
    }

    if(lpsi->fMask & SIF_PAGE)
    {
        lpsi->nPage=si.nPage;
        bCopied=true;
    }

    if(lpsi->fMask & SIF_POS)
    {
        lpsi->nPos=si.nPos;
        bCopied=true;
    }

    if(lpsi->fMask & SIF_TRACKPOS)
    {
        lpsi->nTrackPos=si.nTrackPos;
        bCopied=true;
    }

    return bCopied;
}

//-----------------------------------------------------------------------------

void CScaledScrollBar::SetScrollPos(int pos, bool fRedraw)
{
    if(pos!=si.nPos && pos>=si.nMin && pos<=si.nMax)
    {
        si.nPos=pos;
        CalcWorkVars();
        if(fRedraw)
            Invalidate(&m_rcWnd);
    }
}

//-----------------------------------------------------------------------------

LRESULT CScaledScrollBar::SendScrollMsg(int scrollMsg)
{
    LRESULT lRes=0;
    CScaledWindow* parent=GetParent();

    if(parent)
    {
        WPARAM wParam=MAKEWPARAM(scrollMsg,si.nPos);
        LPARAM lParam=reinterpret_cast<LPARAM>(this);
        // graph renderer needs direction of thumb's move, not just its size in si.nPos
        if(scrollMsg==SB_THUMBTRACK)
#ifdef __POSITION_DIFFERENCE__
            lParam=static_cast<LPARAM>(m_dragPosNew-m_dragPosOld);
#else // pixel difference
            lParam=static_cast<LPARAM>(m_dragPix-m_dragPixOld);
#endif
        UINT uMsg=m_bVertical ? WM_VSCROLL : WM_HSCROLL;
        lRes=parent->SendMessage(uMsg, wParam, lParam);
    }

    return lRes;
}

//*****************************************************************************
//
// Message handlers
//

LRESULT CScaledScrollBar::OnPaint(
    UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CAggDC& dc=*reinterpret_cast<CAggDC*>(wParam);
    bool bThemeOk=false;
    CRect rcTrackUL;
    CRect rcTrackDR;
    bool bSmall=m_bVertical?
        (m_rcClient.Height()<=(m_rcClient.Width()*2+2)):
        (m_rcClient.Width()<=(m_rcClient.Height()*2+2));

    if(!m_Theme.IsThemeNull())
    {
        bThemeOk=SUCCEEDED(m_Theme.DrawThemeBackground(
            dc,
            SBP_ARROWBTN,
            DrawDecodeState(m_bVertical?HT_SBUPBTN:HT_SBLEFTBTN, true),
            &m_rcULAB));

        if(bThemeOk && !bSmall)
        {
            // upper/left track

            rcTrackUL=m_rcWnd;

            if(m_bVertical)
            {
                rcTrackUL.top+=m_rcULAB.Height();
                rcTrackUL.bottom=m_rcThumb.top;
            }
            else
            {
                rcTrackUL.left+=m_rcULAB.Width();
                rcTrackUL.right=m_rcThumb.left;
            }

            if(bThemeOk)
                bThemeOk=SUCCEEDED(m_Theme.DrawThemeBackground(
                dc,
                m_bVertical?SBP_UPPERTRACKVERT:SBP_UPPERTRACKHORZ,
                DrawDecodeState(m_bVertical?HT_SBUPTRACK:HT_SBLEFTTRACK, true),
                &rcTrackUL));

            // thumb and gripper on it

            int thumbstate=DrawDecodeState(m_bVertical?HT_SBVERTTHUMB:HT_SBHORZTHUMB, true);

            if(bThemeOk)
                bThemeOk=SUCCEEDED(m_Theme.DrawThemeBackground(
                dc,
                m_bVertical?SBP_THUMBBTNVERT:SBP_THUMBBTNHORZ,
                thumbstate,
                &m_rcThumb));

            if(bThemeOk)
                bThemeOk=SUCCEEDED(m_Theme.DrawThemeBackground(
                dc,
                SBP_GRIPPERVERT,
                thumbstate,
                &m_rcThumb));

            // bottom/right track

            rcTrackDR=m_rcWnd;

            if(m_bVertical)
            {
                rcTrackDR.top=m_rcThumb.bottom;
                rcTrackDR.bottom-=m_rcDRAB.Height();
            }
            else
            {
                rcTrackDR.left=m_rcThumb.right;
                rcTrackDR.right-=m_rcDRAB.Width();
            }

            if(bThemeOk)
                bThemeOk=SUCCEEDED(m_Theme.DrawThemeBackground(
                dc,
                m_bVertical?SBP_LOWERTRACKVERT:SBP_LOWERTRACKHORZ,
                DrawDecodeState(m_bVertical?HT_SBDOWNTRACK:HT_SBRIGHTTRACK, true),
                &rcTrackDR));
        }

        if(bThemeOk)
            bThemeOk=SUCCEEDED(m_Theme.DrawThemeBackground(
            dc,
            SBP_ARROWBTN,
            DrawDecodeState(m_bVertical?HT_SBDOWNBTN:HT_SBRIGHTBTN, true),
            &m_rcDRAB));
    }

    if(!bThemeOk)
    {
        // upper/left button

        dc.DrawFrameControl(
            &m_rcULAB,
            DFC_SCROLL,
            DrawDecodeState(m_bVertical?HT_SBUPBTN:HT_SBLEFTBTN, false));

        if(!bSmall)
        {
            // upper/left track

            rcTrackUL=m_rcWnd;

            if(m_bVertical)
            {
                rcTrackUL.top+=m_rcULAB.Height();
                rcTrackUL.bottom=m_rcThumb.top;
            }
            else
            {
                rcTrackUL.left+=m_rcULAB.Width();
                rcTrackUL.right=m_rcThumb.left;
            }

            if(!rcTrackUL.IsRectEmpty())
            {
                if(m_stateULTrack.m_state==BS_PUSHED)
                {
                    dc.FillSolidRect(&rcTrackUL, GetSysColor(COLOR_3DDKSHADOW));
                }
                else
                {
                    COLORREF oldtcolor=dc.SetTextColor(GetSysColor(COLOR_SCROLLBAR));
                    COLORREF oldbcolor=dc.SetBkColor(GetSysColor(COLOR_WINDOW));
                    dc.FillRect(&rcTrackUL, m_lightTrackBrush);
                    dc.SetTextColor(oldtcolor);
                    dc.SetBkColor(oldbcolor);
                }
            }

            // thumb 

            dc.DrawFrameControl(
                &m_rcThumb,
                DFC_BUTTON,
                DrawDecodeState(m_bVertical?HT_SBVERTTHUMB:HT_SBHORZTHUMB, false));

            // bottom/right track

            rcTrackDR=m_rcWnd;

            if(m_bVertical)
            {
                rcTrackDR.top=m_rcThumb.bottom;
                rcTrackDR.bottom-=m_rcDRAB.Height();
            }
            else
            {
                rcTrackDR.left=m_rcThumb.right;
                rcTrackDR.right-=m_rcDRAB.Width();
            }

            if(!rcTrackDR.IsRectEmpty())
            {
                if(m_stateDRTrack.m_state==BS_PUSHED)
                {
                    dc.FillSolidRect(&rcTrackDR, GetSysColor(COLOR_3DDKSHADOW));
                }
                else
                {
                    //dc.FillSolidRect(&rcTrackDR, GetSysColor(COLOR_SCROLLBAR));

                    COLORREF oldtcolor=dc.SetTextColor(GetSysColor(COLOR_SCROLLBAR));
                    COLORREF oldbcolor=dc.SetBkColor(GetSysColor(COLOR_WINDOW));
                    dc.FillRect(&rcTrackDR, m_lightTrackBrush);
                    dc.SetTextColor(oldtcolor);
                    dc.SetBkColor(oldbcolor);
                }
            }
        }

        // bottom/right arrow

        dc.DrawFrameControl(
            &m_rcDRAB,
            DFC_SCROLL,
            DrawDecodeState(m_bVertical?HT_SBDOWNBTN:HT_SBRIGHTBTN, false));
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledScrollBar::OnLButtonDown(
    UINT uMsg,
    WPARAM /*wParam*/,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CButtonState* state;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

//	if (!m_bDisabled)
    m_canvas->SetCapture(this);

    m_capturedPart=HitTestParts(pt);
    state=Code2State(m_capturedPart);
    int maxpos=si.nMax-(si.nPage?si.nPage-1:0);

    switch(m_capturedPart)
    {
    case HT_SBUPBTN:
    case HT_SBLEFTBTN:
        if(si.nPos>si.nMin)
            si.nPos--;
        SendScrollMsg(SB_LINEUP);
        m_TimerCode=SB_LINEUP;
        m_canvas->SetTimer(ID_SCROLLTIMER, SCROLLTIMERTICK, this);
        break;

    case HT_SBDOWNBTN:
    case HT_SBRIGHTBTN:
        if(si.nPos<maxpos)
            si.nPos++;
        SendScrollMsg(SB_LINEDOWN);
        m_TimerCode=SB_LINEDOWN;
        m_canvas->SetTimer(ID_SCROLLTIMER, SCROLLTIMERTICK, this);
        break;

    case HT_SBUPTRACK:
    case HT_SBLEFTTRACK:
        si.nPos-=si.nPage;
        if(si.nPos<si.nMin)
            si.nPos=si.nMin;
        SendScrollMsg(SB_PAGEUP);
        m_TimerCode=SB_PAGEUP;
        m_canvas->SetTimer(ID_SCROLLTIMER, SCROLLTIMERTICK, this);
        break;

    case HT_SBDOWNTRACK:
    case HT_SBRIGHTTRACK:
        si.nPos+=si.nPage;
        if(si.nPos>maxpos)
            si.nPos=maxpos;
        SendScrollMsg(SB_PAGEDOWN);
        m_TimerCode=SB_PAGEDOWN;
        m_canvas->SetTimer(ID_SCROLLTIMER, SCROLLTIMERTICK, this);
        break;

    case HT_SBVERTTHUMB:
    case HT_SBHORZTHUMB:
        {
            m_bDragging=true;
            m_dragPosOld=m_dragPosNew=si.nPos;
            m_dragThumbOffset=(m_bVertical ? pt.y : pt.x)-wtopThumb;
            m_dragPix=m_dragPixOld=m_dragThumbOffset+wtopThumb;
        }
        break;
    }

    BUTTON_STATE old=state->m_state;
    if(old==BS_HOT)
    {
        m_canvas->ClearCurrentHot(BS_PUSHED);
    }
    else
    {
        m_canvas->ClearCurrentHot();
        state->m_state=BS_PUSHED;
        if(old!=BS_PUSHED)
            Invalidate(&m_rcWnd);
    }

    SetCursor(LoadCursor(NULL, IDC_ARROW));

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledScrollBar::OnLButtonUp(
    UINT uMsg,
    WPARAM /*wParam*/,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CButtonState* capstate, *hitstate;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    HIT_TEST_CODE hit=HitTestParts(pt);

    hitstate=Code2State(hit);

    if(m_canvas->GetCapture()==this)
    {
        capstate=Code2State(m_capturedPart);
        BUTTON_STATE oldcap=capstate->m_state;

        if(m_canvas->GetTimerTarget()==this)
            m_canvas->KillTimer(ID_SCROLLTIMER);

        if(m_capturedPart!=hit)
        {
            capstate->m_state=BS_NORMAL;

            if(hitstate)
                hitstate->SetHot(true);
        }
        else
        {
            capstate->SetHot(true);
        }

        m_bDragging=false;
        SendScrollMsg(SB_ENDSCROLL);

        m_capturedPart=HT_UNKNOWN;
        m_canvas->ReleaseCapture();

        if(oldcap!=capstate->m_state)
            Invalidate(&m_rcWnd);
    }
    else
    {
        ATLASSERT(hitstate);
        hitstate->SetHot(true);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledScrollBar::OnMouseMove(
    UINT uMsg,
    WPARAM /*wParam*/,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CButtonState* hitstate, *capstate;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    HIT_TEST_CODE hit=HitTestParts(pt);

    hitstate=Code2State(hit);
    if(m_canvas->GetCapture()==this)
    {
        if(m_bDragging)
        {
            bool allowed=true;
            int pix=m_bVertical?pt.y:pt.x;

            m_dragPix=pix;
            m_dragPosNew=PosFromPix(pix-m_dragThumbOffset);

            if(m_dragPixOld!=m_dragPix)
            {
                // if thumb is at either extremal position, don't send tracking message

                if(m_bVertical)
                {
                    if(pix<m_rcWnd.top || pix>m_rcWnd.bottom)
                        allowed=false;
                    else
                    {
                        if(m_dragPixOld<m_dragPix)
                        {
                            if(m_rcThumb.bottom==(m_rcWnd.Height()-m_rcDRAB.Height()))
                                allowed=false;
                        }
                        else
                        {
                            if(m_rcThumb.top==(m_rcWnd.top+m_rcULAB.Height()))
                                allowed=false;
                        }
                    }
                }
                else // horizontal
                {
                    if(pix<m_rcWnd.left || pix>m_rcWnd.right)
                        allowed=false;
                    else
                    {

                        if(m_dragPixOld<m_dragPix)
                        {
                            if(m_rcThumb.right==(m_rcWnd.Width()-m_rcDRAB.Width()))
                                allowed=false;
                        }
                        else
                        {
                            if(m_rcThumb.left==(m_rcWnd.left+m_rcDRAB.Width()))
                                allowed=false;
                        }
                    }
                }

                if(allowed)
                {
                    si.nPos=m_dragPosNew;
                    CalcWorkVars();
                    Invalidate(&m_rcWnd);
                    // NOTE ! message also sends old and new pixel position difference
                    SendScrollMsg(SB_THUMBTRACK);

                }

                m_dragPosOld=m_dragPosNew;
                m_dragPixOld=m_dragPix;
            }
        }
        else
        {
            capstate=Code2State(m_capturedPart);
            BUTTON_STATE oldcap=capstate->m_state;

            if(m_capturedPart!=hit)
            {
                if(m_capturedPart!=HT_SBVERTTHUMB)
                    capstate->m_state=BS_NORMAL;
                else
                    capstate->m_state=BS_PUSHED;
            }
            else
            {
                capstate->m_state=BS_PUSHED;
            }

            if(oldcap!=capstate->m_state)
                Invalidate(&m_rcWnd);
        }
    }
    else
    {
        ATLASSERT(hitstate);
        hitstate->SetHot(true);
    }

    SetCursor(LoadCursor(NULL, IDC_ARROW));

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledScrollBar::OnTimer(
    UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if(wParam==ID_SCROLLTIMER)
    {
        bool bChange=false;
        int maxpos=si.nMax-(si.nPage?si.nPage-1:0);

        switch(m_TimerCode)
        {
        case SB_LINEUP:
        {
            if(si.nPos>si.nMin)
            {
                si.nPos--;
                bChange=true;
            }
        }
        break;

        case SB_LINEDOWN:
        {
            if(si.nPos<maxpos)
            {
                si.nPos++;
                bChange=true;
            }
        }
        break;

        case SB_PAGEUP:
        {
            if(si.nPos>si.nMin)
            {
                si.nPos-=si.nPage;
                if(si.nPos<si.nMin)
                    si.nPos=si.nMin;
                bChange=true;
            }
        }
        break;

        case SB_PAGEDOWN:
        {
            if(si.nPos<maxpos)
            {
                si.nPos+=si.nPage;
                if(si.nPos>maxpos)
                    si.nPos=maxpos;
                bChange=true;
            }
        }
        break;

        default:
            ATLASSERT(0);
            break;

        } // switch


        if(bChange)
        {
            CalcWorkVars();
            Invalidate(&m_rcWnd);
            SendScrollMsg(m_TimerCode);
        }
    }

    return 0;
}
