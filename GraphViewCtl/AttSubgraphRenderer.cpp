#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "Notifications.h"
#include "AggDC.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "XGMMLSaxHandler.h"

class CAttSubgraphRenderer : public IAttSubgraphRenderer, public CScaledWindow
{
	IGraphSubgraph* m_g;
	CComQIPtr<IXGMMLRenderCallback> m_callback;

	CRect m_rcCaption;
	CAggBitmap m_titleTextBmp;
	WINDOW_SIZE	m_winSize;
	GraphTypes::RectF m_rcButMin;

	GraphTypes::SizeF m_captionSize; // size for current scale
	GraphTypes::REAL m_scale;

	static GraphTypes::REAL m_borderLineThickness;
	static GraphTypes::REAL m_borderMinThickness;
	static MARGINS m_captionTextMargin;

	// dimensions (in world coordinate system) imposed by the layout
	GraphTypes::SizeF m_imposedSize;
	// top left position in internal (world) coordinates
	GraphTypes::PointF m_position;

	int CalcCaptionSize(CDCHandle& dc, const TCHAR* title);

	void DrawWindow(CAggDC& dc, const GraphTypes::Color & colourBackground, const GraphTypes::Color & colourBorder); 
	void DrawCaption(CAggDC& dc, const GraphTypes::Color & colourBackground, const GraphTypes::Color & colourBorder);
	void DrawWindowBkgnd(CAggDC& dc, const GraphTypes::Color & colourBackground, const GraphTypes::Color & colourBorder); 

	void CalcMinButtonRect(GraphTypes::RectF& rc);
	void DrawMinButton(CAggDC& dc);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IAttSubgraphRenderer)
		IMPLEMENT_INTERFACE(IListWindowRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CAttSubgraphRenderer(CCanvas & canvas);

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

	// IAttSubgraphRenderer
	LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	BEGIN_MSG_MAP(CAttSubgraphRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
//		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)

	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

GraphTypes::REAL CAttSubgraphRenderer::m_borderLineThickness(2.0);
GraphTypes::REAL CAttSubgraphRenderer::m_borderMinThickness(0.5);
MARGINS CAttSubgraphRenderer::m_captionTextMargin={int(m_borderLineThickness),int(m_borderLineThickness),int(m_borderLineThickness),int(m_borderLineThickness)};

using namespace GraphTypes;

//*****************************************************************************

CAttSubgraphRenderer::CAttSubgraphRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas), 
	m_rcCaption(0, 0, 0, 0),
	m_scale(0.0),
	m_winSize(WINDOW_SIZE_NORM)
{

}

//-----------------------------------------------------------------------------

CUniqueID CAttSubgraphRenderer::GetID()
{
	return m_g->GetID();
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetGraphItem(IGraphItem* item)
{
	m_g=static_cast<IGraphSubgraph*>(item);
	m_callback = m_g->GetPropertyUnknown(PROP_CALLBACK);
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	box=m_rcWnd;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetBoundsBox(
	const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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

void CAttSubgraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	m_position=pt;
	if(co==screen)
	{
		m_canvas->RAllTransform(m_position);		
	}
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::SendMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetFocus(bool bSet)
{
	// parent will never have focus but the other guy must lose it 
	CScaledWindow::SetFocus(bSet);
//	if(bSet)
//		m_canvas->ClearFocus();
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::GetContainerRect(GraphTypes::RectF& rc) const
{
	rc=m_rcWnd;
}

//-----------------------------------------------------------------------------

IGraphItem* CAttSubgraphRenderer::HitTestChildren(const PointF& point)
{
	CComPtr<IGraphItem> gi;
	RectF rc;
	CComPtr<IRenderer> ir;
	CComPtr<IUnknown> unk;

	if (!IsMinimized())
	{
		for(CComPtr<IGraphItemIterator> itr = m_g->GetChildren(); itr->IsValidItem(); itr->NextItem())
		{
			ir=m_canvas->GetRenderer(itr->Get());
			if(ir!=0)
			{
				ir->GetBoundsBox(rc);

				if(rc.Contains(point))
				{
					gi=itr->Get();
					if (CComQIPtr<IGraphSubgraph> childSubgraph = gi)
					{
						CComQIPtr<IAttSubgraphRenderer> isr=m_canvas->GetRenderer(gi);
						if (isr!=0)
						{
							gi = isr->HitTestChildren(point);
						}
					}
					break;
				}
			}
		}
	}

	return gi;
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::GetConnectBounds(
	GraphTypes::RectF& left, 
	GraphTypes::RectF& right,
	unsigned order)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	//CScaledWindow::SetSelected(bSelected, bAdd);
}

//-----------------------------------------------------------------------------

bool CAttSubgraphRenderer::IsSelected()
{
	return m_bSelected;
}

//-----------------------------------------------------------------------------

bool CAttSubgraphRenderer::IsMinimized() const
{
	return m_winSize==WINDOW_SIZE_MIN;
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::OnRemoveGraphItem(IGraphItem* gi)
{
}

//-----------------------------------------------------------------------------

bool CAttSubgraphRenderer::OnEditItem(IGraphItem* gi)
{
	bool bOk=false;

	return bOk;
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetWindowState(WINDOW_SIZE size, int topindex)
{
	m_winSize=size;
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::GetWindowState(WINDOW_SIZE& size, int& topindex) const
{
	size=m_winSize;
	topindex=0; 
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetWindowSize(const GraphTypes::SizeF& size)
{
	// nothing for this graph
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::GetWindowSize(GraphTypes::SizeF& size) const
{
	// nothing for this graph
}

//-----------------------------------------------------------------------------

SizeF CAttSubgraphRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	bool bSizeChange=false;
	bool bPositionChange=false;
	CRect wndrc;
	SizeF wndsize, capsize;

	if(bForce || (m_imposedSize.Width!=0 && m_imposedSize.Height!=0))
	{
		bSizeChange=true;

		SizeF size=m_imposedSize;
		m_canvas->SizeTransform(size);
		wndsize.Width=size.Width;
		wndsize.Height=size.Height;
#ifdef _DEBUG
//		AtlTrace("Scale:%f, Window width:%f, iwidth:%d, height:%f, iheight:%d\r\n",
//			scale, wndsize.Width, int(wndsize.Width), wndsize.Height, int(wndsize.Height)); 
#endif
	}
	else
	{
		wndsize.Width=static_cast<REAL>(m_rcWnd.Width());
		wndsize.Height=static_cast<REAL>(m_rcWnd.Height());
	}

	wndrc.left=m_rcWnd.left;
	wndrc.top=m_rcWnd.top;
	wndrc.right=wndrc.left+round_int(wndsize.Width);
	wndrc.bottom=wndrc.top+round_int(wndsize.Height);

	CalcMinButtonRect(m_rcButMin);

	PointF p = m_position;
	m_canvas->AllTransform(p);
	CPoint ip=static_cast<CPoint>(p);

	if(bForce || ip.x!=m_rcWnd.left || ip.y!=m_rcWnd.top)
	{
		wndrc.left=ip.x;
		wndrc.top=ip.y;	
		wndrc.right=wndrc.left+round_int(wndsize.Width);
		wndrc.bottom=wndrc.top+round_int(wndsize.Height);
		bPositionChange=true;
	}

	if(!bSizeChange)
		bSizeChange=wndrc.Height()!=m_rcWnd.Height() || 
			wndrc.Width()!=m_rcWnd.Width();

	if (!IsMinimized())
	{
		if(bSizeChange||bPositionChange)
		{
			CComPtr<IUnknown> iunk;
			CComPtr<IRenderer> ir;

			for(CComPtr<IGraphItemIterator> itr = m_g->GetChildren(); itr->IsValidItem(); itr->NextItem())
			{
				ir=m_canvas->GetRenderer(itr->Get()); 
				if(ir!=0)
				{
					ir->CalcDisplaySizes(dc, bForce, bIgnoreExtent);
				}
			}
		}
	}

	UINT flags=(bSizeChange?0:SWP_NOSIZE)|(bPositionChange?0:SWP_NOMOVE)|SWP_NOREDRAW;
	SetWindowPos(wndrc, flags);

	return wndsize;
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::ClearCaches()
{

}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RectF rc;
	IRenderer* ir;
	hit=false;
	LRESULT lRes=0;

	if(m_rcButMin.Contains(point))
	{
		lRes=SendMessage(uMsg, wParam, lParam);
		hit=true;
		return lRes;
	}

	if (!IsMinimized())
	{
		for(CComPtr<IGraphItemIterator> itr = m_g->GetChildren(); itr->IsValidItem(); itr->NextItem())
		{
			ir=m_canvas->GetRenderer(itr->Get());
			ATLASSERT(ir!=0);
			ir->GetBoundsBox(rc);
			if(rc.Contains(point))
			{
				if(CComQIPtr<IGraphSubgraph> childSubgraph = itr->Get())
				{
					CComQIPtr<IAttSubgraphRenderer> childSubgraphRenderer = m_canvas->GetRenderer(childSubgraph);
					ATLASSERT(childSubgraphRenderer);
					lRes=childSubgraphRenderer->HitTestDispatchMsg(point, hit, uMsg, wParam, lParam);
				}
				else
				{
					lRes=ir->SendMessage(uMsg, wParam, lParam);
					hit=true;
					break;
				}
			}
		}
	}

	if(!hit && uMsg==WM_CONTEXTMENU)
	{
		lRes=SendMessage(uMsg, wParam, lParam);
		hit=true;
	}

	return lRes;
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
	bool bRedraw=(flags&SWP_NOREDRAW)==0; 
	// parent redraw sends paint to all children,
	// so here we avoid sending redraw to children again
	flags|=SWP_NOREDRAW;
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
		m_rcButMin.Offset(static_cast<GraphTypes::REAL>(offset.x), static_cast<GraphTypes::REAL>(offset.y));
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

		CalcMinButtonRect(m_rcButMin);
	}

	if(bRedraw)
		Invalidate();
}

//-----------------------------------------------------------------------------

int CAttSubgraphRenderer::CalcCaptionSize(CDCHandle& dc, const TCHAR* title)
{
	CDrawVertexHelper tdh;
	tdh.m_text=title;
	tdh.m_scale=m_canvas->GetScale();
	tdh.m_margins=m_captionTextMargin;

	FontInfo fi(
		&m_canvas->GetCaptionFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
		0,
		m_canvas->m_fonts,
		static_cast<GraphTypes::REAL>((DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72));

	m_captionSize=tdh.CalcSize(dc, fi);

	return round_int(m_captionSize.Width);
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::DrawWindow(CAggDC& dc, const GraphTypes::Color & colourBackground, const GraphTypes::Color & colourBorder)
{
	DrawWindowBkgnd(dc, colourBackground, colourBorder);
	DrawCaption(dc, colourBackground, colourBorder);
	DrawMinButton(dc);
	// children are drawn by the upper-level code 
	// since we need to know which child (vertex) is hot
	// and draw it last
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::DrawCaption(CAggDC& dc, const Color & colourBackground, const Color & colourBorder)
{
	REAL border=m_borderLineThickness*m_canvas->GetScale();
	if(border<m_borderMinThickness)
		border=m_borderMinThickness;

	RectF rcfcaption(
		m_rcWnd.left+border,
		m_rcWnd.top+border,
		m_captionSize.Width,
		m_captionSize.Height);

	CRect rccaption=rcfcaption;

	if(m_scale!=m_canvas->GetScale())
	{
		m_scale=m_canvas->GetScale();
		std::_tstring label=m_g->GetLabel();

		//label = (boost::_tformat(_T("%1%, %2%, %3%, %4%")) % rcfcaption.x % rcfcaption.y % rcfcaption.Width % rcfcaption.Height).str();

		CDCHandle tdc(dc.m_hDC);
		CalcCaptionSize(tdc, label.c_str());

		CDrawVertexHelper tdh;
		tdh.m_text=label.c_str();
		tdh.m_cbkgnd=colourBackground;
		tdh.m_cborder=colourBorder;
		tdh.m_margins=m_captionTextMargin;
		tdh.m_scale=m_scale;

		FontInfo fi(
			&m_canvas->GetCaptionFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
			colourBorder,
			m_canvas->m_fonts,
			static_cast<GraphTypes::REAL>((DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72));

		rcfcaption.x=m_rcWnd.left+border;
		rcfcaption.y=m_rcWnd.top+border;
		rcfcaption.Width=m_captionSize.Width;
		rcfcaption.Height=m_captionSize.Height;

		rccaption=rcfcaption;
		CSize size(rccaption.Width(), rccaption.Height());
		boost::scoped_ptr<CAggDC> tmpaggdc(m_titleTextBmp.BeginPaint(dc, size, m_canvas->m_fonts));
		RectF rcf(0.0, 0.0, rcfcaption.Width, rcfcaption.Height);
		tdh.Draw(*tmpaggdc, fi, rcf);
		m_titleTextBmp.EndPaint(tmpaggdc.get());
	}

	rccaption.OffsetRect(1,1);
	m_titleTextBmp.Draw(dc, rccaption, 0);
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::DrawWindowBkgnd(CAggDC& dc, const Color & colourBackground, const Color & colourBorder) 
{
	REAL border=m_borderLineThickness*m_canvas->GetScale();
	if(border<m_borderMinThickness)
		border=m_borderMinThickness;

	RectF rc(m_rcWnd);
	dc.LinearGradientRect(rc, GraphTypes::Color::White, GraphTypes::Color::WhiteSmoke, false);

	PointF tl(m_rcWnd.left+0.5f, m_rcWnd.top+0.5f), br(m_rcWnd.right-0.5f, m_rcWnd.bottom-0.5f);
	dc.Rect(
		tl,
		br,
		GraphTypes::Color::Transparent, //m_canvas->m_meta.m_bgNormalQBColors[idx],
		colourBorder,
		border,
		true);
}

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::CalcMinButtonRect(RectF& rc)
{
	RectF rcminbutt; 
	REAL capheight=m_captionSize.Height*2.0f/3.0f;
	REAL border=m_borderLineThickness*m_canvas->GetScale();
	if(border<m_borderMinThickness)
		border=m_borderMinThickness;
	border*=2.0f;

	rcminbutt.x=m_rcWnd.right-capheight-border;
	rcminbutt.y=m_rcWnd.top+border;
	rcminbutt.Width=capheight;
	rcminbutt.Height=capheight;
	rc=rcminbutt;
}	

//-----------------------------------------------------------------------------

void CAttSubgraphRenderer::DrawMinButton(CAggDC& dc)
{
	RectF rc(m_rcButMin);

	PointF tl, br;

	tl.x=rc.x;
	tl.y=rc.y;
	br.x=rc.x+rc.Width;
	br.y=rc.y+rc.Height;

	dc.Rect(tl, br, Color::Color(153,185,250), Color::Black);

	if(rc.Width>6.0f)
	{
		rc.Inflate(-4,-2);

		br.x=rc.x+rc.Width;
		br.y=rc.y+rc.Height;
		tl.x=rc.x;
		tl.y=br.y-2;

		// a thick line 
		dc.Rect(tl, br, Color::White, Color::White);
	}
}

//*****************************************************************************
//
// Message handlers
//

LRESULT CAttSubgraphRenderer::OnPaint(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CAggDC& dc=*(CAggDC*)wParam;

	//ATLASSERT(m_callback);
	if (m_callback)
	{
		GraphTypes::Color colourBorder = m_callback->GetColor(m_g);
		DrawWindow(dc, GraphTypes::Color::White, colourBorder); 
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_canvas->SetCapture(static_cast<CScaledWindow*>(this));
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnLButtonUp(
	UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_canvas->GetCapture()==static_cast<CScaledWindow*>(this))
		m_canvas->ReleaseCapture();

	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
	PointF ptf=pt;
	if(m_rcButMin.Contains(ptf))
	{
		SetFocus(true);
		NMSGVHDR msg;
		msg.id = GetID();
		m_canvas->NotifyParent(SCW_NOTIFYMINCLICKED, &msg);
		//if(m_winSize==WINDOW_SIZE_MIN)
		//	m_winSize=WINDOW_SIZE_NORM;
		//else
		//	m_winSize=WINDOW_SIZE_MIN;
	}
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnRButtonDown(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnNcHitTest(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return reinterpret_cast<LRESULT>(this);
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnSetCursor(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	//SetCursor(LoadCursor(0, IDC_ARROW));
	return 1;
}

//-----------------------------------------------------------------------------

LRESULT CAttSubgraphRenderer::OnContextMenu(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
	LRESULT lRes=m_canvas->NotifyContextMenu(SCW_NOTIFYSUBGRAPHCONTEXT, m_g->GetID(), pt);
	bHandled=lRes!=0;
	return lRes;
}

//*****************************************************************************

IRenderer* CreateAttSubgraphRenderer(CCanvas & canvas)
{
	CAttSubgraphRenderer * retVal = new CAttSubgraphRenderer(canvas);
	return retVal;
}

