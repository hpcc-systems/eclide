#include "StdAfx.h"

#include <algorithm>

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "Geometry.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "SizeTracker.h"

//#define __CUSTOM_PROFILE__
#ifdef __CUSTOM_PROFILE__
#define PROFILE	&Profile
#include "HWProf.h"
CHWProfile Profile;
#endif

#define __SKIP_INVISIBLE__

class CLegacyEdgeRenderer : public ILegacyEdgeRenderer,	public CScaledWindow, public boost::signals::trackable
{
	IGraphEdge * m_edge; //Should not be a CComPtr as this would create a circular reference.
	CComPtr<IImageRenderer> m_icon;
	GraphTypes::PointF m_sp, m_tp; // points in screen coordinates
	boost::signals::connection m_sigConn;

	CAggBitmap m_textBmp;
	CSizeTracker m_sizeTracker;

	void Draw(WPARAM wParam);
	void DrawLabel(CAggDC& dc);
	bool CalcVisibility(GraphTypes::PointF p1, GraphTypes::PointF p2);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(ILegacyEdgeRenderer)
		IMPLEMENT_INTERFACE(IEdgeRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CLegacyEdgeRenderer(CCanvas & canvas); 
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
	void GetLabelPos(GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
	void SetLabelPos(const GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
	bool IsSelected()
	{
		return CScaledWindow::IsSelected();
	}

protected:
	BEGIN_MSG_MAP(CLegacyEdgeRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(
		UINT /*uMsg*/,
		WPARAM /*wParam*/,
		LPARAM /*lParam*/,
		BOOL& /*bHandled*/);
};

//*****************************************************************************

using namespace GraphTypes;

CLegacyEdgeRenderer::CLegacyEdgeRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_edge(0)
{
}

//-----------------------------------------------------------------------------

CUniqueID CLegacyEdgeRenderer::GetID()
{
	return m_edge->GetID();
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraphEdge> edge = item;

	if(m_edge)
		m_edge->Disconnect(m_sigConn);
	m_edge = edge;
	ATLASSERT(m_edge);
	m_sigConn=m_edge->Connect(boost::ref(*this));

	IRenderer* ir=CreateImageRenderer(*m_canvas, item);
	m_icon=static_cast<IImageRenderer*>(ir);
}

//-----------------------------------------------------------------------------

LRESULT CLegacyEdgeRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetFocus(bool bSet)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetSelected(bool bSelected, bool bAdd)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CLegacyEdgeRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	SizeF size;
	// not invoked for now
	return size;
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	RectF rcf;
	CRect rc;
		
	rc.left=int(m_sp.x);
	rc.top=int(m_sp.y);
	rc.right=int(m_tp.x);
	rc.bottom=int(m_tp.y);
	rc.NormalizeRect();
	rcf=rc;

	box=rcf;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
	// TODO discover use for this
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing intelligent to do for now
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::ClearCaches()
{
	m_sizeTracker.Reset();
	m_textBmp.Clear();
}

//-----------------------------------------------------------------------------

bool CLegacyEdgeRenderer::GetConnectionPoints(
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

unsigned CLegacyEdgeRenderer::GetPoints(PointFVector& vp, Coordinates co/*=world*/)
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

void CLegacyEdgeRenderer::SetPoints(const PointFVector& vp, Coordinates co/*=world*/)
{
//	ATLASSERT(vp.size()==2);
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

void CLegacyEdgeRenderer::GetLabelPos(RectF& pos, Coordinates co/*=screen*/)
{
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


	if(co==world)
	{
		m_canvas->RAllTransform(rcLabel);
	}

	pos=rcLabel;
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::SetLabelPos(const RectF& pos, Coordinates co/*=screen*/)
{
	// some layouts set label position
	RectF rcLabel=pos;
	if(co==world)
	{
		m_canvas->AllTransform(rcLabel);
	}
}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::operator()(
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

void CLegacyEdgeRenderer::Draw(WPARAM wParam) 
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

			DrawLabel(dc);
			m_icon->SendMessage(WM_PAINT, wParam, 1 /* always scale*/);
		} // line length > 1
	}

#ifdef __CUSTOM_PROFILE__
	END_PROF();
	Profile.dumpprint(0);
	Profile.reset();
#endif

}

//-----------------------------------------------------------------------------

void CLegacyEdgeRenderer::DrawLabel(CAggDC& dc)
{
	if (!m_canvas->m_meta.IsEdgeLabelVisible())
		return;

	CSize szExtent;
	std::_tstring label = m_edge->GetLabel();
	unsigned len = label.length();
	if(len==0)
	{
		m_canvas->m_meta.GetMetaEdgeLabel(m_edge->GetCategory(), label);
		len = label.length();
	}

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
bool CLegacyEdgeRenderer::CalcVisibility(PointF p1, PointF p2)
{
#ifdef __SKIP_INVISIBLE__
	RectF vp;
	m_canvas->GetViewport(vp);
	return LineIntersectsRect(p1, p2, vp);
#endif
	return true;
}

//-----------------------------------------------------------------------------

LRESULT CLegacyEdgeRenderer::OnPaint(
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

//*****************************************************************************

IRenderer* CreateLegacyEdgeRenderer(CCanvas & parent)
{
	CLegacyEdgeRenderer * retVal = new CLegacyEdgeRenderer(parent);
	return retVal;
}
