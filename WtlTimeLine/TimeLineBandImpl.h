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

// --- CTimeLineBand implementations for weeks, months, and years ---

#include "TimeLineBand.h"

class CTimeLineWeekBand : public CTimeLineBand
// ----------------------------------------------------------------------------
{
public:

	CTimeLineWeekBand(CTimeLineCtrl& container) :
		CTimeLineBand(container) { }

	virtual BOOL GetPeriodFromDate(const COleDateTime& dt, 
		COleDateTime& dtBeg, COleDateTime& dtEnd) const;

	virtual void OnInitUI(CDCHandle dc);
	virtual void OnDrawYear(DrawParams& p, const COleDateTime& tmYear);

private:

	// Cached rendering of weeks of a year.
	CBitmap m_bmYearInWeeks;
};

// ----------------------------------------------------------------------------

class CTimeLineMonthBand : public CTimeLineBand
// ----------------------------------------------------------------------------
{
public:

	CTimeLineMonthBand(CTimeLineCtrl& container) :
		CTimeLineBand(container) { }

	virtual BOOL GetPeriodFromDate(const COleDateTime& dt, 
		COleDateTime& dtBeg, COleDateTime& dtEnd) const;

	virtual void OnInitUI(CDCHandle dc);
	virtual void OnDrawYear(DrawParams& params, const COleDateTime& tmYear);

private:

	void InitBitmap(CBitmap& bm, CDCHandle dc, BOOL bLeapYear);

private:

	// Cached renderings of months of a year in leap and non-leap forms.
	CBitmap m_bmYearInMonths;
	CBitmap m_bmLeapYearInMonths;
};
// ----------------------------------------------------------------------------

class CTimeLineYearBand : public CTimeLineBand
// ----------------------------------------------------------------------------
{
public:

	CTimeLineYearBand(CTimeLineCtrl& container) :
		CTimeLineBand(container), m_cxText(0) { }

	virtual BOOL GetPeriodFromDate(const COleDateTime& dt, 
		COleDateTime& dtBeg, COleDateTime& dtEnd) const;

	virtual void OnInitUI(CDCHandle dc);
	virtual void OnDrawYear(DrawParams& params, const COleDateTime& tmYear);

	virtual void OnScroll(int dx);

private:

	int m_cxText;
};
// ----------------------------------------------------------------------------
