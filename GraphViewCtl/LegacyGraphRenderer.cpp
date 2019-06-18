#include "StdAfx.h"

#include "Canvas.h"
#include "Geometry.h"
#include "ScaledWindows.h"
#include "ScrollSupport.h"
#include "AggDC.h"
#include "AggHitTest.h"

class CLegacyGraphRenderer : public ILegacyGraphRenderer, public CScaledWindow, public CScrollSupport
{
protected:
    IGraph* m_graph;

    CPoint	m_mouseCapturePos;
    CPoint	m_mouseCurrentPos;
    CRect	m_selectedRangeRect;
    CComPtr<IRenderer> m_lastDispatch; // 0 if msg not dipatched to any child renderer

    GraphTypes::PointF m_dragPageStartPos;
    WTL::CPen m_gridPen;

    CComQIPtr<IGraphVertex> m_vSrc; // source for edge creation

    bool DispatchMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
    bool HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(ILegacyGraphRenderer)
        IMPLEMENT_INTERFACE(IGraphRenderer)
        IMPLEMENT_INTERFACE(IRenderer)
    END_CUNKNOWN(CScaledWindow)

    CLegacyGraphRenderer(CCanvas& canvas, IGraph * graph);
    ~CLegacyGraphRenderer();

// IRenderer

    CUniqueID GetID();
    void SetGraphItem(IGraphItem* item);
    void GetBoundsBox(GraphTypes::RectF& box, GraphTypes::Coordinates co=GraphTypes::screen);
    void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co=GraphTypes::screen);
    void GetPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co);
    void SetPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co);
    void SetSelected(bool bSelected=true, bool bAdd=false);
    void SetFocus(bool bSet=true);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc,
        bool bForce=false,
        bool bIgnoreExtent=false);	
    void SetRedraw(bool bEnable);
    void ClearCaches();

    //IGraphRenderer  ---
    bool GetFocus() const;

protected:
    BEGIN_MSG_MAP(CLegacyGraphRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        MESSAGE_HANDLER(WM_TIMER, OnTimer);
        CHAIN_MSG_MAP(CScrollSupport)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


using namespace GraphTypes;

//*****************************************************************************

CLegacyGraphRenderer::CLegacyGraphRenderer(CCanvas& canvas, IGraph * graph)
    : CScaledWindow(&canvas),
    CScrollSupport(&canvas, this),
    m_graph(graph),
    m_mouseCapturePos(-1,-1),
    m_selectedRangeRect(0,0,0,0)
{
}

//-----------------------------------------------------------------------------

CLegacyGraphRenderer::~CLegacyGraphRenderer()
{
}

//-----------------------------------------------------------------------------

CUniqueID CLegacyGraphRenderer::GetID()
{
    return m_graph->GetID();
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::SetGraphItem(IGraphItem* item)
{
    CComQIPtr<IGraph> graph = item;
    m_graph=graph;
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    CRect rc(m_rcWnd);

    if(m_canvas->m_meta.IsEmulateDecoration())
        ModifyWindowRect(rc);

    RectF rcf(rc);
    box=rc;
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
    CRect rc;
    if(co==world)
    {
        RectF r = box;
        m_canvas->AllTransform(r);
        rc = r;
    }
    else
    {
        rc = box;
    }
    CScaledWindow::SetWindowPos(rc, flags);
    if(m_canvas->m_meta.IsEmulateDecoration())
        SetScrollRect(rc);
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::SetFocus(bool bSet)
{
    CScaledWindow::SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::SetSelected(bool bSelected, bool bAdd)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CLegacyGraphRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce, 
    bool bIgnoreExtent)
{
    SizeF itemsize;
    IRenderer* ir;
    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        ir=m_canvas->GetRenderer(itr->Get());
        if(ir!=0)
        {
            ir->CalcDisplaySizes(dc, bForce, bIgnoreExtent);
        }
    }

    return itemsize;	
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CLegacyGraphRenderer::ClearCaches()
{
    //CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);
    //IEdgeRenderer* ier;

    //for(CComPtr<IEdgeIterator> itr = m_graph->GetEdges(skipper); itr->IsValidItem(); itr->NextItem())
    //{
    //	ier=static_cast<IEdgeRenderer*>(itr->Get()->GetPropertyUnknown(0));
    //	if(ier!=0)
    //		ier->ClearCaches();
    //}

    //CComQIPtr<ILegacyRenderer> lr;

    //for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    //{
    //	lr=itr->Get()->GetPropertyUnknown(0);
    //	if(lr!=0)
    //		lr->ClearCaches();
    //}
}

//-----------------------------------------------------------------------------

bool CLegacyGraphRenderer::DispatchMessage(
    UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    if(m_canvas->m_meta.IsEmulateDecoration())
        if(DispatchToScrollbars(uMsg, wParam, lParam, lResult))
            return true;

    PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 
    if(uMsg==WM_NCHITTEST)
    {
        CPoint pt=p;
        m_canvas->ClientCursorPos(&pt);
        p=pt;
    }

    CComQIPtr<ILegacyRenderer> lr;
    CComQIPtr<ILegacySubgraphRenderer> lsgr;
    IRenderer* ir=0;

    bool bTarget=false;
    RectF bbox;

    // go in reverse order since we want to dispatch to topmost window first

    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetRChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        ir=m_canvas->GetRenderer(itr->Get());
        lr=ir;
        if(lr==0)
            lsgr=ir;

        ATLASSERT(lr!=0 || lsgr!=0 && !(lr!=0 && lsgr!=0));

        if(lr!=0)
        {
            lResult=lr->HitTestDispatchMsg(p, bTarget, uMsg, wParam, lParam);
            if(bTarget)
                break;
        }
        else
        {
            lResult=lsgr->HitTestDispatchMsg(p, bTarget, uMsg, wParam, lParam);
            if(bTarget)
                break;
        }
    }

    if(uMsg==WM_NCHITTEST)
    {
        if(bTarget)
        {
            if(lr!=0)
                m_lastDispatch=lr;
            else
                m_lastDispatch=lsgr;
        }
        else
        {
            m_lastDispatch=0;
        }
    }

    return bTarget;
}

//-----------------------------------------------------------------------------

bool CLegacyGraphRenderer::HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bHandled=false;
    CScaledWindow* capture=m_canvas->GetCapture();

    if(capture)
    {		
        if(capture!=this)
            capture->SendMessage(uMsg, wParam, lParam); 
        bHandled=true;
    }

    return bHandled;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnPaint(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CAggDC& dc=*(CAggDC*)wParam;
    CDCHandle dch(dc.m_hDC);

    m_canvas->SetDPI(
        GetDeviceCaps(dch, LOGPIXELSX), GetDeviceCaps(dc, LOGPIXELSY));

    if(m_canvas->m_meta.IsGridVisible())
        m_canvas->DrawGrid(dch);

    CalcDisplaySizes(dch, false, true);

    // first draw all the edges
    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);
    unsigned ecount=m_graph->GetEdgeCount();	
    dc.BeginPathCache(ecount);

#ifdef __CUSTOM_PROFILE__
    BEGIN_PROF(_T("Edge Loop"));
#endif

    IRenderer* ir;

    for(CComPtr<IEdgeIterator> itr = m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
    {
        ir=m_canvas->GetRenderer(itr->Get());
        if(ir!=0)
            ir->SendMessage(WM_PAINT, wParam, lParam);
    }
    dc.EndPathCache();

#ifdef __CUSTOM_PROFILE__
    END_PROF();
    Profile.dumpprint(0);
    Profile.reset();
#endif

    RectF bbox, rcext;
    bool first=true;

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        ir=m_canvas->GetRenderer(itr->Get());
        ATLASSERT(ir!=0);

        if(ir!=0)
        {
            ir->SendMessage(WM_PAINT, wParam, lParam);
            ir->GetBoundsBox(bbox);
            if(!bbox.IsEmptyArea())
            {
                if(first)
                {
                    rcext=bbox;
                    first=false;
                }
                else
                {
                    rcext.Union(rcext, rcext, bbox);
                }

                CComQIPtr<ILegacyRenderer> lr=ir;
                if(lr!=0)
                {
                    lr->GetImagesBoundsBox(bbox);
                    if(!bbox.IsEmptyArea())
                        rcext.Union(rcext, rcext, bbox);
                }
            }
        }
    }

    m_canvas->StoreGraphExtent(rcext);

    MOUSE_MODES mode = m_canvas->GetMouseMode();
    if (mode==MOUSE_MODE_DRAW_LINKS && m_vSrc != 0 && m_mouseCapturePos.x != -1)
    {
        dc.Line(m_mouseCapturePos, m_mouseCurrentPos, GraphTypes::Color::Navy);
    }
    else if (mode==MOUSE_MODE_SELECTION && !m_selectedRangeRect.IsRectEmpty())
    {
        dc.DrawFocusRect(&m_selectedRangeRect);
    }

    if(m_canvas->m_meta.IsEmulateDecoration())
    {
        int	dctech=dc.GetDeviceCaps(TECHNOLOGY);
        // no scrollbars on printer DC
        if(dctech==DT_RASDISPLAY)
        {
            UpdateScrollbars(static_cast<CRect>(rcext));
            PaintScrollbars(wParam);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
//
//	Message Handlers

LRESULT CLegacyGraphRenderer::OnKeyDown(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CScaledWindow* hooker=m_canvas->GetKbdHook();

    if(hooker)
    {
        ATLASSERT(hooker!=this);
        hooker->SendMessage(uMsg, wParam, lParam); 
    }

    CScaledWindow* focus=m_canvas->m_currentFocus;

    if(focus && focus!=this && focus!=hooker)
        focus->SendMessage(uMsg, wParam, lParam); 

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnLButtonDown(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;
    m_vSrc.p=0;

    m_mouseCapturePos.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
    m_mouseCurrentPos = m_mouseCapturePos;

    bool bProcessed=HandleCapture(uMsg, wParam, lParam);

    if(!bProcessed)
    {
        if(!DispatchMessage(uMsg, wParam, lParam, lRet))
        {
            SetFocus();
            m_canvas->DeselectAll();
            // Notify that "nothing" has selection.
            m_canvas->NotifySelection(CUniqueID());
            m_canvas->SetCapture(this);

            m_dragPageStartPos=m_canvas->GetOffset();
        }
        else
        {
            if(m_canvas->GetMouseMode()==MOUSE_MODE_DRAW_LINKS && lRet!=0)
            {
                m_vSrc=reinterpret_cast<IGraphItem*>(lRet);
                if(m_vSrc!=0)
                    m_canvas->SetCapture(this);
            }
        }
    }

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnLButtonUp(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    if(m_canvas->GetCapture()==this)
    {
        m_canvas->ReleaseCapture();
        m_mouseCurrentPos.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        if (m_canvas->GetMouseMode()==MOUSE_MODE_DRAW_LINKS && m_vSrc!=0)
        {
            CComPtr<IGraphItem> ig;

            ig=m_canvas->FindGraphItem(m_mouseCurrentPos);
            if(ig!=0)
            {
                // for now send notification only for vertex-vertex connections
                CComQIPtr<IGraphVertex> overVertex = ig;
                if(overVertex!=0)
                    m_canvas->NotifyItemsConnected(m_vSrc->GetID(), ig->GetID());
            }
            m_vSrc.p=0;
        }
        m_canvas->Invalidate();
    }
    else
    {
        bool bProcessed=HandleCapture(uMsg, wParam, lParam);

        if(!bProcessed)
            DispatchMessage(uMsg, wParam, lParam, lRet);
    }

    m_selectedRangeRect.SetRectEmpty();
    m_mouseCapturePos.SetPoint(-1, -1);

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnMouseMove(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    if(m_canvas->GetCapture()==this)
    {
        m_mouseCurrentPos.SetPoint(pt.x, pt.y);

        if(m_canvas->GetMouseMode()==MOUSE_MODE_SELECTION) // rubber-banding
        {
            if (m_mouseCurrentPos.x < m_mouseCapturePos.x)
            {
                m_selectedRangeRect.left = m_mouseCurrentPos.x;
                m_selectedRangeRect.right = m_mouseCapturePos.x;
            }
            else
            {
                m_selectedRangeRect.left = m_mouseCapturePos.x;
                m_selectedRangeRect.right = m_mouseCurrentPos.x;
            }
            if (m_mouseCurrentPos.y < m_mouseCapturePos.y)
            {
                m_selectedRangeRect.top = m_mouseCurrentPos.y;
                m_selectedRangeRect.bottom = m_mouseCapturePos.y;
            }
            else
            {
                m_selectedRangeRect.top = m_mouseCapturePos.y;
                m_selectedRangeRect.bottom = m_mouseCurrentPos.y;
            }
            RectF r(m_selectedRangeRect);
            m_canvas->SelectItemsWithin(r);
        }
        else if (m_canvas->GetMouseMode() != MOUSE_MODE_DRAW_LINKS)
        {
            int deltaX = m_mouseCurrentPos.x - m_mouseCapturePos.x;
            int deltaY = m_mouseCurrentPos.y - m_mouseCapturePos.y;
            if (abs(deltaX) > 3 || abs(deltaY) > 3)
            {
                PointF pt=m_dragPageStartPos-PointF((LONG)deltaX, (LONG)deltaY);
                PointF oldoffset=m_canvas->GetOffset();
                m_canvas->SetOffset(pt);

                CPoint ptscroll(
                    round_int(oldoffset.x-pt.x),
                    round_int(oldoffset.y-pt.y));

                m_canvas->Scroll(ptscroll);
            }
        }
        m_canvas->Invalidate();
    }
    else
    {
        bool bProcessed=HandleCapture(uMsg, wParam, lParam);

        if(!bProcessed)
        {
            if(!DispatchMessage(uMsg, wParam, lParam, lRet))
            {
                m_canvas->ClearCurrentHot();
            }
        }
    }

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnMouseDblclk(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    DispatchMessage(uMsg, wParam, lParam, lRet);

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnRButtonDown(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes=0;
    
    if(!DispatchMessage(uMsg, wParam, lParam, lRes))
    {
        PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 
        CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

//#define __RASTERIZER_HIT_TEST__
#ifdef __RASTERIZER_HIT_TEST__
        RectF vp;
        m_canvas->GetViewport(vp);
        CRect rc=static_cast<CRect>(vp);
        boost::scoped_ptr<CAggHitTest> agght(CAggHitTest::CreateAggHitTest(&rc));
#else
        PointF closest;
#endif

        RectF pos;
        for(CComPtr<IEdgeIterator> itr=m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
        {
            CComPtr<IGraphEdge> edge=itr->Get();
            CComQIPtr<IEdgeRenderer> ier=m_canvas->GetRenderer(edge);
            if(ier!=0)
            {
                // first hit test edge label then edge itself

                if (m_canvas->m_meta.IsEdgeLabelVisible())
                {
                    ier->GetLabelPos(pos);
                    if (pos.Contains(p))
                    {
                        lRes=reinterpret_cast<LRESULT>(edge.p);
                        break;
                    }
                }

                ier->GetBoundsBox(pos);
                if(pos.Contains(p))
                {
                    PointFVector vp;
                    ier->GetPoints(vp, screen);
                    if(vp.size()>1)
                    {
#ifndef __RASTERIZER_HIT_TEST__
                        GenLine line(vp[0], vp[vp.size()-1]);
                        line.ClosestPoint(p, closest);
                        int dist=idist(int(p.x), int(p.y), int(closest.x), int(closest.y));
                        // TODO: we can make this a meta value
                        if(dist<5)
                        {
                            lRes=reinterpret_cast<LRESULT>(edge.p);
                            break;
                        }
#else
                        SizeF width;
                        REAL maxWidth;

                        width.Width=m_canvas->m_meta.GetMetaEdgeWidth(edge->GetCategory());
                        m_canvas->SizeTransform(width);
                        maxWidth=width.Width/2;

                        PointF normalizedTo(vp[1].x - vp[0].x, vp[1].y - vp[0].y);

                        double len = sqrt(normalizedTo.x * normalizedTo.x + normalizedTo.y * normalizedTo.y);
                        if(len>1)
                        {
                            if(maxWidth > 0.6)
                            {
                                float angle = atan(normalizedTo.y / normalizedTo.x);
                                normalizedTo.x = maxWidth * cos(angle);
                                normalizedTo.y = maxWidth * sin(angle);

                                PointF newP;

                                // rotate normalizedTo for 90 degrees
                                newP.x=-normalizedTo.y;
                                newP.y=normalizedTo.x;

                                PointF points[4];
                                points[0].x=vp[0].x+newP.x;
                                points[0].y=vp[0].y+newP.y;
                                points[1].x=vp[0].x-newP.x;
                                points[1].y=vp[0].y-newP.y;

                                points[2]=vp[1];
                                REAL quarter=M_PI/4;
                                REAL half=M_PI/2;

                                points[3]=vp[1];

                                if(angle<=quarter && angle>=(-quarter))
                                    points[3].y+=1;
                                else if(angle>quarter && angle<=(half+quarter))
                                    points[3].x-=1;
                                else if(angle>(half+quarter) && angle<(M_PI+quarter))
                                    points[3].y-=1;
                                else
                                    points[3].x+=1;
    
                                if(agght->SolidPolygonHitTest(points, 4, p, 3))
                                {
                                    lRes=reinterpret_cast<LRESULT>(edge.p);
                                    break;
                                }
                            }
                            else
                            {
                                if(agght->LineHitTest(vp[0], vp[1], p, 3))
                                {
                                    lRes=reinterpret_cast<LRESULT>(edge.p);
                                    break;
                                }
                            }
                        }
#endif
                    }
                } 
            }
        }
    }

    return lRes;
}

//-----------------------------------------------------------------------------

bool CLegacyGraphRenderer::GetFocus() const
{
    return CScaledWindow::HasFocus();
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnNcHitTest(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=HTCLIENT;

    bool bProcessed=HandleCapture(uMsg, wParam, lParam);

    if(!bProcessed)
        DispatchMessage(uMsg, wParam, lParam, lRet);

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnSetCursor(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes=1;

    if(m_lastDispatch!=0)
    {
        lRes=m_lastDispatch->SendMessage(uMsg, wParam, lParam);
    }
    else
    {
        m_canvas->CalcMouseMode();
        m_canvas->SetModeCursor();
    }
    return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyGraphRenderer::OnTimer(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;
    CScaledWindow* target=m_canvas->GetTimerTarget(); 
    if(target)
        lRet=target->SendMessage(uMsg, wParam, lParam);
    return lRet;
}


//*****************************************************************************

IGraphRenderer *CreateLegacyGraphRenderer(CCanvas& canvas, IGraph* graph)
{
    CLegacyGraphRenderer * retVal = new CLegacyGraphRenderer(canvas, graph);
    return retVal;
}

