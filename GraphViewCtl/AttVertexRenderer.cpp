#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "SizeTracker.h"
#include "XGMMLSaxHandler.h"

class CAttVertexRenderer : public IAttVertexRenderer, public CScaledWindow
{
    IGraphVertex* m_v;
    CComQIPtr<IXGMMLRenderCallback> m_callback;

    boost::signals::connection m_sigConn;
    CUniqueID m_uniqueID;
    CButtonState m_hotTracking;

    // dimensions (in world coordinate system) imposed by the layout
    GraphTypes::SizeF m_imposedSize;
    // top left position in internal (world) coordinates
    GraphTypes::PointF m_position;
    static GraphTypes::REAL m_borderLineThickness;
    static GraphTypes::REAL m_borderMinThickness;

    GraphTypes::Color m_fillColor;
    CAggBitmap m_textBmp;
    CSizeTracker m_sizeTracker;

    GraphTypes::RectF CalcVertexRect();
    void DrawVertex(CAggDC& dc, const GraphTypes::Color & borderColor, const GraphTypes::Color & fillColor, const GraphTypes::Color & fontColor); 

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IAttVertexRenderer)
        IMPLEMENT_INTERFACE(IVertexRenderer)
        IMPLEMENT_INTERFACE(IRenderer)
    END_CUNKNOWN(CScaledWindow)

    CAttVertexRenderer(CCanvas & canvas);
    ~CAttVertexRenderer();

    void operator()(GRAPHNOTIFICATION modification, IGraphItem * item);

    void SetWindowPos(const CRect& pos, UINT flags);
    virtual void Invalidate(CRect* rc=0, bool bErase=false); 

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
    GraphTypes::SizeF CacheText(CDCHandle& dc,
        bool bForce=false,
        bool bIgnoreExtent=false);	
    void SetRedraw(bool bEnable);
    void ClearCaches();

    // IVertexRenderer 
    void GetTitlePos(GraphTypes::RectF& pos){GetBoundsBox(pos);}
    void GetImagesBoundsBox(GraphTypes::RectF& box){ box.x = m_position.x; box.y = m_position.y; box.Width = m_imposedSize.Width; box.Height = m_imposedSize.Height; return;};
    bool IsSelected(){return CScaledWindow::IsSelected();};
    bool GetPinned(){return false;};
    // IAttVertexRenderer 
    bool IsHot() const;	

protected:
    BEGIN_MSG_MAP(CAttVertexRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

using namespace GraphTypes;

REAL CAttVertexRenderer::m_borderLineThickness(2.0);
REAL CAttVertexRenderer::m_borderMinThickness(0.5);

//*****************************************************************************

CAttVertexRenderer::CAttVertexRenderer(CCanvas & canvas)
    : CScaledWindow(&canvas),
    m_hotTracking(&canvas, this),
    m_v(0)
{

}

CAttVertexRenderer::~CAttVertexRenderer()
{
    if(m_hotTracking.IsHot())
        m_hotTracking.SetHot(false);
}

//-----------------------------------------------------------------------------

CUniqueID CAttVertexRenderer::GetID()
{
    return m_uniqueID;
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetGraphItem(IGraphItem* item)
{
    CComQIPtr<IGraphVertex> v = item;

    if(m_v)
        m_v->Disconnect(m_sigConn);

    m_v = v;
    ATLASSERT(m_v);
    m_callback = m_v->GetPropertyUnknown(PROP_CALLBACK);
    m_sigConn=m_v->Connect(boost::ref(*this));
    // must store ID because vertex may be gone in renderer's destructor
    m_uniqueID=m_v->GetID();
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    box=m_rcWnd;
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
    RectF rc=box;
    if(co==screen)
    {
        m_canvas->RAllTransform(rc);
    }

    m_imposedSize.Width=rc.Width;
    m_imposedSize.Height=rc.Height;
    m_position.x=rc.x; 
    m_position.y=rc.y;
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    pt=m_position;

    if(co==screen)
    {
        m_canvas->AllTransform(pt);
    }
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    m_position=pt;
    if(co==screen)
    {
        m_canvas->RAllTransform(m_position);		
    }
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::SendMessage(
    UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetFocus(bool bSet)
{
    // parent will never have focus but the other guy must lose it 
    if(bSet)
        m_canvas->ClearFocus();
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
    bool oldSelected = m_bSelected;
    CScaledWindow::SetSelected(GetID(), bSelected, bAdd);

    if (bSelected != oldSelected)
    {
        m_canvas->NotifySelection(GetID());
    }
}

//-----------------------------------------------------------------------------

SizeF CAttVertexRenderer::CalcDisplaySizes(CDCHandle& dc, bool bForce, bool bIgnoreExtent)
{
    SizeF extentf;
    SizeF size=m_imposedSize;
    m_canvas->SizeTransform(size);
    extentf = size;

    CRect wndrc;

    wndrc.left=m_rcWnd.left;
    wndrc.top=m_rcWnd.top;
    wndrc.right=wndrc.left+round_int(extentf.Width);
    wndrc.bottom=wndrc.top+round_int(extentf.Height);

    bool bPositionChange=false;

    PointF p = m_position;
    m_canvas->AllTransform(p);
    CPoint ip=p;

    if(bForce || ip.x!=m_rcWnd.left || ip.y!=m_rcWnd.top)
    {
        wndrc.left=ip.x;
        wndrc.top=ip.y;	
        wndrc.right=wndrc.left+round_int(extentf.Width);
        wndrc.bottom=wndrc.top+round_int(extentf.Height);
        bPositionChange=true;
    }

    bool bSizeChange = false;
    if(!bSizeChange)
        bSizeChange=wndrc.Height()!=m_rcWnd.Height() || 
            wndrc.Width()!=m_rcWnd.Width();

    UINT flags=(bSizeChange?0:SWP_NOSIZE)|(bPositionChange?0:SWP_NOMOVE)|SWP_NOREDRAW;
    SetWindowPos(wndrc, flags);

    return extentf;
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::ClearCaches()
{
    m_sizeTracker.Reset();
    m_textBmp.Clear();
}

//-----------------------------------------------------------------------------

bool CAttVertexRenderer::IsHot() const
{
    return m_hotTracking.IsHot();
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::operator()(GRAPHNOTIFICATION notification, IGraphItem * item)
{
    switch(notification)
    {
    case GRAPHNOTIFICATION_LABELCHANGED:
        ClearCaches();
        break;
    }
}
//-----------------------------------------------------------------------------

void CAttVertexRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
    CPoint offset=pos.TopLeft()-m_rcWnd.TopLeft();

    if(flags&SWP_NOSIZE)
    {
        // calling us with no move and no size makes no sense but,
        // for now we have CalcDisplaySizes invoked before each paint
        // and it invokes us, so return for now
        if(flags&SWP_NOMOVE)
            return;

        ATLASSERT(!(flags&SWP_NOMOVE)); // no move, no size, no sense for now

        m_rcWnd.OffsetRect(offset);
    }
    else
    {
        if(flags&SWP_NOMOVE)
        {
            m_rcWnd.right=m_rcWnd.left+pos.Width();
            m_rcWnd.bottom=m_rcWnd.top+pos.Height();			
        }
        else
        {
            m_rcWnd=pos;
        }
    }

    if(!(flags&SWP_NOREDRAW))
    {
        Invalidate();
    }
}

//-----------------------------------------------------------------------------

void CAttVertexRenderer::Invalidate(CRect* rc/*=0*/, bool bErase/*=false*/) 
{ 
    // unfortunately, when redrawing in normal state, there are drawing
    // defects on surrounding vertices at lower zoom levels, so redraw the entire diagram. 
    // When state is hot we can still pass with redrawing just the vertex area

    //if(rc!=0 && m_hotTracking.IsHot())
    //{
    //	RectF rcf=CalcVertexRect(true); 
    //	rcf.Inflate(m_borderLineThickness, m_borderLineThickness);
    //	CRect frameRect=rcf;
    //	m_canvas->Invalidate();
    //}
    //else
    {
        m_canvas->Invalidate();
    }
}

//-----------------------------------------------------------------------------

RectF CAttVertexRenderer::CalcVertexRect()
{
    PointF pt = m_position;
    m_canvas->AllTransform(pt);
    SizeF extentf;
    if(m_imposedSize.Width!=0 && m_imposedSize.Height!=0)
    {
        extentf=m_imposedSize;
        // when hot state is zoomed over 100% scale it as if it is not hot
        m_canvas->SizeTransform(extentf);
    }
    else
    {
        //if(hot)
        //	extentf=m_sizeTracker.m_extent;
        //else
            extentf=m_sizeTracker.m_scaledExtent;
    }

    // center hot vertex rectangle around text, unless zoom is > 100% 
    RectF rcf(pt, extentf); 
    //if(hot && m_canvas->GetScale()<1)
    //{
    //	m_canvas->SizeTransform(extentf);
    //	rcf.Offset((extentf.Width-m_imposedSize.Width)/2, (extentf.Height-m_imposedSize.Height)/2);
    //}
    return rcf;
}

//-----------------------------------------------------------------------------

SizeF CAttVertexRenderer::CacheText(
    CDCHandle& dc, 
    bool bForce,
    bool bIgnoreExtent)
{
    bool bSizeChange=false;
    SizeF extentf;

    if(m_sizeTracker.Update(m_canvas) || bForce)
    {
        bSizeChange=true;

        std::_tstring label = m_v->GetLabel();
        if (!label.length())
        {
            label = boost::lexical_cast<std::_tstring>(m_v->GetID().GetCategory().GetCategory()).c_str();
            label += _T(":");
            label += m_v->GetID().GetID();
        }

        FontInfo fi(
            &m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
            0,
            m_canvas->m_fonts,
            static_cast<GraphTypes::REAL>((DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72));

        CDrawVertexHelper tdh;
//		tdh.m_borderwidth=1;
        tdh.m_text=label.c_str();
//		tdh.m_margins=m_textMargins;
        tdh.m_printing=m_canvas->IsPrinting();
        tdh.m_scale=m_canvas->GetScale();

        if(m_canvas->IsPrinting())
        {
            extentf=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];

            if(bForce || extentf.Height==0)
            {
                extentf=tdh.CalcSize(dc, fi);
                m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()]=extentf;
            }
        }
        else
        {
            m_sizeTracker.m_scaledExtent=extentf=tdh.CalcSize(dc, fi);
            if(m_sizeTracker.m_extent.Width==0)
            {
                tdh.m_scale=1;
                m_sizeTracker.m_extent=tdh.CalcSize(dc, fi);
            }
        }

        m_textBmp.Clear();
    }

    if(m_imposedSize.Width!=0 && m_imposedSize.Height!=0)
    {
        SizeF size=m_imposedSize;
        m_canvas->SizeTransform(size);
        extentf=size;
    }

    return extentf;
}

void CAttVertexRenderer::DrawVertex(CAggDC& dc, const GraphTypes::Color &borderColor, const GraphTypes::Color & fillColor, const GraphTypes::Color & fontColor)
{
    CacheText(CDCHandle(dc));
    std::_tstring label = m_v->GetLabel();
    if (!label.length())
    {
        label = boost::lexical_cast<std::_tstring>(m_v->GetID().GetCategory().GetCategory()).c_str();
        label += _T(":");
        label += m_v->GetID().GetID();
    }

    REAL scale=m_canvas->GetScale();
    RectF rcf=CalcVertexRect(); 

    //label = (boost::_tformat(_T("%1%, %2%, %3%, %4%")) % rcf.x % rcf.y % rcf.Width % rcf.Height).str();

    REAL border = m_borderLineThickness * scale;
    if(border<m_borderMinThickness)
        border=m_borderMinThickness;
    rcf.Inflate(-border/2,-border/2);

    CRect frameRect=rcf;

    if(!frameRect.Width() || !frameRect.Height())
        return;

    // background
    PointF tl(rcf.x, rcf.y), br(rcf.x+rcf.Width, rcf.y+rcf.Height);
    dc.Rect(tl, br, fillColor, borderColor, border, true);

    FontInfo fi(
        &m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
        fontColor,
        m_canvas->m_fonts,
        static_cast<GraphTypes::REAL>((DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72));

    CDrawVertexHelper tdh;
    tdh.m_text=label.c_str();
    tdh.m_cbkgnd=fillColor;
    tdh.m_printing=m_canvas->IsPrinting();
    tdh.m_scale=m_canvas->GetScale();

    if(m_canvas->IsPrinting())
    {
        tdh.Draw(dc, fi, rcf, DT_CENTER);
    }
    else
    {
        //if(m_hotTracking.IsHot())
        //{
        //	if(tdh.m_scale<1)
        //		tdh.m_scale=1;
        //	tdh.Draw(dc, fi, rcf, DT_CENTER);
        //}
        //else
        {
            if(m_fillColor.GetValue() != fillColor.GetValue() || m_textBmp.IsEmpty())
            {
                CSize size=m_sizeTracker.m_scaledExtent;
                boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmp.BeginPaint(dc, size, m_canvas->m_fonts));
                RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
                tdh.Draw(*tmpaggdc, fi, rcf, DT_CENTER | DT_TRANSPARENT);
                m_textBmp.EndPaint(tmpaggdc.get());
                m_fillColor = fillColor;
            }
            m_textBmp.Draw(dc, frameRect, DT_CENTER);
        }
    }
}

//*****************************************************************************
//
// Message handlers
//

LRESULT CAttVertexRenderer::OnPaint(
    UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CAggDC& dc=*(CAggDC*)wParam;

    //ATLASSERT(m_callback);
    if(m_callback && (!m_canvas->m_meta.IsEmulateDecoration() || dc.RectVisible(&m_rcWnd)))
    {
        Color fillColor, borderColor, fontColor;
        m_callback->GetColor(m_v, borderColor, fillColor, fontColor);
        DrawVertex(dc, borderColor, fillColor, fontColor); 
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnLButtonDown(
    UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    // nobody can be hot when clicked
    m_canvas->ClearCurrentHot();
    SetSelected();
    if(m_canvas->GetApplicationZoomMode()!=AZMSelection)
    {
        CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
        m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);
    }
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnLButtonUp(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if(m_canvas->GetApplicationZoomMode()==AZMSelection)
        m_hotTracking.SetHot(true);
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnMouseMove(
    UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if(m_canvas->GetApplicationZoomMode()==AZMSelection)
        if(!(wParam&MK_LBUTTON)) // hot when mouse not pressed down
            m_hotTracking.SetHot(true);

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnMouseDblclk(
    UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnNcHitTest(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    return reinterpret_cast<LRESULT>(this);
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnSetCursor(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    SetCursor(m_canvas->m_HandArrowCursor);
    return 1;
}

//-----------------------------------------------------------------------------

LRESULT CAttVertexRenderer::OnContextMenu(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
    CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
    LRESULT lRes=m_canvas->NotifyContextMenu(SCW_NOTIFYVERTEXCONTEXT, m_v->GetID(), pt);
    bHandled=lRes!=0;
    return lRes;
}

//*****************************************************************************

IRenderer* CreateAttVertexRenderer(CCanvas & canvas)
{
    CAttVertexRenderer * retVal = new CAttVertexRenderer(canvas);
    return retVal;
}

