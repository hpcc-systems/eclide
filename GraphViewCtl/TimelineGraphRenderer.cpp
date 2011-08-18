#include "StdAfx.h"

#include "Canvas.h"
#include "Geometry.h"
#include "ScaledWindows.h"
#include "ScrollSupport.h"
#include "AggDC.h"
#include "AggHitTest.h"

class CTimelineGraphRenderer : public ITimelineGraphRenderer, public CScaledWindow
#ifdef __OLD_SCROLLBARS__
	, public CScrollSupport
#endif
{
protected:
	IGraph* m_graph;

	CPoint	m_mouseCapturePos;
	CPoint	m_mouseCurrentPos;
	CRect	m_selectedRangeRect;

	GraphTypes::PointF m_dragPageStartPos;
	WTL::CPen m_gridPen;

	bool DispatchMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	bool HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(ITimelineGraphRenderer)
		IMPLEMENT_INTERFACE(IGraphRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CTimelineGraphRenderer(CCanvas& canvas, IGraph * graph);
	~CTimelineGraphRenderer();

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

	//IGraphRenderer  ---
	bool GetFocus() const;

	//ITimelineGraphRenderer  ---

protected:
	BEGIN_MSG_MAP(CTimelineGraphRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
#ifdef __OLD_SCROLLBARS__
		CHAIN_MSG_MAP(CScrollSupport)
#endif
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


using namespace GraphTypes;

//*****************************************************************************

CTimelineGraphRenderer::CTimelineGraphRenderer(CCanvas& canvas, IGraph * graph)
	: CScaledWindow(&canvas),
#ifdef __OLD_SCROLLBARS__
	CScrollSupport(&canvas, this),
#endif
	m_graph(graph),
	m_mouseCapturePos(-1,-1),
	m_selectedRangeRect(0,0,0,0)
{
}

//-----------------------------------------------------------------------------

CTimelineGraphRenderer::~CTimelineGraphRenderer()
{
}

//-----------------------------------------------------------------------------

CUniqueID CTimelineGraphRenderer::GetID()
{
	return m_graph->GetID();
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraph> graph = item;
	m_graph=graph;
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	CRect rc(m_rcWnd);
#ifdef __OLD_SCROLLBARS__
	ModifyWindowRect(rc);
#endif
	RectF rcf(rc);
	box=rc;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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
	CScaledWindow::SetWindowPos(rc, flags);
#ifdef __OLD_SCROLLBARS__
	SetScrollRect(rc);
#endif
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::SetFocus(bool bSet)
{
	CScaledWindow::SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::SetSelected(bool bSelected, bool bAdd)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CTimelineGraphRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce, 
	bool bIgnoreExtent)
{
	SizeF itemsize;
	return itemsize;	
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CTimelineGraphRenderer::ClearCaches()
{
}

//-----------------------------------------------------------------------------

bool CTimelineGraphRenderer::DispatchMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
#ifdef __OLD_SCROLLBARS__
	if(DispatchToScrollbars(uMsg, wParam, lParam, lResult))
		return true;
#endif

	PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 
	RectF bbox;

	bool bTarget=false;
	CComQIPtr<ITimelineEdgeRenderer> ier;
	CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

	for(CComPtr<IEdgeIterator> itr = m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
	{
		ier=m_canvas->GetRenderer(itr->Get());
		ATLASSERT(ier);

		ier->GetBoundsBox(bbox);
		if(bbox.Contains(p))
		{
			lResult=ier->SendMessage(uMsg, wParam, lParam);
			bTarget=true;
			break;
		}
	}

	return bTarget;
}

//-----------------------------------------------------------------------------

bool CTimelineGraphRenderer::HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bHandled=false;
	CScaledWindow* capture=m_canvas->GetCapture();

	if(capture)
	{		
		capture->SendMessage(uMsg, wParam, lParam); 
		bHandled=true;
	}
	
	return bHandled;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	CAggDC& dc=*(CAggDC*)wParam;
	CDCHandle dch(dc.m_hDC);

	m_canvas->SetDPI(
		GetDeviceCaps(dch, LOGPIXELSX), GetDeviceCaps(dc, LOGPIXELSY));

//	CalcDisplaySizes(dch, false, true);

	if(m_canvas->m_meta.IsGridVisible())
		m_canvas->DrawGrid(dch);

	CComQIPtr<ITimelineEdgeRenderer> ier;
	CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);
	RectF bbox, rcext;
	bool first=true;

	CComPtr<ITimelineEdgeRenderer> ierHot;

	for(CComPtr<IEdgeIterator> itr = m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
	{
		ier=m_canvas->GetRenderer(itr->Get());
		ATLASSERT(ier);

		if(ier!=0)
		{
			if(ier->IsHot())
			{
				ierHot=ier;
				continue;
			}

			ier->CalcDisplaySizes(dch, false, true);
			ier->SendMessage(WM_PAINT, wParam, lParam);

			ier->GetBoundsBox(bbox);

			if(!bbox.IsEmptyArea())
			{
				if(first)
				{
					rcext=bbox;
					first=false;
				}
				else
				{
					rcext.Union(rcext, rcext, bbox);
				}
			}
		}
	}

	// draw hot edge last

	if(ierHot)
	{
		ierHot->CalcDisplaySizes(dch, false, true);
		ierHot->SendMessage(WM_PAINT, wParam, lParam);

		ierHot->GetBoundsBox(bbox);

		if(!bbox.IsEmptyArea())
		{
			if(first)
			{
				rcext=bbox;
				first=false;
			}
			else
			{
				rcext.Union(rcext, rcext, bbox);
			}
		}
	}

	m_canvas->StoreGraphExtent(rcext);

	MOUSE_MODES mode = m_canvas->CalcMouseMode();
	if (mode==MOUSE_MODE_SELECTION && !m_selectedRangeRect.IsRectEmpty())
		dc.DrawFocusRect(&m_selectedRangeRect);

#ifdef __OLD_SCROLLBARS__
	// no scrollbars on printer DC
	int	dctech=dc.GetDeviceCaps(TECHNOLOGY);
	if(dctech==DT_RASDISPLAY)
	{
		UpdateScrollbars(static_cast<CRect>(rcext));
		PaintScrollbars(wParam);
	}
#endif

	return 0;
}

//-----------------------------------------------------------------------------
//
//	Message Handlers

LRESULT CTimelineGraphRenderer::OnKeyDown(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CScaledWindow* hooker=m_canvas->GetKbdHook();

	if(hooker)
	{
		ATLASSERT(hooker!=this);
		hooker->SendMessage(uMsg, wParam, lParam); 
	}

	CScaledWindow* focus=m_canvas->m_currentFocus;

	if(focus && focus!=this && focus!=hooker)
		focus->SendMessage(uMsg, wParam, lParam); 

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnLButtonDown(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;

	m_mouseCapturePos.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
	m_mouseCurrentPos = m_mouseCapturePos;

	bool bProcessed=HandleCapture(uMsg, wParam, lParam);

	if(!bProcessed)
	{
		if(!DispatchMessage(uMsg, wParam, lParam, lRet))
		{
			SetFocus();
			m_canvas->DeselectAll();
			// Notify that "nothing" has selection.
			m_canvas->NotifySelection(CUniqueID());
			m_canvas->SetCapture(this);

			m_dragPageStartPos=m_canvas->GetOffset();
		}
		else
		{
		}
	}

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnLButtonUp(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;
	CComQIPtr<IGraphVertex> overVertex;

	if(m_canvas->GetCapture()==this)
	{
		m_canvas->ReleaseCapture();
		m_mouseCurrentPos.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else
	{
		bool bProcessed=HandleCapture(uMsg, wParam, lParam);

		if(!bProcessed)
			lRet=DispatchMessage(uMsg, wParam, lParam, lRet);
	}

	m_selectedRangeRect.SetRectEmpty();
	m_mouseCapturePos.SetPoint(-1, -1);

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnMouseMove(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;

	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if(m_canvas->GetCapture()==this)
	{
		m_mouseCurrentPos.SetPoint(pt.x, pt.y);
		MOUSE_MODES mode=m_canvas->CalcMouseMode();

		if(mode==MOUSE_MODE_SELECTION) // rubber-banding
		{
			if (m_mouseCurrentPos.x < m_mouseCapturePos.x)
			{
				m_selectedRangeRect.left = m_mouseCurrentPos.x;
				m_selectedRangeRect.right = m_mouseCapturePos.x;
			}
			else
			{
				m_selectedRangeRect.left = m_mouseCapturePos.x;
				m_selectedRangeRect.right = m_mouseCurrentPos.x;
			}
			if (m_mouseCurrentPos.y < m_mouseCapturePos.y)
			{
				m_selectedRangeRect.top = m_mouseCurrentPos.y;
				m_selectedRangeRect.bottom = m_mouseCapturePos.y;
			}
			else
			{
				m_selectedRangeRect.top = m_mouseCapturePos.y;
				m_selectedRangeRect.bottom = m_mouseCurrentPos.y;
			}
			RectF r(m_selectedRangeRect);
			m_canvas->SelectItemsWithin(r);
		}
		else if (mode != MOUSE_MODE_DRAW_LINKS)
		{
			int deltaX = m_mouseCurrentPos.x - m_mouseCapturePos.x;
			int deltaY = m_mouseCurrentPos.y - m_mouseCapturePos.y;
			if (abs(deltaX) > 3 || abs(deltaY) > 3)
			{
				PointF pt=m_dragPageStartPos-PointF((LONG)deltaX, (LONG)deltaY);
				PointF oldoffset=m_canvas->GetOffset();
				m_canvas->SetOffset(pt);

				CPoint ptscroll(
					round_int(oldoffset.x-pt.x),
					round_int(oldoffset.y-pt.y));

				m_canvas->Scroll(ptscroll);
				//m_canvas->Invalidate();
			}
		}
	}
	else
	{
		bool bProcessed=HandleCapture(uMsg, wParam, lParam);

		if(!bProcessed)
		{
			if(!DispatchMessage(uMsg, wParam, lParam, lRet))
			{
				if(m_canvas->ClearCurrentHot())
					m_canvas->Invalidate();
			}
		}
	}

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnMouseDblclk(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;

	lRet=DispatchMessage(uMsg, wParam, lParam, lRet);

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnRButtonDown(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=0;
	
	if(!DispatchMessage(uMsg, wParam, lParam, lRes))
	{
		PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 
		CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);

//#define __RASTERIZER_HIT_TEST__
#ifdef __RASTERIZER_HIT_TEST__
		RectF vp;
		m_canvas->GetViewport(vp);
		CRect rc=static_cast<CRect>(vp);
		boost::scoped_ptr<CAggHitTest> agght(CAggHitTest::CreateAggHitTest(&rc));
#else
		PointF closest;
#endif

		RectF pos;
		CComQIPtr<IEdgeRenderer> ier;
		CComPtr<IGraphEdge> edge;

		for(CComPtr<IEdgeIterator> itr=m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
		{
			edge=itr->Get();
			ier=m_canvas->GetRenderer(edge);
			if(ier!=0)
			{
				// first hit test edge label then edge itself

				if (m_canvas->m_meta.IsEdgeLabelVisible())
				{
					ier->GetLabelPos(pos);
					if (pos.Contains(p))
					{
						lRes=reinterpret_cast<LRESULT>(edge.p);
						break;
					}
				}

				ier->GetBoundsBox(pos);
				if(pos.Contains(p))
				{
					PointFVector vp;
					ier->GetPoints(vp, screen);
					if(vp.size()>1)
					{
#ifndef __RASTERIZER_HIT_TEST__
						GenLine line(vp[0], vp[vp.size()-1]);
						line.ClosestPoint(p, closest);
						int dist=idist(int(p.x), int(p.y), int(closest.x), int(closest.y));
						// TODO: we can make this a meta value
						if(dist<5)
						{
							lRes=reinterpret_cast<LRESULT>(edge.p);
							break;
						}
#else
						SizeF width;
						REAL maxWidth;

						width.Width=m_canvas->m_meta.GetMetaEdgeWidth(edge->GetCategory());
						m_canvas->SizeTransform(width);
						maxWidth=width.Width/2;

						PointF normalizedTo(vp[1].x - vp[0].x, vp[1].y - vp[0].y);

						double len = sqrt(normalizedTo.x * normalizedTo.x + normalizedTo.y * normalizedTo.y);
						if(len>1)
						{
							if(maxWidth > 0.6)
							{
								float angle = atan(normalizedTo.y / normalizedTo.x);
								normalizedTo.x = maxWidth * cos(angle);
								normalizedTo.y = maxWidth * sin(angle);

								PointF newP;

								// rotate normalizedTo for 90 degrees
								newP.x=-normalizedTo.y;
								newP.y=normalizedTo.x;

								PointF points[4];
								points[0].x=vp[0].x+newP.x;
								points[0].y=vp[0].y+newP.y;
								points[1].x=vp[0].x-newP.x;
								points[1].y=vp[0].y-newP.y;

								points[2]=vp[1];
								REAL quarter=M_PI/4;
								REAL half=M_PI/2;

								points[3]=vp[1];

								if(angle<=quarter && angle>=(-quarter))
									points[3].y+=1;
								else if(angle>quarter && angle<=(half+quarter))
									points[3].x-=1;
								else if(angle>(half+quarter) && angle<(M_PI+quarter))
									points[3].y-=1;
								else
									points[3].x+=1;
	
								if(agght->SolidPolygonHitTest(points, 4, p, 3))
								{
									lRes=reinterpret_cast<LRESULT>(edge.p);
									break;
								}
							}
							else
							{
								if(agght->LineHitTest(vp[0], vp[1], p, 3))
								{
									lRes=reinterpret_cast<LRESULT>(edge.p);
									break;
								}
							}
						}
#endif
					}
				} 
			}
		}
	}

	return lRes;
}

//-----------------------------------------------------------------------------

bool CTimelineGraphRenderer::GetFocus() const
{
	return CScaledWindow::HasFocus();
}

//-----------------------------------------------------------------------------

LRESULT CTimelineGraphRenderer::OnTimer(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;
	CScaledWindow* target=m_canvas->GetTimerTarget(); 
	if(target)
		lRet=target->SendMessage(uMsg, wParam, lParam);
	return lRet;
}


//*****************************************************************************

IGraphRenderer *CreateTimelineGraphRenderer(CCanvas& canvas, IGraph* graph)
{
	CTimelineGraphRenderer * retVal = new CTimelineGraphRenderer(canvas, graph);
	return retVal;
}

