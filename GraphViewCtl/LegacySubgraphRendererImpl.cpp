// This file is currently just a repository for the old Windows-themes
// drawing code that may still be useful someday


#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"

using namespace GraphTypes;

class CLegacySubgraphRenderer 
	: public ILegacySubgraphRenderer, 
	public CUnknown,
	public CScaledWindow
{
	IGraphSubgraph* m_g;
	CCanvas & m_canvas;
	CPartTheme& m_wndTheme;
	CRect m_rcCaption;

	// sizes that vary with each DPI and renderer "window"
	CRectMap m_wnd_sizes; // current size and position 

	// sizes per each DPI that don't vary with each renderer "window"
	static CSizeMap m_captionSizes;

	// sizes that are either fixed per DPI or dynamically scaled
	static CSize m_wndBorder;
	static BORDER_STYLE m_wndBorderStyle;
	static MARGINS m_captionTextMargin;
	static MARGINS m_captionContentMargin;

	CPoint m_origin;
	CPoint m_savePos;
	CPoint m_capturePos;
	GraphTypes::PointF m_position;

	int CalcCaptionSize(CDCHandle& dc, const TCHAR* title);
	int CalcCaptionTotalHeight(void);
	void DrawWindow(CDCHandle& dc); 
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
	void ScaleMargins(MARGINS& margins);
	void DrawEdges(CDCHandle& dc);
	void DrawChildren(CDCHandle& dc);

	HIT_TEST_CODE HitTestCaption(const CPoint& p);

public:
	IMPLEMENT_CUNKNOWNQI(ILegacySubgraphRenderer);

	CLegacySubgraphRenderer(CCanvas & canvas);

	void SetWindowPos(const CRect& pos, UINT flags);

	// IRenderer

	CUniqueID GetID();
	void SetGraphItem(IGraphItem* item);
	void GetBoundsBox(GraphTypes::RectF& box);
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void GetPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co);
	void SetPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co);
	void SetSelected(bool bSelected=true, bool bAdd=false);
	void SetFocus(bool bSet=true);
	LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc,
		bool bForce=false,
		bool bIgnoreExtent=false);	

	// ILegacySubgraphRenderer

	bool IsSelected();

protected:
	BEGIN_MSG_MAP(CLegacySubgraphRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

// height and default width (sample title) for each scale
CSizeMap CLegacySubgraphRenderer::m_captionSizes;


// these statics are either independent of scale factor
// or are scaled dynamically (may consider storing those in
// vector, for each scale factor)

CSize CLegacySubgraphRenderer::m_wndBorder;
MARGINS CLegacySubgraphRenderer::m_captionTextMargin;
MARGINS CLegacySubgraphRenderer::m_captionContentMargin;

BORDER_STYLE CLegacySubgraphRenderer::m_wndBorderStyle=DIALOG_FRAME;

CLegacySubgraphRenderer::CLegacySubgraphRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_canvas(canvas),
	m_wndTheme(CUtilFactory::GetPartTheme(_T("WINDOW"))),
	m_origin(0,0)
{

//#ifdef _DEBUG_THEME
//	CUtilFactory::FlipDebugTheme();
//#endif
}

//-----------------------------------------------------------------------------

CUniqueID CLegacySubgraphRenderer::GetID()
{
	return m_g->GetID();
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetGraphItem(IGraphItem* item)
{
	m_g=static_cast<IGraphSubgraph*>(item);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetBoundsBox(RectF& box)
{
	box=m_rcWnd;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags)
{
	CRect rc=box;
	SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==screen)
	{
		m_canvas.AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	PointF ptnew=pt;
	if(co==screen)
	{
		m_canvas.RAllTransform(ptnew);		
	}

	m_position=ptnew;
//	RectF rcnew(ptnew, m_rcWnd.Size());
//	m_canvas.AllTransform(rcnew);
//	SetBoundsBox(rcnew, SWP_NOSIZE);
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::SendMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetFocus(bool bSet)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	CScaledWindow::SetSelected(bSelected, bAdd);
}

//-----------------------------------------------------------------------------

bool CLegacySubgraphRenderer::IsSelected()
{
	return m_bSelected;
}

//-----------------------------------------------------------------------------

SizeF CLegacySubgraphRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	bool bSizeChange=false;
	bool bPositionChange=false;

	return SizeF();
	CRect wndrc=m_wnd_sizes[m_canvas.GetDpiScale()];
	CSize sizecaption=m_captionSizes[m_canvas.GetDpiScale()];
	int hcaption;

	if(bForce || wndrc.Width()==0 || wndrc.Height()==0) 
	{
		bSizeChange=true;
		int wcaption, wdefault;

		std::_tstring str;
		const TCHAR* txt=m_g->GetLabel(str);

		wcaption=CalcCaptionSize(dc, txt);

		// default width 
		wdefault=wcaption;
		wndrc.right=wndrc.left+wdefault;

		// border is included in total caption height
		CalcWindowBorder(m_wndBorderStyle);

		hcaption=CalcCaptionTotalHeight();

		// default height 10 * caption

		int hdefault=hcaption*10;

		wndrc.bottom=wndrc.top+hcaption+hdefault; // initial height
		wndrc.InflateRect(m_wndBorder.cx, 0);
		wndrc.bottom+=m_wndBorder.cy; // top border included in caption total height

		CComPtr<IGraphItem> item;
		CComPtr<IUnknown> unk;
		CComPtr<IRenderer> ir;

		IGraphItemVector results;
		m_g->GetChildren(results, new MetaVisibleSkipper(m_canvas.m_meta));

		for(IGraphItemVector::iterator itr=results.begin();
			itr!=results.end();
			++itr)
		{
			item=*itr;			
			unk=item->GetPropertyUnknown(0);
			if(unk!=0)
			{
				ir=static_cast<IRenderer*>(unk.p);
				if(ir!=0)
					ir->CalcDisplaySizes(dc, bForce, bIgnoreExtent);
			}
		}
	}

	PointF p = m_position;
	m_canvas.AllTransform(p);
	CPoint ip(int(p.x), int(p.y));

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

	SizeF size;
	size.Height=REAL(wndrc.Height());
	size.Width=REAL(wndrc.Width());
	return size;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
	bool bRedraw=(flags&SWP_NOREDRAW)==0; 
	// parent redraw sends paint to all children,
	// so here we avoid sending redraw to children again
	flags|=SWP_NOREDRAW;
	m_rcWnd = pos;
	return;

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


		// offset all rects
		m_rcClient.OffsetRect(offset);
		m_rcCaption.OffsetRect(offset);

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
		REAL hcaption=REAL(hicaption);

		m_rcCaption=m_rcWnd;

		m_rcCaption.bottom=m_rcCaption.top+hicaption;
		if(m_wndTheme.IsThemeNull())
		{
			m_rcCaption.top+=m_wndBorder.cy;
			m_rcCaption.DeflateRect(m_wndBorder.cx,0);
		}

		m_rcClient=m_rcWnd;

		m_rcClient.DeflateRect(
			m_wndBorder.cx,
			hicaption,
			m_wndBorder.cx, 
			m_wndBorder.cy);
	}

	PointF newp(REAL(m_rcWnd.left), REAL(m_rcWnd.top));
	m_origin.x=m_rcWnd.left;
	m_origin.y=m_rcWnd.top;
	m_canvas.RAllTransform(newp);
	m_position=newp; 

	CRectEx rcex(m_rcWnd); 
	m_wnd_sizes[m_canvas.GetDpiScale()]=rcex;

	if(bRedraw)
		Invalidate();
}

//-----------------------------------------------------------------------------

int CLegacySubgraphRenderer::CalcCaptionSize(CDCHandle& dc, const TCHAR* title)
{
	CString txt=_T("Example Title"); // for minimum caption width 

	CSize extent(0,0);
	CAutoFont f(CUtilFactory::GetCaptionFont(m_canvas.GetFontSize()-1),dc);

	dc.GetTextExtent(txt, txt.GetLength(), &extent);
	m_captionSizes[m_canvas.GetDpiScale()]=extent;

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

int CLegacySubgraphRenderer::CalcCaptionTotalHeight(void)
{
	int height=m_captionSizes[m_canvas.GetDpiScale()].cy;

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

void CLegacySubgraphRenderer::DrawWindow(CDCHandle& dc)
{
	DrawEdges(dc);
	DrawWindowBorder(dc,m_wndBorderStyle);
	DrawWindowCaption(dc, CS_SMALL);
	DrawChildren(dc);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::CalcWindowBorder(BORDER_STYLE style)
{
	bool bThemeOk=false;

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

void CLegacySubgraphRenderer::DrawWindowBorder(
	CDCHandle& dc,
	BORDER_STYLE style)
{
	UINT flags=BF_RECT;
	CRect rc(m_rcWnd);

//	if(m_wndTheme.IsThemeNull())
	if(true)
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

void CLegacySubgraphRenderer::DrawWindowCaption(
	CDCHandle& dc, 
	CAPTION_STYLE style)
{
	CRect rc(m_rcCaption);	
	const TCHAR* txt;
	UINT tflags;
	COLORREF clrtext;
	COLORREF clrtextshadow;
	POINT offset;
	int shadow;

	std::_tstring str;
	txt=m_g->GetLabel(str);

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
		CAutoFont f(CUtilFactory::GetCaptionFont(m_canvas.GetFontSize() - 1),	dc);

		// prepare text rectangle w.r.t. caption rectangle

		// we already deflated rect for window borders but caption
		// text drawn without margin on the left looks ugly
		rc.left+=m_wndBorder.cx;

		rc.left+=
			m_captionContentMargin.cxLeftWidth+
			m_captionTextMargin.cxLeftWidth; 

		// only restore button in minimized case

		rc.right-=m_captionContentMargin.cxRightWidth+
				m_captionTextMargin.cxRightWidth;

		rc.top+=m_captionContentMargin.cyTopHeight+
			m_captionTextMargin.cyTopHeight;

		rc.bottom=rc.top+m_captionSizes[m_canvas.GetDpiScale()].cy;

		DrawCaptionText(
			dc,
			txt,
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

void CLegacySubgraphRenderer::DrawClassicCaptionBkgnd(
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

void CLegacySubgraphRenderer::DrawCaptionText(
	CDCHandle& dc,
	const TCHAR* txt,
	CRect* rctext,
	UINT flags,
	int shadowtype,
	COLORREF clrtext,
	COLORREF clrshadow,
	const POINT& offset)
{
	unsigned len=_tcslen(txt);

	if(shadowtype==TST_CONTINUOUS)
	{
		WCHAR* wtxt=CT2W(txt,CP_UTF8);

		dc.DrawShadowText(
			wtxt,
			len,
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
				len, 
				rcshadow, 
				flags );
		}

		dc.SetTextColor(clrtext);
		dc.DrawText(
			txt, 
			len, 
			rctext, 
			flags );
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::DrawWindowBkgnd(
	CDCHandle& dc) 
{
	dc.FillSolidRect(&m_rcClient,GetSysColor(COLOR_3DFACE));
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::DrawEdges(CDCHandle& dc)
{
	CComPtr<boost::scoped_lock_ref_counted> lock;

	CComQIPtr<IEdgeRenderer> ier;
	IUnknown * iunk;
	IGraphVertex * vertex, * neighbor;
	IGraphEdge * edge;
	IGraphSubgraph * sgneighbor;

	unsigned myorder=m_g->GetOrder();

	for(CComPtr<IVertexIterator> itr = m_g->GetVertexIterator();
		itr->IsValidItem();
		itr->NextItem())
	{
		vertex = itr->Get();

		IGraphEdgeVector edges;
		vertex->GetAdjacentEdges(edges);
		for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
		{
			edge=*eitr;
/*	glenn::????
			neighbor=edge->GetAdjacentVertex(vertex);
			sgneighbor=neighbor->GetParent();

			if(sgneighbor!=0)
			{
				unsigned hisorder=sgneighbor->GetOrder();
				if(hisorder<myorder)
				{
					iunk=edge->GetPropertyUnknown(0); 
					if(iunk!=0)
					{
						ier=iunk;
						if(ier!=0)
							ier->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
					}
				}
			}
*/
			iunk=edge->GetPropertyUnknown(0); 
			if(iunk!=0)
			{
				ier=iunk;
				if(ier!=0)
					ier->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::DrawChildren(CDCHandle& dc)
{
	CComPtr<IGraphItem> item;
	CComPtr<IUnknown> unk;
	CComQIPtr<ILegacyRenderer> lv;

	IGraphItemVector results;
	m_g->GetChildren(results, new MetaVisibleSkipper(m_canvas.m_meta));

	for(IGraphItemVector::iterator itr=results.begin(); itr!=results.end(); ++itr)
	{
		item=*itr;			
		unk=item->GetPropertyUnknown(0);
		if(unk!=0)
		{
			lv=unk.p;
			if(lv!=0)
			{
				lv->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::ScaleMargins(MARGINS& margins)
{
	RectF scaled(
		REAL(margins.cxLeftWidth),
		REAL(margins.cyTopHeight),
		REAL(margins.cxRightWidth),
		REAL(margins.cyBottomHeight));

	m_canvas.ScaleTransform(scaled);

	margins.cxLeftWidth=int(scaled.x);
	margins.cyTopHeight=int(scaled.y);
	margins.cxRightWidth=int(scaled.Width);
	margins.cyBottomHeight=int(scaled.Height);
}

//-----------------------------------------------------------------------------

HIT_TEST_CODE CLegacySubgraphRenderer::HitTestCaption(const CPoint& p)
{
	HIT_TEST_CODE code=HT_UNKNOWN;

	if(m_rcCaption.PtInRect(p))
		code=HT_CAPTION;

	return code;
}
							 
//*****************************************************************************
//
// Message handlers
//

LRESULT CLegacySubgraphRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM /*lParam*/,
	BOOL& /*bHandled*/)
{
	CDCHandle dc=*(CDCHandle*)wParam;

	DrawWindow(dc); 

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnLButtonDown(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	bool bOldSelection=IsSelected();
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	HIT_TEST_CODE hit=HitTestCaption(pt);

	m_g->MoveToEnd();
	SetFocus();

	if((wParam & MK_CONTROL)!=0)
	{
		CScaledWindow::SetSelected(!IsSelected(), true);
	}
	else
	{
		CScaledWindow::SetSelected(true, (wParam & MK_SHIFT)!=0 );
	}
	
	if(hit==HT_CAPTION) 
	{
		m_canvas.SetCapture(this);

		m_capturePos=pt;
			
		PointF savePos=m_position;
		m_canvas.AllTransform(savePos);
		m_savePos.x=int(savePos.x);
		m_savePos.y=int(savePos.y);
	}


	if(IsSelected()!=bOldSelection)
	{
		Invalidate();
		m_canvas.NotifySelection(m_g);
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnLButtonUp(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& bHandled)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	HIT_TEST_CODE hit=HitTestCaption(pt);

	if(m_canvas.GetCapture()==this)
	{
		m_canvas.ReleaseCapture();
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnMouseMove(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	if(m_canvas.GetCapture()==this)
	{
		CRect newrc=m_rcWnd;

		pt.x+=m_savePos.x-m_capturePos.x;
		pt.y+=m_savePos.y-m_capturePos.y;
		newrc.TopLeft()=pt;
		newrc.right=newrc.left+m_rcWnd.Width();
		newrc.bottom=newrc.top+m_rcWnd.Height();
		SetWindowPos(newrc, SWP_NOSIZE);
	}

	return 0;
}

//*****************************************************************************

IRenderer* CreateLegacySubgraphRenderer(CCanvas & canvas)
{
	CLegacySubgraphRenderer * retVal = new CLegacySubgraphRenderer(canvas);
	return retVal;
}

