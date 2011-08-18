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

template <class T>
class CTimeLineScroll
// ----------------------------------------------------------------------------
{
protected:

	typedef COleDateTime     Date;
	typedef COleDateTimeSpan DateSpan;

public:

	// If your window only has one scroll bar, you can use:
	//   CHAIN_MSG_MAP(CTimeLineScroll<T>)
	// Otherwise, for the time line scrollbar, use:
	//   MESSAGE_HANDLER(WM_?SCROLL, OnScroll)

	BEGIN_MSG_MAP(CTimeLineScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
	END_MSG_MAP()

	LRESULT OnScroll(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		ATLASSERT(m_nDaysPerScrollStep > 0);

		const int nBar = uMsg == WM_HSCROLL ? SB_HORZ : SB_VERT;

		T* pT = static_cast<T*>(this);

		SCROLLINFO si = {0};
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;

		pT->GetScrollInfo(nBar, &si);

		int nNewPos = si.nPos;

		switch (LOWORD(wParam))
		{
			case SB_ENDSCROLL:
			{
				const int nAltMax = si.nMax - int(si.nPage) * 2;

				if (nNewPos < int(si.nPage))
				{
					const int nStepsFromMin = 
						(m_dtOrg - m_dtMin).GetDays() / m_nDaysPerScrollStep;

					pT->SetScrollPos(nBar, 
						nStepsFromMin > int(si.nPage) ? 
							si.nPage : 
							nStepsFromMin);
				}
				else if (nNewPos > nAltMax)
				{
					const int nStepsFromMax = 
						((m_dtMax - m_dtOrg).GetDays() - pT->GetDaysInView()) /
							m_nDaysPerScrollStep;

					pT->SetScrollPos(nBar, 
						nStepsFromMax <= 0 ? si.nMax :
						nStepsFromMax > nAltMax ? 
							nAltMax : 
					nAltMax + si.nPage - std::max<int>(0, nStepsFromMax));
				}
				return 0;
			}

			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
				nNewPos = HIWORD(wParam);
				break;

			case SB_LINELEFT:
				--nNewPos;
				break;

			case SB_LINERIGHT:
				++nNewPos;
				break;

			case SB_PAGELEFT:
				nNewPos -= si.nPage;
				break;

			case SB_PAGERIGHT:
				nNewPos += si.nPage;
				break;
		}

		// Calculate days to offset.
		int nDelta = (si.nPos - nNewPos) * m_nDaysPerScrollStep;

		if (nDelta > 0)	// scroll left
		{
			const int nMaxDelta = (m_dtOrg - m_dtMin).GetDays();

			if (nDelta > nMaxDelta)
			{
				nDelta = nMaxDelta;
				nNewPos -= nDelta / m_nDaysPerScrollStep;
			}
		}
		else if (nDelta < 0) // scroll right
		{
			int nMaxDelta = (m_dtMax - m_dtOrg).GetDays() - pT->GetDaysInView();

			if (nMaxDelta <= 0)
			{
				nDelta = -nMaxDelta;
				nNewPos = si.nMax;
			}
			else if (nDelta < -nMaxDelta)
			{
				nDelta = -nMaxDelta;
				nNewPos += nDelta / m_nDaysPerScrollStep;
			}
		}

		if (nDelta)
		{
			m_dtOrg -= DateSpan(nDelta);

			pT->DoScroll(pT->GetDaysExtent(nDelta));
			pT->SetScrollPos(nBar, nNewPos);
		}

		pT->SetMsgHandled(TRUE);
		return 0;
	}

public:

	CTimeLineScroll()
	{
		m_nDaysPerScrollStep = 7;
		m_nDayExtent = 1;
		m_dtMin = Date(2000,1,1,0,0,0);
		m_dtMax = Date(2100,1,1,0,0,0);
		m_dtOrg = Date::GetCurrentTime() - DateSpan(14);	// ie 2 weeks before today
		m_dtOrg.SetDateTime(m_dtOrg.GetYear(), m_dtOrg.GetMonth(), m_dtOrg.GetDay(), 0,0,0);
	}

	int GetDaysExtent(int nDays) const
	{
		return m_nDayExtent * nDays; 
	}

	int GetDaysInView() const
	{ 
		const T* pT = static_cast<const T*>(this);

		RECT rectClient; 
		pT->GetClientRect(&rectClient); 
		return pT->GetDaysExtent(rectClient.right - rectClient.left); 
	}

	int DateToClient(const Date& dt) const
	{
		const T* pT = static_cast<const T*>(this);
		return pT->GetDaysExtent((dt - m_dtOrg).GetDays());
	}

	Date DateFromClient(int x) const
	{
		ATLASSERT(m_nDayExtent > 0);
		return m_dtOrg + DateSpan(int(x / m_nDayExtent));
	}

	void DoScroll(int cx)
	{
		// Implement T::DoScroll if you're scrolling vertically.
		T* pT = static_cast<T*>(this);
		pT->ScrollWindowEx(cx, 0, 0, 0, 0, 0, SW_ERASE | SW_INVALIDATE);
	}

protected:

	int  m_nDaysPerScrollStep;
	int  m_nDayExtent;			// ie pixels per day
	Date m_dtMin;
	Date m_dtMax;
	Date m_dtOrg;
};
// ----------------------------------------------------------------------------
