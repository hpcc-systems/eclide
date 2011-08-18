#include "StdAfx.h"

#include <algorithm>

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "Geometry.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "SizeTracker.h"
#include "XGMMLSaxHandler.h"

#define __SKIP_INVISIBLE__
#define LABEL_BUFFER 8.0f

class CAttEdgeRenderer : public IAttEdgeRenderer, public CScaledWindow
{
	IGraphEdge * m_edge; //Should not be a CComPtr as this would create a circular reference.
	CComQIPtr<IXGMMLRenderCallback> m_callback;

	boost::signals::connection m_sigConn;
	CButtonState m_hotTracking;
	CUniqueID m_uniqueID;

	GraphTypes::PointFVector m_vp; // points are kept in world coordinates
	mutable GraphTypes::PointFVector m_svp; // cached screen coordinates from last CalcVisbility
	GraphTypes::PointF m_position; // label position in world coordinates, imposed externally	

	bool m_spansSubgraphsDetermined;
	bool m_spansSubgraphs;

	static GraphTypes::REAL m_LineThickness;
	static GraphTypes::REAL m_MinThickness;

	CAggBitmap m_textBmpNormal;
	CAggBitmap m_textBmpSelected;
	CSizeTracker m_sizeTracker;

	void Draw(WPARAM wParam, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor);
	void DrawLine(WPARAM wParam, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor);
	void DrawBezier(WPARAM wParam, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor);
	void DrawLabel(CAggDC& dc, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor);

	bool CalcVisibility() const;
	bool CalcVisibility(GraphTypes::PointF p1, GraphTypes::PointF p2);
	virtual void Invalidate(CRect* rc=0, bool bErase=false)
	{
		m_canvas->Invalidate();
	}

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IAttEdgeRenderer)
		IMPLEMENT_INTERFACE(IEdgeRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CAttEdgeRenderer(CCanvas & canvas); 
	~CAttEdgeRenderer(); 

	// Callback
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

	// IEdgeRenderer

	bool GetConnectionPoints(GraphTypes::PointF& src, GraphTypes::PointF& tgt,
		GraphTypes::Coordinates co=GraphTypes::screen,
		GraphTypes::PointF* srcoff=0, GraphTypes::PointF* tgtoff=0);
	unsigned GetPoints(GraphTypes::PointFVector& vp, GraphTypes::Coordinates co);
	void SetPoints(const GraphTypes::PointFVector& vp, GraphTypes::Coordinates co);
	void GetLabelPos(GraphTypes::RectF& rcpos, GraphTypes::Coordinates co=GraphTypes::screen);
	void SetLabelPos(const GraphTypes::RectF& rcpos, GraphTypes::Coordinates co=GraphTypes::screen);
	bool IsSelected();

	// IAttEdgeRenderer

	bool IntersectsViewport() const;

protected:
	BEGIN_MSG_MAP(CAttEdgeRenderer)
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

//*****************************************************************************

GraphTypes::REAL CAttEdgeRenderer::m_LineThickness(2.0);
GraphTypes::REAL CAttEdgeRenderer::m_MinThickness(0.5);

using namespace GraphTypes;

CAttEdgeRenderer::CAttEdgeRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_hotTracking(&canvas, this),
	m_edge(0),
	m_spansSubgraphsDetermined(false),
	m_spansSubgraphs(false)
{

}

//-----------------------------------------------------------------------------

CAttEdgeRenderer::~CAttEdgeRenderer() 
{
	if(m_hotTracking.IsHot())
		m_hotTracking.SetHot(false);
}

//-----------------------------------------------------------------------------

CUniqueID CAttEdgeRenderer::GetID()
{
	return m_uniqueID;
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraphEdge> edge = item;

	if(m_edge)
		m_edge->Disconnect(m_sigConn);
	m_edge = edge;
	ATLASSERT(m_edge);
	m_callback = m_edge->GetPropertyUnknown(PROP_CALLBACK);
	m_sigConn=m_edge->Connect(boost::ref(*this));
	// must store ID since edge may be destroyed before its renderer
	m_uniqueID=m_edge->GetID();
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetFocus(bool bSet)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetSelected(bool bSelected, bool bAdd)
{
	bool oldSelected = m_bSelected;
	CScaledWindow::SetSelected(GetID(), bSelected, bAdd);

	if (bSelected != oldSelected)
	{
		m_canvas->NotifySelection(GetID());
	}

}

//-----------------------------------------------------------------------------

SizeF CAttEdgeRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	SizeF size;
	ATLASSERT(0); // not invoked for now
	return size;
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	RectF rcf;

	IGraphVertex* v_source=m_edge->GetSource();
	ATLASSERT(v_source);
	IGraphVertex* v_target=m_edge->GetTarget();
	ATLASSERT(v_target);
	CComQIPtr<IAttVertexRenderer> vr_source = m_canvas->GetRenderer(v_source);
	ATLASSERT(vr_source);
	CComQIPtr<IAttVertexRenderer> vr_target = m_canvas->GetRenderer(v_target);
	ATLASSERT(vr_target);
	RectF rect_source, rect_target;
	vr_source->GetBoundsBox(rect_source, co);
	vr_target->GetBoundsBox(rect_target, co);
	RectF::Union(box, rect_source, rect_target);
}


//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetBoundsBox(const RectF& box, unsigned flags, Coordinates co)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::ClearCaches()
{
	m_sizeTracker.Reset();
	m_textBmpNormal.Clear();
	m_textBmpSelected.Clear();
}

//-----------------------------------------------------------------------------

bool CAttEdgeRenderer::GetConnectionPoints(
	PointF& src, PointF& tgt,
	Coordinates co/*=screen*/,
	PointF* srcoff/*=0*/, PointF* tgtoff/*=0*/)
{
	bool bOk=false;

	if(m_vp.size()>1)
	{
		src=m_vp[0];
		tgt=m_vp[m_vp.size()-1];

		if(co==screen)
		{
			m_canvas->AllTransform(src);
			m_canvas->AllTransform(tgt);
		}

		bOk=true;
	}
	return bOk;
}

//-----------------------------------------------------------------------------

unsigned CAttEdgeRenderer::GetPoints(PointFVector& vp, Coordinates co/*=world*/)
{
	vp.reserve(m_vp.size());
	vp = m_vp;

	if(co==screen)
	{
		PointFVector::iterator itr=vp.begin();
		while(itr!=vp.end())
		{
			m_canvas->AllTransform(*itr);
			++itr;
		}
	}

	return m_vp.size();
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetPoints(const PointFVector& vp, Coordinates co/*=world*/)
{
	m_vp.clear();
	m_vp.reserve(vp.size());
	m_vp = vp;

	if(co==screen)
	{
		PointFVector::iterator itr=m_vp.begin();
		while(itr!=m_vp.end())
		{
			m_canvas->RAllTransform(*itr);
			++itr;
		}
	}

	m_svp=m_vp;
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::GetLabelPos(RectF& rcpos, Coordinates co/*=screen*/)
{
	SizeF size;

	if(co==screen)
	{
		if(m_canvas->IsPrinting())
		{
			size=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];
		}
		else
		{
			if(m_hotTracking.IsHot() && m_canvas->GetScale()<1)
				size=m_sizeTracker.m_extent;
			else
				size=m_sizeTracker.m_scaledExtent;
		}
	}
	else
	{
		size=m_sizeTracker.m_scaledExtent;
	}

	if (m_position.x == 0 && m_position.y == 0 && m_vp.size())	//  Splines calculated prior to label being assigned.
	{
		if (m_vp.size() % 2 == 1)
		{
			m_position = m_vp[m_vp.size() / 2];
		}
		else
		{
			GraphTypes::PointF pos1 = m_vp[(int)(m_vp.size() / 2) - 1];
			GraphTypes::PointF pos2 = m_vp[(int)(m_vp.size() / 2)];
			m_position.x = (pos1.x + pos2.x) / 2;
			m_position.y = (pos1.y + pos2.y) / 2;
		}
		SizeF sizeGlobal = size;
		if(co==screen)
			m_canvas->RSizeTransform(sizeGlobal);
		m_position.x += LABEL_BUFFER + sizeGlobal.Width / 2.0f;
		m_position.y += sizeGlobal.Height / 2.0f;
	}

	PointF pos=m_position;
	if(co==screen)
		m_canvas->AllTransform(pos);

	RectF rcf(pos, size);
	//LabelPos is the center of the text so we need to offset...
	rcf.Offset(-rcf.Width / 2.0f, -rcf.Height / 2.0f);  

#ifdef __CENTER_LABEL__
	rcf.Offset(-rcf.Width/2,-rcf.Height/2);
#endif
	rcpos=rcf;
	m_rcWnd=rcf;
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::SetLabelPos(const RectF& rcpos, Coordinates co/*=screen*/)
{
	m_position.x = rcpos.x + LABEL_BUFFER;
	m_position.y = rcpos.y;

	if(co==screen)
		m_canvas->RAllTransform(m_position);
}

bool CAttEdgeRenderer::IsSelected()
{
	return CScaledWindow::IsSelected();
}
//-----------------------------------------------------------------------------

bool CAttEdgeRenderer::IntersectsViewport() const
{
	return CalcVisibility();	
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::operator()(GRAPHNOTIFICATION notification, IGraphItem * item)
{
	switch(notification)
	{
	case GRAPHNOTIFICATION_LABELCHANGED:
		ClearCaches();
		//  Do not clear the pos as it has been set by the layout engine.
		//m_position.x = 0.0f;
		//m_position.y = 0.0f;
		break;
	}
}

//-----------------------------------------------------------------------------

void CAttEdgeRenderer::DrawLine(WPARAM wParam, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor) 
{
#ifdef __CUSTOM_PROFILE__
	BEGIN_PROF(_T("Edge Draw"));
#endif

	CAggDC& dc=*(CAggDC*)wParam;
	IGraphVertex * vs;
	IGraphVertex * vt;
	IRenderer * rs, * rt;
	RectF rcs, rct;

	vs=m_edge->GetSource();
	vt=m_edge->GetTarget();

	rs=m_canvas->GetRenderer(vs);
	ATLASSERT(rs!=0);

	rs->GetBoundsBox(rcs);

	rt=m_canvas->GetRenderer(vt);
	ATLASSERT(rt!=0);
	rt->GetBoundsBox(rct);

	GraphTypes::PointF m_sp, m_tp; // points in screen coordinates
	m_sp.x=rcs.x+rcs.Width/2;
	m_sp.y=rcs.y+rcs.Height/2;

	m_tp.x=rct.x+rct.Width/2;
	m_tp.y=rct.y+rct.Height/2;

	if(!rcs.IntersectsWith(rct) && (m_canvas->IsPrinting() || CalcVisibility(m_sp, m_tp)))
	{
		Color color;
		if (m_canvas->m_overPathToFocus && m_canvas->m_overPathToFocus->ContainsStep(vs, vt))
		{
			color=m_canvas->m_meta.GetMetaEdgeColorShortest(m_edge->GetCategory());
		}
		else
		{
			color=m_edge->GetColor();
		}

		if(color.GetAlpha()==0)
			color=m_canvas->m_meta.GetMetaEdgeColorNorm(m_edge->GetCategory());

		CComQIPtr<ILegacyRenderer> lrs=rs;
		if(lrs!=0 && lrs->IsHot())
			color.LessBright(50);

		SizeF width;
		REAL maxWidth;

		width.Width=m_canvas->m_meta.GetMetaEdgeWidth(m_edge->GetCategory());
		m_canvas->SizeTransform(width);
		maxWidth=width.Width/2;

		PointF normalizedTo(m_tp.x - m_sp.x, m_tp.y - m_sp.y);

		// ATLTRACE("pTo.x=%f,pTo.y=%f,pFrom.x=%f,pFrom.y=%f\n\r",pTo.x,pTo.y,pFrom.x,pFrom.y);
		double len = sqrt(normalizedTo.x * normalizedTo.x + normalizedTo.y * normalizedTo.y);

		if(len>1)
		{
			if(maxWidth > 0.5)
			{
				float angle = atan(normalizedTo.y / normalizedTo.x);
				normalizedTo.x = maxWidth * cos(angle);
				normalizedTo.y = maxWidth * sin(angle);

				PointF newP;

				// rotate normalizedTo for 90 degrees
				newP.x=-normalizedTo.y;
				newP.y=normalizedTo.x;

				PointF points[4];
				points[0].x=m_sp.x+newP.x;
				points[0].y=m_sp.y+newP.y;
				points[1].x=m_sp.x-newP.x;
				points[1].y=m_sp.y-newP.y;

				points[2]=m_tp;
				REAL quarter=M_PI/4;
				REAL half=M_PI/2;

				points[3]=m_tp;

				if(angle<=quarter && angle>=(-quarter))
					points[3].y+=1;
				else if(angle>quarter && angle<=(half+quarter))
					points[3].x-=1;
				else if(angle>(half+quarter) && angle<(M_PI+quarter))
					points[3].y-=1;
				else
					points[3].x+=1;

				dc.SolidPolygon(points, color, sizeof(points)/sizeof(points[0]));
			}
			else
			{
				dc.Line(m_sp, m_tp, color);
			}

			DrawLabel(dc, edgeColor, fontColor, firstControlPointColor, lastControlPointColor);
		} // line length > 1
	}

#ifdef __CUSTOM_PROFILE__
	END_PROF();
	Profile.dumpprint(0);
	Profile.reset();
#endif

}

void CAttEdgeRenderer::DrawBezier(WPARAM wParam, const GraphTypes::Color & _edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor) 
{
	CAggDC& dc=*(CAggDC*)wParam;
	if(!m_spansSubgraphsDetermined)
	{
		m_spansSubgraphsDetermined=true;
		IGraphVertex* v=m_edge->GetSource();
		ATLASSERT(v);
		IGraphSubgraph* sgs=v->GetParent();
		ATLASSERT(sgs);
		v=m_edge->GetTarget();
		ATLASSERT(v);
		IGraphSubgraph* sgt=v->GetParent();
		ATLASSERT(sgt);
		m_spansSubgraphs=sgt!=sgs; // this works but maybe better compare IDs?
	}

	//if(!m_spansSubgraphs)
	//{
	//	// check if enclosing subgraph is minimized
	//	IGraphVertex* v=m_edge->GetSource();
	//	ATLASSERT(v);
	//	IGraphSubgraph* sgs=v->GetParent();
	//	ATLASSERT(sgs);
	//	CComQIPtr<IAttSubgraphRenderer> iar=m_canvas->GetRenderer(sgs);
	//	ATLASSERT(iar);
	//	if(iar->IsMinimized())
	//		return;
	//}

	GraphTypes::Color edgeColor = _edgeColor;
	if (m_hotTracking.IsHot())
	{
		if (edgeColor.IsDarker(GraphTypes::Color::Gray))
			edgeColor.MoreBright(80);
		else
			edgeColor.LessBright(80);
	}

	SizeF width(m_LineThickness,0);
	//width.Width=m_canvas->m_meta.GetMetaEdgeWidth(m_edge->GetCategory());

	m_canvas->SizeTransform(width);
	if(width.Width<m_MinThickness)
		width.Width=m_MinThickness;

#ifdef __SKIP_INVISIBLE__

	// assume that CalcVisibility was already invoked
	// to get the vector in screen coordinates
	CComVariant dotted;
	m_edge->GetProperty(PROP_EDGE_DOTTED, dotted);

	dc.Bezier(m_svp, edgeColor, width.Width, true, dotted.bVal != 0, m_canvas->GetScale(), firstControlPointColor, lastControlPointColor);
#else
	CComVariant dotted;
	m_edge->GetProperty(PROP_EDGE_DOTTED, dotted);

	PointFVector::iterator itr2=m_svp.begin();
	for(PointFVector::const_iterator itr1=m_vp.begin();
		itr1!=m_vp.end();
		++itr1,++itr2)
	{
		*itr2=*itr1;
		m_canvas->AllTransform(*itr2);
	}

	if (m_svp.size())
		dc.Bezier(
			m_svp, 
			color,
			width.Width,
			true,
			m_spansSubgraphs || (dotted.vt == VT_BOOL && dotted.bVal),
			m_canvas->GetScale());
#endif

	DrawLabel(dc, edgeColor, fontColor, firstControlPointColor, lastControlPointColor);
}

//-----------------------------------------------------------------------------
void CAttEdgeRenderer::Draw(WPARAM wParam, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor) 
{
	if (m_svp.size())
		DrawBezier(wParam, edgeColor, fontColor, firstControlPointColor, lastControlPointColor);
	else
		DrawLine(wParam, edgeColor, fontColor, firstControlPointColor, lastControlPointColor);
}
//-----------------------------------------------------------------------------

void CAttEdgeRenderer::DrawLabel(CAggDC& dc, const GraphTypes::Color & edgeColor, const GraphTypes::Color & fontColor, const GraphTypes::Color & firstControlPointColor, const GraphTypes::Color & lastControlPointColor)
{
	// If the flag has been set to skip edge label rendering, then get out
	if (!m_canvas->m_meta.IsEdgeLabelVisible())
		return;

	std::_tstring label = m_edge->GetLabel();
	unsigned len = label.length();

	//if(!len)
	//{
	//	m_canvas->m_meta.GetMetaEdgeLabel(m_edge->GetCategory(), label);
	//	len = label.length();
	//}

	if(len==0)
		return;

	FontInfo fi(
		&m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE-1),
		fontColor,
		m_canvas->m_fonts,
		(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72);

	CDrawVertexHelper tdh;
	tdh.m_text=label.c_str();
//	tdh.m_borderwidth=1.0;
	tdh.m_cbkgnd=GraphTypes::Color::Transparent;
	tdh.m_cborder=edgeColor;
//	tdh.m_margins=m_textMargins;
	tdh.m_printing=m_canvas->IsPrinting();
	tdh.m_scale=m_canvas->GetScale();

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
			if(m_sizeTracker.m_extent.Width==0)
			{
				tdh.m_scale=1;
				m_sizeTracker.m_extent=tdh.CalcSize(hdc, fi);
				tdh.m_scale=m_canvas->GetScale();
			}
		}

		m_textBmpNormal.Clear();
		m_textBmpSelected.Clear();
	}

	RectF rcf;
	GetLabelPos(rcf);
	CRect frameRect=static_cast<CRect>(rcf);

	if(frameRect.IsRectEmpty())
		return;

	if(m_canvas->IsPrinting())
	{
		tdh.Draw(dc, fi, rcf, DT_CENTER);
	}
	else
	{
		if(m_hotTracking.IsHot())
		{
			if(tdh.m_scale<1)
				tdh.m_scale=1;
			RectF rchot(
				REAL(frameRect.left), 
				REAL(frameRect.top),
				REAL(frameRect.Width()), 
				REAL(frameRect.Height()));

			tdh.Draw(dc, fi, rchot, DT_CENTER);
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
					tdh.Draw(*tmpaggdc, fi, rcf, DT_CENTER);
					m_textBmpSelected.EndPaint(tmpaggdc.get());
				}

				m_textBmpSelected.Draw(dc, frameRect,
					m_canvas->m_meta.GetEdgeLabelOpacity()?0:DT_TRANSPARENT);
			}
			else
			{
				if(m_textBmpNormal.IsEmpty())
				{
					CSize size=m_sizeTracker.m_scaledExtent;

					boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpNormal.BeginPaint(dc, size, m_canvas->m_fonts));
					RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
					tdh.Draw(*tmpaggdc, fi, rcf, DT_CENTER);
					m_textBmpNormal.EndPaint(tmpaggdc.get());
				}

				m_textBmpNormal.Draw(dc, frameRect,
					m_canvas->m_meta.GetEdgeLabelOpacity()?0:DT_TRANSPARENT);
			}
		}
	}
}

//-----------------------------------------------------------------------------

bool CAttEdgeRenderer::CalcVisibility(PointF p1, PointF p2)
{
#ifdef __SKIP_INVISIBLE__
	RectF vp;
	m_canvas->GetViewport(vp);
	return LineIntersectsRect(p1, p2, vp);
#endif
	return true;
}

//-----------------------------------------------------------------------------
//
// this is not precise for complex Beziers used here

bool CAttEdgeRenderer::CalcVisibility() const
{
#ifdef __SKIP_INVISIBLE__
	unsigned numpoints=m_vp.size();

	if(numpoints>1)
	{
		RectF vp;
		m_canvas->GetViewport(vp);

		ATLASSERT(m_vp.size()==m_svp.size());
		// cache points in screen coordinates for later draw
		for(unsigned int i = 0; i < m_vp.size(); ++i)	//Writing to iteratir _could_ invalidate it
		{
			m_svp[i]=m_vp[i];
			m_canvas->AllTransform(m_svp[i]);
		}
		//PointFVector::iterator itr2=m_svp.begin();
		//for(PointFVector::const_iterator itr1=m_vp.begin(); itr1!=m_vp.end(); ++itr1,++itr2)
		//{
		//	*itr2=*itr1;
		//	m_canvas->AllTransform(*itr2);
		//}

		for(unsigned j=0; j<(numpoints-1); ++j)
		{
			if(LineIntersectsRect(m_svp[j], m_svp[j+1], vp))
				return true;
		}
	}

	return false;
#endif
	return true;
}

//*****************************************************************************
//
// Message handlers
//

LRESULT CAttEdgeRenderer::OnPaint(
	UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//ATLASSERT(m_callback);
	if (m_callback && m_canvas->m_meta.IsVisible(m_edge))
	{
		Color color, fontColor, firstControlPointColor, lastControlPointColor;
		m_callback->GetColor(m_edge, color, fontColor, firstControlPointColor, lastControlPointColor);
		Draw(wParam, color, fontColor, firstControlPointColor, lastControlPointColor);	
	}
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// nobody can be hot when clicked
	m_canvas->ClearCurrentHot();
	SetSelected();
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnLButtonUp(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_canvas->GetApplicationZoomMode()==AZMSelection)
		m_hotTracking.SetHot(true);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_canvas->GetApplicationZoomMode()==AZMSelection)
		if(!(wParam&MK_LBUTTON)) // hot when mouse not pressed down
			m_hotTracking.SetHot(true);

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnMouseDblclk(
	UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnNcHitTest(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return HTCLIENT;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnSetCursor(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	SetCursor(m_canvas->m_HandArrowCursor);
	return 1;
}

//-----------------------------------------------------------------------------

LRESULT CAttEdgeRenderer::OnContextMenu(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
	LRESULT lRes=m_canvas->NotifyContextMenu(SCW_NOTIFYEDGECONTEXT, m_edge->GetID(), pt);
	bHandled=lRes!=0;
	return lRes;
}
//*****************************************************************************

IRenderer* CreateAttEdgeRenderer(CCanvas & parent)
{
	CAttEdgeRenderer * retVal = new CAttEdgeRenderer(parent);
	return retVal;
}
