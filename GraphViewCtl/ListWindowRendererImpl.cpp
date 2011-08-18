#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "ScaledList.h"
#include "AggDC.h"
#include "XMLWriter.h"

using namespace GraphTypes;

class CListWindowRenderer : public IListWindowRenderer, public CScaledWindow
{
	IGraphSubgraph* m_g;

	CPartTheme& m_wndTheme;
	CPartTheme& m_statusTheme;

	CScaledList m_list;
	CButtonState m_maxButtonState;
	CButtonState m_minButtonState;

	CRect m_rcCaption;
	CRect m_rcButMax;
	CRect m_rcButMin;
	CRect m_rcGripper;

	WINDOW_SIZE m_size;
	HIT_TEST_CODE m_capturedPart;

	// sizes that vary with each DPI and renderer "window"
	CRectMap m_wnd_sizes; // current size and position 
	CSizeMap m_norm_sizes;
	CSizeMap m_max_sizes;
	CSizeMap m_max_sizes_list;

	// sizes per each DPI that don't vary with each renderer "window"
	static CSizeMap m_captionSizes;
	static CSizeMap m_min_sizes;

	// sizes that are either fixed per DPI or dynamically scaled
	static CSize m_wndBorder;
	static BORDER_STYLE m_wndBorderStyle;
	static MARGINS m_captionTextMargin;
	static MARGINS m_captionContentMargin;
	static MARGINS m_listMargin;
	static CSize m_sizeGripper;
	static int m_captionButtonTextGap;
	static int m_captionButtonGap;

	GraphTypes::PointF m_position;
	CPoint m_origin;
	CPoint m_savePos;
	CPoint m_capturePos;

	int CalcCaptionSize(CDCHandle& dc, const TCHAR* title);
	int CalcCaptionTotalHeight(void);
	void DrawWindow(CAggDC& dc); 
	void CalcWindowBorder(BORDER_STYLE style);
	void DrawWindowBorder(CDCHandle& dc, BORDER_STYLE style);
	void DrawWindowCaption(CDCHandle& dc, CAPTION_STYLE style);
	void DrawClassicCaptionBkgnd(CDCHandle& dc, const CRect& rc);
	void DrawCaptionText(
		CDCHandle& dc, 
		const TCHAR* txt,	
		CRect* rctext, 
		UINT flags, 
		int shadowtype, 
		COLORREF clrtext, 
		COLORREF clrshadow,
		const POINT& offset);
	void DrawWindowBkgnd(CDCHandle& dc); 
	void CalcGripperRect(CRect& rc);
	void DrawGripper(CDCHandle& dc);
	void CalcMaxButtonRect(CRect& rc);
	void DrawMaxButton(CDCHandle& dc);
	void CalcMinButtonRect(CRect& rc);
	void DrawMinButton(CDCHandle& dc);
	void CalcListRect(CRect& rc);
	void DrawList(CDCHandle& dc);
	void ScaleMargins(MARGINS& margins);
	void DrawEdges(CAggDC& dc);

	HIT_TEST_CODE HitTestCaption(const CPoint& p);
	HIT_TEST_CODE HitTestMaxButton(const CPoint& p);
	HIT_TEST_CODE HitTestMinButton(const CPoint& p);
	HIT_TEST_CODE HitTestGripper(const CPoint& p);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IListWindowRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CListWindowRenderer(CCanvas & canvas);

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

	// IListWindowRenderer

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
	BEGIN_MSG_MAP(CListWindowRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk);
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand);
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

// height and default width (sample title) for each scale
CSizeMap CListWindowRenderer::m_captionSizes;

// Minimum window height for each scale, *INCLUDING* caption
CSizeMap CListWindowRenderer::m_min_sizes;

// these statics are either independent of scale factor
// or are scaled dynamically (may consider storing those in
// vector, for each scale factor)

CSize CListWindowRenderer::m_wndBorder;
MARGINS CListWindowRenderer::m_captionTextMargin;
MARGINS CListWindowRenderer::m_captionContentMargin;
MARGINS CListWindowRenderer::m_listMargin;
CSize CListWindowRenderer::m_sizeGripper;

BORDER_STYLE CListWindowRenderer::m_wndBorderStyle=DIALOG_FRAME;
// gap between caption text and first button
int CListWindowRenderer::m_captionButtonTextGap=4;
// gap between caption buttons
int CListWindowRenderer::m_captionButtonGap=1;

CListWindowRenderer::CListWindowRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_wndTheme(canvas.GetPartTheme(_T("WINDOW"))),
	m_statusTheme(canvas.GetPartTheme(_T("STATUS"))),
	m_list(&canvas),
	m_size(WINDOW_SIZE_NORM),
	m_origin(0,0),
	m_capturedPart(HT_UNKNOWN),
	m_maxButtonState(&canvas, this),
	m_minButtonState(&canvas, this)
{

#ifdef _DEBUG_THEME
	canvas.FlipDebugTheme();
#endif

	m_sizeGripper.cx=-1; // -1 to indicate not initialized
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
}

//-----------------------------------------------------------------------------

CUniqueID CListWindowRenderer::GetID()
{
	return m_g->GetID();
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetGraphItem(IGraphItem* item)
{
	m_g=static_cast<IGraphSubgraph*>(item);
	m_list.SetGraphItem(item);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	box=m_rcWnd;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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

void CListWindowRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	m_position=pt;
	if(co==screen)
	{
		m_canvas->RAllTransform(m_position);		
	}
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::SendMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetFocus(bool bSet)
{
	// parent will never have focus and passes it to children
	m_list.SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::GetContainerRect(GraphTypes::RectF& rc) const
{
	rc=m_rcWnd;
}

//-----------------------------------------------------------------------------

IGraphItem* CListWindowRenderer::HitTestChildren(const PointF& point)
{
	return m_list.HitTestItems(point);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::GetConnectBounds(
	RectF& left, 
	RectF& right, 
	unsigned order)
{
	RectF rcL, rcR;
	REAL thickness=REAL(m_wndBorder.cx);
	REAL widthext=thickness*2;
	REAL cheight; 

	if(m_size!=WINDOW_SIZE_MIN)
	{
		RectF rcitem;
		CScaledList::eLocation loc=m_list.GetConnectBounds(rcitem, order);
		if(loc==CScaledList::top)
		{
			cheight=(REAL)CalcCaptionTotalHeight();
			if(cheight<thickness)
				thickness=cheight;

			rcL.y=rcR.y=REAL(m_rcWnd.top)+cheight-thickness;
			rcL.x=REAL(m_rcWnd.left)-widthext;
			rcR.x=REAL(m_rcWnd.right);

			rcL.Height=rcR.Height=thickness;
			rcR.Width=rcL.Width=widthext;	
		}
		else if(loc==CScaledList::bottom)
		{
			rcL.y=rcR.y=REAL(m_rcWnd.bottom)-thickness;
			rcL.x=REAL(m_rcWnd.left)-widthext;
			rcR.x=REAL(m_rcWnd.right);

			rcL.Height=rcR.Height=thickness;
			rcR.Width=rcL.Width=widthext;	
		}
		else
		{
			rcL.y=rcR.y=rcitem.y;
			rcL.x=REAL(m_rcWnd.left)-widthext;
			rcR.x=REAL(m_rcWnd.right);
			rcL.Height=rcR.Height=rcitem.Height;
			rcR.Width=rcL.Width=widthext;	
		}
	}
	else
	{
		rcL.y=rcR.y=REAL(m_rcWnd.top)+(REAL)CalcCaptionTotalHeight()/2;
		rcL.x=REAL(m_rcWnd.left)-widthext;
		rcR.x=REAL(m_rcWnd.right);

		rcL.Height=rcR.Height=thickness;
		rcR.Width=rcL.Width=widthext;	
	}

	left=rcL;
	right=rcR;
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	CScaledWindow::SetSelected(GetID(), bSelected, bAdd);
}

//-----------------------------------------------------------------------------

bool CListWindowRenderer::IsSelected()
{
	return m_bSelected;
}

//-----------------------------------------------------------------------------

bool CListWindowRenderer::IsMinimized() const
{
	return false;
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::OnRemoveGraphItem(IGraphItem* /*gi*/)
{

}

//-----------------------------------------------------------------------------

bool CListWindowRenderer::OnEditItem(IGraphItem* /*gi*/)
{
	bool bOk=true;

	return bOk;
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetWindowState(WINDOW_SIZE size, int topindex)
{
	// nothing for this graph
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::GetWindowState(WINDOW_SIZE& size, int& topindex) const
{
	size=m_size;
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetWindowSize(const GraphTypes::SizeF& size)
{
	CSize isize=static_cast<CSize>(size);
	CRectEx wndrc=m_wnd_sizes[m_canvas->GetDpiScale()];
	wndrc.right=wndrc.left+isize.cx;
	wndrc.bottom=wndrc.top+isize.cy;
	m_wnd_sizes[m_canvas->GetDpiScale()]=wndrc;
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::GetWindowSize(GraphTypes::SizeF& size) const
{
	CRectEx wndrc;
	CRectMap::const_iterator itr=m_wnd_sizes.find(m_canvas->GetDpiScale());
	if(itr!=m_wnd_sizes.end())
		wndrc=itr->second;
	size.Width=wndrc.Width();
	size.Height=wndrc.Height();
}

//-----------------------------------------------------------------------------

SizeF CListWindowRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	SizeF size;
	IGraphItemVector children;
	unsigned count=m_g->GetChildren(children);

	if(count==0)
		return size;

	bool bSizeChange=false;
	bool bPositionChange=false;

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

		std::_tstring txt = _T("Vertex-");
		txt += m_g->GetLabel();

		wcaption=CalcCaptionSize(dc, txt.c_str());

		sizelistmax=m_list.CalcListMaxSize(dc);
		m_max_sizes_list[m_canvas->GetDpiScale()]=sizelistmax;

		// default width 
		wdefault=__max(sizelistmax.cx,wcaption);
		wndrc.right=wndrc.left+wdefault;

		// border is included in total caption height
		CalcWindowBorder(m_wndBorderStyle);

		hcaption=CalcCaptionTotalHeight();
		int rowheight=m_list.GetRowHeight();

		// default height is maximum list height

		int hdefault=__max(rowheight,sizelistmax.cy);

		// minimum width for now is the same as default caption width
		sizewndmin.cx=sizecaption.cx;
		sizewndmin.cy=hcaption+m_list.GetRowHeight();

		sizewndmax.cx=wdefault;
		sizewndmax.cy=hcaption+hdefault;

		wndrc.bottom=wndrc.top+hcaption+hdefault; // initial height
		wndrc.InflateRect(m_wndBorder.cx, 0);
		wndrc.bottom+=m_wndBorder.cy; // top border included in caption total height

		sizewndmax.cx+=m_wndBorder.cx*2;
		sizewndmax.cy+=m_wndBorder.cy;
		sizewndmin.cy+=m_wndBorder.cy;

		// add caption buttons to window width
		CRect rcbutton;
		CalcMaxButtonRect(rcbutton);
		int gap=m_captionButtonGap;

		if(m_wndTheme.IsThemeNull()) // no gap between buttons for classic
			gap=0;

		gap+=rcbutton.Width()*2+
			m_captionButtonTextGap;

		wndrc.right+=gap; 
		sizewndmax.cx+=gap;

		MARGINS scaled = {0, 0, 0, 0};

		if(!(m_style&NO_MARGIN))
		{
			scaled=m_listMargin;
			ScaleMargins(scaled);
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

#ifdef _DEBUG
		//AtlTrace("Window width:%d, height:%d\r\n",int(wndrc.Width),int(wndrc.Height)); 
#endif
	}

	PointF p = m_position;
	m_canvas->AllTransform(p);
	CPoint ip=static_cast<CPoint>(p);

	if(bForce || ip.x!=m_origin.x || ip.y!=m_origin.y)
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

	UINT flags=(bSizeChange?0:SWP_NOSIZE)|(bPositionChange?0:SWP_NOMOVE);
	SetWindowPos(wndrc, flags);

	size.Height=REAL(wndrc.Height());
	size.Width=REAL(wndrc.Width());
	return size;
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::ClearCaches()
{

}

//-----------------------------------------------------------------------------

void CListWindowRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
	bool bRedraw=(flags&SWP_NOREDRAW)==0; 
	// parent redraw sends paint to all children,
	// so here we avoid sending redraw to children again
	flags|=SWP_NOREDRAW;

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

		PointF newp(REAL(m_rcWnd.left), REAL(m_rcWnd.top));
		m_canvas->RAllTransform(newp);
		m_position=newp; 

		m_origin.x=m_rcWnd.left;
		m_origin.y=m_rcWnd.top;

		// offset all rects
		m_rcClient.OffsetRect(offset);
		m_rcCaption.OffsetRect(offset);

		m_rcButMax.OffsetRect(offset);
		m_rcButMin.OffsetRect(offset);
		m_rcGripper.OffsetRect(offset);

		CRect rclist;
		CalcListRect(rclist);
		m_list.SetWindowPos(rclist, flags);
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

		int hicaption=CalcCaptionTotalHeight();

		m_rcCaption=m_rcWnd;

		m_rcCaption.bottom=m_rcCaption.top+hicaption;
		if(m_wndTheme.IsThemeNull())
		{
			m_rcCaption.top+=m_wndBorder.cy;
			m_rcCaption.DeflateRect(m_wndBorder.cx,0);
		}

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

			CalcMaxButtonRect(m_rcButMax);
			CalcMinButtonRect(m_rcButMin);
			CalcGripperRect(m_rcGripper);

			CRect rclist;
			CalcListRect(rclist);
			m_list.SetWindowPos(rclist, flags);
		}
	}

	CRectEx rcex(m_rcWnd); 
	m_wnd_sizes[m_canvas->GetDpiScale()]=rcex;

	if(bRedraw)
		Invalidate();
}

//-----------------------------------------------------------------------------

int CListWindowRenderer::CalcCaptionSize(CDCHandle& dc, const TCHAR* title)
{
	std::_tstring txt=_T("Example Title"); // for minimum caption width 

	CSize extent(0,0);
	CAutoFont f(m_canvas->GetCaptionFont(m_canvas->GetFontPixHeight()),dc);

	dc.GetTextExtent(txt.c_str(), txt.length(), &extent);
	m_captionSizes[m_canvas->GetDpiScale()]=extent;

	dc.GetTextExtent(title, _tcslen(title), &extent);

	// caption size includes margins for caption text

	bool bThemeOk=false;
	int captionState=FS_INACTIVE;
		
	if(IsSelected())
		captionState=FS_ACTIVE;

	if(!m_wndTheme.IsThemeNull())
	{
		bThemeOk=SUCCEEDED(m_wndTheme.GetThemeMargins(
			dc,
			WP_SMALLCAPTION,
			captionState,
			TMT_CAPTIONMARGINS,
			NULL, 
			&m_captionTextMargin));
	}

	if(!bThemeOk)
	{
		m_captionTextMargin.cxLeftWidth=
		m_captionTextMargin.cxRightWidth=
		m_captionTextMargin.cyBottomHeight=
		m_captionTextMargin.cyTopHeight=0;
	}

	if(m_wndTheme.IsThemeNull())
	{
		m_captionContentMargin.cxLeftWidth=
		m_captionContentMargin.cxRightWidth=
		m_captionContentMargin.cyBottomHeight=
		m_captionContentMargin.cyTopHeight=2;
	}
	else
	{
		bThemeOk=SUCCEEDED(m_wndTheme.GetThemeMargins(
				dc,
				WP_SMALLCAPTION,
				captionState,
				TMT_CONTENTMARGINS,
				NULL, 
				&m_captionContentMargin));

		if(!bThemeOk)
		{
			m_captionContentMargin.cxLeftWidth=
				m_captionContentMargin.cxRightWidth=
				m_captionContentMargin.cyBottomHeight=
				m_captionContentMargin.cyTopHeight=1;
		}
	}

	return extent.cx;
}

//-----------------------------------------------------------------------------
// 
// Currently we do not scale caption text margins and border. If we do, 
// consider pre-calculating everything and caching it.
//

int CListWindowRenderer::CalcCaptionTotalHeight(void)
{
	int height=m_captionSizes[m_canvas->GetDpiScale()].cy;

	// we include top window border in total caption size
	// extra +border is just for extra space if margins are 0
	
	height+=
		m_wndBorder.cy+
		m_captionContentMargin.cyTopHeight+
		m_captionContentMargin.cyBottomHeight+
		m_captionTextMargin.cyTopHeight+
		m_captionTextMargin.cyBottomHeight+
		m_wndBorder.cy;

	return int(height);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawWindow(CAggDC& dc)
{
	DrawEdges(dc);

	CDCHandle hdc(dc.m_hDC);

	DrawWindowBorder(hdc,m_wndBorderStyle);

	if(m_size!=WINDOW_SIZE_MIN)
		DrawWindowBkgnd(hdc);

	DrawWindowCaption(hdc, CS_SMALL);
	DrawMaxButton(hdc);

	if(m_size!=WINDOW_SIZE_MIN)
	{
		DrawMinButton(hdc);
		DrawList(hdc);
		if(m_size==WINDOW_SIZE_NORM)
			DrawGripper(hdc);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::CalcWindowBorder(BORDER_STYLE style)
{
	if(!m_wndTheme.IsThemeNull())
	{
		// All theme-based sizing border calls fail so far
		m_wndBorder.cx=GetSystemMetrics(SM_CXFRAME);
		m_wndBorder.cy=GetSystemMetrics(SM_CYFRAME);
	}
	else
	{
		CDC memDC;
		CRect rc(0,0,20,20), rcsave(rc);

		// We are drawing border only to measure it with BF_ADJUST flag
		// To avoid drawing into desktop (or any window) DC use memory DC 
		
		{
			CWindowDC dc(::GetDesktopWindow());
			memDC.CreateCompatibleDC(dc);
		}

		UINT flags=BF_RECT | BF_ADJUST;

		if( style==LINE )
			flags |= BF_MONO;

		memDC.DrawEdge(
			&rc,
			EDGE_RAISED,
			flags
			);

		m_wndBorder.cx=rcsave.right-rc.right;
		m_wndBorder.cy=rcsave.bottom-rc.bottom;
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawWindowBorder(
	CDCHandle& dc,
	BORDER_STYLE style)
{
	UINT flags=BF_RECT;
	CRect rc(m_rcWnd);

	if(m_wndTheme.IsThemeNull())
	{
		if( style==LINE )
			flags |= BF_MONO;

		dc.DrawEdge(
			&rc,
			EDGE_RAISED,
			flags
			);
	}
	else // themed drawing
	{
		int borderState=FS_INACTIVE;
		
		if(IsSelected())
			borderState=FS_ACTIVE;

		rc.top+=CalcCaptionTotalHeight();
		CRect border(rc);

		border.right=border.left+m_wndBorder.cx;
		m_wndTheme.DrawThemeBackground(
			dc,
			WP_SMALLFRAMELEFT,
			borderState,
			border );

		border=rc;
		border.left=border.right-m_wndBorder.cx;
		m_wndTheme.DrawThemeBackground(
			dc,
			WP_SMALLFRAMERIGHT,
			borderState,
			border );

		border=rc;
		border.top=border.bottom-m_wndBorder.cy;
		m_wndTheme.DrawThemeBackground(
			dc,
			WP_SMALLFRAMEBOTTOM,
			borderState,
			border );
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawWindowCaption(
	CDCHandle& dc, 
	CAPTION_STYLE style)
{
	CRect rc(m_rcCaption);	
	UINT tflags;
	COLORREF clrtext;
	COLORREF clrtextshadow;
	POINT offset;
	int shadow;

	tflags=DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
	shadow=TST_NONE;
	clrtextshadow=0; // to avoid runtime check failures

	int idc=dc.SaveDC();

	if(m_wndTheme.IsThemeNull())
	{
		DrawClassicCaptionBkgnd(dc, rc);
		clrtext=GetSysColor(
			IsSelected()?COLOR_CAPTIONTEXT:COLOR_INACTIVECAPTIONTEXT);
	} 
	else  
	{
		int captionState=CS_INACTIVE;
		
		if(IsSelected())
			captionState=CS_ACTIVE;

		m_wndTheme.DrawThemeBackground( dc, WP_SMALLCAPTION, captionState, rc );

		if(captionState==CS_ACTIVE)
		{
			if(FAILED(m_wndTheme.GetThemeColor(
				WP_SMALLCAPTION, captionState, TMT_CAPTIONTEXT, &clrtext)))
			{
				clrtext=m_wndTheme.GetThemeSysColor(COLOR_CAPTIONTEXT);
			}
		}
		else
		{
			if(FAILED(m_wndTheme.GetThemeColor(
				WP_SMALLCAPTION, captionState, TMT_INACTIVECAPTIONTEXT, &clrtext)))
			{
				clrtext=m_wndTheme.GetThemeSysColor(COLOR_INACTIVECAPTIONTEXT);
			}
		}

		if(SUCCEEDED(m_wndTheme.GetThemeEnumValue(
			WP_SMALLCAPTION, captionState, TMT_TEXTSHADOWTYPE, &shadow)))
		{
			if(shadow!=TST_NONE)
			{
				if(FAILED(m_wndTheme.GetThemeColor(
					WP_SMALLCAPTION, captionState, TMT_TEXTSHADOWCOLOR, &clrtextshadow)))
				{
					clrtextshadow=m_wndTheme.GetThemeSysColor(COLOR_3DDKSHADOW);
				}

				if(FAILED(m_wndTheme.GetThemePosition(
					WP_SMALLCAPTION, captionState, TMT_TEXTSHADOWOFFSET, &offset)))
				{
					offset.x=1;
					offset.y=1;
				}
			}
		}
	
		rc.DeflateRect(m_wndBorder.cx, m_wndBorder.cy, m_wndBorder.cx, 0);
	}

	{
		CAutoFont f(m_canvas->GetCaptionFont(m_canvas->GetFontPixHeight()),	dc);

		// prepare text rectangle w.r.t. caption rectangle

		if(m_size!=WINDOW_SIZE_MIN)
		{
			int gap;

			if(m_wndTheme.IsThemeNull())
			{
				gap=0; // classic has no space between buttons

				// we already deflated rect for window borders but caption
				// text drawn without margin on the left looks ugly
				rc.left+=m_wndBorder.cx;
			}
			else
			{
				gap=m_captionButtonGap;
				rc.left+=
					m_captionContentMargin.cxLeftWidth+
					m_captionTextMargin.cxLeftWidth; 
			}

			rc.right-=m_rcButMax.Width()*2+
				m_captionButtonTextGap+
				gap+
				m_captionContentMargin.cxRightWidth+
				m_captionTextMargin.cxRightWidth;

		} 
		else
		{
			// we already deflated rect for window borders but caption
			// text drawn without margin on the left looks ugly
			rc.left+=m_wndBorder.cx;

			rc.left+=
				m_captionContentMargin.cxLeftWidth+
				m_captionTextMargin.cxLeftWidth; 

			// only restore button in minimized case

			rc.right-=m_rcButMax.Width()+
				m_captionButtonTextGap+
				m_captionContentMargin.cxRightWidth+
				m_captionTextMargin.cxRightWidth;
		}

		rc.top+=m_captionContentMargin.cyTopHeight+
			m_captionTextMargin.cyTopHeight;

		rc.bottom=rc.top+m_captionSizes[m_canvas->GetDpiScale()].cy;

		std::_tstring txt = _T("Vertex-");
		txt += m_g->GetLabel();

		DrawCaptionText(
			dc,
			txt.c_str(),
			&rc,
			tflags,
			shadow,
			clrtext,
			clrtextshadow,
			offset );
	}

	dc.RestoreDC(idc);
}

//-----------------------------------------------------------------------------

#ifndef DC_GRADIENT
#define DC_GRADIENT         0x0020
#endif

void CListWindowRenderer::DrawClassicCaptionBkgnd(
	CDCHandle& dc, 
	const CRect& rc)
{
	BOOL bGrad=FALSE, bSuccess;
	// check if user has classic gradient selected
	bSuccess=::SystemParametersInfo(
		SPI_GETGRADIENTCAPTIONS,
		0,
		&bGrad,
		0)!=0;

	if(bSuccess)
	{
		if(bGrad)
		{
			int flags=DC_TEXT | DC_GRADIENT | (IsSelected()?DC_ACTIVE:0);

			// we must have DC_TEXT and window that has empty but not NULL text

			bSuccess=::DrawCaption(
				::GetDesktopWindow(),
				dc,
				&rc,
				flags)!=0;
		}
		else
			bSuccess=false;
	}

	if(!bSuccess)
	{
		COLORREF clrcaption=GetSysColor(
			IsSelected()?COLOR_ACTIVECAPTION:COLOR_INACTIVECAPTION);

		dc.FillSolidRect(&rc,clrcaption);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawCaptionText(
	CDCHandle& dc,
	const TCHAR* txt,
	CRect* rctext,
	UINT flags,
	int shadowtype,
	COLORREF clrtext,
	COLORREF clrshadow,
	const POINT& offset)
{
	if(shadowtype==TST_CONTINUOUS)
	{
		dc.DrawShadowText(
			CT2W(txt,CP_UTF8),
			_tcslen(txt),
			rctext,
			flags,
			clrtext,
			clrshadow,
			offset.x,
			offset.y );
	}
	else // single or no shadow
	{
		dc.SetBkMode(TRANSPARENT);

		if(shadowtype==TST_SINGLE)
		{
			CRect rcshadow(*rctext);

			rcshadow.OffsetRect(offset);

			dc.SetTextColor(clrshadow);
			dc.DrawText(
				txt, 
				_tcslen(txt), 
				rcshadow, 
				flags );
		}

		dc.SetTextColor(clrtext);
		dc.DrawText(
			txt, 
			_tcslen(txt), 
			rctext, 
			flags );
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawWindowBkgnd(
	CDCHandle& dc) 
{
	dc.FillSolidRect(&m_rcClient,GetSysColor(COLOR_3DFACE));
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::CalcGripperRect(CRect& rc)
{
	if(m_sizeGripper.cx==-1) // not initialized
	{
		bool bThemeOk=false;

		if(!m_statusTheme.IsThemeNull())
		{
			if(SUCCEEDED(m_statusTheme.GetThemePartSize(
				0,
				SP_GRIPPER,
				0,
				NULL,
				TS_DRAW,
				&m_sizeGripper)))
				bThemeOk=true;
		}

		if(!bThemeOk)
		{
			m_sizeGripper.cx=GetSystemMetrics(SM_CXHSCROLL);
			m_sizeGripper.cy=GetSystemMetrics(SM_CYHSCROLL);
		}
	}

	SizeF gripdim(
		REAL(m_sizeGripper.cx),
		REAL(m_sizeGripper.cy));

	m_canvas->ScaleTransform(gripdim);

	CRect rcscale(0,0,int(gripdim.Width),int(gripdim.Height));
	rcscale.OffsetRect(m_rcClient.right-rcscale.Width()-1,
		m_rcClient.bottom-rcscale.Height()-1);

	rc=rcscale;
}	

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawGripper(CDCHandle& dc)
{
	bool bThemeDrawOk=false;
	CRect rc(m_rcGripper);	

	if(!m_statusTheme.IsThemeNull())
	{
		CRect adjusted; // theme gripper does not scale

		int savedc=dc.SaveDC();

		adjusted=rc;
		adjusted.top=adjusted.bottom-m_sizeGripper.cy;
		adjusted.left=adjusted.right-m_sizeGripper.cx;

		dc.IntersectClipRect(&rc);

		if(SUCCEEDED(m_statusTheme.DrawThemeBackground(
				dc,
				SP_GRIPPER,
				0,
				&adjusted)))
			bThemeDrawOk=true;

		dc.RestoreDC(savedc);
	}

	if(!bThemeDrawOk)
	{
		// frame control is not drawn transparently so we clip
		// drawing region to triangle
		CPoint points[3];

		points[0].x=rc.right;
		points[0].y=rc.top;
		points[1]=rc.BottomRight();
		points[2].x=rc.left;
		points[2].y=rc.bottom;

		CRgn rgn;
		rgn.CreatePolygonRgn(points, 3, WINDING);
		dc.SelectClipRgn(rgn);
		dc.DrawFrameControl(&rc,DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
		dc.SelectClipRgn(0);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::CalcMaxButtonRect(CRect& rc)
{
	CRect buttrc;

	// caption total size always includes top window border
	int wh=CalcCaptionTotalHeight()-m_wndBorder.cy;
	wh-=m_captionContentMargin.cyBottomHeight+m_captionContentMargin.cyTopHeight;

	if(m_wndTheme.IsThemeNull()) // classic look has buttons centered vertically
	{
		buttrc.top=m_rcWnd.top+m_wndBorder.cy+m_captionContentMargin.cyTopHeight;
	}
	else
	{
		buttrc.top=m_rcWnd.top+m_wndBorder.cy;
	}

	buttrc.bottom=buttrc.top+wh;
	buttrc.right=m_rcClient.right-m_captionContentMargin.cxRightWidth;
	buttrc.left=buttrc.right-wh;

	rc=buttrc;
}	

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawMaxButton(CDCHandle& dc)
{
	bool bThemeDrawOk=false;

	if(!m_wndTheme.IsThemeNull())
	{
		int state=0;

		if(m_size==WINDOW_SIZE_NORM)
		{
			if(!IsSelected())
				state=MAXBS_DISABLED;

			switch(m_maxButtonState.m_state)
			{
			case BS_NORMAL:
				state+=MAXBS_NORMAL;
				break;
			case BS_HOT:
				state+=MAXBS_HOT;
				break;
			case BS_PUSHED:
				state+=MAXBS_PUSHED;
				break;
			case BS_DISABLED:
				state+=MAXBS_DISABLED;
				break;
			default:
				ATLASSERT(0);
			}

			if(SUCCEEDED(m_wndTheme.DrawThemeBackground(
				dc,
				WP_MAXBUTTON,
				state,
				&m_rcButMax)))
				bThemeDrawOk=true;
		}
		else
		{
			if(!IsSelected())
				state=RBS_DISABLED;

			switch(m_maxButtonState.m_state)
			{
			case BS_NORMAL:
				state+=RBS_NORMAL;
				break;
			case BS_HOT:
				state+=RBS_HOT;
				break;
			case BS_PUSHED:
				state+=RBS_PUSHED;
				break;
			case BS_DISABLED:
				state+=RBS_DISABLED;
				break;
			default:
				ATLASSERT(0);
			}

			if(SUCCEEDED(m_wndTheme.DrawThemeBackground(
				dc,
				WP_RESTOREBUTTON,
				state,
				&m_rcButMax)))
				bThemeDrawOk=true;
		}
	}

	if(!bThemeDrawOk)
	{
		UINT state=
			m_size==WINDOW_SIZE_NORM ? DFCS_CAPTIONMAX : DFCS_CAPTIONRESTORE;

		switch(m_maxButtonState.m_state)
		{
		case BS_NORMAL:
			break;
		case BS_HOT:
			state|=DFCS_HOT;
			break;
		case BS_PUSHED:
			state|=DFCS_PUSHED;
			break;
		case BS_DISABLED:
			state|=DFCS_INACTIVE;
			break;
		default:
			ATLASSERT(0);
		}

		dc.DrawFrameControl(&m_rcButMax,DFC_CAPTION,state);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::CalcMinButtonRect(CRect& rc)
{
	CRect rcmaxbutt(m_rcButMax); // assumes max is always calculated first
	int gap=m_captionButtonGap;

	if(m_wndTheme.IsThemeNull())
		gap=0;

	rcmaxbutt.OffsetRect(-(rcmaxbutt.Width()+gap),0);

	rc=rcmaxbutt;
}	

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawMinButton(CDCHandle& dc)
{
	bool bThemeDrawOk=false;

	if(!m_wndTheme.IsThemeNull())
	{
		int state=0;

		if(!IsSelected())
			state=MINBS_DISABLED;

		switch(m_minButtonState.m_state)
		{
		case BS_NORMAL:
			state+=MINBS_NORMAL;
			break;
		case BS_HOT:
			state+=MINBS_HOT;
			break;
		case BS_PUSHED:
			state+=MINBS_PUSHED;
			break;
		case BS_DISABLED:
			state+=MINBS_DISABLED;
			break;
		default:
			ATLASSERT(0);
		}

		if(SUCCEEDED(m_wndTheme.DrawThemeBackground(
				dc,
				WP_MINBUTTON,
				state,
				&m_rcButMin)))
			bThemeDrawOk=true;
	}

	if(!bThemeDrawOk)
	{
		UINT state=DFCS_CAPTIONMIN;

		switch(m_minButtonState.m_state)
		{
		case BS_NORMAL:
			break;
		case BS_HOT:
			state|=DFCS_HOT;
			break;
		case BS_PUSHED:
			state|=DFCS_PUSHED;
			break;
		case BS_DISABLED:
			state|=DFCS_INACTIVE;
			break;
		default:
			ATLASSERT(0);
		}

		dc.DrawFrameControl(&m_rcButMin,DFC_CAPTION,state);
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::CalcListRect(CRect& rc)
{
	rc=m_rcClient;

	if(!(m_style&NO_MARGIN))
	{
		MARGINS scaled=m_listMargin;
		ScaleMargins(scaled);

		rc.DeflateRect(
			scaled.cxLeftWidth, 
			scaled.cyTopHeight, 
			scaled.cxRightWidth, 
			scaled.cyBottomHeight);
	}

#ifdef DEBUG
	if(m_size==WINDOW_SIZE_MAX)
	{
		CSize maxlist=m_max_sizes_list[m_canvas->GetDpiScale()];
//		int newh=rc.Height();
//		ATLASSERT(maxlist.cy==newh);
	}
#endif
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawList(CDCHandle& dc)
{
	m_list.SendMessage(WM_PAINT, (WPARAM)(&dc), 0);
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::DrawEdges(CAggDC& dc)
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
							{
								ier->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
							}
						}
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CListWindowRenderer::ScaleMargins(MARGINS& margins)
{
	RectF scaled(
		REAL(margins.cxLeftWidth),
		REAL(margins.cyTopHeight),
		REAL(margins.cxRightWidth),
		REAL(margins.cyBottomHeight));

	m_canvas->ScaleTransform(scaled);

	margins.cxLeftWidth=int(scaled.x);
	margins.cyTopHeight=int(scaled.y);
	margins.cxRightWidth=int(scaled.Width);
	margins.cyBottomHeight=int(scaled.Height);
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CListWindowRenderer::HitTestCaption(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcCaption.PtInRect(p))
	{
		code=HitTestMaxButton(p);
		// no min button when already at minimum size
		if(m_size!=WINDOW_SIZE_MIN)
		{
			if(code==HT_UNKNOWN)
				code=HitTestMinButton(p);
		}

		if(code==HT_UNKNOWN)
			code=HT_CAPTION;
	}

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CListWindowRenderer::HitTestMaxButton(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcButMax.PtInRect(p))
		code=HT_MAXBUTTON;

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CListWindowRenderer::HitTestMinButton(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcButMin.PtInRect(p))
		code=HT_MINBUTTON;

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CListWindowRenderer::HitTestGripper(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_size!=WINDOW_SIZE_MIN) // should be only for normal size
	{
		CRect rc(m_rcGripper);

		// add window border to hit test area
		rc.InflateRect(0,0,m_wndBorder.cx, m_wndBorder.cy);

		if(rc.PtInRect(p))
			code=HT_SIZEGRIP;
	}

	return code;
}
							 
//*****************************************************************************
//
// Message handlers
//

LRESULT CListWindowRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM /*lParam*/,
	BOOL& /*bHandled*/)
{
	CAggDC& dc=*(CAggDC*)wParam;

	DrawWindow(dc); 

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::OnLButtonDown(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	LRESULT lRes=0;
	bool bRepaint=false;
	bool bOldSelection=IsSelected();
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	HIT_TEST_CODE hit=HitTestCaption(pt);
	CButtonState* state=0;
	BUTTON_STATE oldstate;
	LPTSTR cursortype=IDC_ARROW;

	m_g->MoveToEnd();
	SetFocus();

	if((wParam & MK_CONTROL)!=0)
	{
		CScaledWindow::SetSelected(GetID(), !IsSelected(), true);
	}
	else
	{
		CScaledWindow::SetSelected(GetID(), true, (wParam & MK_SHIFT)!=0 );
	}

	
	if(hit!=HT_UNKNOWN) // in caption
	{
		m_capturedPart=hit;
		m_canvas->SetCapture(this);

		if(hit==HT_MAXBUTTON)
			state=&m_maxButtonState;
		else if(hit==HT_MINBUTTON)
			state=&m_minButtonState;
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
		// try the gripper, then the list
		if(m_size==WINDOW_SIZE_NORM)
		{
			hit=HitTestGripper(pt);

			if(hit!=HT_UNKNOWN)
			{
				m_capturedPart=hit;
				cursortype=IDC_SIZENWSE;
				m_canvas->SetCapture(this);
				m_capturePos=pt;
			}
		}

		if(hit==HT_UNKNOWN && m_list.HitTest(pt))
			lRes=m_list.SendMessage(uMsg, wParam, lParam);
	}

	if(bRepaint)
		Invalidate();

	if(IsSelected()!=bOldSelection)
	{
		m_canvas->NotifySelection(m_g->GetID());
	}

	SetCursor(LoadCursor(NULL, cursortype));

	return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::OnLButtonUp(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& bHandled)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	HIT_TEST_CODE hit=HitTestCaption(pt);
	bool bRepaint=false;
	CButtonState* capstate=0, *hitstate=0;
	LPTSTR cursortype=IDC_ARROW;

	if(hit==HT_MAXBUTTON)
		hitstate=&m_maxButtonState;
	else if(hit==HT_MINBUTTON)
		hitstate=&m_minButtonState;

	if(m_canvas->GetCapture()==this)
	{
		// only caption, caption buttons and gripper
		// can capture mouse
		if(m_capturedPart==HT_SIZEGRIP)
		{
			cursortype=IDC_SIZENWSE;
		}
		else if(m_capturedPart==HT_CAPTION)
		{
		}
		else
		{
			if(m_capturedPart==HT_MAXBUTTON)
				capstate=&m_maxButtonState;
			else if(m_capturedPart==HT_MINBUTTON)
				capstate=&m_minButtonState;

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
				{
					if(m_size!=WINDOW_SIZE_NORM)
						SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
					else
						SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				}
				else
				{
					SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				}
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
			{
				hitstate->SetHot(true);
			}
		}
		else // check children
		{
			// try the gripper, then the list
			if(m_size==WINDOW_SIZE_NORM)
			{
				hit=HitTestGripper(pt);

				if(hit!=HT_UNKNOWN)
					cursortype=IDC_SIZENWSE;
			}

			if(hit==HT_UNKNOWN && m_list.HitTest(pt))
				m_list.SendMessage(uMsg, wParam, lParam);
		}
	}

	if(bRepaint)
		Invalidate();

	SetCursor(LoadCursor(NULL, cursortype));

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::OnMouseMove(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	HIT_TEST_CODE hit=HitTestCaption(pt);
	bool bRepaint=false;
	CButtonState* capstate=0, *hitstate=0;
	LPTSTR cursortype=IDC_ARROW;

	// only buttons have state
	if(hit==HT_MAXBUTTON)
		hitstate=&m_maxButtonState;
	else if(hit==HT_MINBUTTON)
		hitstate=&m_minButtonState;

	if(m_canvas->GetCapture()==this)
	{
		CRect newrc=m_rcWnd;

		// only caption, caption buttons and gripper
		// can capture mouse in this window
		if(m_capturedPart==HT_SIZEGRIP)
		{
			cursortype=IDC_SIZENWSE;

			CSize minwnd=m_min_sizes[m_canvas->GetDpiScale()];

			// prevent resizing when cursor is on the other side
			// of minimum possible size
			if(pt.x<(m_rcWnd.left+minwnd.cx))
				pt.x=m_capturePos.x;

			if(pt.y<(m_rcWnd.top+minwnd.cy))
				pt.y=m_capturePos.y;

			CSize maxwnd=m_max_sizes[m_canvas->GetDpiScale()];
			newrc.InflateRect(0,0,pt.x-m_capturePos.x,pt.y-m_capturePos.y);

			if(newrc.Width()>maxwnd.cx)
				newrc.right=newrc.left+maxwnd.cx;
		
			if(newrc.Width()<minwnd.cx)
				newrc.right=newrc.left+minwnd.cx;

			if(newrc.Height()>maxwnd.cy)
				newrc.bottom=newrc.top+maxwnd.cy;
		
			if(newrc.Height()<minwnd.cy)
				newrc.bottom=newrc.top+minwnd.cy;

			if(!newrc.EqualRect(m_rcWnd))
			{
				CSize normwnd(newrc.Width(),newrc.Height());
				m_norm_sizes[m_canvas->GetDpiScale()]=normwnd;

				SetWindowPos(newrc, SWP_NOMOVE);
			}

			m_capturePos=pt;
		}
		else if(m_capturedPart==HT_CAPTION)
		{
			pt.x+=m_savePos.x-m_capturePos.x;
			pt.y+=m_savePos.y-m_capturePos.y;
			newrc.TopLeft()=pt;
			newrc.right=newrc.left+m_rcWnd.Width();
			newrc.bottom=newrc.top+m_rcWnd.Height();
			SetWindowPos(newrc, SWP_NOSIZE);
		}
		else
		{
			if(m_capturedPart==HT_MAXBUTTON)
				capstate=&m_maxButtonState;
			else if(m_capturedPart==HT_MINBUTTON)
				capstate=&m_minButtonState;

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
		else // check children
		{
			// try the gripper, then the list
			if(m_size==WINDOW_SIZE_NORM)
			{
				hit=HitTestGripper(pt);

				if(hit!=HT_UNKNOWN)
				{
					m_canvas->ClearCurrentHot();
					cursortype=IDC_SIZENWSE;
				}
			}

			if(hit==HT_UNKNOWN && m_list.HitTest(pt))
				m_list.SendMessage(uMsg, wParam, lParam);
			else
				m_canvas->ClearCurrentHot();
		}
	}

	if(bRepaint)
		Invalidate();

	SetCursor(LoadCursor(NULL, cursortype));

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::OnMouseDblclk(
	UINT /*uMsg*/,
	WPARAM /*wParam*/,
	LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	if(m_size==WINDOW_SIZE_MIN)
	{
		HIT_TEST_CODE hit=HitTestCaption(pt);
		if(hit==HT_CAPTION)
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::OnSysCommand(
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
		rc.bottom=rc.top+hcaption;
		// for minimized classic caption bars we add bottom border
		if(m_wndTheme.IsThemeNull())
			rc.bottom+=m_wndBorder.cy;
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
	else if(wParam==SC_RESTORE)
	{
		m_size=WINDOW_SIZE_NORM;
		CSize sizewndnorm=m_norm_sizes[m_canvas->GetDpiScale()];

		rc.right=rc.left+sizewndnorm.cx;
		rc.bottom=rc.top+sizewndnorm.cy;
		SetWindowPos(rc, SWP_NOMOVE);
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CListWindowRenderer::OnNcHitTest(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LRESULT lRes=0;

	if(m_list.HitTest(pt))
		lRes=m_list.SendMessage(uMsg, wParam, lParam);

	return lRes;
}

//*****************************************************************************

IRenderer *CreateWindowListRenderer(CCanvas & canvas)
{
	CListWindowRenderer * retVal = new CListWindowRenderer(canvas);
	return retVal;
}

