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
#include "TimeLineBand.h"

// ----------------------------------------------------------------------------
UINT
CTimeLineCalendar::GetDaysInMonth(int nMonth, int nYear)
{
	ATLASSERT(nMonth >= 1 && nMonth <= 12);

	static const BYTE nDays[] = 
		{ 0,  31, 28, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31 };

	return 
		(nMonth == 2) ? (IsLeapYear(nYear) ? 29 : 28) :
		(nMonth >= 1 && nMonth <= 12) ? nDays[nMonth] : 0;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
BOOL
CTimeLineBand::HilightSelection(DrawParams& params, LPCTSTR szFormat)
{
	if (IsSelected() && params.xSelBeg < params.xSelEnd)
	{
		CDCHandle dc(params.hTargetDC);

		dc.FillSolidRect(params.xSelBeg, m_yPos, 
			params.xSelEnd - params.xSelBeg, m_nHeight, 
			GetSysColor(COLOR_HIGHLIGHT));

		if (szFormat != 0)
		{
			CString str = m_wndTimeLine.m_dtSelBeg.Format(szFormat);

			const int crOrig = dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));

			dc.TextOut(params.xSelBeg + 1, m_yPos, str, str.GetLength());
			dc.SetTextColor(crOrig);
		}

		return TRUE;
	}

	return FALSE;
}

// ----------------------------------------------------------------------------
