#include "stdafx.h"
#include "resource.h"

#include "gdi.h"
#include "gdiimpl.h"

using namespace GraphTypes;
using namespace boost;


CGraphFastRenderImpl::CGraphFastRenderImpl(IGraph * graph, CMeta & meta) : CRender(graph, meta)
{
	m_graphRender=CreateRenderer(*this, graph);
}

//-----------------------------------------------------------------------------

void CGraphFastRenderImpl::append(IGraphSubgraph * g, const GraphTypes::RectF & r)
{
	CComPtr<IRenderer> tmp = m_graphRender->CreateRenderer(g);
}

//-----------------------------------------------------------------------------

void CGraphFastRenderImpl::append(const CUniqueID & v, const GraphTypes::RectF & r)
{
	CComPtr<IRenderer> tmp = m_graphRender->CreateRenderer(m_graph->GetVertex(v));
}

//-----------------------------------------------------------------------------

LRESULT CGraphFastRenderImpl::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	return m_graphRender->SendMessage(uMsg, wParam, lParam); 
} 

//-----------------------------------------------------------------------------

void CGraphFastRenderImpl::SetTopParent(HWND hwnd)
{
	m_graphRender->SetTopParent(hwnd);
}

//*****************************************************************************

IRender * CreateFastRenderer(IGraph * graph, CMeta & meta)
{
	return new CGraphFastRenderImpl(graph, meta);
}

//*****************************************************************************

/*
#define M_PI 3.14159265358979323846f
#define ToRadian( degree ) ((degree) * (M_PI / 180.0f)) 
#define ToDegree( radian ) ((radian) * (180.0f / M_PI))  

inline REAL CGraphFastRenderImpl::CalcLineLength(
	const PointF& first,
	const PointF& second )
{
	REAL length;
	REAL xpart, ypart;

	xpart=first.x-second.x;
	ypart=first.y-second.y;

	length=sqrt(xpart*xpart+ypart*ypart);
	return length;
}

REAL CGraphFastRenderImpl::CalcLineLength(
	const PointFVector& line, REALVector& seglen )
{
	REAL length=0.0, segment;
	PointFVector::const_iterator i;
	PointF first, second;

	seglen.clear();

	i=line.begin();
	first=*i;
	++i;

	while( i!=line.end() )
	{
		second=*i;
		segment=CalcLineLength(first,second);
		seglen.push_back(segment);
		length+=segment;
		first=second;
		++i;
	}

	return length;
}

bool CGraphFastRenderImpl::CalcArrowWidths(
	REAL startwidth,
	REALVector& vwidths,
	const PointFVector line )
{
	int count=int(line.size());

	if ( count<3 )
		return false;

	REALVector seglengths;	
	REAL total, curlen, width;
	REALVector::const_reverse_iterator rstart;
		
	total=CalcLineLength(line, seglengths);
	curlen=0.0;	

	vwidths.clear();
	rstart=seglengths.rbegin();

	while( --count>1 )
	{
		curlen+=*rstart;
		width=startwidth*curlen/total;
		vwidths.insert(vwidths.begin(),width); // decreasing width order
		++rstart;
	}

	return true;
}

void CGraphFastRenderImpl::paintArrow(CDCHandle &dc, IGraphEdge * e, PointF pFrom, PointF pTo, const Color & c, const Color &fontColor) 
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, c.ToCOLORREF());
	CAutoPen autoPen(pen, dc);
	
	CBrush brush;
	brush.CreateSolidBrush(c.ToCOLORREF());
	CAutoBrush autoBrush(brush, dc);

	int maxWidth = WidthTransform(m_meta.GetMetaEdgeWidth(e->GetCategory()));

	PointF normilizedTo(pTo.x - pFrom.x, pTo.y - pFrom.y);
	float len = sqrt(normilizedTo.x * normilizedTo.x + normilizedTo.y * normilizedTo.y);
	if(len > 1)
	{
		PointF midPoint(pFrom.x + normilizedTo.x * 1 / 2, pFrom.y + normilizedTo.y * 1 / 2);
		PointF endPoint(pFrom.x + normilizedTo.x, pFrom.y + normilizedTo.y);

		float angle = atan(normilizedTo.y / normilizedTo.x);
		normilizedTo.x = maxWidth * cos(angle);
		normilizedTo.y = maxWidth * sin(angle);

		float radians = ToRadian(90);
		PointF newP;
		newP.x = normilizedTo.x * cos(radians) - normilizedTo.y * sin(radians);
		newP.y = normilizedTo.x * sin(radians) + normilizedTo.y * cos(radians);

		CPoint pnts[3];
		pnts[0] = CPoint(static_cast<int>(endPoint.x), static_cast<int>(endPoint.y));
		pnts[1] = CPoint(static_cast<int>(pFrom.x + newP.x), static_cast<int>(pFrom.y + newP.y));
		pnts[2] = CPoint(static_cast<int>(pFrom.x - newP.x), static_cast<int>(pFrom.y - newP.y));

		dc.Polygon(pnts, 3);

		/*
		dc.SetTextColor(fontColor.ToCOLORREF());

		CString txt;
		getText(e, txt);
		CRect textRect = calcTextBounds(dc, e);
		int w = textRect.Width();
		int h = textRect.Height();
		textRect.left = static_cast<LONG>(midPoint.x - w / 2);
		textRect.right = static_cast<LONG>(midPoint.x + w / 2);
		textRect.top = static_cast<LONG>(midPoint.y - h / 2);
		textRect.bottom = static_cast<LONG>(midPoint.y + h / 2);

		//dc.SetBkMode(TRANSPARENT);
		dc.DrawText(txt, txt.GetLength(), &textRect, DT_NOPREFIX | DT_CENTER);
	}

	int maxWidth = WidthTransform(m_meta.GetMetaEdgeWidth(m_graph->getEdgeCat(e)));

	dc.MoveTo((int)pFrom.x, (int)pFrom.y);
	dc.LineTo(static_cast<int>(pTo.x), static_cast<int>(pTo.y));

	for (float i = 1; i < maxWidth; i += 0.5)
	{
		PointF normilizedTo(pTo.x - pFrom.x, pTo.y - pFrom.y);
		float len = sqrt(normilizedTo.x * normilizedTo.x + normilizedTo.y * normilizedTo.y);
		float angle = atan(normilizedTo.y / normilizedTo.x);

		normilizedTo.x = i * cos(angle);
		normilizedTo.y = i * sin(angle);

		float radians = ToRadian(90);
		PointF newP1;
		newP1.x = pFrom.x + (normilizedTo.x * cos(radians) - normilizedTo.y * sin(radians));
		newP1.y = pFrom.y + (normilizedTo.x * sin(radians) + normilizedTo.y * cos(radians));

		dc.MoveTo(static_cast<int>(newP1.x), static_cast<int>(newP1.y));
		dc.LineTo((int)pTo.x , (int)pTo.y);

		radians = ToRadian(-90);
		PointF newP2;
		newP2.x = pFrom.x + (normilizedTo.x * cos(radians) - normilizedTo.y * sin(radians));
		newP2.y = pFrom.y + (normilizedTo.x * sin(radians) + normilizedTo.y * cos(radians));

		dc.MoveTo(static_cast<int>(newP2.x), static_cast<int>(newP2.y));
		dc.LineTo((int)pTo.x , (int)pTo.y);
	}
	*/
	/*
	dc.MoveTo((int)pFrom.x, (int)pFrom.y);
	int maxWidth = WidthTransform(m_meta.GetMetaEdgeWidth(m_graph->getEdgeCat(e)));

	AllTransform(pFrom);
	AllTransform(pTo);
	float deltaX = pTo.x - pFrom.x;
	float deltaY = pTo.y - pFrom.y;

	dc.MoveTo((int)pFrom.x, (int)pFrom.y);

	for(int i = maxWidth; i > 0; --i)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, i, c.ToCOLORREF());
		CAutoPen autoPen(pen, dc);
		dc.LineTo(static_cast<int>(pFrom.x + deltaX * (maxWidth - i + 1) / maxWidth), static_cast<int>(pFrom.y + deltaY * (maxWidth - i + 1) / maxWidth));
	}

	if (m_meta.GetMetaDebug())
	{
		CFont scaledFont;
		createFont(dc, scaledFont, m_fontSize);
		CAutoFont f(scaledFont, dc);
		CString txt = boost::lexical_cast<std::_tstring>(m_graph->getEdgeCat(e)).c_str();

		RECT bounds = {0, 0, 0, 0};
		bounds.left = (int)((pFrom.x + pTo.x) / 2);
		bounds.top = (int)((pFrom.y + pTo.y) / 2);
		dc.DrawText(txt, txt.GetLength(), &bounds, DT_CALCRECT|DT_NOPREFIX|DT_CENTER);
		dc.DrawText(txt, txt.GetLength(), &bounds, DT_NOPREFIX | DT_CENTER);
	}
	*/
/*
}

void CGraphFastRenderImpl::paintSegment(
	CDCHandle &dc, 
	PointF pFrom, 
	PointF pTo, 
	REAL FromWidth,
	REAL ToWidth,
	const Color & c) 
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, c.ToCOLORREF());
	CAutoPen autoPen(pen, dc);
	
	CBrush brush;
	brush.CreateSolidBrush(c.ToCOLORREF());
	CAutoBrush autoBrush(brush, dc);

	bool bArrow=ToWidth<0.1;

	PointF normilizedTo(pTo.x - pFrom.x, pTo.y - pFrom.y);
	float angle = atan(normilizedTo.y / normilizedTo.x);
	normilizedTo.x = FromWidth * cos(angle);
	normilizedTo.y = FromWidth * sin(angle);

	float radians = ToRadian(90);
	PointF newP;
	newP.x = normilizedTo.x * cos(radians) - normilizedTo.y * sin(radians);
	newP.y = normilizedTo.x * sin(radians) + normilizedTo.y * cos(radians);

	int pos=0;
	CPoint pnts[4];
	PointF tpt;

	if ( bArrow )
	{
		tpt=pTo;
		AllTransform(tpt);
		pnts[pos++] = CPoint(static_cast<int>(tpt.x), static_cast<int>(tpt.y));
	}

	tpt.x=pFrom.x + newP.x;
	tpt.y=pFrom.y + newP.y;
	AllTransform(tpt);
	pnts[pos++] = CPoint(static_cast<int>(tpt.x), static_cast<int>(tpt.y));

	tpt.x=pFrom.x - newP.x;
	tpt.y=pFrom.y - newP.y;
	AllTransform(tpt);
	pnts[pos++] = CPoint(static_cast<int>(tpt.x), static_cast<int>(tpt.y));

	if( !bArrow )
	{
		normilizedTo.x = ToWidth * cos(angle);
		normilizedTo.y = ToWidth * sin(angle);

		newP.x = normilizedTo.x * cos(radians) - normilizedTo.y * sin(radians);
		newP.y = normilizedTo.x * sin(radians) + normilizedTo.y * cos(radians);

		tpt.x=pTo.x - newP.x;
		tpt.y=pTo.y - newP.y;
		AllTransform(tpt);
		pnts[pos++] = CPoint(static_cast<int>(tpt.x), static_cast<int>(tpt.y));

		tpt.x=pTo.x + newP.x;
		tpt.y=pTo.y + newP.y;
		AllTransform(tpt);
		pnts[pos++] = CPoint(static_cast<int>(tpt.x), static_cast<int>(tpt.y));
	}

	dc.Polygon(pnts, bArrow?3:4);
}

void CGraphFastRenderImpl::drawLines(CDCHandle& dc)
{
	CRect _clientRect;
	dc.GetClipBox(_clientRect);
	RectF clientRect(_clientRect);

	CComPtr<clib::scoped_lock_ref_counted> lock;
	for (StlLinked<IEdgeIterator> e = m_graph->getUserEdgeIterator(lock, new MetaVisibleSkipper(m_meta)); e->IsValidItem(); e->NextItem())
	{
		CComPtr<IGraphEdge> edge = e->Get();
		CComPtr<IGraphVertex> _from = edge->GetSource();
		CComPtr<IGraphVertex> _to = edge->GetTarget();
		CUniqueID from = _from->GetID();
		CUniqueID to = _to->GetID();
		PointF p;
		PointF p1 = _from->GetPoint();
		PointF p2 = _to->GetPoint();
		AllTransform(p1);
		AllTransform(p2);
		if(m_calculateExtent || clientRect.Contains(p1) || clientRect.Contains(p2) || Intersect_Lines(clientRect, p1, p2, p))
		{
			Color c(!m_overNode.isEmpty() && (m_overToSel && m_overToSel->ContainsStep(_from, _to)) ? m_meta.GetMetaEdgeColorShortest(edge->GetCategory()) : m_meta.GetMetaEdgeColorNorm(edge->GetCategory()));
			if (from == m_overNode || to == m_overNode)
				c.Darken();

			Color fontColor(c);//m_meta.GetMetaEdgeFontColor(m_graph->getEdgeCat(e->Get())));
			fontColor.Darken();
#ifdef BENDY_LINES
			int numpoints=m_graph->getEdgePointsCount(e->Get());
#else		
			int numpoints=0;
#endif
			if ( numpoints )
			{
				CPen pen;
				pen.CreatePen(PS_SOLID, 1, c.ToCOLORREF());
				CAutoPen autoPen(pen, dc);

				CBrush brush;
				brush.CreateSolidBrush(c.ToCOLORREF());
				CAutoBrush autoBrush(brush, dc);

				PointFVector line; 
				PointF first, second;

				first=_from->GetPoint();

				line.push_back(first);

				for (int i=0; i<numpoints; i++ )
				{
					first = edge->GetPoint(i);
					line.push_back(first);
				}

				first = _to->GetPoint();
				line.push_back(first);

				assert(line.size()>=3);

				REAL max_width = m_meta.GetMetaEdgeWidth(edge->GetCategory());

				REALVector widths;
				CalcArrowWidths(
					max_width,
					widths,
					line );


				REALVector::iterator rvi=widths.begin();
				REAL fromWidth=max_width;
				REAL toWidth;
				int count=int(line.size());

				PointFVector::iterator ipts=line.begin();
				first=*ipts;
				++ipts;

				while( --count>1 )
				{
					toWidth=*rvi;
					second=*ipts;
					paintSegment(dc, first, second, fromWidth, toWidth, c); 
					fromWidth=toWidth;
					first=second;
					++rvi;
					++ipts;
				}

				second=*ipts;
				paintSegment(dc, first, second, fromWidth, 0.0, c); 
			}
			else
			{
				CComPtr<IGraphEdge> edge = e->Get();
				paintArrow(dc, edge, p1, p2, c, fontColor);
			}
		}
	}
}

void CGraphFastRenderImpl::drawVertices(CDCHandle& dc)
{
	CRect clientRect;
	dc.GetClipBox(clientRect);

	CComPtr<clib::scoped_lock_ref_counted> lock;
	for (StlLinked<IVertexIterator> n = m_graph->getUserVertexIterator(lock, new MetaVisibleSkipper(m_meta)); n->IsValidItem(); n->NextItem())
	{
		CComPtr<IGraphVertex> vertex = n->Get();
		CUniqueID id = vertex->GetID();
		PointF p = vertex->GetPoint();
		AllTransform(p);
		if(m_calculateExtent || (p.x >= clientRect.left && p.x < clientRect.right && p.y >= clientRect.top && p.y < clientRect.bottom))
		{
			if (id != m_overNode)	//Don't draw over until the end.
			{
				if (vertex->GetSelected())
					GetRenderer(id)->PaintSelVertex(dc);
				else
					GetRenderer(id)->PaintNormVertex(dc);
			}
		}
	}

	if (!m_overNode.isEmpty())
	{
		int oldFontSize = GetFontSize();
		SetFontSize(DEFAULT_FONT_SIZE);
		CComPtr<IGraphVertex> vertex = m_graph->GetVertex(m_overNode);
		if (vertex->GetSelected())
			GetRenderer(m_overNode)->PaintSelVertex(dc);
		else
			GetRenderer(m_overNode)->PaintOverVertex(dc);
		SetFontSize(oldFontSize);
	}
}
*/
