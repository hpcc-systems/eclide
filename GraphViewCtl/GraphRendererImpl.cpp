#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "ScrollSupport.h"
#include "Geometry.h"
#include "AggDC.h"
#include "AggHitTest.h"

class CGraphRenderer : public IGraphRenderer, public CScaledWindow, public CScrollSupport
{
    IGraph* m_graph;
    CPoint	m_mouseCapturePos;
    GraphTypes::PointF m_dragPageStartPos;

    // edge creation and feedback
    CComPtr<IGraphVertex> m_vSrc; // source for edge creation
    GraphTypes::PointF	m_ptSrc, m_ptDest; // edge feedback line points
    GraphTypes::Color m_colorFdbNeutral;
    GraphTypes::Color m_colorFdbGood;
    GraphTypes::Color m_colorFdbBad;
    GraphTypes::Color m_colorFdb; // current one of above three


    bool DispatchMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
    bool HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IGraphRenderer)
        IMPLEMENT_INTERFACE(IRenderer)
    END_CUNKNOWN(CScaledWindow)

    CGraphRenderer(CCanvas& canvas, IGraph * graph);
    ~CGraphRenderer();

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

    //IGraphRenderer
    bool GetFocus() const;

protected:
    BEGIN_MSG_MAP(CGraphRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk);
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER(WM_TIMER, OnTimer);
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor);
        CHAIN_MSG_MAP(CScrollSupport)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


using namespace GraphTypes;

//*****************************************************************************

CGraphRenderer::CGraphRenderer(CCanvas& canvas, IGraph * graph)
    : CScaledWindow(&canvas), 
    CScrollSupport(&canvas, this),
    m_graph(graph)
{
    m_colorFdbNeutral.SetValue(Color::DimGray);
    m_colorFdbGood.SetValue(Color::ForestGreen);
    m_colorFdbBad.SetValue(Color::Crimson);
    m_colorFdb=m_colorFdbNeutral;

    m_mouseCapturePos.x=m_mouseCapturePos.y=0;
}

//-----------------------------------------------------------------------------

CGraphRenderer::~CGraphRenderer()
{
}

//-----------------------------------------------------------------------------

CUniqueID CGraphRenderer::GetID()
{
    return m_graph->GetID();
}

//-----------------------------------------------------------------------------

void CGraphRenderer::SetGraphItem(IGraphItem* item)
{
    m_graph=static_cast<IGraph*>(item);
}

//-----------------------------------------------------------------------------

void CGraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    CRect rc(m_rcWnd);
    ModifyWindowRect(rc);
    RectF rcf(rc);
    box=rc;
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CGraphRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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
    SetScrollRect(rc);
}

//-----------------------------------------------------------------------------

void CGraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CGraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CGraphRenderer::SetFocus(bool bSet)
{
    CScaledWindow::SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CGraphRenderer::SetSelected(bool bSelected, bool bAdd)
{
    CScaledWindow::SetSelected(GetID(), bSelected, bAdd);
}

//-----------------------------------------------------------------------------

void CGraphRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CGraphRenderer::ClearCaches()
{
    CComQIPtr<IRenderer> ir;
    IGraphItemVector results;

    m_graph->GetChildren(results); 

    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr)
    {
        ir=m_canvas->GetRenderer(*itr);
        if(ir!=0)
            ir->ClearCaches();
    }
}

//-----------------------------------------------------------------------------

bool CGraphRenderer::DispatchMessage(
    UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
    if(DispatchToScrollbars(uMsg, wParam, lParam, lResult))
        return true;

    // go in reverse order since we want to dispatch to topmost
    // window first

    PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

    CComPtr<IGraphItem> item;
    CComPtr<IRenderer> lr;
    bool bTarget=false;
    RectF bbox;

    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetRChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        item=itr->Get();			
        lr=m_canvas->GetRenderer(item);
        if(lr!=0)
        {
            lr->GetBoundsBox(bbox);

            if(bbox.Contains(p))
            {
                lResult=lr->SendMessage(uMsg, wParam, lParam);
                bTarget=true;
                break;
            }
        }
    }

    return bTarget;
}

//-----------------------------------------------------------------------------

bool CGraphRenderer::HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bHandled=false;
    CScaledWindow* capture=m_canvas->GetCapture();

    ATLASSERT((uMsg>=WM_MOUSEFIRST && uMsg<=WM_MOUSELAST) ||
                (uMsg>=WM_KEYFIRST && uMsg<=WM_KEYLAST));

    if(capture)
    {		
        capture->SendMessage(uMsg, wParam, lParam); 
        bHandled=true;
    }
    
    return bHandled;
}

//-----------------------------------------------------------------------------

SizeF CGraphRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce, 
    bool bIgnoreExtent)
{
    CComPtr<IGraphItem> item;
    CComPtr<IRenderer> lr;
    SizeF itemsize;

    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        item=itr->Get();			
        lr=m_canvas->GetRenderer(item);
        if(lr!=0)
        {
            itemsize=lr->CalcDisplaySizes(dc, bForce, bIgnoreExtent);
        }
    }

    return itemsize;	
}

//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::OnPaint(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    CAggDC& dc=*(CAggDC*)wParam;
    CDCHandle dch(dc.m_hDC);

    m_canvas->SetDPI(
        GetDeviceCaps(dc, LOGPIXELSX), GetDeviceCaps(dc, LOGPIXELSY));

    CalcDisplaySizes(dch, false, true);

    if(m_canvas->m_meta.IsGridVisible())
        m_canvas->DrawGrid(dch);

    CComPtr<IGraphItem> item;
    CComPtr<IRenderer> lr;

    RectF bbox, rcext;
    bool first=true;

    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        item=itr->Get();			
        lr=m_canvas->GetRenderer(item);
        if(lr!=0)
        {
            lr->SendMessage(WM_PAINT, wParam, 0);
            lr->GetBoundsBox(bbox);
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
            }
        }
    }

    m_canvas->StoreGraphExtent(rcext);

    if(!m_ptSrc.Equals(m_ptDest))
    {
        dc.Line(m_ptSrc, m_ptDest, m_colorFdb);
    }

    UpdateScrollbars(static_cast<CRect>(rcext));
    PaintScrollbars(wParam);

    return 0;
}

//-----------------------------------------------------------------------------
//
//	Message Handlers

LRESULT CGraphRenderer::OnKeyDown(
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

LRESULT CGraphRenderer::OnLButtonDown(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    // this should not be happening unless mouse up message was lost
    ATLASSERT(m_canvas->GetCapture()!=this); 
#ifdef _DEBUG
    if(m_canvas->GetCapture()==this)
        m_canvas->ReleaseCapture();
#endif

    bool bProcessed=HandleCapture(uMsg, wParam, lParam);

    if(!bProcessed)
    {
        if(!DispatchMessage(uMsg, wParam, lParam, lRet))
        {
            if(!HasFocus())
            {
                SetFocus();
            }
            
            if(!IsSelected())
            {
                CUniqueIDVector selected;
                unsigned count=m_canvas->GetSelected(&selected);
                selected.Reset();
                SetSelected(true, false);
                //if(count>0)
                //	m_canvas->NotifySelection(GetID(), selected.Get());
                //else
                    m_canvas->NotifySelection(GetID());
            }

            m_canvas->SetCapture(this);

            m_mouseCapturePos.x=GET_X_LPARAM(lParam);
            m_mouseCapturePos.y=GET_Y_LPARAM(lParam); 
            m_dragPageStartPos=m_canvas->GetOffset();
        }
        else
        {
            if(lRet!=0)
            {
                //ATLASSERT(m_vSrc==0);
                CComQIPtr<IGraphVertex> vertex;
                vertex=reinterpret_cast<IGraphItem*>(lRet);

                if(vertex!=0)
                {
                    m_vSrc=vertex;
                    m_ptSrc.x=static_cast<REAL>(GET_X_LPARAM(lParam));
                    m_ptSrc.y=static_cast<REAL>(GET_Y_LPARAM(lParam));
                    m_ptDest=m_ptSrc;
                    m_canvas->SetCapture(this);
                }
            }
        }
    }

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::OnLButtonUp(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    if(m_canvas->GetCapture()==this)
    {
        PointF ptf((LONG)GET_X_LPARAM(lParam), (LONG)GET_Y_LPARAM(lParam));
        CComPtr<IGraphItem> ig;
        CComQIPtr<IGraphVertex> v;

        m_ptSrc=m_ptDest;

        ig=m_canvas->FindGraphItem(ptf);
        if(ig!=0)
        {
        // for now send notification only for vertex-vertex connections
            v=ig.p;
            if(v!=0 && m_vSrc!=0)
                m_canvas->NotifyItemsConnected(m_vSrc->GetID(), ig->GetID());
        }

        m_vSrc=0;
        m_colorFdb=m_colorFdbNeutral;
        SetCursor(LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW)));
        m_canvas->Invalidate();
        m_canvas->ReleaseCapture();
    }
    else
    {
        bool bProcessed=HandleCapture(uMsg, wParam, lParam);

        if(!bProcessed)
            DispatchMessage(uMsg, wParam, lParam, lRet);
    }

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::OnMouseMove(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    if(m_canvas->GetCapture()==this)
    {
        if((wParam&MK_LBUTTON)!=0)
        {
            if(m_vSrc)
            {
                PointF ptf((LONG)GET_X_LPARAM(lParam), (LONG)GET_Y_LPARAM(lParam));
                m_ptDest=ptf;

                // when we have a capture we dispatch WM_NCHITTEST instead of WM_MOUSEMOVE
                if(DispatchMessage(WM_NCHITTEST, wParam, lParam, lRet))
                {
                    // message was dispatched to some target, see what we hit
                    if(lRet!=0)
                    {
                        CComQIPtr<IGraphVertex> vertex;
                        vertex=reinterpret_cast<IGraphItem*>(lRet);

                        if(vertex!=0)
                        {
                                // check if target will support edge from us
                            bool bAccept=false;
                            CComQIPtr<ITextboxRenderer> tb=m_canvas->GetRenderer(vertex);
                            if (tb!=0)
                                bAccept=tb->AcceptEdge(m_vSrc);

                            m_colorFdb=bAccept?m_colorFdbGood:m_colorFdbBad;
                            SetCursor(LoadCursor(0, bAccept?IDC_ARROW:IDC_NO));
                        }
                        else
                        {
                            m_colorFdb=m_colorFdbBad;
                            SetCursor(LoadCursor(0, IDC_NO));
                        }
                    }
                }
                else // nothing under
                {
                    m_colorFdb=m_colorFdbNeutral;
                    SetCursor(LoadCursor(0, IDC_ARROW));
                }

                // if we don't do this, nothing will redraw the line
                m_canvas->Invalidate();
            }
            else // didn't start with click on item
            {
                CPoint mouseCurrentPos(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

                int deltaX = mouseCurrentPos.x - m_mouseCapturePos.x;
                int deltaY = mouseCurrentPos.y - m_mouseCapturePos.y;
                if (abs(deltaX) > 3 || abs(deltaY) > 3)
                {
                    PointF pt=m_dragPageStartPos-PointF((LONG)deltaX, (LONG)deltaY);
                    m_canvas->SetOffset(pt);
                    m_canvas->Invalidate();
                }
            }
        }
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

LRESULT CGraphRenderer::OnMouseDblclk(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;

    DispatchMessage(uMsg, wParam, lParam, lRet);

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::OnRButtonDown(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes=0;
    
    if(!DispatchMessage(uMsg, wParam, lParam, lRes))
    {
        CComPtr<IGraphEdge> edge;
        CComQIPtr<IEdgeRenderer> ier;
        PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 
        CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

#define __RASTERIZER_HIT_TEST__
#ifdef __RASTERIZER_HIT_TEST__
        RectF vp;
        m_canvas->GetViewport(vp);
        CRect rc=static_cast<CRect>(vp);
        boost::scoped_ptr<CAggHitTest> agght(CAggHitTest::CreateAggHitTest());
        agght->SetRect(rc);
#endif

        PointF closest;
        RectF labelpos;

        for(CComPtr<IEdgeIterator> itr = m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
        {
            edge=itr->Get();
            ier=m_canvas->GetRenderer(edge);
            if(ier!=0)
            {
                if(m_canvas->m_meta.IsEdgeLabelVisible())
                {
                    ier->GetLabelPos(labelpos);
                    if(labelpos.Contains(p))
                    {
                        lRes=reinterpret_cast<LRESULT>(edge.p);
                        break;
                    }
                }

                PointFVector vp;
                ier->GetPoints(vp, screen);
                if(m_canvas->m_meta.GetMetaSplines())
                {
                    if(vp.size()==4)
                    {
#ifndef __RASTERIZER_HIT_TEST__
                        if(PointOnBezier4(vp[0], vp[1], vp[2], vp[3], p, 4.0))
                        {
                            lRes=reinterpret_cast<LRESULT>(edge.p);
                            break;
                        }
#else
                        SizeF width;
                        width.Width=m_canvas->m_meta.GetMetaEdgeWidth(edge->GetCategory());
                        m_canvas->SizeTransform(width);

                        if(agght->Bezier4HitTest(vp[0], vp[1], vp[2], vp[3], p, width.Width, 0, true, m_canvas->GetScale()))
                        {
                            lRes=reinterpret_cast<LRESULT>(edge.p);
                            break;
                        }
#endif
                    }
                    else
                    {
                            ATLASSERT(0); // for now
                    }
                }
                else if(vp.size()>1)
                {
                    GenLine line(vp[0], vp[vp.size()-1]);
                    line.ClosestPoint(p, closest);
                    int dist=idist(int(p.x), int(p.y), int(closest.x), int(closest.y));
                    if(dist<3)
                    {
                        lRes=reinterpret_cast<LRESULT>(edge.p);
                        break;
                    }
                }
            }
        }
    }

    return lRes;
}

bool CGraphRenderer::GetFocus() const
{
    return CScaledWindow::HasFocus();
}
//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::OnTimer(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRet=0;
    CScaledWindow* target=m_canvas->GetTimerTarget(); 
    if(target)
        lRet=target->SendMessage(uMsg, wParam, lParam);
    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CGraphRenderer::OnSetCursor(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if(m_colorFdb.GetValue()==m_colorFdbBad.GetValue())
    {
        SetCursor(LoadCursor(0, IDC_NO));
    }
    else
    {
        SetCursor(LoadCursor(0, IDC_ARROW));
    }
    return 1; // tell everybody else we processed it
}


//*****************************************************************************

IGraphRenderer *CreateNewGraphRenderer(CCanvas& canvas, IGraph* graph)
{
    CGraphRenderer * retVal = new CGraphRenderer(canvas, graph);
    return retVal;
}

