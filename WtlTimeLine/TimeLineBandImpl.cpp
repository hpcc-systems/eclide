// ----------------------------------------------------------------------------
// Written by Tony Ioanides (tonyi@bigpond.com)
// Copyright (c) 2003 Tony Ioanides. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, is permitted provided that:
// 
// 1. Any redistribution of source code must retain the above copyright notice,
//    this list of conditions, and the following disclaimer. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include "TimeLineBandImpl.h"

static const int nDaysPerWeek = 7;

// ----------------------------------------------------------------------------
BOOL
CTimeLineWeekBand::GetPeriodFromDate(const COleDateTime& dt, 
	COleDateTime& dtBeg, COleDateTime& dtEnd) const
{
	// GetDayOfWeek() is 1-based.
	dtBeg = dt - COleDateTimeSpan(dt.GetDayOfWeek() - 1);
	dtEnd = dtBeg + COleDateTimeSpan(nDaysPerWeek);
	return TRUE;
}

// ----------------------------------------------------------------------------
void
CTimeLineWeekBand::OnInitUI(CDCHandle dc)
{
	// In case you're wondering why this isn't 53...
	const int nMaxWeeks = 56;

	// ...it's possible that we could start rendering from the last day of the 
	// second week in the bitmap. Having to render up to 53 weeks in a year,
	// we add some extra weeks to the bitmap so we don't underflow.

	const int cxWeek = GetDaysExtent(nDaysPerWeek);
	const SIZE sizeBitmap = { cxWeek * nMaxWeeks, m_nHeight };

	if (! m_bmYearInWeeks.IsNull())
		m_bmYearInWeeks.DeleteObject();

	m_bmYearInWeeks.CreateCompatibleBitmap(dc, sizeBitmap.cx, sizeBitmap.cy);

	HBITMAP hbmOrig = dc.SelectBitmap(m_bmYearInWeeks);
	dc.PatBlt(0, 0, sizeBitmap.cx, sizeBitmap.cy, WHITENESS);

	int x = 0;

	for (int w = 0; w < nMaxWeeks; w += 2)
	{
		dc.PatBlt(x, 0, cxWeek, sizeBitmap.cy, PATCOPY);
		x += cxWeek * 2;
	}

	dc.SelectBitmap(hbmOrig);
}

// ----------------------------------------------------------------------------
void
CTimeLineWeekBand::OnDrawYear(DrawParams& params, const COleDateTime& dtYear)
{
	// We calculate a 'week number' as the number of weeks since a fixed point
	// in time. This is used to ensure a smooth alternation of week numbers
	// across years.

	const COleDateTime dtEpoch(1800, 1, 5, 0,0,0);
	const COleDateTime dtStartOfYear(dtYear.GetYear(), 1, 1, 0,0,0);

	const int nWeeksElapsed = 
		((dtStartOfYear - dtEpoch).GetDays() + (dtEpoch.GetDayOfWeek() - 1)) / 
			nDaysPerWeek;

	const int nFirstDayOfFirstWeek = dtStartOfYear.GetDayOfWeek() - 1;

	const int nDaysOffset = (nWeeksElapsed % 2) ?
		nFirstDayOfFirstWeek : (nFirstDayOfFirstWeek + nDaysPerWeek);

	HGDIOBJ hbmOrig = SelectObject(params.hBitmapDC, m_bmYearInWeeks);

	BitBlt(
		params.hTargetDC, params.xOrigin, m_yPos, params.cxYear, m_nHeight, 
		params.hBitmapDC, GetDaysExtent(nDaysOffset), 0, SRCCOPY);
	SelectObject(params.hBitmapDC, hbmOrig);

	HilightSelection(params);
}

// ============================================================================

// ----------------------------------------------------------------------------
BOOL
CTimeLineMonthBand::GetPeriodFromDate(const COleDateTime& dt, 
	COleDateTime& dtBeg, COleDateTime& dtEnd) const
{
	const int y = dt.GetYear();
	const int m = dt.GetMonth();

	dtBeg.SetDateTime(y, m, 1, 0,0,0);
	dtEnd = dtBeg + COleDateTimeSpan(CTimeLineCalendar::GetDaysInMonth(m, y));
	return TRUE;
}

// ----------------------------------------------------------------------------
void
CTimeLineMonthBand::OnInitUI(CDCHandle dc)
{
	InitBitmap(m_bmYearInMonths, dc, FALSE);
	InitBitmap(m_bmLeapYearInMonths, dc, TRUE);
}

// ----------------------------------------------------------------------------
void
CTimeLineMonthBand::OnDrawYear(DrawParams& params, const COleDateTime& dtYear)
{
	HGDIOBJ hbmOrig = SelectObject(params.hBitmapDC, 
		CTimeLineCalendar::IsLeapYear(dtYear) ? 
			m_bmLeapYearInMonths : 
			m_bmYearInMonths);

	BitBlt(params.hTargetDC, params.xOrigin, m_yPos, params.cxYear, m_nHeight, 
		params.hBitmapDC, 0, 0, SRCCOPY);
	SelectObject(params.hBitmapDC, hbmOrig);

	HilightSelection(params, _T("%b"));
}

// ----------------------------------------------------------------------------
void
CTimeLineMonthBand::InitBitmap(CBitmap& bm, CDCHandle dc, BOOL bLeapYear)
{
	const int nMaxDays = 366;
	const SIZE sizeBitmap = { GetDaysExtent(nMaxDays), m_nHeight };

	if (! bm.IsNull())
		bm.DeleteObject();

	bm.CreateCompatibleBitmap(dc, sizeBitmap.cx, sizeBitmap.cy);

	HBITMAP hbmOrig = dc.SelectBitmap(bm);
	dc.PatBlt(0, 0, sizeBitmap.cx, sizeBitmap.cy, WHITENESS);

	const int nChars = GetDaysExtent(1) > 1 ? 3 : 1;
	const int nMonthsPerYear = 12;
	const int nYear = bLeapYear ? 1996 : 1997;

	ATLASSERT(bLeapYear == CTimeLineCalendar::IsLeapYear(nYear));

	int x = 0;

	for (int i = 1; i <= nMonthsPerYear; ++i)
	{
		const int cx = GetDaysExtent(CTimeLineCalendar::GetDaysInMonth(i, nYear));

		if (i % 2)
			dc.PatBlt(x, 0, cx, sizeBitmap.cy, PATCOPY);

		COleDateTime dt;
		dt.SetDate(nYear, i, 1);

		const CString str = dt.Format(_T("%b"));
		dc.TextOut(x + 1, 0, str, nChars);

		x += cx;
	}

	dc.SelectBitmap(hbmOrig);
}

// ============================================================================

// ----------------------------------------------------------------------------
BOOL
CTimeLineYearBand::GetPeriodFromDate(const COleDateTime& dt, 
	COleDateTime& dtBeg, COleDateTime& dtEnd) const
{
	const int y = dt.GetYear();
	dtBeg.SetDateTime(y, 1, 1, 0,0,0);
	dtEnd.SetDateTime(y + 1, 1, 1, 0,0,0);
	return TRUE;
}

// ----------------------------------------------------------------------------
void
CTimeLineYearBand::OnInitUI(CDCHandle dc)
{
	LPCTSTR szYear = _T("0000 ");

	SIZE s;
	dc.GetTextExtent(szYear, _tcslen(szYear), &s);

	m_cxText = s.cx;
}

// ----------------------------------------------------------------------------
void
CTimeLineYearBand::OnDrawYear(DrawParams& params, const COleDateTime& dtYear)
{
	TCHAR szBuffer[10] = {0};
	_itot(dtYear.GetYear(), szBuffer, 10);

	// Keep the year in view (if possible), even if the year isn't entirely
	// in view.
	const int xText = 
		(params.xOrigin > 0) ? params.xOrigin : 
		(params.xOrigin + params.cxYear - m_cxText > 0) ? 0 : 
			(params.xOrigin + params.cxYear - m_cxText);

	CDCHandle dc(params.hTargetDC);
	dc.PatBlt(xText, m_yPos, -2, m_nHeight, PATCOPY);

	if (HilightSelection(params))
	{
		const int crOrig = dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));

		dc.TextOut(xText + 1, m_yPos, szBuffer, min(_tcslen(szBuffer), 4));
		dc.SetTextColor(crOrig);
	}
	else
	{
		dc.PatBlt(params.xOrigin, m_yPos, params.cxYear, m_nHeight, WHITENESS);
		dc.TextOut(xText + 1, m_yPos, szBuffer, min(_tcslen(szBuffer), 4));
	}
}

// ----------------------------------------------------------------------------
void
CTimeLineYearBand::OnScroll(int dx)
{
	// Because the year digits are always left-justified, if we're scrolling
	// right we need to remove the digits from the the region that will be
	// moved to the right.

	if (dx > 0)
	{
		CClientDC dc(m_wndTimeLine);
		dc.PatBlt(0, m_yPos, m_cxText, m_nHeight, WHITENESS);
	}

	RECT rect = { 0, m_yPos, m_cxText, m_yPos + m_nHeight };
	m_wndTimeLine.InvalidateRect(&rect);
}

// ----------------------------------------------------------------------------

