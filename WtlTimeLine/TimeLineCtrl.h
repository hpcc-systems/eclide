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

#pragma once

#include "atlgdix.h"
#include "TimeLineScroll.h"

#include <deque>

// ----------------------------------------------------------------------------
// Notifications

#define TLN_FIRST		0U - 00U
#define TLN_LAST		0U - 09U

#define TLN_SELCHANGED	(TLN_FIRST -  1)	// (LPARAM)(LPNMTLSELCHANGED)lpnmh

typedef struct tagNMTLSELCHANGED
{
	NMHDR	   hdr;
	BOOL       bValidRange;
	SYSTEMTIME dateFrom;
	SYSTEMTIME dateTo;
} NMTLSELCHANGED, * LPNMTLSELCHANGED;
// ----------------------------------------------------------------------------

class CTimeLineCtrl :
	public CWindowImpl<CTimeLineCtrl>,
	public COffscreenDraw<CTimeLineCtrl>,
	public CTimeLineScroll<CTimeLineCtrl>
// ----------------------------------------------------------------------------
{
public:

	CTimeLineCtrl();

	BEGIN_MSG_MAP(CTimeLineCtrl)
		CHAIN_MSG_MAP(COffscreenDraw<CTimeLineCtrl>)
		
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)

		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		// MSG_WM_CAPTURECHANGED(OnCaptureChanged)

		CHAIN_MSG_MAP(CTimeLineScroll<CTimeLineCtrl>)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	LRESULT OnDestroy();

	LRESULT OnLButtonDown(UINT nFlags, CPoint point);
	LRESULT OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnCaptureChanged(HWND hWnd);

	void DoPaint(CDCHandle dc);
	void DoScroll(int cx);

	UINT GetMaxClientHeight() const;
	void AutoPosition(BOOL bBottom = TRUE);

protected:

	friend class CTimeLineBand;
	typedef std::deque<CTimeLineBand*> Bands;

	struct HitTestInfo
	{
		POINT ptHit;
		int   nBand;
	};

	UINT GetBandHeight() const
		{ return m_sizeChar.cy; }

	CTimeLineBand* GetSelectedBand() const;
	void GetSelectionRect(RECT& rect) const;

	BOOL HitTest(HitTestInfo& ht) const;

	void InitGdiObjects();

	BOOL AddBand(CTimeLineBand* pBand);
	void UpdateSelection(int nBand = -1, int xHit = 0);

	void NotifySelChanged();
	void NotifyParent(UINT nCode, LPNMHDR lpnmh);

protected:

	Bands m_bands;

	// GDI resources
	COLORREF m_crBand;
	COLORREF m_crText;
	CFont    m_font;
	CBitmap  m_bmInit;

	// UI Metrics
	SIZE m_sizeChar;

	// Interaction tracking
	int  m_nSelectedBand;
	Date m_dtSelBeg;
	Date m_dtSelEnd;	// Note: dtSelEnd is _not_ in the range.
};
// ----------------------------------------------------------------------------
