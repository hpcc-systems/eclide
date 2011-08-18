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
#include "TimeLineCtrl.h"

#include "TimeLineBandImpl.h"

// ----------------------------------------------------------------------------
CTimeLineCtrl::CTimeLineCtrl()
{
	m_nSelectedBand = -1;

	m_crBand = RGB(0xE0,0xE0,0xE0);
	m_crText = RGB(0xC0,0xC0,0xC8);
}

// ----------------------------------------------------------------------------
LRESULT
CTimeLineCtrl::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	SCROLLINFO si = {0};
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin = 0;
	si.nMax = 208;	// 4 years (in weeks)
	si.nPage = 26;	// 1/2 year (in weeks)
	si.nPos = (si.nMax - si.nPage) / 2;

	SetScrollInfo(SB_HORZ, &si, FALSE);

	AddBand(new CTimeLineWeekBand(*this));
	AddBand(new CTimeLineMonthBand(*this));
	AddBand(new CTimeLineYearBand(*this));

	m_font.CreatePointFont(80, "Verdana");
	InitGdiObjects();

	SetMsgHandled(FALSE);
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CTimeLineCtrl::OnDestroy()
{
	for (Bands::iterator i = m_bands.begin(); i != m_bands.end(); ++i)
		delete *i;

	m_bands.clear();

	SetMsgHandled(FALSE);
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CTimeLineCtrl::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	HitTestInfo ht;
	ht.ptHit = point;

	if (GetCapture() == m_hWnd && HitTest(ht))
		UpdateSelection(ht.nBand, point.x);

	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CTimeLineCtrl::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	HitTestInfo ht;
	ht.ptHit = point;

	if (HitTest(ht)) 
	{
		SetCapture();
		UpdateSelection(ht.nBand, point.x);
	}
	else
		UpdateSelection();

	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CTimeLineCtrl::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	ReleaseCapture();
	return 0;
}

// ----------------------------------------------------------------------------
LRESULT
CTimeLineCtrl::OnCaptureChanged(HWND /*hWnd*/) 
{
	SetMsgHandled(FALSE);
	return 0;
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::DoPaint(CDCHandle dc)
{
	CBrush brBand;
	brBand.CreateSolidBrush(m_crBand);

	HBRUSH hbrOrig = dc.SelectBrush(brBand);
	HFONT hfontOrig = dc.SelectFont(m_font);

	CDC dcBitmap;
	dcBitmap.CreateCompatibleDC(dc);

	HBITMAP hbmOrig = dcBitmap.SelectBitmap(m_bmInit);

	dc.SetTextColor(m_crText);
	dc.SetBkMode(TRANSPARENT);

	Date dtYear = DateFromClient(0);

	CTimeLineBand::DrawParams params;

	params.hTargetDC = dc;
	params.hBitmapDC = dcBitmap;
	params.xOrigin = -GetDaysExtent(dtYear.GetDayOfYear() - 1);

	const Date dtToday = Date::GetCurrentTime();

	RECT rectSelect;
	GetSelectionRect(rectSelect);

	RECT rectClient;
	GetClientRect(&rectClient);

	while (params.xOrigin < rectClient.right)
	{
		const int nDaysInYear = CTimeLineCalendar::GetDaysInYear(dtYear);

		params.cxYear  = GetDaysExtent(nDaysInYear);
		params.xSelBeg = max(rectSelect.left,  params.xOrigin);
		params.xSelEnd = min(rectSelect.right, params.xOrigin + params.cxYear);

		// Draw all bands for the current year.
		for (Bands::iterator i = m_bands.begin(); i != m_bands.end(); ++i)
			(*i)->OnDrawYear(params, dtYear);

		// Draw the 'today' marker (if appropriate)

		if (dtToday.GetYear() == dtYear.GetYear())
		{
			CBrush brToday;
			brToday.CreateSolidBrush(RGB(0xFF,0,0xFF));

			dc.SelectBrush(brToday);
			dc.PatBlt(params.xOrigin + GetDaysExtent(dtToday.GetDayOfYear() - 1), 0, 
				GetDaysExtent(1), GetBandHeight() * m_bands.size(), PATCOPY);
			dc.SelectBrush(brBand);
		}

		params.xOrigin += params.cxYear;

		dtYear += DateSpan(nDaysInYear);
	}

	dcBitmap.SelectBitmap(hbmOrig);

	dc.SelectFont(hfontOrig);
	dc.SelectBrush(hbrOrig);
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::DoScroll(int cx)
{
	for (Bands::iterator i = m_bands.begin(); i != m_bands.end(); ++i)
		(*i)->OnScroll(cx);

	CTimeLineScroll<CTimeLineCtrl>::DoScroll(cx);
}

// ----------------------------------------------------------------------------
CTimeLineBand*
CTimeLineCtrl::GetSelectedBand() const
{
	return (m_nSelectedBand >= 0 && m_nSelectedBand < (int) m_bands.size()) ?
		m_bands[m_nSelectedBand] : 0;
}

// ----------------------------------------------------------------------------
UINT
CTimeLineCtrl::GetMaxClientHeight() const
{
	return m_bands.size() * GetBandHeight() + GetSystemMetrics(SM_CYHSCROLL);
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::AutoPosition(BOOL bBottom)
{
	if (IsWindow())
	{
		CRect rectWindow;
		rectWindow.SetRectEmpty();

		AdjustWindowRectEx(rectWindow, GetStyle(), FALSE, GetExStyle());
		rectWindow.bottom += GetMaxClientHeight();

		CRect rectParent;
		GetParent().GetClientRect(&rectParent);

		const int y = bBottom ? rectParent.bottom - rectWindow.Height() : 0;
		SetWindowPos(0, 0, y, rectParent.Width(), rectWindow.Height(),
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

// ----------------------------------------------------------------------------
BOOL
CTimeLineCtrl::HitTest(HitTestInfo& ht) const
{
	CRect rectClient;
	GetClientRect(rectClient);

	if (rectClient.PtInRect(ht.ptHit))
		for (Bands::const_iterator i = m_bands.begin(); i != m_bands.end(); ++i)
			if ((*i)->PtInBand(ht.ptHit.y))
			{
				ht.nBand = i - m_bands.begin();
				return TRUE;
			}

	ht.nBand = -1;
	return FALSE;
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::GetSelectionRect(RECT& rect) const
{
	CTimeLineBand* pSelectedBand = GetSelectedBand();

	if (pSelectedBand)
	{
		pSelectedBand->GetPosition(rect);

		rect.left  = DateToClient(m_dtSelBeg);
		rect.right = DateToClient(m_dtSelEnd);
	}
	else
		SetRectEmpty(&rect);
}

// ----------------------------------------------------------------------------
BOOL
CTimeLineCtrl::AddBand(CTimeLineBand* pBand)
{
	ATLASSERT(pBand != 0);

	if (pBand)
		m_bands.push_back(pBand);

	return pBand != 0;
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::UpdateSelection(int nBand, int xHit)
{
	CRect rectSelect;
		
	GetSelectionRect(rectSelect);
	InvalidateRect(rectSelect, FALSE);

	m_nSelectedBand = nBand;
	CTimeLineBand* pSelectedBand = GetSelectedBand();

	if (pSelectedBand && pSelectedBand->GetPeriodFromDate(DateFromClient(xHit),
			m_dtSelBeg, m_dtSelEnd))
	{
		ATLASSERT(m_dtSelBeg < m_dtSelEnd);

		GetSelectionRect(rectSelect);
		InvalidateRect(rectSelect, FALSE);
	}
	else
	{
		m_nSelectedBand = -1;
		m_dtSelBeg.SetStatus(Date::invalid);
		m_dtSelEnd.SetStatus(Date::invalid);
	}

	UpdateWindow();
	NotifySelChanged();
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::NotifySelChanged()
{
	NMTLSELCHANGED nmsc = {0};

	if (m_nSelectedBand >= 0 && 
		m_dtSelBeg.GetStatus() == Date::valid &&
		m_dtSelEnd.GetStatus() == Date::valid)
	{
		ATLASSERT(m_dtSelBeg < m_dtSelEnd);
		nmsc.bValidRange = TRUE;

		// Notify with an _inclusive_ range.
		m_dtSelBeg.GetAsSystemTime(nmsc.dateFrom);
		(m_dtSelEnd - DateSpan(1)).GetAsSystemTime(nmsc.dateTo);
	}

	NotifyParent(TLN_SELCHANGED, reinterpret_cast<NMHDR*>(&nmsc));
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::NotifyParent(UINT nCode, LPNMHDR lpnmh)
{
	lpnmh->code = nCode;
	lpnmh->hwndFrom = m_hWnd;
	lpnmh->idFrom = GetDlgCtrlID();

	GetParent().SendMessage(WM_NOTIFY, lpnmh->idFrom, (LPARAM) lpnmh);
}

// ----------------------------------------------------------------------------
void
CTimeLineCtrl::InitGdiObjects()
{
	CBrush brBand;
	brBand.CreateSolidBrush(m_crBand);

	CClientDC dcClient(m_hWnd);

	if (m_bmInit.IsNull())
		m_bmInit.CreateCompatibleBitmap(dcClient, 4, 4);

	// Prepare DC

	CDC dc;
	dc.CreateCompatibleDC(dcClient);
	dc.SaveDC();

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_crText);
	dc.SelectFont(m_font);
	dc.SelectBitmap(m_bmInit);
	dc.SelectBrush(brBand);

	dc.GetTextExtent(_T("0"), 1, &m_sizeChar);

	// Update size and position of bands, and prepare UI objects for all bands.

	int y = 0;

	for (Bands::iterator i = m_bands.begin(); i != m_bands.end(); ++i)
	{
		(*i)->SetPosition(y, GetBandHeight());
		(*i)->OnInitUI(dc.m_hDC);

		y += GetBandHeight();
	}

	dc.RestoreDC(-1);
}

// ----------------------------------------------------------------------------
