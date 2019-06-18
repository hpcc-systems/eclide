#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "SizeTracker.h"

#define __SKIP_INVISIBLE__

class CLegacyVertexRenderer : public ILegacyRenderer, public CScaledWindow,	public boost::signals::trackable
{
    IGraphVertex * m_v;	//Should not be a CComPtr as this would create a circular reference.
    MARGINS m_textMargins;
    CComPtr<IImageRenderer> m_image;
    CButtonState m_hotTracking;
    boost::signals::connection m_sigConn;

    GraphTypes::PointF m_position;
    bool m_bPinned;

    CAggBitmap m_textBmpNormal;
    CAggBitmap m_textBmpSelected;
    CSizeTracker m_sizeTracker;

    GraphTypes::PointF m_savePos;
    GraphTypes::PointF m_capturePos;
    bool m_hitVertex;

    void CalcColors(
        GraphTypes::Color& fillColor,
        GraphTypes::Color& borderColor,
        GraphTypes::Color& fontColor);

    void Draw(CAggDC& dc);
    bool CalcVisibility();
    virtual void SetWindowPos(const CRect& pos, UINT flags);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(ILegacyRenderer)
        IMPLEMENT_INTERFACE(IVertexRenderer)
        IMPLEMENT_INTERFACE(IRenderer)
    END_CUNKNOWN(CScaledWindow)

    CLegacyVertexRenderer(CCanvas & canvas);
    ~CLegacyVertexRenderer();

    virtual void Invalidate(CRect* rc=0, bool bErase=false); 
    void operator()(GRAPHNOTIFICATION modification, IGraphItem * item);

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

    // ILegacyRenderer
    void GetTitlePos(GraphTypes::RectF& rcpos);
    void GetImagesBoundsBox(GraphTypes::RectF& box);
    bool IsSelected();
    bool IsHot() const;
    bool GetPinned();
    void SetPinned(bool pinned);
    bool GetDragging();
    LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
         UINT uMsg, WPARAM wParam, LPARAM lParam);


protected:
    BEGIN_MSG_MAP(CLegacyVertexRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

using namespace GraphTypes;

CLegacyVertexRenderer::CLegacyVertexRenderer(CCanvas & canvas)
    : CScaledWindow(&canvas),
    m_v(0),
    m_hotTracking(&canvas, this),
    m_bPinned(false)
{
    m_textMargins.cxLeftWidth=4;
    m_textMargins.cxRightWidth=4;
    m_textMargins.cyTopHeight=4;
    m_textMargins.cyBottomHeight=4;
}

//-----------------------------------------------------------------------------

CLegacyVertexRenderer::~CLegacyVertexRenderer()
{
    if(m_hotTracking.IsHot())
    {
        m_hotTracking.SetHot(false);
        m_canvas->NotifyHotChanged(GetID(), false);
    }
}

//-----------------------------------------------------------------------------

CUniqueID CLegacyVertexRenderer::GetID()
{
    return m_v->GetID();
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetGraphItem(IGraphItem* item)
{
    CComQIPtr<IGraphVertex> v=item;
    ATLASSERT(v);

    if(m_v)
        m_v->Disconnect(m_sigConn);
    m_v=v;
    m_sigConn=m_v->Connect(boost::ref(*this));

    m_image=reinterpret_cast<IImageRenderer*>(CreateImageRenderer(*m_canvas, item));
    ATLASSERT(m_image);
    // vertices can have several renderers in timeline mode
    m_image->SetParentRenderer(this);
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    GetTitlePos(box);
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetBoundsBox(const RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
    RectF rc = box;
    if(co==world)
    {
        m_canvas->AllTransform(rc);
    }
    SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    pt=m_position;

    if(co==screen)
    {
        m_canvas->AllTransform(pt);
    }
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    m_position=pt;

    if(co==screen)
    {
        m_canvas->RAllTransform(m_position);		
    }
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
    bool oldSelected = m_bSelected;
    CScaledWindow::SetSelected(GetID(), bSelected, bAdd);

    if (bSelected != oldSelected)
    {
        m_canvas->NotifySelection(GetID());
    }
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetFocus(bool bSet/*=true*/)
{
    bool bChange=bSet!=HasFocus();
    CScaledWindow::SetFocus(bSet);
    if(bChange)
        m_canvas->NotifyFocusChanged(GetID(), bSet);
    if(bSet)
        m_v->MoveToEnd();
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

GraphTypes::SizeF CLegacyVertexRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce,
    bool bIgnoreExtent)
{
    SizeF extentf;

    std::_tstring label = m_v->GetLabel();
    if (!label.length())
    {
        label = boost::lexical_cast<std::_tstring>(m_v->GetID().GetCategory().GetCategory()).c_str();
        label += _T(":");
        label += m_v->GetID().GetID();
    }

    if(m_sizeTracker.Update(m_canvas))
    {
        FontInfo fi(
            &m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
            0,
            m_canvas->m_fonts,
            static_cast<GraphTypes::REAL>((DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72));

        CDrawVertexHelper tdh;
        tdh.m_borderwidth=1;
        tdh.m_text=label.c_str();
        tdh.m_margins=m_textMargins;
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

        m_textBmpNormal.Clear();
        m_textBmpSelected.Clear();
    }

    ATLASSERT(m_image!=0);
    m_image->CalcDisplaySizes(dc, bForce, bIgnoreExtent);

    return extentf;
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::ClearCaches()
{
    m_sizeTracker.Reset();
    m_textBmpNormal.Clear();
    m_textBmpSelected.Clear();
}

//-----------------------------------------------------------------------------
//
// text rectangle in screen coordinates
//
void CLegacyVertexRenderer::GetTitlePos(GraphTypes::RectF& rcpos)
{
    SizeF size;

    if(m_canvas->IsPrinting())
    {
        size=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];
    }
    else
    {
        if(m_hotTracking.IsHot())
            size=m_sizeTracker.m_extent;
        else
            size=m_sizeTracker.m_scaledExtent;
    }

    PointF pos=m_position;
    m_canvas->AllTransform(pos);
    RectF rcf(pos, size);
    rcf.Offset(-rcf.Width/2,-rcf.Height/2);
    m_rcWnd = static_cast<CRect>(rcf);
    rcpos=rcf;
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::GetImagesBoundsBox(GraphTypes::RectF& box)
{
    m_image->GetBoundsBox(box);
}

//-----------------------------------------------------------------------------

bool CLegacyVertexRenderer::IsSelected()
{
    return m_bSelected; // member in CScaledWindow 
}

//-----------------------------------------------------------------------------

bool CLegacyVertexRenderer::IsHot()	const
{
    return m_hotTracking.IsHot();
}

//-----------------------------------------------------------------------------

bool CLegacyVertexRenderer::GetPinned()
{
    return m_bPinned;
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetPinned(bool pinned)
{
    if(pinned!=m_bPinned)
    {
        m_bPinned=pinned;
        Invalidate();
    }
}
//-----------------------------------------------------------------------------

bool CLegacyVertexRenderer::GetDragging()
{
    if(m_canvas->GetCapture()==this && IsSelected())
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::HitTestDispatchMsg(const GraphTypes::PointF& point,
    bool& hit, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes=0;
    hit=false;

    RectF rctext;
    GetTitlePos(rctext);

    if(rctext.Contains(point))
    {
        hit=true;
        if(uMsg!=WM_USER) // known message we do not process, just hit testing
            lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
        if(uMsg==WM_NCHITTEST)
            m_hitVertex=true;
        else if(uMsg==WM_LBUTTONDOWN)
        {
            // image selection is orthogonal to vertex selection so
            // images won't be deselected automaticaly when vertex is selected
            if(IsSelected())
                m_canvas->m_selectedImage.Clear();
        }
    }
    else
    {
        ATLASSERT(m_image);
        lRes=m_image->HitTestDispatchMsg(point, hit, uMsg, wParam, lParam);
        if(uMsg==WM_NCHITTEST)
        {
            m_hitVertex=false;
        }
        else if(uMsg==WM_RBUTTONDOWN)
        {
            if(hit)
                lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
        }
        else if(uMsg==WM_LBUTTONDOWN)
        {
            if(hit && !m_image->HasCapture())
                lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
        }
        else if(uMsg==WM_LBUTTONDBLCLK)
        {
            if(hit && lRes) // image render returns 1 if no resizing handle was hit
                lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
        }
    }

    return lRes;
}

//-----------------------------------------------------------------------------

bool CLegacyVertexRenderer::CalcVisibility()
{
    bool visible=true;
#ifdef __SKIP_INVISIBLE__
    RectF rcunion, rcimages;
    GetTitlePos(rcunion);

    GetImagesBoundsBox(rcimages);

    if(!rcimages.IsEmptyArea())
        rcunion.Union(rcunion, rcunion, rcimages);

    RectF vp;
    m_canvas->GetViewport(vp);
    visible=vp.IntersectsWith(rcunion)!=FALSE;
#endif
    return visible;
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
    RectF rc(pos);
    m_position.x=rc.x+rc.Width/2.0f;
    m_position.y=rc.y+rc.Height/2.0f;
    m_canvas->RAllTransform(m_position);

    CScaledWindow::SetWindowPos(pos, flags);
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::Invalidate(CRect* rc/*=0*/, bool bErase/*=false*/) 
{
    if(IsRedraw())
        m_canvas->Invalidate();
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::operator()(
    GRAPHNOTIFICATION notification, IGraphItem * item)
{
    switch(notification)
    {
    case GRAPHNOTIFICATION_LABELCHANGED:
        ClearCaches();
        break;
    }
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::CalcColors(
    Color& fillColor, Color& borderColor, Color& fontColor)
{
    CCategory cat = m_v->GetCategory();
    fontColor=m_canvas->m_meta.GetMetaVertexFontColor(cat);

    // don't draw hot state on printer device
    if(m_hotTracking.IsHot() && !m_canvas->IsPrinting())
    {
        fillColor=m_canvas->m_meta.GetMetaVertexColorFillOver(cat);
        borderColor=m_canvas->m_meta.GetMetaVertexColorBorderOver(cat);
        fontColor=m_canvas->m_meta.GetMetaVertexFontColorSel(cat);
    }
    else
    {
        if(IsSelected())
        {
            fillColor=m_canvas->m_meta.GetMetaVertexColorFillSel(cat);
            borderColor=m_canvas->m_meta.GetMetaVertexColorBorderSel(cat);
            fontColor=m_canvas->m_meta.GetMetaVertexFontColorSel(cat);
        }
        else
        {

            // neither selected nor hot/over
            fillColor=m_canvas->m_meta.GetMetaVertexColorFillNorm(cat);
            borderColor=m_canvas->m_meta.GetMetaVertexColorBorderNorm(cat);

            if (m_v->GetBorderColor().GetValue() != 0x0)
                borderColor.SetValue(m_v->GetBorderColor().GetValue());
            if (m_v->GetFillColor().GetValue() != 0x0)
                fillColor.SetValue(m_v->GetFillColor().GetValue());
            if (m_v->GetFontColor().GetValue() != 0x0)
                fontColor.SetValue(m_v->GetFontColor().GetValue());
        }
    }
}

//-----------------------------------------------------------------------------

void CLegacyVertexRenderer::Draw(CAggDC& dc) 
{
    std::_tstring label = m_v->GetLabel();
    if (!label.length())
    {
        label = boost::lexical_cast<std::_tstring>(m_v->GetID().GetCategory().GetCategory()).c_str();
        label += _T(":");
        label += m_v->GetID().GetID();
    }

    RectF rcf;
    GetTitlePos(rcf);
    CRect frameRect=static_cast<CRect>(rcf);

    if(!frameRect.Width() || !frameRect.Height())
        return;

    Color fillColor, borderColor, fontColor;
    CalcColors(fillColor, borderColor, fontColor);

    FontInfo fi(
        &m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
        fontColor,
        m_canvas->m_fonts,
        static_cast<GraphTypes::REAL>((DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72));

    CDrawVertexHelper tdh;
    tdh.m_text=label.c_str();
    tdh.m_borderwidth=1.0;
    tdh.m_cbkgnd=fillColor;
    tdh.m_cborder=borderColor;
    tdh.m_margins=m_textMargins;
    tdh.m_printing=m_canvas->IsPrinting();
    tdh.m_scale=m_canvas->GetScale();
    tdh.m_focusRect=HasFocus()&& frameRect.Height()>8;
    tdh.m_pins=m_bPinned;

    if(m_canvas->IsPrinting())
    {
        tdh.Draw(dc, fi, rcf);
    }
    else
    {
        if(m_hotTracking.IsHot())
        {
            tdh.m_scale=1;
            RectF rchot(
                REAL(frameRect.left), 
                REAL(frameRect.top),
                REAL(frameRect.Width()), 
                REAL(frameRect.Height()));

            tdh.Draw(dc, fi, rchot);
        }
        else
        {
            if(IsSelected())
            {
                if(m_textBmpSelected.IsEmpty())
                {
                    CSize size=m_sizeTracker.m_scaledExtent;

                    boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpSelected.BeginPaint(dc, size, m_canvas->m_fonts));
                    RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
                    CDrawVertexHelper tmptdh(tdh);
                    tmptdh.m_focusRect=false;
                    tmptdh.m_pins=false;
                    tmptdh.Draw(*tmpaggdc, fi, rcf);
                    m_textBmpSelected.EndPaint(tmpaggdc.get());
                }

                m_textBmpSelected.Draw(dc, frameRect, 0);
                tdh.DrawFocusAndPin(dc, rcf);
            }
            else
            {
                if(m_textBmpNormal.IsEmpty())
                {
                    CSize size=m_sizeTracker.m_scaledExtent;

                    boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpNormal.BeginPaint(dc, size, m_canvas->m_fonts));
                    RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
                    CDrawVertexHelper tmptdh(tdh);
                    tmptdh.m_focusRect=false;
                    tmptdh.m_pins=false;
                    tmptdh.Draw(*tmpaggdc, fi, rcf);
                    m_textBmpNormal.EndPaint(tmpaggdc.get());
                }

                m_textBmpNormal.Draw(dc, frameRect, 0);
                tdh.DrawFocusAndPin(dc, rcf);
            }
        }
    }
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnPaint(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    if(m_canvas->m_meta.IsVisible(m_v))
    {
        CAggDC& dc=*(CAggDC*)wParam;

        // for printer device we don't calculate visibility
        if(m_canvas->IsPrinting() || CalcVisibility())
        {
            Draw(dc);
            m_image->SendMessage(uMsg, wParam, lParam);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
//
//	The very fact thatwe are getting this message means that upper level code
//	found mouse clicked either over this vertex or over one of its images.
//

LRESULT CLegacyVertexRenderer::OnLButtonDown(
    UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    // nobody can be hot when we get capture
    m_canvas->ClearCurrentHot();

    SetFocus();

    if((wParam & MK_CONTROL)==0 && m_canvas->CountSelected()<2)
    {
        SetSelected();
    }

    if(m_canvas->GetMouseMode()!=MOUSE_MODE_DRAW_LINKS)
    {
        m_canvas->SetCapture(this);
        m_capturePos=pt;

        RectF rcf;
        GetTitlePos(rcf);

        m_savePos.x=rcf.x;
        m_savePos.y=rcf.y;
    }

    return reinterpret_cast<LRESULT>(m_v);
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnRButtonDown(
    UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    m_canvas->ClearCurrentHot();

    // like in Windows Explorer selection does not change if CTRL key
    // is pressed and if user clicks over already selected item
    if((wParam & MK_CONTROL)==0)
    {
        if(!IsSelected())
            SetSelected();
    }

    return reinterpret_cast<LRESULT>(m_v);
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnLButtonUp(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    if(m_canvas->GetCapture()==this)
    {
        m_canvas->ReleaseCapture();

        RectF rcf;
        GetTitlePos(rcf);
        m_rcWnd=static_cast<CRect>(rcf);

        if(m_rcWnd.PtInRect(pt))
        {
            m_hotTracking.SetHot(true);			
            m_canvas->NotifyHotChanged(GetID(), true);
            // when hot become topmost to avoid other vertices showing through
            m_v->MoveToEnd();
            if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
                m_canvas->SetHilightID(GetID());
        }

        if((wParam & MK_CONTROL)!=0)
        {
            SetSelected(!IsSelected(), true);
        }
        else if(m_canvas->CountSelected()>1)
        {
            SetSelected(true, (wParam & MK_SHIFT)!=0 );
        }
    }

    m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnMouseMove(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    if(m_canvas->GetCapture()==this)
    {
        // nobody could be hot during capture
        m_canvas->ClearCurrentHot();

        RectF rcf;
        GetTitlePos(rcf);
        CRect rc=static_cast<CRect>(rcf);

        CPoint offset(
            pt.x+round_int(m_savePos.x-m_capturePos.x)-rc.left,
            pt.y+round_int(m_savePos.y-m_capturePos.y)-rc.top);

        if (offset.x == 0 && offset.y == 0)
            return 0;

        rc.OffsetRect(offset);
        SetWindowPos(rc, SWP_NOSIZE);

        CUniqueID singleItem; // starts empty, i.e. item(s) selected

        if(IsSelected())
        {
            m_canvas->MoveAllSelected(offset, this);
        }
        else
        {
            singleItem = GetID();
        }

        RectF viewRect;
        m_canvas->GetViewport(viewRect);
        CRect rcv=static_cast<CRect>(viewRect); // for int instead of float comparisons

        if (m_canvas->DragDropNotifications() && !rcv.PtInRect(pt))
        {
            m_canvas->ReleaseCapture();
            if(IsSelected())
            {
                offset.x = round_int(m_savePos.x - rcf.x) - offset.x;
                offset.y = round_int(m_savePos.y - rcf.y) - offset.y;
                m_canvas->MoveAllSelected(offset, NULL);
            }
            m_canvas->NotifyDoDragDrop(m_capturePos);
            return 0;
        }
    }
    else
    {
        // when hot become topmost to avoid other vertices showing through
        m_v->MoveToEnd();
        m_hotTracking.SetHot(true);
        m_canvas->NotifyHotChanged(GetID(), true);
        if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
            m_canvas->SetHilightID(GetID());
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnMouseDblclk(
    UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnNcHitTest(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return HTCLIENT;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyVertexRenderer::OnSetCursor(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes=1;

    if(m_hitVertex)
    {
        MOUSE_MODES mode=m_canvas->CalcMouseMode();
        if(mode==MOUSE_MODE_DRAW_LINKS)
            m_canvas->SetModeCursor();
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    else
    {
        lRes=m_image->SendMessage(uMsg, wParam, lParam);
    }

    return lRes;
}

//*****************************************************************************

IRenderer *CreateLegacyVertexRenderer(CCanvas & parent)
{
    CLegacyVertexRenderer * retVal = new CLegacyVertexRenderer(parent);
    return retVal;
}
