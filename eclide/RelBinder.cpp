#include "stdafx.h"
#include "RelBinder.h"
using namespace Gdiplus;

CBezierBinder::CBezierBinder(HWND SourceHwnd, HWND TargetHwnd) : CRelationshipBinder(SourceHwnd, TargetHwnd)
{
	m_source.x = 0;
	m_source.y = 0;
	m_dest.x = 0;
	m_dest.y =0;
}

void CBezierBinder::appendPoint(const CPoint &p)
{
	m_points.push_back(p);
}

void CBezierBinder::Draw(HDC dc, HWND hwndOwner, POINT ptOffset)
{
	//	ATLASSERT(dc);
	Graphics graphics(dc);
	Pen greenPen(Color::Green, 3);
	Pen redPen(Color::Red, 3);
	Pen bluePen(Color::Blue, 3);
	PointF curvePoints[64];
	SolidBrush redBrush(Color::Red);

	CRelationshipBinder::Draw(dc, hwndOwner, CPoint(0, 0));

	CPoint source(m_source);
	CPoint dest(m_dest);
	source.Offset(-ptOffset.x, -ptOffset.y);
	dest.Offset(-ptOffset.x, -ptOffset.y);

	CPoint bezOff;
	bezOff.x = -rcSource.Width() / 2;
	bezOff.y = -rcSource.Height() / 2;
	bezOff.Offset(ptOffset);

	CPoint ptFrom((rcSource.Width()/2) + rcSource.left, (rcSource.Height()/2) + rcSource.top);
	CPoint ptTo((rcTarget.Width()/2) + rcTarget.left, (rcTarget.Height()/2) + rcTarget.top);

	if (m_source.x == 0 && m_source.y == 0 && m_dest.x == 0 && m_dest.y ==0)
	{
		m_source = ptFrom;
		m_dest = ptTo;
	}

	HPEN oldPen = (HPEN)::SelectObject(dc, (LPVOID)::GetStockObject(BLACK_PEN));
	CPoint rcOld;

	if (ptFrom == source && ptTo == dest)
	{
		unsigned i = 0;
		for(std::list<CPoint>::iterator itr = m_points.begin(); itr != m_points.end(); ++itr, ++i)
		{
			if (itr != m_points.begin())
//				::LineTo(dc, itr->x - bezOff.x, itr->y - bezOff.y);
			::MoveToEx(dc, itr->x - bezOff.x, itr->y - bezOff.y, &rcOld);
			curvePoints[i] = PointF(itr->x - bezOff.x, itr->y - bezOff.y);
		}
		graphics.DrawBeziers(&greenPen, curvePoints, i);
//		for(int j = 0; j < i; ++j)
		{
//			graphics.FillEllipse(&redBrush, Rect(curvePoints[j].X - 5, curvePoints[j].Y - 5, 10, 10));
		}
	}
	else
	{
		::MoveToEx(dc, ptFrom.x, ptFrom.y, &rcOld);
//		::LineTo(dc, ptTo.x, ptTo.y);
		graphics.DrawLine(&greenPen, PointF(ptFrom.x, ptFrom.y), PointF(ptTo.x, ptTo.y));
	}

	SelectObject(dc, (LPVOID)oldPen);
}
