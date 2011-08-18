#pragma once

class CRelationshipBinder
{
public:
	CRelationshipBinder(
		HWND SourceHwnd,
		HWND TargetHwnd)
	{
		ATLASSERT(::IsWindow(SourceHwnd));
		ATLASSERT(::IsWindow(TargetHwnd));
		m_hwndSource = SourceHwnd;
		m_hwndTarget = TargetHwnd;
	};      

protected:
	void DrawArrov(HDC dc, RECT rect, BOOL bDownwards)
	{
		HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, (LPVOID)::GetStockObject(BLACK_BRUSH));
		POINT points[3];
		points[0].x = rect.left;
		points[0].y = bDownwards ? rect.top : rect.bottom;
		points[1].x = rect.right;
		points[1].y = bDownwards ? rect.top : rect.bottom;
		points[2].x = rect.left + (rect.right-rect.left)/2;
		points[2].y = bDownwards ? rect.bottom : rect.top;
		::Polygon(dc,points,3);
		SelectObject(dc, (LPVOID)oldBrush);
	};

public:
	virtual void Draw(HDC dc, HWND hwndOwner, POINT ptOffset)
	{
		ATLASSERT(dc);
		::GetWindowRect(hwndOwner, &rcOwner);
		::GetWindowRect(m_hwndSource, &rcSource);
		::GetWindowRect(m_hwndTarget, &rcTarget);
		rcSource.OffsetRect(-rcOwner.TopLeft());
		rcTarget.OffsetRect(-rcOwner.TopLeft());

		rcSource.OffsetRect(ptOffset);
		rcTarget.OffsetRect(ptOffset);

/*		switch( m_Type ) {
case BINDER_HORIZONTAL:
	{
		CPoint ptFrom(0, (rcSource.Height()/2) + rcSource.top);
		CPoint ptTo(0, (rcTarget.Height()/2) + rcTarget.top);

		int dir = rcSource.left > rcTarget.left ? -1 : 1;

		if( dir<0 ) {
			ptFrom.x = rcSource.left;
			ptTo.x = rcTarget.right-1;
		}
		else {
			ptFrom.x = rcSource.right-1;
			ptTo.x = rcTarget.left;
		};

		HPEN oldPen = (HPEN)::SelectObject(dc, (LPVOID)::GetStockObject(BLACK_PEN));

		const int iHandleSize = 14;
		CPoint rcOld;
		::MoveToEx(dc, ptFrom.x, ptFrom.y, &rcOld);
		ptFrom.x += iHandleSize*dir;
		::LineTo(dc, ptFrom.x, ptFrom.y);
		::MoveToEx(dc, ptTo.x, ptTo.y, &rcOld);
		ptTo.x -= iHandleSize*dir;
		::LineTo(dc, ptTo.x, ptTo.y);
		// Make connecting line
		::LineTo(dc, ptFrom.x, ptFrom.y);

		SelectObject(dc, (LPVOID)oldPen);

	};
	break;
case BINDER_VERTICAL:
	{
		CPoint ptFrom((rcSource.Width()/2) + rcSource.left, 0);
		CPoint ptTo((rcTarget.Width()/2) + rcTarget.left, 0);
		int dir = rcSource.top > rcTarget.top ? -1 : 1;

		if( dir<0 ) {
			ptFrom.y = rcSource.top;
			ptTo.y = rcTarget.bottom-1;
		}
		else {
			ptFrom.y = rcSource.bottom-1;
			ptTo.y = rcTarget.top;
		};

		HPEN oldPen = (HPEN)::SelectObject(dc, (LPVOID)::GetStockObject(BLACK_PEN));

		CPoint rcOld;
		::MoveToEx(dc, ptFrom.x, ptFrom.y, &rcOld);
		::LineTo(dc, ptFrom.x, ptFrom.y);
		::MoveToEx(dc, ptTo.x, ptTo.y, &rcOld);
		::LineTo(dc, ptTo.x, ptTo.y);
		// Make connecting line
		::LineTo(dc, ptFrom.x, ptFrom.y);

		SelectObject(dc, (LPVOID)oldPen);

	};
	break;
case BINDER_CLASS_AGGR:
case BINDER_CLASS_INHERIT:
	{
		CPoint ptFrom((rcSource.Width()/2) + rcSource.left, 0);
		CPoint ptTo((rcTarget.Width()/2) + rcTarget.left, 0);
		int dir = rcSource.top > rcTarget.top ? -1 : 1;

		if( dir<0 ) {
			ptFrom.y = rcSource.top;
			ptTo.y = rcTarget.bottom-1;
		}
		else {
			ptFrom.y = rcSource.bottom-1;
			ptTo.y = rcTarget.top;
		};

		HPEN oldPen = (HPEN)::SelectObject(dc, (LPVOID)::GetStockObject(BLACK_PEN));

		const int iHandleSize = 14;
		CPoint rcOld;
		::MoveToEx(dc, ptFrom.x, ptFrom.y, &rcOld);
		::LineTo(dc, ptFrom.x, ptFrom.y);
		::MoveToEx(dc, ptTo.x, ptTo.y, &rcOld);
		ptTo.y -= iHandleSize*dir;
		CRect rc;
		if( m_Type==BINDER_CLASS_AGGR )
			rc.SetRect(ptTo.x-iHandleSize/3,ptTo.y,ptTo.x+iHandleSize/3,ptTo.y+iHandleSize*dir);
		else
			rc.SetRect(ptTo.x-iHandleSize/2,ptTo.y+iHandleSize*dir,ptTo.x+iHandleSize/2,ptTo.y);
		DrawArrov(dc,rc,TRUE);
		::LineTo(dc, ptTo.x, ptTo.y);
		// Make connecting line
		::LineTo(dc, ptFrom.x, ptFrom.y);

		SelectObject(dc, (LPVOID)oldPen);

	};
	break;
default:
	ATLASSERT(FALSE);
		};*/
	};

	HWND GetSourceHwnd() const { return m_hwndSource; };
	HWND GetTargetHwnd() const { return m_hwndTarget; };

protected:
	HWND m_hwndSource;
	HWND m_hwndTarget;
	CRect rcOwner, rcSource, rcTarget;
};


class CCenterBinder : public CRelationshipBinder
{
	CCenterBinder(HWND SourceHwnd, HWND TargetHwnd) : CRelationshipBinder(SourceHwnd, TargetHwnd)
	{
	}

	virtual void Draw(HDC dc, HWND hwndOwner, POINT ptOffset)
	{
		ATLASSERT(dc);
		CRelationshipBinder::Draw(dc, hwndOwner, ptOffset);

		CPoint ptFrom((rcSource.Width()/2) + rcSource.left, (rcSource.Height()/2) + rcSource.top);
		CPoint ptTo((rcTarget.Width()/2) + rcTarget.left, (rcTarget.Height()/2) + rcTarget.top);

		HPEN oldPen = (HPEN)::SelectObject(dc, (LPVOID)::GetStockObject(BLACK_PEN));

		CPoint rcOld;
		::MoveToEx(dc, ptFrom.x, ptFrom.y, &rcOld);
		::LineTo(dc, ptTo.x, ptTo.y);

		SelectObject(dc, (LPVOID)oldPen);
	}
};

class CBezierBinder : public CRelationshipBinder
{
protected:
	CPoint m_source;
	CPoint m_dest;
	std::list<CPoint> m_points;
public:
	CBezierBinder(HWND SourceHwnd, HWND TargetHwnd);

	void appendPoint(const CPoint &p);
	virtual void Draw(HDC dc, HWND hwndOwner, POINT ptOffset);
};

