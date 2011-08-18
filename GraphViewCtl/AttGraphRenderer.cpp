#include "StdAfx.h"

#include "Canvas.h"
#include "Geometry.h"
#include "ScaledWindows.h"
#include "ScrollSupport.h"
#include "AggDC.h"
#include "AggHitTest.h"
#include "XMLWriter.h"
#include "Notifications.h"

class CAttGraphRenderer : public IGraphRenderer, public CScaledWindow, public CScrollSupport
{
protected:
	IGraph* m_graph;

	CPoint	m_mouseCapturePos;
	CPoint	m_mouseCurrentPos;
	GraphTypes::PointF m_dragPageStartPos;
	WTL::CPen m_gridPen;
	CAggHitTest* m_agght;
	CComPtr<IRenderer> m_lastDispatch; 

	bool DispatchMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	bool HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IGraphRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CAttGraphRenderer(CCanvas& canvas, IGraph * graph);
	~CAttGraphRenderer();

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

	//IGraphRenderer
	bool GetFocus() const;

protected:
	BEGIN_MSG_MAP(CAttGraphRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		CHAIN_MSG_MAP(CScrollSupport)
	END_MSG_MAP()

	IAttVertexRenderer * RecursivePaint(CAggDC & dc, IGraphSubgraph * parent, GraphTypes::RectF & rcext, GraphTypes::RectF & bbox, bool & first, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

using namespace GraphTypes;

//*****************************************************************************

CAttGraphRenderer::CAttGraphRenderer(CCanvas& canvas, IGraph* graph)
	: CScaledWindow(&canvas), 
	CScrollSupport(&canvas, this),
	m_graph(graph),
	m_agght(CAggHitTest::CreateAggHitTest()),
	m_mouseCapturePos(-1,-1)
{
}

//-----------------------------------------------------------------------------

CAttGraphRenderer::~CAttGraphRenderer()
{
	delete m_agght;
}

//-----------------------------------------------------------------------------

CUniqueID CAttGraphRenderer::GetID()
{
	return m_graph->GetID();
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraph> graph = item;
	m_graph=graph;
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	CRect rc(m_rcWnd);
	if(m_canvas->m_meta.IsEmulateDecoration())
		ModifyWindowRect(rc);
	RectF rcf(rc);
	box=rc;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
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
	if(m_canvas->m_meta.IsEmulateDecoration())
		SetScrollRect(rc);
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::SetFocus(bool bSet)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::SetSelected(bool bSelected, bool bAdd)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CAttGraphRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce, 
	bool bIgnoreExtent)
{
	SizeF size;

	return size;	
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CAttGraphRenderer::ClearCaches()
{

}

//-----------------------------------------------------------------------------

bool CAttGraphRenderer::DispatchMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	if(m_canvas->m_meta.IsEmulateDecoration())
		if(DispatchToScrollbars(uMsg, wParam, lParam, lResult))
			return true;

	PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 
	// WM_NCHITTEST comes in screen coordinates, we prefer client
	if(uMsg==WM_NCHITTEST)
	{
		CPoint pt=p;
		m_canvas->ClientCursorPos(&pt);
		p=pt;
	}

	bool bTarget=false; 
	RectF bbox;
	CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);
	for(CComPtr<IGraphItemIterator> itr = m_graph->GetRChildren(skipper); itr->IsValidItem(); itr->NextItem())
	{
		CComQIPtr<IAttSubgraphRenderer> ir;
		CComQIPtr<IAttVertexRenderer> ivr;
		ir=m_canvas->GetRenderer(itr->Get());
		if(ir==0)
			ivr=m_canvas->GetRenderer(itr->Get());
		ATLASSERT(ir || ivr);

		if(ir)
		{
			ir->GetBoundsBox(bbox);

			if(bbox.Contains(p))
			{
				lResult=ir->HitTestDispatchMsg(p, bTarget, uMsg, wParam, lParam);
				if(uMsg==WM_NCHITTEST && lResult && lResult!=HTCLIENT)
				{
					m_lastDispatch=reinterpret_cast<IRenderer*>(lResult);
					lResult=HTCLIENT;
				}
				break;
			}
		}

		if(ivr)
		{
			ivr->GetBoundsBox(bbox);
			if(bbox.Contains(p))
			{
				bTarget=true;
				lResult=ivr->SendMessage(uMsg, wParam, lParam);
				if(uMsg==WM_NCHITTEST && lResult!=HTCLIENT)
				{
					m_lastDispatch=reinterpret_cast<IRenderer*>(ivr.p);
					lResult=HTCLIENT;
				}
				break;
			}
		}

	}

	// don't do edge hit-testing (SLOW!) when zooming
	if(m_canvas->GetApplicationZoomMode()==AZMSelection && !bTarget)
	{
		RectF vport;
		m_canvas->GetViewport(vport);
		CRect rcvp=vport;
		m_agght->SetRect(rcvp);

		IGraphEdge* edge;
		IAttEdgeRenderer* ier;
		RectF labelpos;
		PointFVector vp;

		SizeF width(4,0);
		//width.Width=m_canvas->m_meta.GetMetaEdgeWidth(edge->GetCategory());
		m_canvas->SizeTransform(width);

		for(CComPtr<IEdgeIterator> itr = m_graph->GetUserEdgeIterator(skipper); itr->IsValidItem(); itr->NextItem())
		{
			edge=itr->Get();
			ier=reinterpret_cast<IAttEdgeRenderer*>(m_canvas->GetRenderer(edge));
			if(ier!=0)
			{
				ier->GetLabelPos(labelpos);
				if(labelpos.Contains(p))
				{
					lResult=ier->SendMessage(uMsg, wParam, lParam);
					if(uMsg==WM_NCHITTEST)
						m_lastDispatch=ier;
					bTarget=true;
					break;
				}

				int numpoints=ier->GetPoints(vp, screen);
				if(numpoints>0 && ier->IntersectsViewport())
				{
					// Agg's memory manager gets stretched otherwise
					m_agght->ClearCaches();

					if(m_agght->BezierHitTest(vp, p, width.Width, 1, true, m_canvas->GetScale()))
					{
						lResult=ier->SendMessage(uMsg, wParam, lParam);
						bTarget=true;
						if(uMsg==WM_NCHITTEST)
							m_lastDispatch=ier;
						break;
					}
				}
			}
		} // for edge loop
	}

	if(!bTarget && uMsg==WM_NCHITTEST)
		m_lastDispatch=0;

	return bTarget; 
}

//-----------------------------------------------------------------------------

bool CAttGraphRenderer::HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bHandled=false;
	CScaledWindow* capture=m_canvas->GetCapture();

	if(capture)
	{		
		if(capture!=this)
			capture->SendMessage(uMsg, wParam, lParam); 
		bHandled=true;
	}

	return bHandled;
}

//-----------------------------------------------------------------------------
IAttVertexRenderer * CAttGraphRenderer::RecursivePaint(CAggDC & dc, IGraphSubgraph * parent, RectF & rcext, RectF & bbox, bool & first, WPARAM wParam, LPARAM lParam)
{
	IAttVertexRenderer * hotvertex = NULL;
	CDCHandle dch(dc.m_hDC);
	CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_canvas->m_meta);
	for(CComPtr<IGraphItemIterator> itr = parent->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
	{
		if(CComQIPtr<IGraphSubgraph> sg=itr->Get())
		{
			CComQIPtr<IAttSubgraphRenderer> ir = m_canvas->GetRenderer(sg);
			ATLASSERT(sg);
			ir->CalcDisplaySizes(dch, false, true);
			ir->GetBoundsBox(bbox);

			CRect rcsg(bbox);
			if(!m_canvas->m_meta.IsEmulateDecoration() || dc.RectVisible(&rcsg))
			{
				ir->SendMessage(WM_PAINT, wParam, lParam);

				if(!ir->IsMinimized())
				{
					hotvertex = RecursivePaint(dc, sg, rcext, bbox, first, wParam, lParam);
					//IAttVertexRenderer* ivr;

					//for(CComPtr<IGraphItemIterator> itrc=sg->GetChildren(); itrc->IsValidItem(); itrc->NextItem())
					//{
					//	ivr=reinterpret_cast<IAttVertexRenderer*>(m_canvas->GetRenderer(itrc->Get()));
					//	if(ivr->IsHot())
					//		hotvertex=ivr;
					//	else
					//		ivr->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
					//}
				}
			}

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
		else if(CComQIPtr<IGraphVertex> v = itr->Get())
		{
			CComQIPtr<IAttVertexRenderer> ivr = m_canvas->GetRenderer(v);
			ATLASSERT(ivr);
			ivr->CalcDisplaySizes(dch, false, true);
			ivr->GetBoundsBox(bbox);

			CRect rcsg(bbox);
			if(!m_canvas->m_meta.IsEmulateDecoration() || dc.RectVisible(&rcsg))
				ivr->SendMessage(WM_PAINT, wParam, lParam);

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
	return hotvertex;
}

LRESULT CAttGraphRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	CAggDC& dc=*(CAggDC*)wParam;
	CDCHandle dch(dc.m_hDC);
	
	m_canvas->SetDPI(
		GetDeviceCaps(dch, LOGPIXELSX), GetDeviceCaps(dc, LOGPIXELSY));

	if(m_canvas->m_meta.IsGridVisible())
		m_canvas->DrawGrid(dch);

	RectF rcext, bbox;
	bool first=true;

	IAttVertexRenderer* hotvertex = RecursivePaint(dc, m_graph, rcext, bbox, first, wParam, lParam);

	m_canvas->StoreGraphExtent(rcext);

	for(CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator(); itr->IsValidItem(); itr->NextItem())
	{
		if(CComQIPtr<IAttEdgeRenderer> ier = m_canvas->GetRenderer(itr->Get()))
		{
			if(!m_canvas->m_meta.IsEmulateDecoration() || ier->IntersectsViewport())
				ier->SendMessage(WM_PAINT, wParam, lParam);
		}
	}

	if(hotvertex)
		hotvertex->SendMessage(WM_PAINT, (WPARAM)&dc, 0);			

	if(m_canvas->m_meta.IsEmulateDecoration())
	{
		int	dctech=dc.GetDeviceCaps(TECHNOLOGY);
		// no scrollbars on printer DC
		if(dctech==DT_RASDISPLAY)
		{
			UpdateScrollbars(static_cast<CRect>(rcext));
			PaintScrollbars(wParam);
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
//	Message Handlers

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnLButtonDown(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));	
	m_mouseCapturePos.SetPoint(pt.x, pt.y); 
	m_mouseCurrentPos = m_mouseCapturePos;

	bool bProcessed=HandleCapture(uMsg, wParam, lParam);

	if(!bProcessed)
	{
		bool sbhit=m_canvas->m_meta.IsEmulateDecoration();
		if(sbhit)
			sbhit=DispatchToScrollbars(uMsg, wParam, lParam, lRet);

		if(!sbhit && !DispatchMessage(uMsg, wParam, lParam, lRet))
		{
			m_canvas->DeselectAll();

			// this notification may cause zooming
			m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);
			m_canvas->SetCapture(this);
			m_dragPageStartPos=m_canvas->GetOffset();
		}
	}
	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnLButtonUp(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;

	if(m_canvas->GetCapture()==this)
	{
		m_canvas->ReleaseCapture();
		m_mouseCurrentPos.SetPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else
	{
		bool bProcessed=HandleCapture(uMsg, wParam, lParam);

		if(!bProcessed)
			DispatchMessage(uMsg, wParam, lParam, lRet);
	}

	m_mouseCapturePos.SetPoint(-1, -1);

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnMouseMove(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;

	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if(m_canvas->GetCapture()==this)
	{
		m_mouseCurrentPos.SetPoint(pt.x, pt.y);

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
	else
	{
		bool bProcessed=HandleCapture(uMsg, wParam, lParam);

		if(!bProcessed)
		{
			if(!DispatchMessage(uMsg, wParam, lParam, lRet))
			{
				m_canvas->ClearCurrentHot();
			}
		}
	}

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnMouseDblclk(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=0;

	DispatchMessage(uMsg, wParam, lParam, lRet);

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnNcHitTest(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet=HTCLIENT;

	bool bProcessed=HandleCapture(uMsg, wParam, lParam);

	if(!bProcessed)
		DispatchMessage(uMsg, wParam, lParam, lRet);

	return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnSetCursor(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(HIWORD(lParam)==WM_LBUTTONDOWN)
	{
		CComQIPtr<IAttSubgraphRenderer> isg=m_lastDispatch;
		if(isg!=0)
			SetCursor(LoadCursor(0, IDC_ARROW));
		else
			SetCursor(m_canvas->m_HandClosedCursor);
	}
	else if(m_lastDispatch!=0 && m_canvas->GetApplicationZoomMode()==AZMSelection)
	{
		m_lastDispatch->SendMessage(uMsg, wParam, lParam);
	}
	else
	{
		m_canvas->SetZoomModeCursor();
	}
	return 1;
}

//----------------------------------------------------------------------------

LRESULT CAttGraphRenderer::OnContextMenu(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=0;
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
	m_canvas->ClientCursorPos(&pt);

	if (!DispatchMessage(uMsg, wParam, MAKELPARAM(pt.x,pt.y), lRes))
	{
		lRes=m_canvas->NotifyContextMenu(SCW_NOTIFYGRAPHCONTEXT, m_graph->GetID(), pt);
	}

	return lRes;
}

//----------------------------------------------------------------------------

bool CAttGraphRenderer::GetFocus() const
{
	return CScaledWindow::HasFocus();
}

//*****************************************************************************

IGraphRenderer *CreateAttGraphRenderer(CCanvas& canvas, IGraph* graph)
{
	CAttGraphRenderer * retVal = new CAttGraphRenderer(canvas, graph);
	return retVal;
}

