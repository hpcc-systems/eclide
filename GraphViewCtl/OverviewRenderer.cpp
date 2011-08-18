#include "StdAfx.h"
#include "resource.h"
#include "Canvas.h"
#include "ScaledWindows.h"
#include "Geometry.h"
#include "AggDC.h"
#include "XMLWriter.h"

class COverviewRenderer : public IOverviewRenderer, public CScaledWindow
{
	IGraph* m_graph;
	
	CTransform m_transform;
	GraphTypes::PointF m_position; // zoom renderer top/left offset (device coordinates)
	GraphTypes::SizeF m_wndSize; // pixel size of zoom renderer (device coordinates)
	GraphTypes::SizeF m_restoreSize; // old pixel size of zoom renderer (device coordinates)
	CAggDC* m_memBmp;
	bool m_inWindow;
	std::_tstring m_title;
	CRect m_rcCaption;
	CRect m_rcButClose;
	CRect m_rcButMin;
	CRect m_rcGripper;
	CPartTheme& m_statusTheme;
	CButtonState m_closeButtonState;
	CButtonState m_minButtonState;
	CSize m_minWndSize;
	int m_titleHeight;

	static CSize m_sizeGripper;
	static MARGINS m_captionTextMargin;
	static MARGINS m_captionContentMargin;
	static int m_border;
	static int m_captionButtonGap;

	GraphTypes::Color m_colorBorder;
	GraphTypes::Color m_colorActiveTitleBar;
	GraphTypes::Color m_colorTitleBar;
	GraphTypes::Color m_colorActiveTitle;
	GraphTypes::Color m_colorInactiveTitle; 

	GraphTypes::PointF m_vppoints[4]; // app viewport in world coordinates
	
	CPoint m_capturePos;
	GraphTypes::PointF	m_savePos;
	HIT_TEST_CODE m_capturedPart;
	WINDOW_SIZE m_size;

	void SetWindowPos(const CRect& pos, UINT flags);
	virtual void Invalidate(CRect* rc=0, bool bErase=false) 
	{ 
		m_canvas->Invalidate(rc,bErase);
	}
	void ScreenToClient(CPoint& pt);
	void CenterViewport(const CPoint& pt);

	void Draw(CAggDC& dc);
	void DrawCaption(CAggDC& dc);
	void CalcGripperRect(CRect& rc);
	void DrawGripper(CAggDC& dc);
	void CalcCloseButtonRect(CRect& rc);
	void DrawCloseButton(CAggDC& dc);
	void CalcMinButtonRect(CRect& rc);
	void DrawMinButton(CAggDC& dc);
	void DrawRestoreButton(CAggDC& dc);

	HIT_TEST_CODE HitTestCaption(const CPoint& p);
	HIT_TEST_CODE HitTestCloseButton(const CPoint& p);
	HIT_TEST_CODE HitTestMinButton(const CPoint& p);
	HIT_TEST_CODE HitTestGripper(const CPoint& p);
	HIT_TEST_CODE HitTestPage(const CPoint& p);


public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IOverviewRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	COverviewRenderer(CCanvas& canvas, IGraph * graph);
	~COverviewRenderer();

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

	// IOverviewRenderer
	void SetInWindow(bool set);
	bool CursorInViewport(const CPoint& p);
	void RSizeTransform(GraphTypes::PointF& point);

protected:
	BEGIN_MSG_MAP(COverviewRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

CSize COverviewRenderer::m_sizeGripper;
MARGINS COverviewRenderer::m_captionTextMargin;
MARGINS COverviewRenderer::m_captionContentMargin;
int COverviewRenderer::m_border=1;
// gap between caption buttons
int COverviewRenderer::m_captionButtonGap=2;

using namespace GraphTypes;

//*****************************************************************************

COverviewRenderer::COverviewRenderer(CCanvas& canvas, IGraph * graph)
	: CScaledWindow(&canvas),
	m_graph(graph),
	m_position(10L,10L),
	m_wndSize(100,100),
	m_memBmp(0),
	m_inWindow(false),
	m_titleHeight(0),
	m_title(TEXT("Overview")),
	m_statusTheme(canvas.GetPartTheme(_T("STATUS"))),
	m_capturedPart(HT_UNKNOWN),
	m_size(WINDOW_SIZE_NORM),
	m_closeButtonState(&canvas, this),
	m_minButtonState(&canvas, this)
{
	m_transform.SetRotation(0);

	m_captionContentMargin.cxLeftWidth=
	m_captionContentMargin.cxRightWidth=
	m_captionContentMargin.cyBottomHeight=
	m_captionContentMargin.cyTopHeight=2;

	m_captionTextMargin.cxLeftWidth=m_captionTextMargin.cxRightWidth=2;
	m_captionTextMargin.cyBottomHeight=m_captionTextMargin.cyTopHeight=1;

	m_colorBorder.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));

	m_colorActiveTitleBar.SetFromCOLORREF(GetSysColor(COLOR_ACTIVECAPTION));
	m_colorTitleBar.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));

	m_colorActiveTitle.SetFromCOLORREF(GetSysColor(COLOR_CAPTIONTEXT));
	m_colorInactiveTitle.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTIONTEXT));

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

//-----------------------------------------------------------------------------

COverviewRenderer::~COverviewRenderer()
{
	delete m_memBmp;
}

//-----------------------------------------------------------------------------

CUniqueID COverviewRenderer::GetID()
{
	return m_graph->GetID();
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetGraphItem(IGraphItem* item)
{
	m_graph=static_cast<IGraph*>(item);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	RectF rc(m_rcWnd); 
	box=rc;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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

void COverviewRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==world)
	{
		m_canvas->RAllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	m_position=pt;

	if(co==world)
	{
		m_canvas->AllTransform(m_position);		
	}

	CRect rc(
		static_cast<int>(m_position.x),
		static_cast<int>(m_position.y),
		m_rcWnd.Width(),
		m_rcWnd.Height());

	SetWindowPos(rc, SWP_NOSIZE);
}

//-----------------------------------------------------------------------------

LRESULT COverviewRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetFocus(bool bSet)
{
	CScaledWindow::SetFocus(bSet);
	SetSelected(bSet);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetSelected(bool bSelected, bool bAdd)
{
	// bAdd is forced to true because we don't want to deselect subgraphs/vertices
	CScaledWindow::SetSelected(GetID(), bSelected, true);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::ClearCaches()
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetInWindow(bool set) 
{ 
	m_inWindow=set; 
	if(m_inWindow)
	{
		m_position.x=m_position.y=0; 
	}
}

//-----------------------------------------------------------------------------

bool COverviewRenderer::CursorInViewport(const CPoint& p)
{
	return HitTestPage(p)==HT_CONTAINER;
}

//-----------------------------------------------------------------------------

void COverviewRenderer::RSizeTransform(PointF& point)
{
	m_transform.RSizeTransform(point);
}

//-----------------------------------------------------------------------------

SizeF COverviewRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce, 
	bool bIgnoreExtent)
{
	SizeF size;

	if(m_titleHeight==0)
	{
		CSize extent;

		CAutoFont f(m_canvas->GetCaptionFont(::MulDiv(DEFAULT_FONT_SIZE-1,m_canvas->GetDPIY(),72)), dc);
		dc.GetTextExtent(m_title.c_str(), m_title.length(), &extent);
		m_titleHeight=extent.cy;
		m_titleHeight+=m_captionTextMargin.cyTopHeight+m_captionTextMargin.cyBottomHeight;

		CRect rcwnd;
		rcwnd.left=static_cast<int>(m_position.x);
		rcwnd.top=static_cast<int>(m_position.y);
		rcwnd.right=rcwnd.left+static_cast<int>(m_wndSize.Width);
		rcwnd.bottom=rcwnd.top+static_cast<int>(m_wndSize.Height);
		SetWindowPos(rcwnd, SWP_NOREDRAW);

		// TODO - maybe fix minimum size based on button visibility
		m_minWndSize.cx=50;
		m_minWndSize.cy=m_titleHeight+m_border+50;
	}

	if(m_size==WINDOW_SIZE_NORM)
	{
		RectF rcExtent;
		// graph extent in world coordinates, excluding rotation
		m_canvas->GetGraphExtent(rcExtent);

		m_canvas->RDpiTransform(rcExtent);
		m_canvas->RTranslateTransform(rcExtent);
		m_canvas->RScaleTransform(rcExtent);
		m_canvas->RSpacingTransform(rcExtent);

		// fit extent into the client area
		//REAL maxdim=std::max(rcExtent.Height,rcExtent.Width);
		REAL clientHeight=m_wndSize.Height-m_rcCaption.Height();
		//REAL mindim=min(m_wndSize.Width, clientHeight);
		//REAL scale=mindim/maxdim;
		REAL scalew=m_wndSize.Width/rcExtent.Width;
		REAL scaleh=clientHeight/rcExtent.Height;
		REAL scale=min(scalew,scaleh);

		m_transform.SetDPI(m_canvas->GetDPIX(),m_canvas->GetDPIY()); 
		m_transform.SetScale(scale);
		if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
			m_transform.SetSpacing(m_canvas->GetSpacingPoint());
		else
			m_transform.SetSpacing(m_canvas->GetSpacing());

		SizeF zoomextent;
		rcExtent.GetSize(&zoomextent);
		m_transform.SizeTransform(zoomextent);

		// transform extent's top/left point from world to zoom system to get offset
		PointF offset(rcExtent.x,rcExtent.y);
		m_transform.SizeTransform(offset);
		// size transform doesn't include spacing
		m_transform.SpacingTransform(offset);

		// center 
		if(m_wndSize.Width>zoomextent.Width)
			offset.x-=(m_wndSize.Width-zoomextent.Width)/2;
		if(clientHeight>zoomextent.Height)
			offset.y-=(clientHeight-zoomextent.Height)/2;

		// and account for the world being drawn at (0,caption height)
		offset.y-=m_rcCaption.Height();

		m_transform.SetOffset(offset);
		m_transform.SetRotation(m_canvas->GetRotation());

		// get viewport and xform it to world coordinates
		RectF vpf;
		m_canvas->GetViewport(vpf);
		CRect rcvp=static_cast<CRect>(vpf);

		m_vppoints[0].x=static_cast<REAL>(rcvp.left);
		m_vppoints[0].y=static_cast<REAL>(rcvp.top);
		m_vppoints[1].x=static_cast<REAL>(rcvp.right);
		m_vppoints[1].y=static_cast<REAL>(rcvp.top);
		m_vppoints[2].x=static_cast<REAL>(rcvp.right);
		m_vppoints[2].y=static_cast<REAL>(rcvp.bottom);
		m_vppoints[3].x=static_cast<REAL>(rcvp.left);
		m_vppoints[3].y=static_cast<REAL>(rcvp.bottom);

		m_canvas->RAllTransform(m_vppoints[0]);
		m_canvas->RAllTransform(m_vppoints[1]);
		m_canvas->RAllTransform(m_vppoints[2]);
		m_canvas->RAllTransform(m_vppoints[3]);

		// transform into zoom coordinates
		m_transform.AllTransform(m_vppoints[0]);
		m_transform.AllTransform(m_vppoints[1]);
		m_transform.AllTransform(m_vppoints[2]);
		m_transform.AllTransform(m_vppoints[3]);

		rcExtent.GetSize(&size);
	}

	return size;	
}

//-----------------------------------------------------------------------------

void COverviewRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
	CRect rcold(m_rcWnd);

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

		if(m_inWindow)
		{
			m_rcCaption.SetRect(0,0,0,0);
		}
		else
		{
			m_rcCaption.SetRect(0,0,m_rcWnd.Width(), m_titleHeight+m_border*2);

			CalcCloseButtonRect(m_rcButClose);
			CalcMinButtonRect(m_rcButMin);

			if(m_size!=WINDOW_SIZE_MIN)
				CalcGripperRect(m_rcGripper);
		}

		delete m_memBmp;
		m_memBmp=0;
	}

	m_position.x=static_cast<REAL>(m_rcWnd.left);
	m_position.y=static_cast<REAL>(m_rcWnd.top);
	m_wndSize.Width=static_cast<REAL>(m_rcWnd.Width());
	m_wndSize.Height=static_cast<REAL>(m_rcWnd.Height());

	if (!(flags&SWP_NOREDRAW))
	{
		Invalidate(&rcold);
		Invalidate(&m_rcWnd);
	}
}

//-----------------------------------------------------------------------------

void COverviewRenderer::ScreenToClient(CPoint& pt)
{
	// restrict mouse position inside the viewport (our screen)
	if(pt.x<0)
		pt.x=0;
	if(pt.y<0)
		pt.y=0;
	pt.x-=m_rcWnd.left;
	pt.y-=m_rcWnd.top;
}

//-----------------------------------------------------------------------------

void COverviewRenderer::CenterViewport(const CPoint& pt)
{
	// click point is to become new viewport center 
	PointF vpcenternew(pt.x, pt.y);
	m_transform.RAllTransform(vpcenternew); // from overview screen to world

	// current viewport center
	RectF vp;
	m_canvas->GetViewport(vp);
	PointF vpcenter(vp.x+vp.Width/2, vp.y+vp.Height/2);
	m_canvas->RAllTransform(vpcenter); // to world

	//PointF diff(vpcenternew.x-vpcenter.x, vpcenternew.y-vpcenter.y);
	//m_canvas->RotateTransform(diff);

	PointF offset(vpcenternew.x - vpcenter.x, vpcenternew.y - vpcenter.y);
	//m_transform.RSizeTransform(offset);
	m_canvas->SizeTransform(offset);
	PointF savePos = m_canvas->GetOffset();
	offset.x+=savePos.x;
	offset.y+=savePos.y;
	m_canvas->SetOffset(offset);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::DrawCaption(CAggDC& dc)
{
	CRect rc(m_rcCaption);	
	UINT flags;

	flags=DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

	int idc=dc.SaveDC();

	Color clrcaption=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
	Color clrtext=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

	PointF tl, br;

	tl.x=static_cast<REAL>(m_rcCaption.left);
	tl.y=static_cast<REAL>(m_rcCaption.top);
	br.x=static_cast<REAL>(m_rcCaption.right-1); // Agg draws one pixel more
	br.y=static_cast<REAL>(m_rcCaption.bottom);

	dc.Rect(tl, br, clrcaption, m_colorBorder);

	CDCHandle hdc(dc.m_hDC);
	CAutoFont f(m_canvas->GetCaptionFont(::MulDiv(DEFAULT_FONT_SIZE-1,m_canvas->GetDPIY(),72)), hdc);

	rc.DeflateRect(
		m_captionTextMargin.cxLeftWidth,
		m_captionTextMargin.cyTopHeight,
		m_captionTextMargin.cxRightWidth,
		m_captionTextMargin.cyBottomHeight);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(clrtext.ToCOLORREF());
	dc.DrawText(m_title.c_str(), m_title.length(), rc, flags);
	dc.RestoreDC(idc);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::CalcGripperRect(CRect& rc)
{
	CRect rcgrip(0,0, m_sizeGripper.cx,m_sizeGripper.cy);

	rcgrip.OffsetRect(m_rcWnd.Width()-m_border-m_sizeGripper.cx,
		m_rcWnd.Height()-m_border-m_sizeGripper.cy);
	rc=rcgrip;
}

//-----------------------------------------------------------------------------

void COverviewRenderer::DrawGripper(CAggDC& dc)
{
	bool bThemeDrawOk=false;
	CRect rc(m_rcGripper);	

	if(!m_statusTheme.IsThemeNull())
	{
		if(SUCCEEDED(m_statusTheme.DrawThemeBackground(
				dc,
				SP_GRIPPER,
				0,
				&rc)))
			bThemeDrawOk=true;
	}

	if(!bThemeDrawOk)
	{
		// frame control is not drawn transparently so we clip
		// drawing region to triangle
		CPoint points[3];
		rc.DeflateRect(2, 2, 0, 0);

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

void COverviewRenderer::CalcCloseButtonRect(CRect& rc)
{
	CRect buttrc;

	int wh = m_rcCaption.Height();
	wh -= m_captionContentMargin.cyBottomHeight + m_captionContentMargin.cyTopHeight;

	// button is centered vertically
	buttrc.top = m_border + m_captionContentMargin.cyTopHeight;

	buttrc.bottom = buttrc.top + wh;
	buttrc.right = m_rcWnd.Width() - m_border - m_captionContentMargin.cxRightWidth;
	buttrc.left = buttrc.right - wh;

	rc = buttrc;
}

//-----------------------------------------------------------------------------

void COverviewRenderer::DrawCloseButton(CAggDC& dc)
{
	CRect rc(m_rcButClose);

	Color color=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
	Color colorBorder=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

	switch(m_closeButtonState.m_state)
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

	tl.x+=2;
	tl.y+=2;
	br.x-=1;
	br.y-=1;
	dc.Line(tl, br, colorBorder);

	tl.x=static_cast<REAL>(rc.left+2);
	tl.y=static_cast<REAL>(rc.bottom-1);
	br.x=static_cast<REAL>(rc.right-1);
	br.y=static_cast<REAL>(rc.top+2);
	dc.Line(tl, br, colorBorder);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::CalcMinButtonRect(CRect& rc)
{
	CRect rcminbutt(m_rcButClose); // assumes Close button is always calculated first

	rcminbutt.OffsetRect(-(rcminbutt.Width()+m_captionButtonGap),0);

	rc=rcminbutt;
}

//-----------------------------------------------------------------------------

void COverviewRenderer::DrawMinButton(CAggDC& dc)
{
	CRect rc(m_rcButMin);

	Color color=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
	Color colorBorder=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

	switch(m_minButtonState.m_state)
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

	rc.DeflateRect(4,2);

	tl.x=static_cast<REAL>(rc.left);
	tl.y=static_cast<REAL>(rc.bottom-1);
	br.x=static_cast<REAL>(rc.right);
	br.y=static_cast<REAL>(rc.bottom);

	// a thick line 
	dc.Rect(tl, br, colorBorder, colorBorder);
}

//-----------------------------------------------------------------------------

void COverviewRenderer::DrawRestoreButton(CAggDC& dc)
{
	CRect rc(m_rcButMin);

	ATLASSERT(m_size==WINDOW_SIZE_MIN);

	Color color=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
	Color colorBorder=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

	switch(m_minButtonState.m_state)
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

	rc.DeflateRect(4,4);
	rc.OffsetRect(2,2);

	tl.x=static_cast<REAL>(rc.left);
	tl.y=static_cast<REAL>(rc.top);
	br.x=static_cast<REAL>(rc.right);
	br.y=static_cast<REAL>(rc.bottom);

	dc.Rect(tl, br, color, colorBorder);
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE COverviewRenderer::HitTestCaption(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcCaption.PtInRect(p))
	{
		code=HitTestCloseButton(p);
		if(code==HT_UNKNOWN)
			code=HitTestMinButton(p);
		if(code==HT_UNKNOWN)
			code=HT_CAPTION;
	}

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE COverviewRenderer::HitTestCloseButton(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcButClose.PtInRect(p))
		code=HT_CLOSEBUTTON;

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE COverviewRenderer::HitTestMinButton(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcButMin.PtInRect(p))
		code=HT_MINBUTTON;

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE COverviewRenderer::HitTestGripper(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_size!=WINDOW_SIZE_MIN) // should be only for normal size
	{
		CRect rc(m_rcGripper);

		// add window border to hit test area
		rc.InflateRect(0,0,m_border,m_border);

		if(rc.PtInRect(p))
			code=HT_SIZEGRIP;
	}

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE COverviewRenderer::HitTestPage(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;
	CRect rcpage(
		static_cast<CPoint>(m_vppoints[0]),
		static_cast<CPoint>(m_vppoints[2]));

	rcpage.NormalizeRect();
	CRect client(0, m_rcCaption.bottom, m_rcWnd.Width(), m_rcWnd.Height());
	rcpage.IntersectRect(rcpage,client);

	if(rcpage.PtInRect(p))
		code=HT_CONTAINER;

	return code;
}

//-----------------------------------------------------------------------------

void COverviewRenderer::Draw(CAggDC& dc)
{
	PointFVector vp, drawpt;
	GraphTypes::RectF rect;
	CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

	if(!m_memBmp)
	{
		CRect rc(0, 0, m_rcWnd.Width(), m_rcWnd.Height());
		m_memBmp=CAggDC::CreateMemoryDC(dc, m_canvas->m_fonts, &rc);
	}
	
	if(!m_memBmp)
		return;

	if(m_size==WINDOW_SIZE_NORM)
	{
		for (CComPtr<IVertexIterator> itr = m_graph->GetUserVertexIterator(skipper); itr->IsValidItem(); itr->NextItem())
		{
			CComQIPtr<IVertexRenderer> irv=m_canvas->GetRenderer(itr->Get());
			ATLASSERT(irv!=0);

			irv->GetImagesBoundsBox(rect);
			vp.clear();
			vp.push_back(GraphTypes::PointF(rect.x, rect.y));
			vp.push_back(GraphTypes::PointF(rect.x + rect.Width, rect.y));
			m_transform.AllTransform(vp[0]); // into zoom world
			m_transform.AllTransform(vp[vp.size()-1]);
			drawpt.push_back(vp[0]);
			drawpt.push_back(vp[vp.size()-1]);

			vp.clear();
			vp.push_back(GraphTypes::PointF(rect.x + rect.Width, rect.y));
			vp.push_back(GraphTypes::PointF(rect.x + rect.Width, rect.y + rect.Height));
			m_transform.AllTransform(vp[0]); // into zoom world
			m_transform.AllTransform(vp[vp.size()-1]);
			drawpt.push_back(vp[0]);
			drawpt.push_back(vp[vp.size()-1]);

			vp.clear();
			vp.push_back(GraphTypes::PointF(rect.x + rect.Width, rect.y + rect.Height));
			vp.push_back(GraphTypes::PointF(rect.x, rect.y + rect.Height));
			m_transform.AllTransform(vp[0]); // into zoom world
			m_transform.AllTransform(vp[vp.size()-1]);
			drawpt.push_back(vp[0]);
			drawpt.push_back(vp[vp.size()-1]);

			vp.clear();
			vp.push_back(GraphTypes::PointF(rect.x, rect.y + rect.Height));
			vp.push_back(GraphTypes::PointF(rect.x, rect.y));
			m_transform.AllTransform(vp[0]); // into zoom world
			m_transform.AllTransform(vp[vp.size()-1]);
			drawpt.push_back(vp[0]);
			drawpt.push_back(vp[vp.size()-1]);
		}

		for (CComPtr<IEdgeIterator> itr = m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
		{
			CComQIPtr<IEdgeRenderer> ire=m_canvas->GetRenderer(itr->Get());
			ATLASSERT(ire!=0);

			vp.clear();

			if(ire->GetPoints(vp, GraphTypes::world)>1)
			{
				m_transform.AllTransform(vp[0]); // into zoom world
				m_transform.AllTransform(vp[vp.size()-1]);
				drawpt.push_back(vp[0]);
				drawpt.push_back(vp[vp.size()-1]);
			}
		}

		// clear background and draw border
		Color clrbkgnd;
		clrbkgnd.SetFromCOLORREF(GetSysColor(COLOR_APPWORKSPACE));

		PointF tl, br;

		tl.x=0;
		tl.y=0;
		br.x=static_cast<REAL>(m_rcWnd.Width()-1); // Agg draws one pixel more
		br.y=static_cast<REAL>(m_rcWnd.Height()-1);

		m_memBmp->Rect(tl, br, clrbkgnd, m_colorBorder);
	}

	if(!m_inWindow)
	{
		DrawCaption(*m_memBmp);
		DrawCloseButton(*m_memBmp);
		if(m_size==WINDOW_SIZE_NORM)
			DrawMinButton(*m_memBmp);
		else
			DrawRestoreButton(*m_memBmp);
	}

	if(m_size==WINDOW_SIZE_NORM)
	{
		CRect rcpage(
			static_cast<CPoint>(m_vppoints[0]),
			static_cast<CPoint>(m_vppoints[2]));

		rcpage.NormalizeRect();
		CRect client(0, m_rcCaption.bottom, m_rcWnd.Width(), m_rcWnd.Height());
		bool bintersect=rcpage.IntersectRect(rcpage, client) != 0;
		bool binpage=rcpage==client!=0; // true if entire client rect is inside page

		WTL::CBrush brush;
		HBRUSH oldbrush;

		// draw subgraphs

		brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		oldbrush=m_memBmp->SelectBrush(brush);

		WTL::CPen pen;
		pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWFRAME));
		HPEN oldpen=m_memBmp->SelectPen(pen);

		IRenderer* ir;
		RectF boxf;
		CRect boxi;
		IGraphItemVector children;
		for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
		{
			if (CComQIPtr<IGraphSubgraph> subgraph = itr->Get()) //Skip vertices.
			{
				ir=m_canvas->GetRenderer(subgraph);
				ATLASSERT(ir);
				if(ir!=0)
				{
					ir->GetBoundsBox(boxf);
					m_canvas->RAllTransform(boxf); // to world coordinates
					m_transform.AllTransform(boxf); // into zoom world

					if (boxf.Width < 10000 && boxf.Height < 10000)
					{
						boxi=static_cast<CRect>(boxf);
						// TODO - check if this edge-cut issue is really related to unavoidable rounding errors
						if(boxi.left<1)
							boxi.left=1;
						if(boxi.right>m_rcWnd.Width()-1)
							boxi.right=m_rcWnd.Width()-1;
						if(boxi.top<=m_rcCaption.Height())
							boxi.top=m_rcCaption.Height()+1;
						if(boxi.bottom>m_rcWnd.Height()-1)
							boxi.bottom=m_rcWnd.Height()-1;

						m_memBmp->Rectangle(&boxi);
					}
				}
			}
		}

		m_memBmp->SelectBrush(oldbrush);
		m_memBmp->SelectPen(oldpen);
		brush.DeleteObject();

		// edges

		if(drawpt.size()>1)
		{
			// Draw edges first in XOR mode, then clip the viewport and draw edges normally.
			// This makes edges visible outside the viewport, regardless of edges and background
			// colors. We do this only if page does not enclose entire client area.

			Color color=m_canvas->m_meta.GetMetaEdgeColorNorm(CCategory());
			WTL::CPen pen;
			pen.CreatePen(PS_SOLID, 1, color.ToCOLORREF());
			HPEN oldpen=m_memBmp->SelectPen(pen);

			CRgn rgn;

			if(!binpage)
			{
				int oldROP=m_memBmp->SetROP2(R2_XORPEN);
				for(unsigned i=0; i<drawpt.size(); i+=2)
				{
					m_memBmp->MoveTo(drawpt[i]);
					m_memBmp->LineTo(drawpt[i+1]);
				}

				if(oldROP)
					m_memBmp->SetROP2(oldROP);

				CPoint points[5];
				points[0]=static_cast<CPoint>(m_vppoints[0]);
				points[1]=static_cast<CPoint>(m_vppoints[1]);
				points[2]=static_cast<CPoint>(m_vppoints[2]);
				points[3]=static_cast<CPoint>(m_vppoints[3]);
				points[4]=points[0];

				rgn.CreatePolygonRgn(points, 5, ALTERNATE);
				m_memBmp->SelectClipRgn(rgn);
			}


			for(unsigned i=0; i<drawpt.size(); i+=2)
			{
				m_memBmp->MoveTo(drawpt[i]);
				m_memBmp->LineTo(drawpt[i+1]);
			}

			if(!binpage)
				m_memBmp->SelectClipRgn(0);

			m_memBmp->SelectPen(oldpen);
		}

		// draw page

		if(bintersect)
		{
			rcpage.MoveToXY(rcpage.left - 1, rcpage.top - 1);
			m_memBmp->Rect(rcpage.TopLeft(), rcpage.BottomRight(), 0x7FFFFFFF, 0x7F000000);
			//brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			//oldbrush=m_memBmp->SelectBrush(brush);
			//HPEN oldpen=m_memBmp->SelectPen((HPEN)GetStockObject(NULL_PEN));
			//m_memBmp->Rectangle(&rcpage);
			//m_memBmp->SelectBrush(oldbrush);
			//m_memBmp->SelectPen(oldpen);
			//brush.DeleteObject();
		}

		if(!m_inWindow)
			DrawGripper(*m_memBmp);
	}

	BOOL bOk=FALSE;

	if(m_inWindow) // window version handles alpha via layered API
	{
		bOk=dc.BitBlt(
			m_rcWnd.left,
			m_rcWnd.top,
			m_rcWnd.Width(),
			m_rcWnd.Height(),
			*m_memBmp,
			0,
			0,
			SRCCOPY);
	}
	else
	{
		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = m_canvas->m_meta.GetOverviewOpacity();
		bf.AlphaFormat = 0; //AC_SRC_ALPHA;
		bOk=dc.DrawAlphaBlend(
			m_rcWnd.left,
			m_rcWnd.top,
			m_rcWnd.Width(),
			m_rcWnd.Height(),
			*m_memBmp,
			0,      
			0,      
			m_rcWnd.Width(),
			m_rcWnd.Height(),
			bf);
	}

	ATLASSERT(bOk);
}

//-----------------------------------------------------------------------------
//
//	Message Handlers

LRESULT COverviewRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM /*lParam*/,
	BOOL& /*bHandled*/)
{
	CAggDC& dc=*(CAggDC*)wParam;
	CDCHandle dch(dc.m_hDC);
	CalcDisplaySizes(dch, false, true); 
	Draw(dc);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT COverviewRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	ScreenToClient(pt);
	HIT_TEST_CODE hit=HitTestCaption(pt);
	bool bRepaint=false;
	CButtonState* state=0;
	BUTTON_STATE oldstate;
	LPTSTR cursortype=IDC_ARROW;

	SetFocus();

	if(hit!=HT_UNKNOWN) // in caption
	{
		m_capturedPart=hit;
		m_canvas->SetCapture(this);

		if(hit==HT_MINBUTTON)
			state=&m_minButtonState;
		else if(hit==HT_CLOSEBUTTON)
			state=&m_closeButtonState;
		else if(hit==HT_CAPTION)
		{
			m_capturePos=pt;

			m_savePos.x=m_position.x;
			m_savePos.y=m_position.y;
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
		// try the gripper, then the page
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
			else
			{
				hit=HitTestPage(pt);
				if(hit==HT_CONTAINER)
				{
					m_capturedPart=hit;
					m_canvas->SetCapture(this);
					cursortype=MAKEINTRESOURCE(IDC_HAND_CLOSED);
					m_capturePos=pt;
					m_savePos=m_canvas->GetOffset();
				}
				else
				{
					CenterViewport(pt);
				}
			}
		}
	}

	if(bRepaint)
		Invalidate(&m_rcWnd);

	if(cursortype==MAKEINTRESOURCE(IDC_HAND_CLOSED))
		SetCursor(
			LoadCursor(
					ATL::_AtlBaseModule.GetResourceInstance(),cursortype));
	else
		SetCursor(LoadCursor(NULL, cursortype));

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT COverviewRenderer::OnLButtonUp(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	ScreenToClient(pt);
	HIT_TEST_CODE hit=HitTestCaption(pt);
	bool bRepaint=false;
	CButtonState* capstate=0, *hitstate=0;
	LPTSTR cursortype=IDC_ARROW;

	if(hit==HT_MINBUTTON)
		hitstate=&m_minButtonState;
	else if(hit==HT_CLOSEBUTTON)
		hitstate=&m_closeButtonState;

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
		else if(m_capturedPart==HT_CONTAINER)
		{
			cursortype=MAKEINTRESOURCE(IDC_HAND_OPEN);
		}
		else
		{
			if(m_capturedPart==HT_MINBUTTON)
				capstate=&m_minButtonState;
			else if(m_capturedPart==HT_CLOSEBUTTON)
				capstate=&m_closeButtonState;

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

				if(hit==HT_MINBUTTON)
				{
					if(m_size==WINDOW_SIZE_MIN)
						SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
					else
						SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				}
				else if(hit==HT_CLOSEBUTTON)
				{
					SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
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
		if(hit!=HT_UNKNOWN) // in caption
		{
			if(hitstate)
			{
				hitstate->SetHot(true);
			}
		}
		else // check children
		{
			// try the gripper
			if(m_size==WINDOW_SIZE_NORM)
			{
				hit=HitTestGripper(pt);

				if(hit!=HT_UNKNOWN)
					cursortype=IDC_SIZENWSE;
				else
				{
					hit=HitTestPage(pt);
					if(hit==HT_CONTAINER)
						cursortype=MAKEINTRESOURCE(IDC_HAND_OPEN);
				}
			}
		}
	}

	if(bRepaint)
		Invalidate(&m_rcWnd);

	if(cursortype==MAKEINTRESOURCE(IDC_HAND_OPEN))
		SetCursor(
			LoadCursor(
					ATL::_AtlBaseModule.GetResourceInstance(),cursortype));
	else
		SetCursor(LoadCursor(NULL, cursortype));

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT COverviewRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	ScreenToClient(pt);
	HIT_TEST_CODE hit=HitTestCaption(pt);
	bool bRepaint=false;
	CButtonState* capstate=0, *hitstate=0;
	LPTSTR cursortype=IDC_ARROW;

	// only buttons have state
	if(hit==HT_MINBUTTON)
		hitstate=&m_minButtonState;
	else if(hit==HT_CLOSEBUTTON)
		hitstate=&m_closeButtonState;

	if(m_canvas->GetCapture()==this)
	{
		PointF ptf(pt);
		CRect newrc(m_rcWnd);

		if(m_capturedPart==HT_SIZEGRIP)
		{
			cursortype=IDC_SIZENWSE;
			CSize diff(pt.x-m_capturePos.x,pt.y-m_capturePos.y);  

			// prevent resizing when cursor is on the other side
			// of minimum possible size
			if(pt.x<m_minWndSize.cx)
			{
				newrc.right=m_rcWnd.left+m_minWndSize.cx;
				diff.cx=0;
			}

			if(pt.y<m_minWndSize.cy)
			{
				newrc.bottom=m_rcWnd.top+m_minWndSize.cy;
				diff.cy=0;
			}

			newrc.InflateRect(0, 0, diff.cx, diff.cy);

#ifdef _DEBUG
//			AtlTrace("Size: width %d, height %d\r\n", newrc.Width(), newrc.Height());
#endif

			if(newrc.Width()<=m_minWndSize.cx)
				newrc.right=newrc.left+m_minWndSize.cx;
			if(newrc.Height()<=m_minWndSize.cy)
				newrc.bottom=newrc.top+m_minWndSize.cy;

			if(!newrc.EqualRect(m_rcWnd))
				SetWindowPos(newrc, SWP_NOMOVE);

			m_capturePos=pt;
		}
		else if(m_capturedPart==HT_CAPTION)
		{
			pt.x+=static_cast<int>(m_position.x)-m_capturePos.x;
			pt.y+=static_cast<int>(m_position.y)-m_capturePos.y;
			newrc.TopLeft()=pt;
			newrc.right=newrc.left+m_rcWnd.Width();
			newrc.bottom=newrc.top+m_rcWnd.Height();
			SetWindowPos(newrc, SWP_NOSIZE);
		}
		else if(m_capturedPart==HT_CONTAINER)
		{
			cursortype=MAKEINTRESOURCE(IDC_HAND_CLOSED);

			PointF offset(ptf.x-m_capturePos.x,ptf.y-m_capturePos.y);
			m_transform.RSizeTransform(offset);
			m_canvas->SizeTransform(offset);

			offset.x+=m_savePos.x;
			offset.y+=m_savePos.y;
			m_canvas->SetOffset(offset);
			// everything needs to be invalidated here
			Invalidate();
		}
		else
		{
			if(m_capturedPart==HT_MINBUTTON)
				capstate=&m_minButtonState;
			else if(m_capturedPart==HT_CLOSEBUTTON)
				capstate=&m_closeButtonState;

			BUTTON_STATE oldcap=capstate->m_state;

			if(m_capturedPart!=hit)
				capstate->m_state=BS_NORMAL;
			else
				capstate->m_state=BS_PUSHED;

			if(oldcap!=capstate->m_state)
				bRepaint=true;
		}
	}
	else // no capture, check which cursor to set
	{
		if(hit!=HT_UNKNOWN) // in caption
		{
			if(hitstate)
			{
				hitstate->SetHot(true);
				bRepaint=true;
			}
			else
				m_canvas->ClearCurrentHot();
		}
		else // check children
		{
			m_canvas->ClearCurrentHot();

			if(m_size==WINDOW_SIZE_NORM)
			{
				// try the gripper

				hit=HitTestGripper(pt);

				if(hit!=HT_UNKNOWN)
					cursortype=IDC_SIZENWSE;
				else if(HitTestPage(pt)==HT_CONTAINER)
					cursortype=MAKEINTRESOURCE(IDC_HAND_OPEN);
			}
		}
	}

	if(bRepaint)
		Invalidate(&m_rcWnd);

	if(cursortype==MAKEINTRESOURCE(IDC_HAND_CLOSED) || cursortype==MAKEINTRESOURCE(IDC_HAND_OPEN))
		SetCursor(LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(),cursortype));
	else
		SetCursor(LoadCursor(NULL, cursortype));

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT COverviewRenderer::OnSysCommand(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM /*lParam*/, 
	BOOL& /*bHandled*/)
{
	CRect rc(m_rcWnd);

	if(wParam==SC_MINIMIZE)
	{
		m_size=WINDOW_SIZE_MIN;

		m_restoreSize.Width=m_wndSize.Width;
		m_restoreSize.Height=m_wndSize.Height;

		int hcaption=m_titleHeight+m_border*2;
		rc.bottom=rc.top+hcaption+1;
		SetWindowPos(rc, SWP_NOMOVE);
	}
	else if(wParam==SC_RESTORE)
	{
		m_size=WINDOW_SIZE_NORM;

		rc.left=static_cast<int>(m_position.x);
		rc.top=static_cast<int>(m_position.y);
		rc.right=rc.left+static_cast<int>(m_restoreSize.Width);
		rc.bottom=rc.top+static_cast<int>(m_restoreSize.Height);
		SetWindowPos(rc, SWP_NOMOVE);
	}
	else if(wParam==SC_CLOSE)
	{
		m_canvas->m_meta.SetOverviewVisible(false);
		Invalidate(&m_rcWnd);
	}

	return 0;
}

//*****************************************************************************

IOverviewRenderer *CreateOverviewRenderer(CCanvas& canvas, IGraph* graph)
{
	COverviewRenderer * retVal = new COverviewRenderer(canvas, graph);
	return retVal;
}

