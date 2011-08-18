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

#include "TimeLineCtrl.h"

// --- CTimeLineCtrl internal implementation classes ---

class CTimeLineCalendar
// ----------------------------------------------------------------------------
// Namespace protection of date-related utilities (why aren't these part of 
// COleDateTime?!!)
{
public:

	static BOOL IsLeapYear(int nYear)
		{ return (!(nYear % 4)) && ( (nYear % 100) || (!(nYear % 400)) ); }

	static UINT GetDaysInYear(int nYear)
		{ return IsLeapYear(nYear) ? 366 : 365; }

	static UINT GetDaysInMonth(int nMonth, int nYear);

	static BOOL IsLeapYear(const COleDateTime& dt)
		{ return IsLeapYear(dt.GetYear()); }

	static UINT GetDaysInYear(const COleDateTime& dt)
		{ return GetDaysInYear(dt.GetYear()); }
};
// ----------------------------------------------------------------------------

class CTimeLineBand
// ----------------------------------------------------------------------------
// Derived CTimeLineBands are responsible for rendering a year within a
// CTimeLineCtrl, and for determining the periods within a year.
{
public:

	struct DrawParams
	{
		HDC  hTargetDC;
		HDC  hBitmapDC;
		int  xOrigin;
		int  cxYear;
		int  xSelBeg;
		int  xSelEnd;
	};

public:

	CTimeLineBand(CTimeLineCtrl& container) :
		m_wndTimeLine(container), m_yPos(0), m_nHeight(0) { }

	virtual ~CTimeLineBand() { }

	// Returns the period in which <dt> lies. Note that the period does 
	// not  include <dtEnd>, ie <dtEnd> represents the next day after the 
	// last day of the period.
	virtual BOOL GetPeriodFromDate(const COleDateTime& dt, 
		COleDateTime& dtBeg, COleDateTime& dtEnd) const = 0;

	// This is called to allow a band to cache any GDI resources it needs
	// fo rendering a year. All GDI resources will be restored by the caller.
	virtual void OnInitUI(CDCHandle dc) = 0;

	virtual void OnDrawYear(DrawParams& params, const COleDateTime& dtYear) = 0;
	virtual void OnScroll(int /*xyDelta*/) { }

	// Utility members.

	BOOL PtInBand(int y) const
		{ return y >= m_yPos && y < m_yPos + m_nHeight; }

	void GetPosition(RECT& rect) const
	{
		rect.top = m_yPos;
		rect.bottom = m_yPos + m_nHeight;
	}

	void SetPosition(int y, int nHeight)
	{
		ATLASSERT(nHeight > 0);
		m_yPos = y;
		m_nHeight = nHeight;
	}

protected:

	BOOL IsSelected() const
		{ return m_wndTimeLine.GetSelectedBand() == this; }

	BOOL HilightSelection(DrawParams& params, LPCTSTR szFormat = 0);

	int GetDaysExtent(int nDays) const
		{ return m_wndTimeLine.GetDaysExtent(nDays); }

protected:

	CTimeLineCtrl& m_wndTimeLine;	// The host of this band.

	int m_yPos;
	int m_nHeight;
};
// ----------------------------------------------------------------------------
