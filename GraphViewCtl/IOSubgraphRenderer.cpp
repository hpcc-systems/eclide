#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "ScaledList.h"
#include "AggDC.h"
#include "AggHitTest.h"

class CIOSubgraphRenderer : public IIOSubgraphRenderer, public CScaledWindow
{
    IGraphSubgraph* m_g;
    unsigned m_I, m_O; 

    CPartTheme& m_wndTheme;
    CPartTheme& m_statusTheme;

    CButtonState m_ButtonState;

    CRect m_rcCaption;
    CRect m_rcButton; 
    CRect m_rcList;

    GraphTypes::Color m_colorBkgnd;
    GraphTypes::Color m_colorBorder;
    GraphTypes::Color m_colorActiveTitleBar;
    GraphTypes::Color m_colorTitleBar;
    GraphTypes::Color m_colorActiveTitle;
    GraphTypes::Color m_colorInactiveTitle; 

    // conn colors with edge attached and without
    GraphTypes::Color m_colorEdgeBkgnd;
    GraphTypes::Color m_colorNoEdgeBkgnd;
    GraphTypes::Color m_colorOutline;

    WINDOW_SIZE m_size;
    HIT_TEST_CODE m_capturedPart;
    GraphTypes::REAL m_scale;

    // sizes that vary with each DPI and renderer "window"
    CRectMap m_wnd_sizes; // current size and position 
    CSizeMap m_norm_sizes;
    CSizeMap m_max_sizes;
    CSizeMap m_max_sizes_list;
    CSizeMap m_sizes_list_item; // cx contains max input item width

    // sizes per each DPI that don't vary with each renderer "window"
    static CSizeMap m_captionSizes;
    static CSizeMap m_min_sizes;

    // sizes that are either fixed per DPI or dynamically scaled
    static CSize m_wndBorder;
    static BORDER_STYLE m_wndBorderStyle;
    static MARGINS m_captionTextMargin;
    static MARGINS m_captionContentMargin;
    static MARGINS m_listMargin;
    static int m_captionButtonTextGap;

    // conn image dimensions in terms of list item text height
    GraphTypes::SizeF m_connFactor; 
    GraphTypes::SizeF m_dividerLine;
    bool m_bITaller; // if input size height is larger than output

    GraphTypes::PointF m_position;
    CPoint m_savePos;
    CPoint m_capturePos;

    int CalcCaptionSize(CDCHandle& dc, const TCHAR* title);
    int CalcCaptionTotalHeight(void);
    void Draw(CAggDC& dc); 
    void CalcWindowBorder(BORDER_STYLE style);
    void DrawCaption(CAggDC& dc, CAPTION_STYLE style);
    void DrawWindowBkgnd(CAggDC& dc); 
    void CalcButtonRect(CRect& rc);
    void DrawButton(CAggDC& dc);
    CSize CalcListMaxSize(CDCHandle& dc);
    void CalcListRect(CRect& rc);
    void AdjustChildrenBBox(void);
    void DrawList(CAggDC& dc);
    void DrawEdges(CAggDC& dc);
    void DrawConnectors(CAggDC& dc);
    GraphTypes::SizeF GetConnectorSize();

    HIT_TEST_CODE HitTestCaption(const CPoint& p);
    HIT_TEST_CODE HitTestButton(const CPoint& p);
    IGraphVertex* HitTestConnectors(const GraphTypes::PointF& ptest, double tolerance=0);
    LRESULT ForwardMsgToChildren(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IIOSubgraphRenderer)
        IMPLEMENT_INTERFACE(IListWindowRenderer)
        IMPLEMENT_INTERFACE(IRenderer)
    END_CUNKNOWN(CScaledWindow)

    CIOSubgraphRenderer(CCanvas & canvas);
    ~CIOSubgraphRenderer();

    void SetWindowPos(const CRect& pos, UINT flags);
    virtual void Invalidate(CRect* rc=0, bool bErase=false) 
    { 
        m_canvas->Invalidate(rc,bErase);
    }

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

    // IIOSubgraphRenderer

    void GetContainerRect(GraphTypes::RectF& rc) const;
    IGraphItem* HitTestChildren(const GraphTypes::PointF& point);
    void GetConnectBounds(
        GraphTypes::RectF& left, 
        GraphTypes::RectF& right,
        unsigned order);
    bool IsSelected();
    bool IsMinimized() const;
    void OnRemoveGraphItem(IGraphItem* gi);
    bool OnEditItem(IGraphItem* gi);
    void SetWindowState(WINDOW_SIZE size, int topindex);
    void GetWindowState(WINDOW_SIZE& size, int& topindex) const;
    void SetWindowSize(const GraphTypes::SizeF& size);
    void GetWindowSize(GraphTypes::SizeF& size) const;

protected:
    BEGIN_MSG_MAP(CIOSubgraphRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

// height and default width (sample title) for each scale
CSizeMap CIOSubgraphRenderer::m_captionSizes;

// Minimum window height for each scale, *INCLUDING* caption
CSizeMap CIOSubgraphRenderer::m_min_sizes;

// these statics are either independent of scale factor
// or are scaled dynamically (may consider storing those in
// vector, for each scale factor)

CSize CIOSubgraphRenderer::m_wndBorder;
MARGINS CIOSubgraphRenderer::m_captionTextMargin;
MARGINS CIOSubgraphRenderer::m_captionContentMargin;
MARGINS CIOSubgraphRenderer::m_listMargin;

BORDER_STYLE CIOSubgraphRenderer::m_wndBorderStyle=DIALOG_FRAME;
// gap between caption text and first button
int CIOSubgraphRenderer::m_captionButtonTextGap=4;

using namespace GraphTypes;

//*****************************************************************************

CIOSubgraphRenderer::CIOSubgraphRenderer(CCanvas & canvas)
    : CScaledWindow(&canvas),
    m_g(0),
    m_I(0),
    m_O(0),
    m_wndTheme(canvas.GetPartTheme(_T("WINDOW"))),
    m_statusTheme(canvas.GetPartTheme(_T("STATUS"))),
    m_size(WINDOW_SIZE_MAX),
    m_capturedPart(HT_UNKNOWN),
    m_ButtonState(&canvas, this),
    m_scale(m_canvas->GetScale())
{
    m_style|=NO_MARGIN;

    if(m_style&NO_MARGIN)
    {
        m_listMargin.cxLeftWidth=0;
        m_listMargin.cxRightWidth=0;
        m_listMargin.cyTopHeight=0;
        m_listMargin.cyBottomHeight=0;
    }
    else
    {
        m_listMargin.cxLeftWidth=8;
        m_listMargin.cxRightWidth=8;
        m_listMargin.cyTopHeight=8;
        m_listMargin.cyBottomHeight=8;
    }

    m_captionTextMargin.cxLeftWidth=
    m_captionTextMargin.cxRightWidth=
    m_captionTextMargin.cyBottomHeight=
    m_captionTextMargin.cyTopHeight=0;

    m_captionContentMargin.cxLeftWidth=
    m_captionContentMargin.cxRightWidth=
    m_captionContentMargin.cyBottomHeight=
    m_captionContentMargin.cyTopHeight=2;

    m_colorBkgnd.SetFromCOLORREF(GetSysColor(COLOR_GRAYTEXT));
    m_colorBorder.SetFromCOLORREF(GetSysColor(COLOR_ACTIVEBORDER));
    m_colorOutline.SetFromCOLORREF(GetSysColor(COLOR_ACTIVEBORDER));

    m_colorActiveTitleBar.SetFromCOLORREF(GetSysColor(COLOR_ACTIVECAPTION));
    m_colorTitleBar.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));

    m_colorActiveTitle.SetFromCOLORREF(GetSysColor(COLOR_CAPTIONTEXT));
    m_colorInactiveTitle.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTIONTEXT));

    m_colorNoEdgeBkgnd.SetFromCOLORREF(GetSysColor(COLOR_ACTIVECAPTION));
//	m_colorEdgeBkgnd=m_colorNoEdgeBkgnd;
//	m_colorEdgeBkgnd.Lighten();	
    m_colorEdgeBkgnd.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));

    m_colorOutline.SetValue(Color::Black);

    m_connFactor.Height=REAL(0.4);
    m_connFactor.Width=REAL(0.6);

    m_dividerLine.Width=m_dividerLine.Height=1;
}

//-----------------------------------------------------------------------------

CIOSubgraphRenderer::~CIOSubgraphRenderer()
{
}

//-----------------------------------------------------------------------------

CUniqueID CIOSubgraphRenderer::GetID()
{
    return m_g->GetID();
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetGraphItem(IGraphItem* item)
{
    m_g=static_cast<IGraphSubgraph*>(item);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    if(m_size==WINDOW_SIZE_MIN)
    {
        box=m_rcWnd;
    }
    else
    {
        SizeF connsize=GetConnectorSize();
        CRect rcoutline(m_rcWnd);
        rcoutline.InflateRect(round_int(connsize.Width), 0, round_int(connsize.Width), 0);
        box=rcoutline;
    }
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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
    SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    pt=m_position;

    if(co==screen)
    {
        m_canvas->AllTransform(pt);
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    m_position=pt;
    if(co==screen)
    {
        m_canvas->RAllTransform(m_position);		
    }
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::SendMessage(
    UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetFocus(bool bSet)
{
    // parent will never have focus but the other guy must lose it 
    if(bSet)
        m_canvas->ClearFocus();
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::GetContainerRect(GraphTypes::RectF& rc) const
{
    rc=m_rcWnd;
}

//-----------------------------------------------------------------------------

IGraphItem* CIOSubgraphRenderer::HitTestChildren(const PointF& point)
{
    CComPtr<IGraphItem> gi(0);
    CPoint pt=static_cast<PointF>(point);

    if(m_rcClient.PtInRect(pt))
    {
        RectF rc;
        IGraphItemVector results;
        CComQIPtr<ITextboxRenderer> tb;
        CComPtr<IGraphItem> item;

        m_g->GetChildren(results);

        for(IGraphItemVector::iterator itr=results.begin();
            itr!=results.end();
            ++itr)
        {
            item=*itr;			
            tb=m_canvas->GetRenderer(item);
            if(tb!=0)
            {
                tb->GetBoundsBox(rc);
                if(rc.Contains(point))
                {
                    gi=item;
                    break;
                }
            }
        }
    }

    if(gi==0)
        gi=HitTestConnectors(point);

    return gi;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::GetConnectBounds(
    RectF& left, 
    RectF& right, 
    unsigned order)
{
    RectF rcL, rcR;
    CComQIPtr<ITextboxRenderer> tb;
    IGraphItemVector children;
    unsigned count=m_g->GetChildren(children);
    ATLASSERT(count && order<count);
    SizeF connsize=GetConnectorSize();

    tb=m_canvas->GetRenderer(children[order]);
    ATLASSERT(tb);

    if(m_size!=WINDOW_SIZE_MIN)
    {
        RectF rcitem;

        tb->GetBoundsBox(rcitem);
        rcL.y=rcR.y=rcitem.y+(rcitem.Height-connsize.Height)/2;
        if(rcL.y>m_rcWnd.bottom)
            rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;

        rcL.x=m_rcWnd.left-connsize.Width;
        rcR.x=m_rcWnd.right+connsize.Width-connsize.Height;
    }
    else
    {
        rcL.y=rcR.y=m_rcWnd.top+((REAL)CalcCaptionTotalHeight()-connsize.Height)/2;
        rcL.x=m_rcWnd.left-connsize.Width;
        rcR.x=m_rcWnd.right+connsize.Width-connsize.Height;
    }

    rcL.Height=rcR.Height=connsize.Height;
    rcR.Width=rcL.Width=connsize.Width;	

    ITextboxRenderer::eIOMode mode=tb->GetIOMode();

    if(mode==ITextboxRenderer::iomOut)
    {
        left=rcR;
        right=rcR;
    }
    else if(mode==ITextboxRenderer::iomIn)
    {
        left=rcL;
        right=rcL;
    }
    else
    {
        ATLASSERT(0);
        left=rcL;
        right=rcR;
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
    CScaledWindow::SetSelected(GetID(), bSelected, bAdd);
}

//-----------------------------------------------------------------------------

bool CIOSubgraphRenderer::IsSelected()
{
    return m_bSelected;
}

//-----------------------------------------------------------------------------

bool CIOSubgraphRenderer::IsMinimized() const
{
    return false;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::OnRemoveGraphItem(IGraphItem* gi)
{
    // clear cached sizes to cause recalc 
    ClearCaches();
}

//-----------------------------------------------------------------------------

bool CIOSubgraphRenderer::OnEditItem(IGraphItem* gi)
{
    bool bOk=false; // don't allow

    return bOk;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetWindowState(WINDOW_SIZE size, int topindex)
{
    topindex; // no meaning for this graph
    if(size!=m_size)
    {
        ATLASSERT(size!=WINDOW_SIZE_NORM);
        if(size==WINDOW_SIZE_MAX)
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        else if(size==WINDOW_SIZE_MIN)
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::GetWindowState(WINDOW_SIZE& size, int& topindex) const
{
    size=m_size;
    topindex=0;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetWindowSize(const GraphTypes::SizeF& size)
{
    // this renderer has fixed size - either max or min
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::GetWindowSize(GraphTypes::SizeF& size) const
{
    CRectEx wndrc;
    CRectMap::const_iterator itr=m_wnd_sizes.find(m_canvas->GetDpiScale());
    if(itr!=m_wnd_sizes.end())
        wndrc=itr->second;
    size.Width=static_cast<REAL>(wndrc.Width());
    size.Height=static_cast<REAL>(wndrc.Height());
}

//-----------------------------------------------------------------------------

SizeF CIOSubgraphRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce,
    bool bIgnoreExtent)
{
    SizeF size;
    IGraphItemVector children;
    unsigned count=m_g->GetChildren(children);

    ATLASSERT(count); 
    if(count==0)
        return size;

    bool bSizeChange=false;
    bool bPositionChange=false;

    REAL scale=m_canvas->GetScale();
    if(scale!=m_scale)
    {
        m_scale=scale;
        bForce=true;
    }

    CRect wndrc=m_wnd_sizes[m_canvas->GetDpiScale()];
    CSize sizelistmax=m_max_sizes_list[m_canvas->GetDpiScale()];
    CSize sizewndmax=m_max_sizes[m_canvas->GetDpiScale()];
    CSize sizewndmin=m_min_sizes[m_canvas->GetDpiScale()];
    CSize sizewndnorm=m_norm_sizes[m_canvas->GetDpiScale()];
    CSize sizecaption=m_captionSizes[m_canvas->GetDpiScale()];
    int hcaption;

    if(bForce || wndrc.Width()==0 || wndrc.Height()==0) 
    {
        bSizeChange=true;
        int wcaption, wdefault;

        std::_tstring str = m_g->GetLabel();

        wcaption=CalcCaptionSize(dc, str.c_str());
        wcaption+=
            m_captionTextMargin.cxLeftWidth+
            m_captionTextMargin.cxRightWidth+
            m_captionContentMargin.cxLeftWidth+
            m_captionContentMargin.cxRightWidth;

        sizelistmax=CalcListMaxSize(dc);
        m_max_sizes_list[m_canvas->GetDpiScale()]=sizelistmax;

        // default width 
        wdefault=sizelistmax.cx;
        wndrc.right=wndrc.left+wdefault;

        // border is included in total caption height
        CalcWindowBorder(m_wndBorderStyle);

        hcaption=CalcCaptionTotalHeight();

        // default height is maximum list height
        int hdefault=sizelistmax.cy;

        // minimum width for now is the same as default caption width
        sizewndmin.cx=sizecaption.cx;
        CSize sizeitem=m_sizes_list_item[m_canvas->GetDpiScale()];

        sizewndmin.cy=hcaption+sizeitem.cy;

        sizewndmax.cx=__max(sizelistmax.cx,wcaption);
        sizewndmax.cy=hcaption+hdefault;

        wndrc.bottom=wndrc.top+hcaption+hdefault; // initial height
        wndrc.InflateRect(m_wndBorder.cx, 0);
        wndrc.bottom+=m_wndBorder.cy; // top border included in caption total height

        sizewndmax.cx+=m_wndBorder.cx*2;
        sizewndmax.cy+=m_wndBorder.cy;
        sizewndmin.cy+=m_wndBorder.cy;

        // add caption button to window width

        CalcButtonRect(m_rcButton);
    
        int add=m_rcButton.Width()+
            m_captionContentMargin.cxRightWidth+
            m_captionButtonTextGap;

        ATLASSERT(add>0);

        wndrc.right+=add; 
        sizewndmax.cx+=add;

        MARGINS scaled = {0, 0, 0, 0};

        if(!(m_style&NO_MARGIN))
        {
            scaled=m_listMargin;
            m_canvas->ScaleMargins(scaled);
            wndrc.InflateRect(0, scaled.cyTopHeight, 0, scaled.cyBottomHeight);
        }

        if(!(m_style&NO_MARGIN))
            sizewndmax.cy+=scaled.cyTopHeight+scaled.cyBottomHeight;

        m_max_sizes[m_canvas->GetDpiScale()]=sizewndmax;

        if(!(m_style&NO_MARGIN))
            sizewndmin.cy+=scaled.cyTopHeight+scaled.cyBottomHeight;

        m_min_sizes[m_canvas->GetDpiScale()]=sizewndmin;

        sizewndnorm.cx=wndrc.Width();
        sizewndnorm.cy=wndrc.Height();
            
        m_norm_sizes[m_canvas->GetDpiScale()]=sizewndnorm;
    }

    PointF p = m_position;
    m_canvas->AllTransform(p);
    CPoint ip=static_cast<CPoint>(p);

    if(bForce || ip.x!=wndrc.left || ip.y!=wndrc.top)
    {
        int height=wndrc.Height();
        int width=wndrc.Width();

        wndrc.left=ip.x;
        wndrc.top=ip.y;	
        wndrc.right=wndrc.left+width;
        wndrc.bottom=wndrc.top+height;
        bPositionChange=true;
    }

    if(!bSizeChange)
        bSizeChange=wndrc.Height()!=m_rcWnd.Height() || 
            wndrc.Width()!=m_rcWnd.Width();

    UINT flags=(bSizeChange?0:SWP_NOSIZE)|(bPositionChange?0:SWP_NOMOVE)|SWP_NOREDRAW;
    SetWindowPos(wndrc, flags);

    size.Height=REAL(wndrc.Height());
    size.Width=REAL(wndrc.Width());
    return size;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::ClearCaches()
{
    m_wnd_sizes.clear(); // current size and position 
    m_norm_sizes.clear();
    m_max_sizes.clear();
    m_max_sizes_list.clear();
    m_sizes_list_item.clear(); // cx contains max input item width

    IGraphItemVector results;
    unsigned count = m_g->GetChildren(results);

    if(count>0)
    {
        CComPtr<IRenderer> ir;
        for(IGraphItemVector::iterator itr=results.begin();
            itr!=results.end();
            ++itr)
        {
            ir=m_canvas->GetRenderer(*itr); 
            ATLASSERT(ir);
            ir->ClearCaches();
        }
    }

    Invalidate();
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
    bool bRedraw=(flags&SWP_NOREDRAW)==0; 
    // parent redraw sends paint to all children,
    // so here we avoid sending redraw to children again
    flags|=SWP_NOREDRAW;
    CRect oldCaption=m_rcCaption;

    if(flags&SWP_NOSIZE)
    {
        // calling us with no move and no size makes no sense but,
        // for now we have CalcDisplaySizes invoked before each paint
        // and it invokes us, so return for now
        if(flags&SWP_NOMOVE)
            return;

        ATLASSERT(!(flags&SWP_NOMOVE)); // no move, no size, no sense for now
        CPoint offset=pos.TopLeft()-m_rcWnd.TopLeft();
        m_rcWnd.OffsetRect(offset);

        PointF newp(m_rcWnd.TopLeft());
        m_canvas->RAllTransform(newp);
        m_position=newp; 

        // offset all rects
        m_rcClient.OffsetRect(offset);
        m_rcCaption.OffsetRect(offset);

        m_rcButton.OffsetRect(offset);
        m_rcList.OffsetRect(offset);
        AdjustChildrenBBox();
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
            PointF newp(pos.TopLeft());
            m_canvas->RAllTransform(newp);
            m_position=newp; 
            m_rcWnd=pos;
        }

        int hicaption=CalcCaptionTotalHeight();

        m_rcCaption=m_rcWnd;

        m_rcCaption.bottom=m_rcCaption.top+hicaption;
        m_rcCaption.DeflateRect(m_wndBorder.cx, m_wndBorder.cy);

        if(m_size==WINDOW_SIZE_MIN)
        {
            m_rcClient.SetRectEmpty();
        }
        else
        {
            // calculate client rect, button and client rects
            m_rcClient=m_rcWnd;

            m_rcClient.DeflateRect(
                m_wndBorder.cx,
                hicaption,
                m_wndBorder.cx, 
                m_wndBorder.cy);

            CalcButtonRect(m_rcButton);
            CalcListRect(m_rcList);
            AdjustChildrenBBox();
        }
    }

    CRect rcex(m_rcWnd); 
    m_wnd_sizes[m_canvas->GetDpiScale()]=rcex;

    if(bRedraw)
        Invalidate();
}

//-----------------------------------------------------------------------------

int CIOSubgraphRenderer::CalcCaptionSize(CDCHandle& dc, const TCHAR* title)
{
    std::_tstring txt=_T("Example Title"); // for minimum caption width 

    CSize extent(0,0);
    CAutoFont f(m_canvas->GetCaptionFont(m_canvas->GetFontPixHeight()),dc);

    // remember height and average width for this scale
    dc.GetTextExtent(txt.c_str(), txt.length(), &extent);
    m_captionSizes[m_canvas->GetDpiScale()]=extent;

    // return title text extent at this scale (used for windows max width)
    dc.GetTextExtent(title, _tcslen(title), &extent);

    return extent.cx;
}

//-----------------------------------------------------------------------------
// 
// Currently we do not scale caption text margins and border. If we do, 
// consider pre-calculating everything and caching it.
//

int CIOSubgraphRenderer::CalcCaptionTotalHeight(void)
{
    int height=m_captionSizes[m_canvas->GetDpiScale()].cy;

    // we include top window border in total caption size
    // extra +border*2 is just for better looks
    
    height+=
        m_wndBorder.cy+
        m_captionContentMargin.cyTopHeight+
        m_captionContentMargin.cyBottomHeight+
        m_captionTextMargin.cyTopHeight+
        m_captionTextMargin.cyBottomHeight+
        m_wndBorder.cy*2;

    return int(height);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::Draw(CAggDC& dc)
{
    DrawConnectors(dc);

    DrawEdges(dc);

    if(m_size!=WINDOW_SIZE_MIN)
        DrawWindowBkgnd(dc);

    DrawCaption(dc, CS_SMALL);
    DrawButton(dc);

    if(m_size!=WINDOW_SIZE_MIN)
        DrawList(dc);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::CalcWindowBorder(BORDER_STYLE style)
{
    m_wndBorder.cx=1;
    m_wndBorder.cy=1;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::DrawCaption(
    CAggDC& dc, 
    CAPTION_STYLE style)
{
    CRect rc(m_rcCaption);	
    UINT flags;

    std::_tstring str = m_g->GetLabel();

    flags=DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

    int idc=dc.SaveDC();

    Color clrcaption=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
    Color clrtext=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

    PointF points[4];
    points[0].x=static_cast<REAL>(m_rcCaption.left);
    points[0].y=static_cast<REAL>(m_rcCaption.top);
    points[1].x=static_cast<REAL>(m_rcCaption.right);
    points[1].y=static_cast<REAL>(m_rcCaption.top);
    points[2].x=static_cast<REAL>(m_rcCaption.right);
    points[2].y=static_cast<REAL>(m_rcCaption.bottom);
    points[3].x=static_cast<REAL>(m_rcCaption.left);
    points[3].y=static_cast<REAL>(m_rcCaption.bottom);

    dc.SolidPolygon(
        points, 
        clrcaption, 
        sizeof(points)/sizeof(points[0]),
        2,
        m_colorBorder);

    CDCHandle hdc(dc.m_hDC);
    CAutoFont f(m_canvas->GetCaptionFont(m_canvas->GetFontPixHeight()), hdc);

    // prepare text rectangle w.r.t. caption rectangle

    // we already deflated rect for window borders but caption
    // text drawn without margin on the left looks ugly
    rc.left+=m_wndBorder.cx;

    rc.left+=
        m_captionContentMargin.cxLeftWidth+
        m_captionTextMargin.cxLeftWidth; 

    rc.right-=m_rcButton.Width()+
        m_captionButtonTextGap+
        m_captionContentMargin.cxRightWidth+
        m_captionTextMargin.cxRightWidth;

    rc.top+=m_captionContentMargin.cyTopHeight+
        m_captionTextMargin.cyTopHeight;

    rc.bottom=rc.top+m_captionSizes[m_canvas->GetDpiScale()].cy;

    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(clrtext.ToCOLORREF());
    dc.DrawText(str.c_str(), str.length(), rc, flags);
    dc.RestoreDC(idc);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::DrawWindowBkgnd(
    CAggDC& dc) 
{
    PointF wndoutline[4];
    CRect rc(m_rcWnd);

    // Agg draws outline outside given rectangle
    rc.DeflateRect(1,1);

    wndoutline[0].x=static_cast<REAL>(rc.left);
    wndoutline[0].y=static_cast<REAL>(rc.top);
    wndoutline[1].x=static_cast<REAL>(rc.right);
    wndoutline[1].y=static_cast<REAL>(rc.top);
    wndoutline[2].x=static_cast<REAL>(rc.right);
    wndoutline[2].y=static_cast<REAL>(rc.bottom);
    wndoutline[3].x=static_cast<REAL>(rc.left);
    wndoutline[3].y=static_cast<REAL>(rc.bottom);

    dc.SolidPolygon(
        wndoutline,
        m_colorBorder/*m_colorBkgnd*/,
        sizeof(wndoutline)/sizeof(wndoutline[0]),
        2,
        m_colorBorder);

    // vertical splitter line in different color

    int iwidth=m_sizes_list_item[m_canvas->GetDpiScale()].cx;

    //PointF p1(LONG(rc.left+iwidth-1), (LONG)rc.top);
    //PointF p2(LONG(rc.left+iwidth+1), (LONG)rc.bottom);
    //dc.Rect(p1, p2, GraphTypes::Color(255, 255, 0, 0), GraphTypes::Color(255, 0, 0, 255));

    PointF p1(LONG(rc.left+iwidth), (LONG)rc.top);
    PointF p2(LONG(rc.left+iwidth), (LONG)rc.bottom);
    dc.Line(p1, p2, m_colorTitleBar);
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::CalcButtonRect(CRect& rc)
{
    CRect buttrc;

    // caption total size always includes top window border
    int wh=CalcCaptionTotalHeight()-m_wndBorder.cy;
    wh-=m_captionContentMargin.cyBottomHeight+m_captionContentMargin.cyTopHeight;

    // buttons centered vertically
    buttrc.top=m_rcWnd.top+m_wndBorder.cy+m_captionContentMargin.cyTopHeight;

    buttrc.bottom=buttrc.top+wh;
    buttrc.right=m_rcWnd.right-m_wndBorder.cy-m_captionContentMargin.cxRightWidth;
    buttrc.left=buttrc.right-wh;

    rc=buttrc;
}	

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::DrawButton(CAggDC& dc)
{
    CRect rc(m_rcButton);

    Color color=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
    Color colorBorder=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

    switch(m_ButtonState.m_state)
    {
    case BS_NORMAL:
        break;
    case BS_HOT:
        if(IsSelected())
            color.MoreBright(40);
        else
            color.LessBright(20);
        break;
    case BS_PUSHED:
        color.LessBright(40);
        break;
    case BS_DISABLED:
        break;
    default:
        ATLASSERT(0);
    }

    PointF tl, br;

    tl.x=static_cast<REAL>(rc.left);
    tl.y=static_cast<REAL>(rc.top);
    br.x=static_cast<REAL>(rc.right);
    br.y=static_cast<REAL>(rc.bottom);

    dc.Rect(tl, br, color, colorBorder);

    if(m_size==WINDOW_SIZE_MIN)
    {
        rc.DeflateRect(2,2);

        tl.x=static_cast<REAL>(rc.left);
        tl.y=static_cast<REAL>(rc.top);
        br.x=static_cast<REAL>(rc.right);
        br.y=static_cast<REAL>(rc.bottom);

        dc.Rect(tl, br, color, colorBorder);
    }
    else
    {
        rc.DeflateRect(4,2);

        tl.x=static_cast<REAL>(rc.left);
        tl.y=static_cast<REAL>(rc.bottom-1);
        br.x=static_cast<REAL>(rc.right);
        br.y=static_cast<REAL>(rc.bottom);

        // a thick line 
        dc.Rect(tl, br, colorBorder, colorBorder);
    }
}

//-----------------------------------------------------------------------------

CSize CIOSubgraphRenderer::CalcListMaxSize(CDCHandle& dc)
{
    CComPtr<IUnknown> iunk;
    CComQIPtr<IGraphVertex> vertex;
    CComQIPtr<ITextboxRenderer> tb;
    IGraphItemVector results;
    CSize listsize;
    SizeF itemextent, storedsize;
    RectF rcitem;
    CSize cumulI(0,0), cumulO(0,0);
    unsigned ins=0, outs=0;
    unsigned count = m_g->GetChildren(results);

    ATLASSERT(count);

    if(count>0)
    {
        for(IGraphItemVector::iterator itr=results.begin();
            itr!=results.end();
            ++itr)
        {
            vertex=*itr;			
            ATLASSERT(vertex);

            tb=m_canvas->GetRenderer(vertex); 
            if(tb!=0)
            {
                itemextent=tb->CalcDisplaySizes(dc, true, true);
                if(m_canvas->m_meta.GetRowCompress() && vertex->GetEdgeCount()>0)
                    itemextent.Height=1;

                rcitem=itemextent;

                tb->SetBoundsBox(rcitem, SWP_NOREDRAW); // initial bounds correspond to extent

                if(itemextent.Height>storedsize.Height)
                    storedsize.Height=itemextent.Height;

                ITextboxRenderer::eIOMode mode=tb->GetIOMode();

                if(mode==ITextboxRenderer::iomOut)
                {
                    outs++;
                    if(int(itemextent.Width)>cumulO.cx)
                        cumulO.cx=int(itemextent.Width);
                    cumulO.cy+=int(itemextent.Height);
                }
                else if(mode==ITextboxRenderer::iomIn)
                {
                    ins++;
                    if(int(itemextent.Width)>cumulI.cx)
                        cumulI.cx=int(itemextent.Width);
                    cumulI.cy+=int(itemextent.Height);
                }
            }
        }
    }

    m_I=ins;
    m_O=outs;
    if(m_I>1)
        cumulI.cy+=(m_I-1)*int(m_dividerLine.Width);
    if(m_O>1)
        cumulO.cy+=(m_O-1)*int(m_dividerLine.Height);
    listsize.cx=cumulI.cx+cumulO.cx+int(m_dividerLine.Width);
    m_bITaller=cumulI.cy>cumulO.cy;
    listsize.cy=m_bITaller?cumulI.cy:cumulO.cy;
    storedsize.Width=static_cast<REAL>(cumulI.cx);
    m_sizes_list_item[m_canvas->GetDpiScale()]=static_cast<CSize>(storedsize);
    return listsize;
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::CalcListRect(CRect& rc)
{
    rc=m_rcClient;

    if(!(m_style&NO_MARGIN))
    {
        MARGINS scaled=m_listMargin;
        m_canvas->ScaleMargins(scaled);

        rc.DeflateRect(
            scaled.cxLeftWidth, 
            scaled.cyTopHeight, 
            scaled.cxRightWidth, 
            scaled.cyBottomHeight);
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::AdjustChildrenBBox(void)
{
    CComQIPtr<IGraphVertex> vertex;
    CComQIPtr<ITextboxRenderer> tb;
    IGraphItemVector results;
    RectF rcIitem, rcOitem, rcBbox;
    REAL heightO = 0.0f, heightI = 0.0f;
    int compI=0, compO=0;
    int iwidth=m_sizes_list_item[m_canvas->GetDpiScale()].cx;
    CSize listsize=m_max_sizes_list[m_canvas->GetDpiScale()];

    m_g->GetChildren(results);

    rcIitem.x=static_cast<REAL>(m_rcList.left);
    rcOitem.x=static_cast<REAL>(rcIitem.x+iwidth+m_dividerLine.Width);
    rcIitem.y=rcOitem.y=static_cast<REAL>(m_rcList.top);
    rcIitem.Width=static_cast<REAL>(iwidth);
    rcOitem.Width=static_cast<REAL>(m_rcList.Width()-iwidth-m_dividerLine.Width);

    // calc number of compressed inputs and outputs


    if(m_canvas->m_meta.GetRowCompress())
    {
        for(IGraphItemVector::iterator itr=results.begin();
            itr!=results.end();
            ++itr)
        {
            vertex=*itr;			
            ATLASSERT(vertex);

            if(vertex->GetEdgeCount()>0)
            {
                tb=m_canvas->GetRenderer(vertex); 
                ATLASSERT(tb!=0);

                ITextboxRenderer::eIOMode mode=tb->GetIOMode();

                if(mode==ITextboxRenderer::iomOut)
                {
                    compO++;
                }
                else if(mode==ITextboxRenderer::iomIn)
                {
                    compI++;
                }
                else
                {
                    ATLASSERT(0);
                }
            }
        }
    }

    if(m_O>1)
    {
        // determine the height of items that are not compressed
        if(m_bITaller) // only in case inputs are taller and dominate graph height
        {
            if(compO==m_O || compO==0) // all or none of the items compressed, divide equally
            {
                heightO=(listsize.cy-(m_O-1)*m_dividerLine.Height)/m_O;
            }
            else
            {
                // exclude all dividers in between the rows
                heightO=listsize.cy-(m_O-1)*m_dividerLine.Height;
                // list height excluding compressed items of height 1
                heightO-=compO;
                // equal height of non-compressed rows
                heightO/=(m_O-compO);
            }
            ATLASSERT(heightO>0);
        }
    }
    else
        heightO=static_cast<REAL>(listsize.cy);

    if(m_I>1)
    {
        // determine the height of items that are not compressed
        if(!m_bITaller) // only in case outputs are taller and dominate graph height
        {
            if(compI==m_I || compI==0) // all or none of the items compressed, divide equally
            {
                heightI=(listsize.cy-(m_I-1)*m_dividerLine.Height)/m_I;
            }
            else
            {
                // exclude all dividers in between the rows
                heightI=listsize.cy-(m_I-1)*m_dividerLine.Height;
                // list height excluding compressed items of height 1
                heightI-=compI;
                // equal height of non-compressed rows
                heightI/=(m_I-compI);
            }
            ATLASSERT(heightI>0);
        }
    }
    else
        heightI=static_cast<REAL>(listsize.cy);

    
    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr)
    {
        vertex=*itr;			
        ATLASSERT(vertex);

        tb=m_canvas->GetRenderer(vertex); 
        if(tb!=0)
        {
            tb->GetBoundsBox(rcBbox);
            ATLASSERT(rcBbox.Height>0);
            ITextboxRenderer::eIOMode mode=tb->GetIOMode();

            if(mode==ITextboxRenderer::iomOut)
            {
                if(!m_bITaller) // outputs are taller and dominate graph height
                {
                    if(vertex->GetEdgeCount()>0 && m_canvas->m_meta.GetRowCompress()) // is this one compressed
                        rcOitem.Height=1;
                    else // current Bbox should be fine
                        rcOitem.Height=rcBbox.Height;
                }
                else
                {
                    if(compO>0) // there are compressed rows
                    {
                        if(compO==m_O) // all are compressed, divide equally
                            rcOitem.Height=heightO;
                        else
                        {
                            if(vertex->GetEdgeCount()>0) // it is this one
                                rcOitem.Height=1;
                            else
                                rcOitem.Height=heightO;
                        }
                    }
                    else
                    {
                        rcOitem.Height=heightO;
                    }
                }

                tb->SetBoundsBox(rcOitem, SWP_NOREDRAW);
                rcOitem.y+=rcOitem.Height+m_dividerLine.Height;
            }
            else if(mode==ITextboxRenderer::iomIn)
            {
                if(m_bITaller) // inputs are taller and dominate graph height
                {
                    if(vertex->GetEdgeCount()>0 && m_canvas->m_meta.GetRowCompress()) // is this one compressed
                        rcIitem.Height=1;
                    else // current Bbox should be fine
                        rcIitem.Height=rcBbox.Height;
                }
                else
                {
                    if(compI>0) // there are compressed rows
                    {
                        if(compI==m_I) // all are compressed, divide equally
                            rcIitem.Height=heightI;
                        else
                        {
                            if(vertex->GetEdgeCount()>0) // it is this one
                                rcIitem.Height=1;
                            else
                                rcIitem.Height=heightI;
                        }
                    }
                    else
                    {
                        rcIitem.Height=heightI;
                    }
                }

                tb->SetBoundsBox(rcIitem, SWP_NOREDRAW);
                rcIitem.y+=rcIitem.Height+m_dividerLine.Height;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::DrawList(CAggDC& dc)
{
    CComQIPtr<ITextboxRenderer> tb;
    IGraphItemVector results;

    unsigned count = m_g->GetChildren(results);
    ATLASSERT(count); // not an error but worthy of signalling for now

    if(count>0)
    {
        for(IGraphItemVector::iterator itr=results.begin();
            itr!=results.end();
            ++itr)
        {
            ATLASSERT(*itr);			
            tb=m_canvas->GetRenderer(*itr); 
            if(tb!=0)
                tb->SendMessage(WM_PAINT, reinterpret_cast<WPARAM>(&dc), 0);
        }
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::DrawEdges(CAggDC& dc)
{
    CComQIPtr<IEdgeRenderer> ier;
    CComPtr<IUnknown> iunk;
    CComQIPtr<IGraphVertex> vertex, neighbor;
    CComPtr<IGraphEdge> edge;
    CComPtr<IGraphSubgraph> sgneighbor;
    IGraphItemVector results;
    IGraphEdgeVector edges;

    unsigned myorder=m_g->GetOrder();
    unsigned count = m_g->GetChildren(results);

    if(count>0)
    {
        for(IGraphItemVector::iterator itr=results.begin();
            itr!=results.end();
            ++itr)
        {
            vertex=*itr;			

            if(vertex!=0)
            {
                edges.clear();
                vertex->GetAdjacentEdges(edges);

                for(IGraphEdgeVector::iterator eitr = edges.begin();
                    eitr != edges.end();
                    ++eitr)
                {
                    edge = *eitr;

                    neighbor=edge->GetAdjacentVertex(vertex);
                    sgneighbor=neighbor->GetParent();

                    if(sgneighbor!=0)
                    {
                        unsigned hisorder=sgneighbor->GetOrder();
                        if(hisorder<myorder)
                        {
                            ier=m_canvas->GetRenderer(edge); 
                            if(ier!=0)
                                ier->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void CIOSubgraphRenderer::DrawConnectors(CAggDC& dc)
{
    CComQIPtr<IGraphVertex> vertex;
    CComQIPtr<ITextboxRenderer> tb;
    IGraphItemVector results;
    IGraphEdgeVector edges;
    RectF rcL, rcR;
    SizeF connsize=GetConnectorSize();

    bool bHasEdges;

    m_g->GetChildren(results);

    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr)
    {
        vertex=*itr;			
        tb=m_canvas->GetRenderer(vertex);
        ATLASSERT(tb.p);

        if(m_size!=WINDOW_SIZE_MIN)
        {
            RectF rcitem;

            tb->GetBoundsBox(rcitem);
            rcL.y=rcR.y=rcitem.y+(rcitem.Height-connsize.Height)/2;
            if(rcL.y>m_rcWnd.bottom)
                rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;

            rcL.x=m_rcWnd.left-connsize.Width;
            rcR.x=static_cast<REAL>(m_rcWnd.right);
        }
        else
        {
            rcL.y=rcR.y=m_rcWnd.top+((REAL)CalcCaptionTotalHeight()-connsize.Height)/2;
            rcL.x=REAL(m_rcWnd.left)-connsize.Width;
            rcR.x=REAL(m_rcWnd.right);
        }

        rcL.Height=rcR.Height=connsize.Height;
        rcR.Width=rcL.Width=connsize.Width;	
        edges.clear();
        bHasEdges=vertex->GetAdjacentEdges(edges)>0;
        Color cbkgnd=bHasEdges?m_colorEdgeBkgnd:m_colorNoEdgeBkgnd;

        ITextboxRenderer::eIOMode mode=tb->GetIOMode();
        if(mode==ITextboxRenderer::iomOut)
        {
            dc.LollyPop(rcR, cbkgnd, m_colorOutline, false);
        }
        else if(mode==ITextboxRenderer::iomIn)
        {
            dc.LollyPop(rcL, cbkgnd, m_colorOutline, true);
        }
    }
}

//-----------------------------------------------------------------------------

SizeF CIOSubgraphRenderer::GetConnectorSize()
{
    SizeF size;
    // conn is m_factor times height of text 
    size.Height=m_rcCaption.Height()*m_connFactor.Height;
    size.Width=m_rcCaption.Height()*m_connFactor.Width;
    return size;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CIOSubgraphRenderer::HitTestCaption(const CPoint& p)
{
    HIT_TEST_CODE code=HT_UNKNOWN;

    if(m_rcCaption.PtInRect(p))
    {
        code=HitTestButton(p);

        if(code==HT_UNKNOWN)
            code=HT_CAPTION;
    }

    return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CIOSubgraphRenderer::HitTestButton(const CPoint& p)
{
    HIT_TEST_CODE code=HT_UNKNOWN;

    if(m_rcButton.PtInRect(p))
        code=m_size==WINDOW_SIZE_MIN ? HT_MAXBUTTON : HT_MINBUTTON;

    return code;
}

//-----------------------------------------------------------------------------

IGraphVertex* CIOSubgraphRenderer::HitTestConnectors(
    const GraphTypes::PointF& ptest, double tolerance/*=0*/)
{
    IGraphItemVector results;

    m_g->GetChildren(results);
    ATLASSERT(results.size());

    RectF vp;
    m_canvas->GetViewport(vp);
    CRect rc=static_cast<CRect>(vp);
    boost::scoped_ptr<CAggHitTest> agght(CAggHitTest::CreateAggHitTest());
    agght->SetRect(rc);

    CComQIPtr<IGraphVertex> vertex;
    CComQIPtr<ITextboxRenderer> tb;
    RectF rcL, rcR;
    SizeF connsize=GetConnectorSize();

    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr)
    {
        vertex=*itr;			
        tb=m_canvas->GetRenderer(vertex);
        ATLASSERT(tb.p);

        if(m_size!=WINDOW_SIZE_MIN)
        {
            RectF rcitem;

            tb->GetBoundsBox(rcitem);
            rcL.y=rcR.y=rcitem.y+(rcitem.Height-connsize.Height)/2;
            if(rcL.y>m_rcWnd.bottom)
                rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;

            rcL.x=m_rcWnd.left-connsize.Width;
            rcR.x=static_cast<REAL>(m_rcWnd.right);
        }
        else
        {
            rcL.y=rcR.y=m_rcWnd.top+((REAL)CalcCaptionTotalHeight()-connsize.Height)/2;
            rcL.x=REAL(m_rcWnd.left)-connsize.Width;
            rcR.x=REAL(m_rcWnd.right);
        }

        rcL.Height=rcR.Height=connsize.Height;
        rcR.Width=rcL.Width=connsize.Width;	

        ITextboxRenderer::eIOMode mode=tb->GetIOMode();
        if(mode==ITextboxRenderer::iomOut)
        {
            if(agght->LollyPopHitTest(rcR, false, ptest, tolerance))
                return vertex;
        }
        else if(mode==ITextboxRenderer::iomIn)
        {
            if(agght->LollyPopHitTest(rcL, true, ptest, tolerance))
                return vertex;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::ForwardMsgToChildren(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CPoint p(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    LRESULT lRes=0;
    CComQIPtr<ITextboxRenderer> vr;
    CComPtr<IGraphItem> item;
    RectF rcitem;
    PointF pf(p);
    IGraphItemVector results;
    m_g->GetChildren(results);
    
    ATLASSERT(
        uMsg==WM_LBUTTONDOWN||
        uMsg==WM_MOUSEMOVE||
        uMsg==WM_LBUTTONUP||
        uMsg==WM_NCHITTEST); // for now

    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr)
    {
        item=*itr;			
        vr=m_canvas->GetRenderer(item);

        if(vr!=0)
        {
            vr->GetBoundsBox(rcitem);
            if(rcitem.Height<2) // should be a compressed row, skip it
                continue;

            if(rcitem.Contains(pf))
            {
                lRes=vr->SendMessage(uMsg, wParam, lParam);
                break;
            }
        }
    }

    return lRes;
}

//*****************************************************************************
//
// Message handlers
//

LRESULT CIOSubgraphRenderer::OnPaint(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    CAggDC& dc=*(CAggDC*)wParam;

    Draw(dc); 

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnLButtonDown(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    LRESULT lRes=0;
    bool bRepaint=false;
    bool bselected=IsSelected();
    CUniqueID oldSelection;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    HIT_TEST_CODE hit=HitTestCaption(pt);
    CButtonState* state=0;
    BUTTON_STATE oldstate;

    m_g->MoveToEnd();
    SetFocus();

    // no multiple selection of subgraphs
    if(!bselected)
    {
        //TODO use new selection method oldSelection=m_canvas->GetSelected();
        CScaledWindow::SetSelected(GetID());
    }

    if(hit!=HT_UNKNOWN) // in caption
    {
        m_capturedPart=hit;
        m_canvas->SetCapture(this);

        if(hit==HT_MAXBUTTON || hit==HT_MINBUTTON)
        {
            state=&m_ButtonState;
        }
        else if(hit==HT_CAPTION)
        {
            m_capturePos=pt;
            
            PointF savePos=m_position;
            m_canvas->AllTransform(savePos);
            m_savePos.x=int(savePos.x);
            m_savePos.y=int(savePos.y);
        }

        if(state)
        {
            oldstate=state->m_state;

            if(oldstate==BS_HOT)
            {
                m_canvas->ClearCurrentHot(BS_PUSHED);
            }
            else
            {
                m_canvas->ClearCurrentHot();
                state->m_state=BS_PUSHED;
                if(oldstate!=BS_PUSHED)
                    bRepaint|=true;
            }
        }
    }
    else // check children
    {
        if(hit==HT_UNKNOWN) // not in caption
        {
            
            lRes=ForwardMsgToChildren(uMsg, wParam, lParam);
            // above can not hit connectors
            if(lRes==0)
            {
                PointF ptf(pt);
                CComPtr<IGraphItem> gitem=HitTestChildren(ptf);
                lRes=reinterpret_cast<LRESULT>(gitem.p);
            }
        }
    }

    bRepaint|=IsSelected()!=bselected;

    if(bRepaint)
        Invalidate();

    if(IsSelected()!=bselected)
    {
        m_canvas->NotifySelection(GetID(), oldSelection);
    }

    return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnLButtonUp(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& bHandled)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    HIT_TEST_CODE hit=HitTestCaption(pt);
    bool bRepaint=false;
    CButtonState* capstate=0, *hitstate=0;

    if(hit==HT_MAXBUTTON||hit==HT_MINBUTTON)
        hitstate=&m_ButtonState;

    if(m_canvas->GetCapture()==this)
    {
        // only caption and caption button
        // can capture mouse
        if(m_capturedPart==HT_CAPTION)
        {
        }
        else
        {
            if(m_capturedPart==HT_MAXBUTTON||m_capturedPart==HT_MINBUTTON)
                capstate=&m_ButtonState;

            BUTTON_STATE oldcap=capstate->m_state;

            if(m_capturedPart!=hit)
            {
                capstate->m_state=BS_NORMAL;

                if(hitstate)
                {
                    hitstate->SetHot(true);
                }
            }
            else
            {
                capstate->SetHot(true);

                if(hit==HT_MAXBUTTON)
                    SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
                else
                    SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
            }

            if(oldcap!=capstate->m_state)
                bRepaint|=true;
        }

        m_capturedPart=HT_UNKNOWN;
        m_canvas->ReleaseCapture();
    }
    else // no capture in effect
    {
        if(hit!=HT_UNKNOWN) // caption
        {
            if(hitstate)
                hitstate->SetHot(true);
            else
                m_canvas->ClearCurrentHot();
        }
        else
        {
            // only a child may be hot now
            m_canvas->ClearCurrentHot();
            ForwardMsgToChildren(uMsg, wParam, lParam);
        }
    }

    if(bRepaint)
        Invalidate();

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnMouseMove(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    HIT_TEST_CODE hit=HitTestCaption(pt);
    bool bRepaint=false;
    CButtonState* capstate=0, *hitstate=0;

    // only buttons have state
    if(hit==HT_MAXBUTTON || hit==HT_MINBUTTON)
        hitstate=&m_ButtonState;

    if(m_canvas->GetCapture()==this)
    {
        CRect newrc=m_rcWnd;

        // only caption and caption button
        // can capture mouse in this window
        if(m_capturedPart==HT_CAPTION)
        {
            pt.x+=m_savePos.x-m_capturePos.x;
            pt.y+=m_savePos.y-m_capturePos.y;
            newrc.TopLeft()=pt;
            newrc.right=newrc.left+m_rcWnd.Width();
            newrc.bottom=newrc.top+m_rcWnd.Height();
            SetWindowPos(newrc, SWP_NOSIZE);
            if(m_canvas->m_meta.IsAvoidObstacles())
                m_canvas->NotifyRecalcEdgePoints();
        }
        else
        {
            if(m_capturedPart==HT_MAXBUTTON || m_capturedPart==HT_MINBUTTON)
                capstate=&m_ButtonState;

            BUTTON_STATE oldcap=capstate->m_state;

            if(m_capturedPart!=hit)
                capstate->m_state=BS_NORMAL;
            else
                capstate->m_state=BS_PUSHED;

            if(oldcap!=capstate->m_state)
                bRepaint|=true;
        }
    }
    else // no capture in effect
    {
        if(hit!=HT_UNKNOWN) // caption
        {
            if(hitstate)
                hitstate->SetHot(true);
            else
                m_canvas->ClearCurrentHot();
        }
        else 
        {
            // only a child may be hot now
            m_canvas->ClearCurrentHot();
            ForwardMsgToChildren(uMsg, wParam, lParam);
        }
    }

    if(bRepaint)
        Invalidate();

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnMouseDblclk(
    UINT /*uMsg*/,
    WPARAM /*wParam*/,
    LPARAM lParam, BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    if(m_size==WINDOW_SIZE_MIN)
    {
        HIT_TEST_CODE hit=HitTestCaption(pt);
        if(hit==HT_CAPTION)
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnRButtonDown(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    PointF ptf(pt);

    CComPtr<IGraphItem> item=HitTestChildren(ptf);

    if(item!=0)
        lRes=reinterpret_cast<LRESULT>(item.p);
    else
        lRes=reinterpret_cast<LRESULT>(m_g);

    return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnSysCommand(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/, 
    BOOL& /*bHandled*/)
{
    CRect rc=m_rcWnd;

    if(wParam==SC_MINIMIZE)
    {
        m_size=WINDOW_SIZE_MIN;

        int hcaption=CalcCaptionTotalHeight();
        rc.bottom=rc.top+hcaption+m_wndBorder.cy;
        SetWindowPos(rc, SWP_NOMOVE);
    }
    else if(wParam==SC_MAXIMIZE)
    {
        m_size=WINDOW_SIZE_MAX;
        CSize sizewndmax=m_max_sizes[m_canvas->GetDpiScale()];

        rc.right=rc.left+sizewndmax.cx;
        rc.bottom=rc.top+sizewndmax.cy;
        SetWindowPos(rc, SWP_NOMOVE);
    }

    if(m_canvas->m_meta.IsAvoidObstacles())
        m_canvas->NotifyRecalcEdgePoints();


    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIOSubgraphRenderer::OnNcHitTest(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    PointF ptf((REAL)GET_X_LPARAM(lParam), (REAL)GET_Y_LPARAM(lParam));

    CComPtr<IGraphItem> gitem=HitTestChildren(ptf);
    LRESULT lRes=reinterpret_cast<LRESULT>(gitem.p);

    return lRes;
}

//*****************************************************************************

IRenderer* CreateIOSubgraphRenderer(CCanvas & canvas)
{
    CIOSubgraphRenderer * retVal = new CIOSubgraphRenderer(canvas);
    return retVal;
}

