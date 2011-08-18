#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "ScaledList.h"
#include "AggDC.h"
#include "AggHitTest.h"
#ifdef _DEBUG
#include "ThreadTest.h"
#endif

class CDatasetRenderer : public IDatasetRenderer, public CScaledWindow, public boost::signals::trackable
{
	IGraphSubgraph* m_g;
	boost::signals::connection m_sigConn;
#ifdef _DEBUG
	ThreadTest tt;
#endif

	CPartTheme& m_wndTheme;
	CPartTheme& m_statusTheme;

	CScaledList m_list;
	CButtonState m_maxButtonState;
	CButtonState m_minButtonState;

	CRect m_rcCaption;
	CRect m_rcButMax;
	CRect m_rcButMin;
	CRect m_rcGripper;
	CRect m_rcOutline;

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

	// conn image dimensions in terms of list item text height
	GraphTypes::SizeF m_connFactor; 

	WINDOW_SIZE m_size;
	HIT_TEST_CODE m_capturedPart;
	GraphTypes::REAL m_scale;

	// sizes that vary with each DPI and renderer "window"
	CRectMap m_wnd_sizes; // current size and position 
	CSizeMap m_norm_sizes;
	CSizeMap m_max_sizes;
	CSizeMap m_max_sizes_list;
	GraphTypes::SizeF m_norm_size; // normal size in world coordinates

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
	CPoint m_topleftoffset;
	CPoint m_capturePos;

	int CalcCaptionSize(CDCHandle& dc, const TCHAR* title);
	int CalcCaptionTotalHeight(void);
	void DrawWindow(CAggDC& dc); 
	void CalcWindowBorder(BORDER_STYLE style);
	void DrawCaption(CAggDC& dc, CAPTION_STYLE style);
	void DrawWindowBkgnd(CAggDC& dc); 
	void CalcGripperRect(CRect& rc);
	void DrawGripper(CAggDC& dc);
	void CalcMaxButtonRect(CRect& rc);
	void DrawMaxButton(CAggDC& dc);
	void CalcMinButtonRect(CRect& rc);
	void DrawMinButton(CAggDC& dc);
	void CalcListRect(CRect& rc);
	void DrawList(CAggDC& dc);
	void DrawEdges(CAggDC& dc);
	void DrawConnectors(CAggDC& dc);
	GraphTypes::SizeF GetConnectorSize();

	HIT_TEST_CODE HitTestCaption(const CPoint& p);
	HIT_TEST_CODE HitTestMaxButton(const CPoint& p);
	HIT_TEST_CODE HitTestMinButton(const CPoint& p);
	HIT_TEST_CODE HitTestGripper(const CPoint& p);
	IGraphVertex* HitTestConnectors(const GraphTypes::PointF& ptest, double tolerance=0);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IDatasetRenderer)
		IMPLEMENT_INTERFACE(IListWindowRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CDatasetRenderer(CCanvas & canvas);
	~CDatasetRenderer();

	void SetWindowPos(const CRect& pos, UINT flags);
	virtual void Invalidate(CRect* rc=0, bool bErase=false) 
	{ 
		m_canvas->Invalidate(rc,bErase);
	}

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
	BEGIN_MSG_MAP(CDatasetRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(LBN_LABELEDIT, OnLabelEdit)
		MESSAGE_HANDLER(SWM_CHILDRENCHANGE, OnChildrenChange)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLabelEdit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnChildrenChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};

//-----------------------------------------------------------------------------

// height and default width (sample title) for each scale
CSizeMap CDatasetRenderer::m_captionSizes;

// Minimum window height for each scale, *INCLUDING* caption
CSizeMap CDatasetRenderer::m_min_sizes;

// these statics are either independent of scale factor
// or are scaled dynamically (may consider storing those in
// vector, for each scale factor)

CSize CDatasetRenderer::m_wndBorder;
MARGINS CDatasetRenderer::m_captionTextMargin;
MARGINS CDatasetRenderer::m_captionContentMargin;
MARGINS CDatasetRenderer::m_listMargin;
CSize CDatasetRenderer::m_sizeGripper;

BORDER_STYLE CDatasetRenderer::m_wndBorderStyle=DIALOG_FRAME;
// gap between caption text and first button
int CDatasetRenderer::m_captionButtonTextGap=4;
// gap between caption buttons
int CDatasetRenderer::m_captionButtonGap=1;

using namespace GraphTypes;

//*****************************************************************************

CDatasetRenderer::CDatasetRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_g(0),
	m_wndTheme(canvas.GetPartTheme(_T("WINDOW"))),
	m_statusTheme(canvas.GetPartTheme(_T("STATUS"))),
	m_list(&canvas),
	m_size(WINDOW_SIZE_NORM),
	m_capturedPart(HT_UNKNOWN),
	m_maxButtonState(&canvas, this),
	m_minButtonState(&canvas, this),
	m_scale(m_canvas->GetScale())
{
#ifdef _DEBUG
	tt.TTInit();
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

	m_connFactor.Height=REAL(0.4);
	m_connFactor.Width=REAL(0.6);
}

//-----------------------------------------------------------------------------

CDatasetRenderer::~CDatasetRenderer()
{
}

//-----------------------------------------------------------------------------

CUniqueID CDatasetRenderer::GetID()
{
	return m_g->GetID();
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetGraphItem(IGraphItem* item)
{
	if(m_g)
	{
		m_g->Disconnect(m_sigConn);
	}
	m_g=static_cast<IGraphSubgraph*>(item);
	if(m_g)
	{
		m_sigConn = m_g->Connect(boost::ref(*this));
		this->operator()(GRAPHNOTIFICATION_VERTEXADDED, m_g);
	}
	m_list.SetGraphItem(item);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	if(m_size==WINDOW_SIZE_MIN)
		box=m_rcWnd;
	else
		box.Union(box, m_rcWnd, m_rcOutline);
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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

void CDatasetRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	m_position=pt;
	if(co==screen)
	{
		m_canvas->RAllTransform(m_position);		
	}

	// N.B. we may call SetWindowPos here but it is left to the CalcDisplaySizes
	// preceeding the drawing stage
}

//-----------------------------------------------------------------------------

LRESULT CDatasetRenderer::SendMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetFocus(bool bSet)
{
	// parent will never have focus and passes it to children
	m_list.SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::GetContainerRect(GraphTypes::RectF& rc) const
{
	rc=m_rcWnd;
}

//-----------------------------------------------------------------------------

IGraphItem* CDatasetRenderer::HitTestChildren(const PointF& point)
{
	IGraphItem* item=m_list.HitTestItems(point);
	if(item==0)
		item=HitTestConnectors(point);
	return item;
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::GetConnectBounds(
	RectF& left, 
	RectF& right, 
	unsigned order)
{
	RectF rcL, rcR;
	SizeF connsize=GetConnectorSize();

	if(m_size!=WINDOW_SIZE_MIN)
	{
		RectF rcitem;
		CScaledList::eLocation loc=m_list.GetConnectBounds(rcitem, order);
		if(loc==CScaledList::top)
		{
			rcL.y=rcR.y=m_rcWnd.top+((REAL)CalcCaptionTotalHeight()-connsize.Height)/2;
		}
		else if(loc==CScaledList::bottom)
		{
			rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;
		}
		else
		{
			rcL.y=rcR.y=rcitem.y+(rcitem.Height-connsize.Height)/2;
			if(rcL.y>m_rcWnd.bottom)
				rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;
		}

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
	rcR.Width=rcL.Width=connsize.Height;	

	ITextboxRenderer::eIOMode mode=ITextboxRenderer::iomBoth;
	IGraphItem* item=m_g->GetChild(order);
	ATLASSERT(item);
	CComQIPtr<ITextboxRenderer> ir=m_canvas->GetRenderer(item);

	if(ir!=0)
		mode=ir->GetIOMode();

	if(mode==ITextboxRenderer::iomOut) // make both right
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
		left=rcL;
		right=rcR;
	}
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	CScaledWindow::SetSelected(GetID(), bSelected, bAdd);
}

//-----------------------------------------------------------------------------

bool CDatasetRenderer::IsSelected()
{
	return m_bSelected;
}

//-----------------------------------------------------------------------------

bool CDatasetRenderer::IsMinimized() const
{
	return m_size==WINDOW_SIZE_MIN;
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::OnRemoveGraphItem(IGraphItem* /*gi*/)
{
	// clear cached sizes to cause recalc 
	ClearCaches();
}

//-----------------------------------------------------------------------------

bool CDatasetRenderer::OnEditItem(IGraphItem* gi)
{
	bool bOk=m_list.OnEditItem(gi);

	// don't allow editing compressed vertices
	if(bOk && m_canvas->m_meta.GetRowCompress())
	{
		CComQIPtr<IGraphVertex> v=gi;
		if(v!=0 && v->GetEdgeCount()>0)
			bOk=false;
	}

	return bOk;
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetWindowState(WINDOW_SIZE size/*=WINDOW_SIZE_NORM*/,
	int topindex/*=0*/)
{
	m_list.SetTopIndex(topindex);

	if(size==WINDOW_SIZE_MAX)
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	else if(size==WINDOW_SIZE_MIN)
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::GetWindowState(WINDOW_SIZE& size, int& topindex) const
{
	size=m_size;
	topindex=m_list.GetTopIndex();
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetWindowSize(const GraphTypes::SizeF& size)
{
	CSize isize=static_cast<CSize>(size);

	if(m_size==WINDOW_SIZE_NORM)
	{
		CRectEx wndrc=m_wnd_sizes[m_canvas->GetDpiScale()];
		wndrc.right=wndrc.left+isize.cx;
		wndrc.bottom=wndrc.top+isize.cy;
		// SetWindowPos will set m_wnd_sizes
		SetWindowPos(wndrc, SWP_NOREDRAW);
	}

	// normal size in world coordinates
	SizeF normf(size);
	m_canvas->RSizeTransform(normf);
	m_norm_size=normf;
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::GetWindowSize(GraphTypes::SizeF& size) const
{
	CSizeMap::const_iterator itr=m_norm_sizes.find(m_canvas->GetDpiScale());
	ATLASSERT(itr!=m_norm_sizes.end());
	CSize isize(itr->second);
	size.Width=static_cast<REAL>(isize.cx);
	size.Height=static_cast<REAL>(isize.cy);
}

//-----------------------------------------------------------------------------

SizeF CDatasetRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool /*bIgnoreExtent*/)
{
	SizeF size;
	bool bSizeChange=false;
	bool bPositionChange=false;

	REAL scale=m_canvas->GetScale();
	if(scale!=m_scale)
	{
		m_scale=scale;
		bForce=true;
	}

	CRect wndrc=m_wnd_sizes[m_canvas->GetDpiScale()];

	if(bForce || wndrc.Width()==0 || wndrc.Height()==0) 
	{
		int wcaption, hcaption, wdefault;
		CSize sizelistmax;
		CSize sizewndmax;
		CSize sizewndmin;
		CSize sizewndnorm;
		CSize sizecaption;

		bSizeChange=true;
		wcaption=CalcCaptionSize(dc, m_g->GetLabel());
		sizecaption=m_captionSizes[m_canvas->GetDpiScale()];

		wcaption+=
			m_captionTextMargin.cxLeftWidth+
			m_captionTextMargin.cxRightWidth+
			m_captionContentMargin.cxLeftWidth+
			m_captionContentMargin.cxRightWidth;

		sizelistmax=m_list.CalcListMaxSize(dc);
		m_max_sizes_list[m_canvas->GetDpiScale()]=sizelistmax;

		// default window width 
		wdefault=sizelistmax.cx;
		wndrc.right=wndrc.left+wdefault;

		// border is included in total caption height
		CalcWindowBorder(m_wndBorderStyle);

		hcaption=CalcCaptionTotalHeight();
		int rowheight=m_list.GetRowHeight();

		// default height is maximum list height unless there are more than 10 rows

		IGraphItemVector results;
		unsigned count=m_g->GetChildren(results);

		int hdefault;
		if(count>10)
			hdefault=rowheight*10;
		else
			hdefault=sizelistmax.cy;

		// minimum width for now is the same as example caption width
		sizewndmin.cx=sizecaption.cx;
		sizewndmin.cy=hcaption+count>0?rowheight:0;

		sizewndmax.cx=__max(sizelistmax.cx, wcaption);
		sizewndmax.cy=hcaption+sizelistmax.cy;

		wndrc.bottom=wndrc.top+hcaption+hdefault; // initial height
		wndrc.InflateRect(m_wndBorder.cx, 0);
		wndrc.bottom+=m_wndBorder.cy; // top border included in caption total height

		sizewndmax.cx+=m_wndBorder.cx*2;
		sizewndmax.cy+=m_wndBorder.cy;
		sizewndmin.cy+=m_wndBorder.cy;

		// add caption buttons to window width

		// just for finding left side of min button
		m_rcWnd.left=m_rcWnd.top=0;
		m_rcWnd.right=m_rcWnd.bottom=200;

		CalcMaxButtonRect(m_rcButMax);
		CalcMinButtonRect(m_rcButMin);
	
		int add=m_rcWnd.right-m_rcButMin.left-
			m_wndBorder.cx-
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

		if(m_norm_size.Width==0 || m_norm_size.Height==0)
		{
			sizewndnorm.cx=wndrc.Width() > sizewndmax.cx ? wndrc.Width() : sizewndmax.cx;
			sizewndnorm.cy=wndrc.Height();
			m_norm_sizes[m_canvas->GetDpiScale()]=sizewndnorm;

			// normal size in world coordinates
			SizeF norm(sizewndnorm);
			m_canvas->RSizeTransform(norm);
			m_norm_size=norm;
		}
		else
		{
			SizeF norm=m_norm_size;
			m_canvas->SizeTransform(norm);
			sizewndnorm=static_cast<CSize>(norm);
			// normal size can not be larger than max size
			if(sizewndnorm.cx>sizewndmax.cx)
				sizewndnorm.cx=sizewndmax.cx;
			if(sizewndnorm.cy>sizewndmax.cy)
				sizewndnorm.cy=sizewndmax.cy;
	
			m_norm_sizes[m_canvas->GetDpiScale()]=sizewndnorm;
		}

		if(m_size==WINDOW_SIZE_MAX)
		{
			wndrc.right=wndrc.left+sizewndmax.cx;
			wndrc.bottom=wndrc.top+sizewndmax.cy;
		}
		else if(m_size==WINDOW_SIZE_MIN)
		{
			// let default for minimized size
			wndrc.bottom=wndrc.top+sizewndmin.cy;
		}
		else
		{
			wndrc.right=wndrc.left+sizewndnorm.cx;
			wndrc.bottom=wndrc.top+sizewndnorm.cy;
		}

#ifdef _DEBUG
		//AtlTrace("Window width:%d, height:%d\r\n",int(wndrc.Width),int(wndrc.Height)); 
#endif
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

void CDatasetRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::ClearCaches()
{
	m_wnd_sizes.clear();
	m_norm_sizes.clear();
	m_max_sizes.clear();
	m_max_sizes_list.clear();
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::SetWindowPos(const CRect& pos, UINT flags)
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

		m_rcButMax.OffsetRect(offset);
		m_rcButMin.OffsetRect(offset);
		m_rcGripper.OffsetRect(offset);
		m_rcOutline.OffsetRect(offset);

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

			CalcGripperRect(m_rcGripper);

			GraphTypes::CRectEx rclist;
			CalcListRect(rclist);
			// when minimized window is restored list needs to move
			m_list.SetWindowPos(rclist, flags&(~SWP_NOMOVE));
		}

		CalcMaxButtonRect(m_rcButMax);
		CalcMinButtonRect(m_rcButMin);

		SizeF connsize=GetConnectorSize();
		m_rcOutline=m_rcWnd;
		m_rcOutline.top=m_rcClient.top;
		m_rcOutline.InflateRect(round_int(connsize.Width), 0, round_int(connsize.Width), 0);

		if(m_size==WINDOW_SIZE_NORM && (flags&SWP_NOSIZE)==0)
		{
			CSize normwnd(m_rcWnd.Width(), m_rcWnd.Height());
			m_norm_sizes[m_canvas->GetDpiScale()]=normwnd;
		}
	}

	CRectEx rcex(m_rcWnd);
	m_wnd_sizes[m_canvas->GetDpiScale()]=rcex;

	if(bRedraw)
		Invalidate();
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::operator()(
	GRAPHNOTIFICATION notification, IGraphItem * /*item*/)
{
	switch(notification)
	{
#ifdef _DEBUG
		tt.TTest();
#endif

	case GRAPHNOTIFICATION_VERTEXADDED:
	case GRAPHNOTIFICATION_VERTEXREMOVED:
		{
			// N.B. this is supposed to be guaranteed to execute on GUI thread
			ClearCaches();
			//m_canvas->PostMsg(this, SWM_CHILDRENCHANGE);
		}
		break;

	case GRAPHNOTIFICATION_LABELCHANGED:
		break;
	}
}

//-----------------------------------------------------------------------------

int CDatasetRenderer::CalcCaptionSize(CDCHandle& dc, const TCHAR* title)
{
	std::_tstring txt=_T("Example Title"); // for minimum caption width 

	CSize extent(0,0);
	CAutoFont f(m_canvas->GetCaptionFont(m_canvas->GetFontPixHeight()),dc);

	// remember height and average width for this scale
	dc.GetTextExtent(txt.c_str(), txt.length(), &extent);
	//m_captionSizes[m_canvas->GetDpiScale()]=extent;

	// return title text extent at this scale (used for windows max width)
	dc.GetTextExtent(title, _tcslen(title), &extent);
	m_captionSizes[m_canvas->GetDpiScale()]=extent;

	return extent.cx;
}

//-----------------------------------------------------------------------------
// 
// Currently we do not scale caption text margins and border. If we do, 
// consider pre-calculating everything and caching it.
//

int CDatasetRenderer::CalcCaptionTotalHeight(void)
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
		m_wndBorder.cy*2;

	return int(height);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawWindow(CAggDC& dc)
{
	DrawConnectors(dc);

	DrawEdges(dc);

	if(m_size!=WINDOW_SIZE_MIN)
		DrawWindowBkgnd(dc);

	DrawCaption(dc, CS_SMALL);
	DrawMaxButton(dc);

	if(m_size!=WINDOW_SIZE_MIN)
	{
		DrawMinButton(dc);
		DrawList(dc);
		if(m_size==WINDOW_SIZE_NORM)
		{
			CRect rc;
			m_list.GetClientRect(rc);
			if(!rc.IsRectEmpty())
				DrawGripper(dc);
		}
	}
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::CalcWindowBorder(BORDER_STYLE /*style*/)
{
	m_wndBorder.cx=1;
	m_wndBorder.cy=1;
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawCaption(
	CAggDC& dc, 
	CAPTION_STYLE /*style*/)
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

	if(m_size!=WINDOW_SIZE_MIN)
	{
		int gap=m_captionButtonGap;
		rc.left+=
			m_captionContentMargin.cxLeftWidth+
			m_captionTextMargin.cxLeftWidth; 

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

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(clrtext.ToCOLORREF());
	dc.DrawText(str.c_str(), str.length(), rc, flags);
	dc.RestoreDC(idc);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawWindowBkgnd(
	CAggDC& dc) 
{
	// m_wndBorderStyle unused for now
	PointF wndoutline[4];

	// Agg draws outline outside given rectangle
	wndoutline[0].x=static_cast<REAL>(m_rcWnd.left+1);
	wndoutline[0].y=static_cast<REAL>(m_rcWnd.top+1);
	wndoutline[1].x=static_cast<REAL>(m_rcWnd.right-1);
	wndoutline[1].y=static_cast<REAL>(m_rcWnd.top+1);
	wndoutline[2].x=static_cast<REAL>(m_rcWnd.right-1);
	wndoutline[2].y=static_cast<REAL>(m_rcWnd.bottom-1);
	wndoutline[3].x=static_cast<REAL>(m_rcWnd.left+1);
	wndoutline[3].y=static_cast<REAL>(m_rcWnd.bottom-1);

	dc.SolidPolygon(
		wndoutline, 
		m_colorBkgnd,
		sizeof(wndoutline)/sizeof(wndoutline[0]),
		2,
		m_colorBorder);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::CalcGripperRect(CRect& rc)
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
	rcscale.OffsetRect(m_rcClient.right-rcscale.Width(),
		m_rcClient.bottom-rcscale.Height());

	rc=rcscale;
}	

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawGripper(CAggDC& dc)
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

void CDatasetRenderer::CalcMaxButtonRect(CRect& rc)
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

void CDatasetRenderer::DrawMaxButton(CAggDC& dc)
{
	CRect rc(m_rcButMax);

	Color color=IsSelected() ? m_colorActiveTitleBar : m_colorTitleBar;
	Color colorBorder=IsSelected() ? m_colorActiveTitle : m_colorInactiveTitle;

	switch(m_maxButtonState.m_state)
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

	if(m_size==WINDOW_SIZE_NORM)
	{
		rc.DeflateRect(2,2);
	}
	else
	{
		rc.DeflateRect(4,4);
		rc.OffsetRect(2,2);
	}

	tl.x=static_cast<REAL>(rc.left);
	tl.y=static_cast<REAL>(rc.top);
	br.x=static_cast<REAL>(rc.right);
	br.y=static_cast<REAL>(rc.bottom);

	dc.Rect(tl, br, color, colorBorder);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::CalcMinButtonRect(CRect& rc)
{
	CRect rcminbutt(m_rcButMax); // assumes max is always calculated first
	int gap=m_captionButtonGap;

	rcminbutt.OffsetRect(-(rcminbutt.Width()+gap),0);

	rc=rcminbutt;
}	

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawMinButton(CAggDC& dc)
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

void CDatasetRenderer::CalcListRect(CRect& rc)
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

void CDatasetRenderer::DrawList(CAggDC& dc)
{
	m_list.SendMessage(WM_PAINT, (WPARAM)(&dc), 0);
}

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawEdges(CAggDC& dc)
{
	CComQIPtr<IEdgeRenderer> ier;
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
			ATLASSERT(vertex);

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

//-----------------------------------------------------------------------------

void CDatasetRenderer::DrawConnectors(CAggDC& dc)
{
	IGraphItemVector results;

	if(m_g->GetChildren(results)==0)
		return;

	CComQIPtr<IGraphVertex> vertex;
	IGraphEdgeVector edges;
	RectF rcL, rcR;

	SizeF connsize=GetConnectorSize();
	bool bHasEdges;
	unsigned order=0;

	for(IGraphItemVector::iterator itr=results.begin();
		itr!=results.end();
		++itr, ++order)
	{
		vertex=*itr;			
		ATLASSERT(vertex);

		if(m_size!=WINDOW_SIZE_MIN)
		{
			RectF rcitem;
			CScaledList::eLocation loc=m_list.GetConnectBounds(rcitem, order);
			if(loc==CScaledList::top)
			{
				rcL.y=rcR.y=m_rcWnd.top+((REAL)CalcCaptionTotalHeight()-connsize.Height)/2;
			}
			else if(loc==CScaledList::bottom)
			{
				rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;
			}
			else
			{
				rcL.y=rcR.y=rcitem.y+(rcitem.Height-connsize.Height)/2;
				if(rcL.y>m_rcWnd.bottom)
					rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;
			}

			rcL.x=static_cast<REAL>(m_rcWnd.left-connsize.Width);
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

		ITextboxRenderer::eIOMode mode=ITextboxRenderer::iomBoth;
		CComQIPtr<ITextboxRenderer> ir=m_canvas->GetRenderer(vertex);

		if(ir!=0)
			mode=ir->GetIOMode();

		if(mode==ITextboxRenderer::iomOut) 
		{
			dc.LollyPop(rcR, cbkgnd, m_colorOutline, false);
		}
		else if(mode==ITextboxRenderer::iomIn)
		{
			dc.LollyPop(rcL, cbkgnd, m_colorOutline, true);
		}
		else
		{
			dc.LollyPop(rcR, cbkgnd, m_colorOutline, false);
			dc.LollyPop(rcL, cbkgnd, m_colorOutline, true);
		}
	}
}

//-----------------------------------------------------------------------------

SizeF CDatasetRenderer::GetConnectorSize()
{
	SizeF size;
	// conn is m_factor times height of text 
	size.Height=m_rcCaption.Height()*m_connFactor.Height;
	size.Width=m_rcCaption.Height()*m_connFactor.Width;
	return size;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CDatasetRenderer::HitTestCaption(const CPoint& p)
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

HIT_TEST_CODE CDatasetRenderer::HitTestMaxButton(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcButMax.PtInRect(p))
		code=HT_MAXBUTTON;

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CDatasetRenderer::HitTestMinButton(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcButMin.PtInRect(p))
		code=HT_MINBUTTON;

	return code;
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CDatasetRenderer::HitTestGripper(const CPoint& p)
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

//-----------------------------------------------------------------------------

IGraphVertex* CDatasetRenderer::HitTestConnectors(
	const PointF& ptest,
	double tolerance/*=0*/)
{
	IGraphItemVector results;

	if(m_g->GetChildren(results)==0)
		return 0;

	RectF vp;
	m_canvas->GetViewport(vp);
	CRect rc=static_cast<CRect>(vp);
	boost::scoped_ptr<CAggHitTest> agght(CAggHitTest::CreateAggHitTest());
	agght->SetRect(rc);

	CComQIPtr<IGraphVertex> vertex;
	RectF rcL, rcR;

	SizeF connsize=GetConnectorSize();
	unsigned order=0;

	for(IGraphItemVector::iterator itr=results.begin();
		itr!=results.end();
		++itr, ++order)
	{
		vertex=*itr;			
		ATLASSERT(vertex);

		if(m_size!=WINDOW_SIZE_MIN)
		{
			RectF rcitem;
			CScaledList::eLocation loc=m_list.GetConnectBounds(rcitem, order);
			if(loc==CScaledList::top)
			{
				rcL.y=rcR.y=m_rcWnd.top+((REAL)CalcCaptionTotalHeight()-connsize.Height)/2;
			}
			else if(loc==CScaledList::bottom)
			{
				rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;
			}
			else
			{
				rcL.y=rcR.y=rcitem.y+(rcitem.Height-connsize.Height)/2;
				if(rcL.y>m_rcWnd.bottom)
					rcL.y=rcR.y=m_rcWnd.bottom-connsize.Height;
			}

			rcL.x=static_cast<REAL>(m_rcWnd.left-connsize.Width);
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
	
		ITextboxRenderer::eIOMode mode=ITextboxRenderer::iomBoth;
		CComQIPtr<ITextboxRenderer> ir=m_canvas->GetRenderer(vertex);

		if(ir!=0)
			mode=ir->GetIOMode();

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
		else
		{
			if(agght->LollyPopHitTest(rcR, false, ptest, tolerance))
				return vertex;
			if(agght->LollyPopHitTest(rcL, true, ptest, tolerance))
				return vertex;
		}
	}

	return 0;
}

//*****************************************************************************
//
// Message handlers
//

LRESULT CDatasetRenderer::OnPaint(
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

LRESULT CDatasetRenderer::OnLButtonDown(
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
	LPTSTR cursortype=IDC_ARROW;

	m_g->MoveToEnd();
	if(m_rcWnd.PtInRect(pt)) // could have been be a hit on connectors
		SetFocus();

	// no multiple selection for subgraphs for now
	if(!bselected)
	{
//TODO new selection call		oldSelection=m_canvas->GetSelected();
		CScaledWindow::SetSelected(GetID());
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

			m_topleftoffset.x=pt.x-m_rcWnd.left;
			m_topleftoffset.y=pt.y-m_rcWnd.top;
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

		if(hit==HT_UNKNOWN)
		{
			if(m_list.HitTest(pt))
			{
				lRes=m_list.SendMessage(uMsg, wParam, lParam);
			}
			else
			{
#ifdef __OLD_HIT_TESTING__
				CRect rcl;

				// tweak position in x direction so that we
				// get hit if user clicks on extruded connectors
				m_list.GetWindowRect(rcl);
				pt.x=rcl.right-2; 

				LPARAM ltweak=MAKELPARAM(pt.x, pt.y);
				lRes=m_list.SendMessage(WM_NCHITTEST, wParam, ltweak);
#else
				PointF ptf(REAL(pt.x), REAL(pt.y)); 
				lRes=reinterpret_cast<LPARAM>(HitTestConnectors(ptf));
#endif
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

	SetCursor(LoadCursor(NULL, cursortype));

	return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CDatasetRenderer::OnLButtonUp(
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

LRESULT CDatasetRenderer::OnMouseMove(
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
			CSize diff(pt.x-m_capturePos.x,pt.y-m_capturePos.y);  

			// prevent resizing when cursor is on the other side
			// of minimum possible size
			if(pt.x<(m_rcWnd.left+minwnd.cx))
			{
				newrc.right=m_rcWnd.left+minwnd.cx;
				diff.cx=0;
			}

			if(pt.y<(m_rcWnd.top+minwnd.cy))
			{
				newrc.bottom=m_rcWnd.top+minwnd.cy;
				diff.cy=0;
			}

			newrc.InflateRect(0, 0, diff.cx, diff.cy);

#ifdef _DEBUG
//			AtlTrace("Size: width %d, height %d\r\n", newrc.Width(), newrc.Height());
#endif

			CSize maxwnd=m_max_sizes[m_canvas->GetDpiScale()];

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
				SetWindowPos(newrc, SWP_NOMOVE);
				// normal size in world coordinates
				SizeF norm(static_cast<REAL>(newrc.Width()), static_cast<REAL>(newrc.Height()));
				m_canvas->RSizeTransform(norm);
				m_norm_size=norm;
			}

			m_capturePos=pt;
			if(m_canvas->m_meta.IsAvoidObstacles())
				m_canvas->NotifyRecalcEdgePoints();
		}
		else if(m_capturedPart==HT_CAPTION)
		{
			newrc.left=pt.x-m_topleftoffset.x;
			newrc.top=pt.y-m_topleftoffset.y;
			newrc.right=newrc.left+m_rcWnd.Width();
			newrc.bottom=newrc.top+m_rcWnd.Height();

			SetWindowPos(newrc, SWP_NOSIZE);
			if(m_canvas->m_meta.IsAvoidObstacles())
				m_canvas->NotifyRecalcEdgePoints();
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

LRESULT CDatasetRenderer::OnMouseDblclk(
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

LRESULT CDatasetRenderer::OnRButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=0;
	bool bRepaint=false;
	bool bselected=IsSelected();
	CUniqueID oldSelection;
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	PointF ptf(pt);

	if(m_rcWnd.PtInRect(pt)) // could have been be a hit on connectors
	{
		m_g->MoveToEnd();
		SetFocus();
		if(!bselected)
		{
// TODO new selection call			oldSelection=m_canvas->GetSelected();
			CScaledWindow::SetSelected(GetID());
		}

		CComPtr<IGraphItem> item=HitTestChildren(ptf);
		if(item!=0)
		{
			lRes=reinterpret_cast<LRESULT>(item.p);
			// make list selection and focus work
			m_list.SendMessage(WM_LBUTTONDOWN, wParam, lParam);
			bRepaint=true;
		}
		else
		{
			lRes=reinterpret_cast<LRESULT>(m_g);
		}

		bRepaint|=IsSelected()!=bselected;

		if(bRepaint)
			Invalidate();

		if(IsSelected()!=bselected)
		{
			m_canvas->NotifySelection(m_g->GetID(), oldSelection);
		}

	}

	return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CDatasetRenderer::OnSysCommand(
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

	if(m_canvas->m_meta.IsAvoidObstacles())
		m_canvas->NotifyRecalcEdgePoints();

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CDatasetRenderer::OnNcHitTest(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	LRESULT lRes=0;

	if(m_list.HitTest(pt))
		lRes=m_list.SendMessage(uMsg, wParam, lParam);
	else
	{
		PointF ptf(REAL(pt.x), REAL(pt.y)); 
		lRes=reinterpret_cast<LPARAM>(HitTestConnectors(ptf));
	}

	return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CDatasetRenderer::OnLabelEdit(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return m_list.SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

LRESULT CDatasetRenderer::OnChildrenChange(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ClearCaches();
	return 0;
}

//*****************************************************************************

IRenderer* CreateDatasetRenderer(CCanvas & canvas)
{
	CDatasetRenderer* retVal = new CDatasetRenderer(canvas);
	return retVal;
}

