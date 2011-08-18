#include "StdAfx.h"

#include <algorithm>

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "Geometry.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "SizeTracker.h"
#include <limits.h>


#define __SKIP_INVISIBLE__

class CTimelineEdgeRenderer : public ITimelineEdgeRenderer,	public CScaledWindow
{
	IGraphEdge * m_edge; //Should not be a CComPtr as this would create a circular reference.
	IGraphVertex* m_vs, *m_vt;

	GraphTypes::PointF m_sp, m_tp; // these points are kept in screen/device coordinates

	CAggBitmap m_textBmp;
	CSizeTracker m_sizeTracker; // edge label
	CButtonState m_hotTracker;
	CAggBitmap m_textBmpNormalS; // source vertex
	CAggBitmap m_textBmpSelectedS;
	CSizeTracker m_sizeTrackerS; 
	CAggBitmap m_textBmpNormalT; // target vertex 
	CAggBitmap m_textBmpSelectedT;
	CSizeTracker m_sizeTrackerT; 
	MARGINS m_textMargins; //  label margins

	GraphTypes::PointF m_fromPos;
	GraphTypes::PointF m_toPos;
	GraphTypes::SizeF m_vLineExtent; // extent of vertical edge notch
	GraphTypes::SizeF m_vertexMargin; // margin for vertex bbox w.r.t. edge

	void Draw(WPARAM wParam);
	void DrawLabel(CAggDC& dc);
	bool CalcVisibility(GraphTypes::PointF p1, GraphTypes::PointF p2);
	void CalcColors(
		GraphTypes::Color& fillColor,
		GraphTypes::Color& borderColor,
		GraphTypes::Color& fontColor, bool source);
	GraphTypes::RectF CalcVertexTextBBox(bool source);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(ITimelineEdgeRenderer)
		IMPLEMENT_INTERFACE(IEdgeRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CTimelineEdgeRenderer(CCanvas & canvas); 

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

	// IEdgeRenderer
	bool GetConnectionPoints(GraphTypes::PointF& src, GraphTypes::PointF& tgt,
		GraphTypes::Coordinates co=GraphTypes::screen,
		GraphTypes::PointF* srcoff=0, GraphTypes::PointF* tgtoff=0);
	unsigned GetPoints(GraphTypes::PointFVector& vp, GraphTypes::Coordinates co);
	void SetPoints(const GraphTypes::PointFVector& vp, GraphTypes::Coordinates co);
	void GetLabelPos(GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
	void SetLabelPos(const GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
	bool IsSelected()
	{
		return CScaledWindow::IsSelected();
	}

	// ITimelineEdgeRenderer
	void GetFromPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
	void SetFromPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
	void GetToPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
	void SetToPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
	bool IsHot();

protected:
	BEGIN_MSG_MAP(CTimelineEdgeRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

using namespace GraphTypes;

CTimelineEdgeRenderer::CTimelineEdgeRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_edge(0),
	m_hotTracker(&canvas, this),
	m_vLineExtent(0.0f,15.0f),
	m_vertexMargin(5.0f,5.0f)
{
	m_textMargins.cxLeftWidth=4;
	m_textMargins.cxRightWidth=4;
	m_textMargins.cyTopHeight=4;
	m_textMargins.cyBottomHeight=4;
}

//-----------------------------------------------------------------------------

CUniqueID CTimelineEdgeRenderer::GetID()
{
	return m_edge->GetID();
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraphEdge> edge = item;
	m_edge = edge;
	ATLASSERT(m_edge);

	m_vs=edge->GetSource();
	ATLASSERT(m_vs);
	m_vt=edge->GetTarget();
	ATLASSERT(m_vt);
}

//-----------------------------------------------------------------------------

LRESULT CTimelineEdgeRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetFocus(bool bSet)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetSelected(bool bSelected, bool bAdd)
{
	bool oldSelected = m_bSelected;
	CScaledWindow::SetSelected(GetID(), bSelected, bAdd);
	if (bSelected != oldSelected)
		m_canvas->NotifySelection(GetID());
}

//-----------------------------------------------------------------------------

SizeF CTimelineEdgeRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	CDrawVertexHelper tdh;

	tdh.m_text=m_vs->GetLabel();
	tdh.m_scale=m_canvas->GetScale();
	tdh.m_margins=m_textMargins;
	tdh.m_printing=m_canvas->IsPrinting();

	FontInfo fi(
		&m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
		ARGB(GraphTypes::Color::Black),
		m_canvas->m_fonts,
		(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72);

	if(m_sizeTrackerS.Update(m_canvas))
	{
		CDCHandle hdc(dc.m_hDC);

		if(m_canvas->IsPrinting())
		{
			SizeF extentf=m_sizeTrackerS.m_extentMap[m_canvas->GetDpiScale()];

			if(extentf.Height==0)
			{
				extentf=tdh.CalcSize(hdc, fi);
				m_sizeTrackerS.m_extentMap[m_canvas->GetDpiScale()]=extentf;
			}
		}
		else
		{
			m_sizeTrackerS.m_scaledExtent=tdh.CalcSize(hdc, fi);
			if(m_sizeTrackerS.m_extent.Width==0)
			{
				tdh.m_scale=1;
				m_sizeTrackerS.m_extent=tdh.CalcSize(dc, fi);
			}
		}

		m_textBmpNormalS.Clear();
		m_textBmpSelectedS.Clear();
	}

	tdh.m_text=m_vt->GetLabel();
	tdh.m_scale=m_canvas->GetScale();

	if(m_sizeTrackerT.Update(m_canvas))
	{
		CDCHandle hdc(dc.m_hDC);

		if(m_canvas->IsPrinting())
		{
			SizeF extentf=m_sizeTrackerT.m_extentMap[m_canvas->GetDpiScale()];

			if(extentf.Height==0)
			{
				extentf=tdh.CalcSize(hdc, fi);
				m_sizeTrackerT.m_extentMap[m_canvas->GetDpiScale()]=extentf;
			}
		}
		else
		{
			m_sizeTrackerT.m_scaledExtent=tdh.CalcSize(hdc, fi);
			if(m_sizeTrackerT.m_extent.Width==0)
			{
				tdh.m_scale=1;
				m_sizeTrackerT.m_extent=tdh.CalcSize(dc, fi);
			}
		}

		m_textBmpNormalT.Clear();
		m_textBmpSelectedT.Clear();
	}

	return SizeF();
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	GetFromPosition(m_sp, GraphTypes::screen);
	GetToPosition(m_tp, GraphTypes::screen);

	RectF rcext(m_sp.x, m_sp.y, m_tp.x-m_sp.x, 1);

	RectF bbox=CalcVertexTextBBox(true);
	ATLASSERT(!bbox.IsEmptyArea());
	rcext.Union(rcext, rcext, bbox);

	bbox=CalcVertexTextBBox(false);
	ATLASSERT(!bbox.IsEmptyArea());
	rcext.Union(rcext, rcext, bbox);

	box=rcext;
	if(co==world)
		m_canvas->RAllTransform(box);
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
	// TODO discover use for this
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::ClearCaches()
{
	m_textBmp.Clear();

	m_textBmpNormalS.Clear(); 
	m_textBmpSelectedS.Clear();
	m_textBmpNormalT.Clear(); 
	m_textBmpSelectedT.Clear();
}

//-----------------------------------------------------------------------------

bool CTimelineEdgeRenderer::GetConnectionPoints(
	PointF& src, PointF& tgt,
	Coordinates co/*=screen*/,
	PointF* srcoff/*=0*/, PointF* tgtoff/*=0*/)
{
	src=m_sp;
	tgt=m_tp;

	if(co==world)
	{
		m_canvas->RAllTransform(src);
		m_canvas->RAllTransform(tgt);
	}

	return true;
}

//-----------------------------------------------------------------------------

unsigned CTimelineEdgeRenderer::GetPoints(PointFVector& vp, Coordinates co/*=world*/)
{
	PointF p1(m_sp);
	PointF p2(m_tp);

	if(co==world) 
	{
		m_canvas->RAllTransform(p1);
		m_canvas->RAllTransform(p2);
	}

	vp.push_back(p1);
	vp.push_back(p2);

	return 2;
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetPoints(const PointFVector& vp, Coordinates co/*=world*/)
{
	if(vp.size()==2)
	{
		m_sp=vp[0];
		m_tp=vp[1];

		if(co==world)
		{
			m_canvas->AllTransform(m_sp);
			m_canvas->AllTransform(m_tp);
		}
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::GetLabelPos(RectF& pos, Coordinates co/*=screen*/)
{
	ATLASSERT(0);
	if(co==world)
	{
//		RectF rc=rcLabel;
//		m_canvas->RAllTransform(rc);
//		pos=rc;
	}
	else
	{
//		pos=rcLabel;
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetLabelPos(const RectF& pos, Coordinates co/*=screen*/)
{
	ATLASSERT(0);
	if(co==world)
	{
//		RectF rc=pos;
//		m_canvas->AllTransform(rc);
//		rcLabel=rc;
	}
	else
	{
//		rcLabel=pos;
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::GetFromPosition(PointF& pt, Coordinates co)
{
	pt=m_fromPos;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
		ATLASSERT(pt.x > static_cast<double> (INT_MIN / 4) - 1.0);
		ATLASSERT(pt.x < static_cast<double> (INT_MAX / 4) + 1.0);
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetFromPosition(const PointF& pt, Coordinates co)
{
	m_fromPos=pt;

	if(co==screen)
	{
		ATLASSERT(pt.x > static_cast<double> (INT_MIN / 4) - 1.0);
		ATLASSERT(pt.x < static_cast<double> (INT_MAX / 4) + 1.0);

		m_canvas->RAllTransform(m_fromPos);
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::GetToPosition(PointF& pt, Coordinates co)
{
	pt=m_toPos;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
		ATLASSERT(pt.x > static_cast<double> (INT_MIN / 4) - 1.0);
		ATLASSERT(pt.x < static_cast<double> (INT_MAX / 4) + 1.0);
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::SetToPosition(const PointF& pt, Coordinates co)
{
	m_toPos=pt;

	if(co==screen)
	{
		ATLASSERT(pt.x > static_cast<double> (INT_MIN / 4) - 1.0);
		ATLASSERT(pt.x < static_cast<double> (INT_MAX / 4) + 1.0);

		m_canvas->RAllTransform(m_toPos);
	}
}

//-----------------------------------------------------------------------------

bool CTimelineEdgeRenderer::IsHot()
{
	return m_hotTracker.IsHot();
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::CalcColors(
	Color& fillColor, Color& borderColor, Color& fontColor, bool source)
{
	IGraphVertex* v=source?m_vs:m_vt;

	bool bHilight=m_canvas->GetHilightID()==v->GetID();
	CCategory cat = v->GetCategory();
	fontColor=m_canvas->m_meta.GetMetaVertexFontColor(cat);

	// don't draw hot state on printer device
	if(!m_canvas->IsPrinting() && IsHot())
	{
		fillColor=m_canvas->m_meta.GetMetaVertexColorFillOver(cat);
		borderColor=m_canvas->m_meta.GetMetaVertexColorBorderOver(cat);
		fontColor=m_canvas->m_meta.GetMetaVertexFontColorSel(cat);
	}
	else
	{
		if(bHilight)
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
				// neither selected nor hot
				fillColor=m_canvas->m_meta.GetMetaVertexColorFillNorm(cat);
				borderColor=m_canvas->m_meta.GetMetaVertexColorBorderNorm(cat);

				if (v->GetBorderColor().GetValue() != 0x0)
					borderColor.SetValue(v->GetBorderColor().GetValue());
				if (v->GetFillColor().GetValue() != 0x0)
					fillColor.SetValue(v->GetFillColor().GetValue());
				if (v->GetFontColor().GetValue() != 0x0)
					fontColor.SetValue(v->GetFontColor().GetValue());
			}
		}
	}
}

//-----------------------------------------------------------------------------

RectF CTimelineEdgeRenderer::CalcVertexTextBBox(bool source)
{
	SizeF size;
	if(m_canvas->IsPrinting())
	{
		size=source ?
			(m_sizeTrackerS.m_extentMap[m_canvas->GetDpiScale()]):
			(m_sizeTrackerT.m_extentMap[m_canvas->GetDpiScale()]);
	}
	else
	{
		size=source ?
			(m_hotTracker.IsHot()?m_sizeTrackerS.m_extent:m_sizeTrackerS.m_scaledExtent) :
			(m_hotTracker.IsHot()?m_sizeTrackerT.m_extent:m_sizeTrackerT.m_scaledExtent);
	}

	RectF vp, bbox;

	m_canvas->GetViewport(vp);

	if(source)
	{

		if((m_sp.x+m_vertexMargin.Width)<vp.x)
		{
			bbox.x=vp.x;
			if((bbox.x+size.Width+m_vertexMargin.Width)>m_tp.x)
				bbox.x=m_tp.x-size.Width-m_vertexMargin.Width;
		}
		else
		{
			bbox.x=m_sp.x+m_vertexMargin.Width;
		}

		bbox.y=m_sp.y-size.Height-m_vertexMargin.Height;
	}
	else
	{
		if((m_tp.x-m_vertexMargin.Width)>(vp.x+vp.Width))
		{
			bbox.x=vp.x+vp.Width-size.Width;
			if((bbox.x-m_vertexMargin.Width)<m_sp.x)
				bbox.x=m_sp.x+size.Width+m_vertexMargin.Width;
		}
		else
		{
			bbox.x=m_tp.x-size.Width-m_vertexMargin.Width;
		}

		bbox.y=m_tp.y+m_vertexMargin.Height;
	}

	bbox.Width=size.Width;
	bbox.Height=size.Height;

	return bbox;
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::Draw(WPARAM wParam) 
{
	CAggDC& dc=*(CAggDC*)wParam;

	GetFromPosition(m_sp, GraphTypes::screen);
	GetToPosition(m_tp, GraphTypes::screen);

#ifdef _DEBUG
//	ATLTRACE(_T("Source x=%f,y=%f, Target x=%f,y=%f\n"), m_sp.x, m_sp.y, m_tp.x, m_tp.y); 
#endif
	if(m_canvas->IsPrinting() || CalcVisibility(m_sp, m_tp))
	{
		Color color=m_edge->GetColor();

		if(color.GetAlpha()==0)
			color=m_canvas->m_meta.GetMetaEdgeColorNorm(m_edge->GetCategory());

		WORD hls[3];
		color.GetHLS(hls);
		if(hls[1]>230) // almost white
			color.LessBright(50);

		if(IsHot())
			color.LessBright(50);

		//SizeF width;
		//REAL maxWidth;

		//width.Width=m_canvas->m_meta.GetMetaEdgeWidth(m_edge->GetCategory());
		//m_canvas->SizeTransform(width);
		//maxWidth=width.Width/2;

		dc.Line(m_sp, m_tp, color);
		// left vertical line
		PointF ptl(m_sp), pbl(m_sp); 
		PointF voffset(m_vLineExtent);
		m_canvas->SizeTransform(voffset);
		ptl.y+=voffset.y;
		pbl.y-=voffset.y;
		dc.Line(ptl, pbl, color);
		// right vertical
		PointF ptr(m_tp), pbr(m_tp); 
		ptr.y+=voffset.y;
		pbr.y-=voffset.y;
		dc.Line(ptr, pbr, color);

		// edge label
		DrawLabel(dc);

		// Source vertex 

		CDrawVertexHelper tdh;
		tdh.m_text=m_vs->GetLabel();
		tdh.m_borderwidth=1.0;
		tdh.m_margins=m_textMargins;
		tdh.m_printing=m_canvas->IsPrinting();
		tdh.m_scale=IsHot()?1.0:m_canvas->GetScale();

		FontInfo fi(
			&m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
			0,
			m_canvas->m_fonts,
			(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72);

		CalcColors(tdh.m_cbkgnd, tdh.m_cborder, fi.m_color, true);

		RectF bbox=CalcVertexTextBBox(true);
		CRect frameRect=static_cast<CRect>(bbox);

		if(m_canvas->IsPrinting())
		{
			tdh.Draw(dc, fi, bbox);
		}
		else
		{
			if(m_hotTracker.IsHot())
			{
				tdh.m_scale=1;
				tdh.Draw(dc, fi, bbox);
			}
			else
			{
				bool bHilight=m_canvas->GetHilightID()==m_vs->GetID();

				if(bHilight)
				{
					tdh.Draw(dc, fi, bbox);
				}
				else
				{
					if(IsSelected())
					{
						if(m_textBmpSelectedS.IsEmpty())
						{
							CSize size=m_sizeTrackerS.m_scaledExtent;

							boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpSelectedS.BeginPaint(dc, size, m_canvas->m_fonts));
							RectF rcf(0.0, 0.0, m_sizeTrackerS.m_scaledExtent.Width, m_sizeTrackerS.m_scaledExtent.Height);
							tdh.Draw(*tmpaggdc, fi, rcf);
							m_textBmpSelectedS.EndPaint(tmpaggdc.get());
						}

						m_textBmpSelectedS.Draw(dc, frameRect, 0);
					}
					else
					{
						if(m_textBmpNormalS.IsEmpty())
						{
							CSize size=m_sizeTrackerS.m_scaledExtent;

							boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpNormalS.BeginPaint(dc, size, m_canvas->m_fonts));
							RectF rcf(0.0, 0.0, m_sizeTrackerS.m_scaledExtent.Width, m_sizeTrackerS.m_scaledExtent.Height);
							tdh.Draw(*tmpaggdc, fi, rcf);
							m_textBmpNormalS.EndPaint(tmpaggdc.get());
						}

						m_textBmpNormalS.Draw(dc, frameRect, 0);
					}
				}
			}
		}

		// Target vertex

		tdh.m_text=m_vt->GetLabel();
		tdh.m_scale=IsHot()?1.0:m_canvas->GetScale();

		CalcColors(tdh.m_cbkgnd, tdh.m_cborder, fi.m_color, false);
		bbox=CalcVertexTextBBox(false);

		frameRect=static_cast<CRect>(bbox);

		if(m_canvas->IsPrinting())
		{
			tdh.Draw(dc, fi, bbox);
		}
		else
		{
			if(IsHot())
			{
				tdh.m_scale=1;
				tdh.Draw(dc, fi, bbox);
			}
			else
			{
				bool bHilight=IsHot() || m_canvas->GetHilightID()==m_vt->GetID();

				if(bHilight)
				{
					tdh.Draw(dc, fi, bbox);
				}
				else
				{
					if(IsSelected())
					{
						if(m_textBmpSelectedT.IsEmpty())
						{
							CSize size=m_sizeTrackerT.m_scaledExtent;

							boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpSelectedT.BeginPaint(dc, size, m_canvas->m_fonts));
							RectF rcf(0.0, 0.0, m_sizeTrackerT.m_scaledExtent.Width, m_sizeTrackerT.m_scaledExtent.Height);
							tdh.Draw(*tmpaggdc, fi, rcf);
							m_textBmpSelectedT.EndPaint(tmpaggdc.get());
						}

						m_textBmpSelectedT.Draw(dc, frameRect, 0);
					}
					else
					{
						if(m_textBmpNormalT.IsEmpty())
						{
							CSize size=m_sizeTrackerT.m_scaledExtent;

							boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpNormalT.BeginPaint(dc, size, m_canvas->m_fonts));
							RectF rcf(0.0, 0.0, m_sizeTrackerT.m_scaledExtent.Width, m_sizeTrackerT.m_scaledExtent.Height);
							tdh.Draw(*tmpaggdc, fi, rcf);
							m_textBmpNormalT.EndPaint(tmpaggdc.get());
						}

						m_textBmpNormalT.Draw(dc, frameRect, 0);
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CTimelineEdgeRenderer::DrawLabel(CAggDC& dc)
{
#define _DEBUG_SHOW_TIME_
#ifdef _DEBUG_SHOW_TIME_
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	time_period edgeSpan = m_edge->GetDateRange();
	ptime beginE=edgeSpan.begin();
	ptime endE=edgeSpan.end();

	std::string charlabel=to_simple_string(edgeSpan);
	std::_tstring label=CA2T(charlabel.c_str());
	unsigned len = label.length();
#else
	if (!m_canvas->m_meta.IsEdgeLabelVisible())
		return;

	std::_tstring label = m_edge->GetLabel();
	unsigned len = label.length();
	if(len==0)
	{
		m_canvas->m_meta.GetMetaEdgeLabel(m_edge->GetCategory(), label);
		len = label.length();
	}
#endif

	if(len==0)
		return;

	CDrawVertexHelper tdh;
	tdh.m_text=label.c_str();
	tdh.m_scale=m_canvas->GetScale();
	tdh.m_printing=m_canvas->IsPrinting();
	tdh.m_cbkgnd.SetFromCOLORREF(GetSysColor(COLOR_WINDOW));
	tdh.m_cborder=tdh.m_cbkgnd;

	Color fcolor=m_canvas->m_meta.GetMetaEdgeFontColor(m_edge->GetCategory());

	FontInfo fi(
		&m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
		fcolor,
		m_canvas->m_fonts,
		(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72);


	if(m_sizeTracker.Update(m_canvas))
	{
		CDCHandle hdc(dc.m_hDC);

		if(m_canvas->IsPrinting())
		{
			SizeF extentf=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];

			if(extentf.Height==0)
			{
				extentf=tdh.CalcSize(hdc, fi);
				m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()]=extentf;
			}
		}
		else
		{
			m_sizeTracker.m_scaledExtent=tdh.CalcSize(hdc, fi);
		}

		if(!m_canvas->IsPrinting())
		{
			m_textBmp.Clear();

			CSize size=m_sizeTracker.m_scaledExtent;

			boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmp.BeginPaint(dc, size, m_canvas->m_fonts));
			RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
			tdh.Draw(*tmpaggdc, fi, rcf);
			m_textBmp.EndPaint(tmpaggdc.get());
		}
	}

	RectF rcLabel;
	if(m_canvas->IsPrinting())
	{
		SizeF extentf=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];
		rcLabel.Width=extentf.Width;
		rcLabel.Height=extentf.Height;
	}
	else
	{
		rcLabel.Width=m_sizeTracker.m_scaledExtent.Width;
		rcLabel.Height=m_sizeTracker.m_scaledExtent.Height;
	}

	rcLabel.x=(m_sp.x+m_tp.x-rcLabel.Width)/2;
	rcLabel.y=(m_sp.y+m_tp.y-rcLabel.Height)/2;
#ifdef _DEBUG_SHOW_TIME_
	RectF vp;
	m_canvas->GetViewport(vp);
	if(rcLabel.x>=vp.Width || (rcLabel.x+rcLabel.Width)<=0)
		rcLabel.x=(vp.Width-rcLabel.Width)/2;
#endif
	if(!m_canvas->IsPrinting())
	{
		CRect frameRect=static_cast<CRect>(rcLabel);

		if(m_canvas->m_meta.GetEdgeLabelOpacity()==0)
		{
			m_textBmp.Draw(dc, frameRect, DT_TRANSPARENT);
		}
		else
		{
			m_textBmp.Draw(dc, frameRect, 0);
		}
	}
	else
	{
		tdh.Draw(
			dc, 
			fi,
			rcLabel, 
			m_canvas->m_meta.GetEdgeLabelOpacity()==0?DT_TRANSPARENT:0);
	}
}

//-----------------------------------------------------------------------------
//
// Points are in screen coordinates
//
bool CTimelineEdgeRenderer::CalcVisibility(PointF p1, PointF p2)
{
#ifdef __SKIP_INVISIBLE__
	RectF vp;
	m_canvas->GetViewport(vp);
	return LineIntersectsRect(p1, p2, vp);
#endif
	return true;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineEdgeRenderer::OnPaint(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	bool bvis=m_canvas->m_meta.IsVisible(m_edge) && IsRedraw();

	if(bvis)
		Draw(wParam);	

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineEdgeRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

	// nothing can be hot now
	m_canvas->ClearCurrentHot();
	// but clearing hot object clears hilight ID, so reset it here
	//RectF vbox=CalcVertexTextBBox(true);
	//if(vbox.Contains(p))
	//{
	//	m_canvas->SetHilightID(m_vs->GetID());
	//}
	//else
	//{
	//	vbox=CalcVertexTextBBox(false);
	//	if(vbox.Contains(p))
	//		m_canvas->SetHilightID(m_vt->GetID());
	//}

	SetSelected();

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	return reinterpret_cast<LRESULT>(m_edge);
}

//-----------------------------------------------------------------------------

LRESULT CTimelineEdgeRenderer::OnLButtonUp(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineEdgeRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

	bool bHot=IsHot();
	bool bIDChange=false;

	RectF vbox=CalcVertexTextBBox(true);
	if(vbox.Contains(p))
	{
		bIDChange=m_canvas->SetHilightID(m_vs->GetID());
	}
	else
	{
		vbox=CalcVertexTextBBox(false);
		if(vbox.Contains(p))
			bIDChange=m_canvas->SetHilightID(m_vt->GetID());
		else
			bIDChange=m_canvas->SetHilightID(CUniqueID());
	}

	m_hotTracker.SetHot(true);

	if(GetCursor()!=LoadCursor(NULL, IDC_ARROW))
		SetCursor(LoadCursor(NULL, IDC_ARROW));

	if(bHot!=IsHot())
	{
		m_canvas->NotifyHotChanged(GetID(), IsHot());
		m_canvas->Invalidate();
	}
	else if (bIDChange)
	{
		m_canvas->Invalidate();
	}

	return 0;
}

//*****************************************************************************

IRenderer* CreateTimelineEdgeRenderer(CCanvas & parent)
{
	CTimelineEdgeRenderer * retVal = new CTimelineEdgeRenderer(parent);
	return retVal;
}
