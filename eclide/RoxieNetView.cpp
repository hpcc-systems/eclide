// RoxieNetView.cpp : implementation of the CRoxieNetView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

using namespace boost;
using namespace Gdiplus;

#include "RoxieNetView.h"

void Execute(CRoxieNetView * self, const int cat, const CString & id, const PointF & defaultPos)
{
	CComInitialize com;

	HeaderT headerEdge;
	GridT dataEdge;
	HeaderT headerVertex;
	GridT dataVertex;
	self->m_mapper.ExecuteVertex(cat, id, headerEdge, dataEdge, headerVertex, dataVertex);

	boost::mutex::scoped_lock scoped_lock(self->m_mutex);
	for(unsigned int i = 0; i < dataEdge.size(); ++i)
	{
		vertex_descriptor from = self->conditionalAdd(_variant_t(dataEdge[i][EDGE_FROM_CAT]), CString(dataEdge[i][EDGE_FROM_ID]), defaultPos);
		vertex_descriptor to = self->conditionalAdd(_variant_t(dataEdge[i][EDGE_TO_CAT]), CString(dataEdge[i][EDGE_TO_ID]), defaultPos);
		std::pair<edge_descriptor, bool> newEdge = add_edge(from, to, self->m_graph);
		if (newEdge.second == true)
		{
			self->edge_cat[newEdge.first] = _variant_t(dataEdge[i][EDGE_CAT]);
			self->init(newEdge.first);
		}
	}

	self->CalculateShortestPaths(self->selNode, self->pathToSel);
}

void Execute2(CRoxieNetView * self)
{
	CComInitialize com;
	std::vector<int>cats;
	std::vector<CString>ids;

	unsigned i = 0;
	vertex_iterator n, end;

	for (boost::tie(n, end) = vertices(self->m_graph); n != end; ++n, ++i)
	{
		cats.push_back(self->vertex_cat[*n]);
		ids.push_back(self->vertex_id[*n].c_str());
	}

	HeaderT headerEdge;
	GridT dataEdge;
	HeaderT headerVertex;
	GridT dataVertex;
	self->m_mapper.ExecuteVertex(cats, ids, headerEdge, dataEdge, headerVertex, dataVertex);

	boost::mutex::scoped_lock scoped_lock(self->m_mutex);
	for(unsigned int i = 0; i < dataEdge.size(); ++i)
	{
		vertex_descriptor from = self->conditionalAdd(_variant_t(dataEdge[i][EDGE_FROM_CAT]), CString(dataEdge[i][EDGE_FROM_ID]), PointF(0, 0));
		vertex_descriptor to = self->conditionalAdd(_variant_t(dataEdge[i][EDGE_TO_CAT]), CString(dataEdge[i][EDGE_TO_ID]), PointF(0, 0));
		std::pair<edge_descriptor, bool> newEdge = add_edge(from, to, self->m_graph);
		if (newEdge.second == true)
		{
			self->edge_cat[newEdge.first] = _variant_t(dataEdge[i][EDGE_CAT]);
			self->init(newEdge.first);
		}
	}

	self->CalculateShortestPaths(self->selNode, self->pathToSel);
}

CRoxieNetView::CRoxieNetView() : 
	m_mapper(_Global[GLOBAL_SERVER_WORKUNIT], _Global[GLOBAL_CLUSTER], _Global[GLOBAL_QUEUE], "EdgarVisual.EdgarNamesRoot"),
	m_scale(0.80),
	m_rotate(0.0)
{
	typedef minstd_rand base_generator_type;
	base_generator_type generator;

	//::generate_random_graph(m_graph, 400, 700, generator);
	CGraphDataImpl<CRoxieNetView>::randomize();

	init();

	selNode = -1;
	overNode = -1;
	dragNode = -1;
}

BOOL CRoxieNetView::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	HWND hWndCtl = ::GetFocus();
	if(IsChild(hWndCtl))
	{
		// find a direct child of the dialog from the window that has focus
		while(::GetParent(hWndCtl) != m_hWnd)
			hWndCtl = ::GetParent(hWndCtl);

		// give control a chance to translate this message
		if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
			return TRUE;
	}

	return IsDialogMessage(pMsg);
}

void Test(CRoxieNetView * self)
{
	while(self->m_iTimerID)
	{
		if (self->ReCalc())
			self->Invalidate();
		Sleep(20);
	}
}

LRESULT CRoxieNetView::OnCreate(LPCREATESTRUCT lParam)
{
	SetMsgHandled(false);

	SetScrollSize(10000, 10000, false);
	SetScrollOffset(5000, 5000);

	m_iTimerID = 1;//SetTimer(1,20);

	m_mapper.GetMeta();
	threads.create_thread(boost::bind(&Execute, this, 1, "MICROSOFT", PointF(0, 0)));
	threads.create_thread(boost::bind(&Test, this));

	return 1;
}

void CRoxieNetView::OnDestroy()
{
	SetMsgHandled(false);

	//KillTimer(m_iTimerID);
	m_iTimerID = 0;

	threads.join_all();
}

void CRoxieNetView::OnTimer(UINT id, TIMERPROC timerProc)
{
	SetMsgHandled(true);
	if (id == m_iTimerID)
	{
		if (isModified())
		{
			isNewMouseOver();
			//Invalidate();
		}
	}
}

void CRoxieNetView::OnLButtonDown(UINT vKey, CPoint &pos)
{
	SetCapture(); 
	mouseCapturePos = pos;

	dragNode = getVertexAt(toWorld(pos));
	if (dragNode == -1)
		GetScrollOffset(dragPageStartPos);
	else
		dragNodeStartPos = PointF(getPoint(dragNode));

}

void CRoxieNetView::OnMouseMove(UINT vKey, CPoint &pos)
{
	mouseCurrentPos = pos;

	if (vKey & MK_LBUTTON) 
	{
		if (dragNode == -1)
			SetScrollOffset(dragPageStartPos.x - (mouseCurrentPos.x - mouseCapturePos.x), dragPageStartPos.y - (mouseCurrentPos.y - mouseCapturePos.y));
		else
			setPoint(dragNode, PointF(dragNodeStartPos.X + (mouseCurrentPos.x - mouseCapturePos.x) / m_scale, dragNodeStartPos.Y + (mouseCurrentPos.y - mouseCapturePos.y) / m_scale));
		Invalidate();
	}
	else
	{
		if (isNewMouseOver())
			Invalidate();
	}
}

void CRoxieNetView::OnLButtonUp(UINT vKey, CPoint &pos)
{
	if (dragNode != -1 && pos == mouseCapturePos && selNode != dragNode)
	{
		selNode = dragNode;
		CalculateShortestPaths(selNode, pathToSel);
		Invalidate();
	}
	dragNode = -1;
	ReleaseCapture(); 
}

void CRoxieNetView::OnLButtonDblClick(UINT vKey, CPoint &pos)
{
	vertex_descriptor v = getVertexAt(toWorld(pos));
	if (v != -1)
		execute(v);
	else
		execute();
}

void CRoxieNetView::OnMouseLeave()
{
	overNode = -1;
	mouseCurrentPos.SetPoint(-1, -1);
}

RectF transformRect(const RectF &r, const Graphics & g)
{
	Region region(r);
	RectF retVal;
	region.GetBounds(&retVal, &g);
	return retVal;
}

void CRoxieNetView::DoPaint(CDCHandle dc)
{
	Graphics g(dc);
	POINT scrollOffset;
	GetScrollOffset(scrollOffset);
	scrollOffset.x -= 5000;
	scrollOffset.y -= 5000;
	g.TranslateTransform((REAL)-scrollOffset.x, (REAL)-scrollOffset.y);
	g.ScaleTransform(m_scale, m_scale);
	g.RotateTransform(m_rotate);

	render(g, getLayout());
}

bool CRoxieNetView::isNewMouseOver()
{
	vertex_descriptor newOver = getVertexAt(toWorld(mouseCurrentPos));
	if (newOver != overNode)
	{
		overNode = newOver;
		if (selNode != -1 && overNode != -1)
			pathToSel.CalculateShortestRoute(overNode, overToSel);
		return true;
	}
	return false;
}

void CRoxieNetView::setScale(REAL pScale)
{
	m_scale = pScale;
	Invalidate();
}

int CRoxieNetView::ScrollWindowEx(int dx, int dy, UINT uFlags, LPCRECT lpRectScroll, LPCRECT lpRectClip, HRGN hRgnUpdate, LPRECT lpRectUpdate) throw()
{
	ATLASSERT(::IsWindow(m_hWnd));
	return 0;
}

PointF CRoxieNetView::toWorld(POINT p)
{
	PointF p2((REAL)p.x, (REAL)p.y);
	return toWorld(p2);
}

PointF CRoxieNetView::toWorld(RectF p)
{
	PointF p2(p.X, p.Y);
	return toWorld(p2);
}

PointF CRoxieNetView::toWorld(PointF p)
{
	POINT scrollOffset;
	GetScrollOffset(scrollOffset);
	scrollOffset.x -= 5000;
	scrollOffset.y -= 5000;

	Graphics g(m_hWnd);
	g.TranslateTransform((REAL)-scrollOffset.x, (REAL)-scrollOffset.y);
	g.ScaleTransform(m_scale, m_scale);

	PointF points[1] = {p};
	g.TransformPoints(CoordinateSpaceWorld, CoordinateSpaceDevice, points, 1);
	return points[0];
}

void CRoxieNetView::execute(vertex_descriptor v)
{
	threads.create_thread(boost::bind(&Execute, this, vertex_cat[v], vertex_id[v].c_str(), getPoint(v)));
}

void CRoxieNetView::execute()
{
	threads.create_thread(boost::bind(&Execute2, this));
}

vertex_descriptor CRoxieNetView::conditionalAdd(const int pCat, const CString & pId, const PointF & defaultPos)
{
	vertex_descriptor retVal;
	VertexID id(pCat, pId);
	if (m_lookup.find(id) == m_lookup.end())
	{
		retVal = add_vertex(m_graph);
		vertex_cat[retVal] = pCat;
		vertex_id[retVal] = pId;
		vertex_label[retVal] = pId;
		init(retVal, defaultPos);

		m_lookup[id] = retVal;
	}
	else
		retVal = m_lookup[id];
	return retVal;
}

