#include "StdAfx.h"
#include ".\pageloadedview.h"

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  ===========================================================================
RowRange::RowRange()
{
    m_from = 0;
    m_to = 0;
    m_state = S_BLANK;
}

RowRange::RowRange(int from, int to, STATE state)
{
    m_from = from;
    m_to = to;
    m_state = state;
}

bool RowRange::IsValid()	const
{
    return (m_to > 0 && m_from > 0 && m_to - m_from >= 0);
}

size_t RowRange::SplitMerge(RowRange & other, RowRangeVector & results) const
{
    results.clear();
    RowRange result_1;
    RowRange result_2;
    RowRange result_3;
    if (m_state == other.m_state && m_from == other.m_from && m_to == other.m_to)
    {
        //Do Nothing
    }
    else if (IsMatchedNeighbour(other))
    {
        other.m_state = result_1.m_state = m_state;
        other.m_from = result_1.m_from = min(m_from, other.m_from);
        other.m_to = result_1.m_to = max(m_to, other.m_to);
    }
    else if (Contains(other))
    {
        if (m_state == other.m_state)
        {
            other.m_state = m_state;
            other.m_from = m_from;
            other.m_to = m_to;
        }
        else
        {
            result_1.m_state = m_state;
            result_1.m_from = m_from;
            result_1.m_to = other.m_from - 1;

            result_2.m_state = other.m_state;
            result_2.m_from = other.m_from;
            result_2.m_to = other.m_to;

            result_3.m_state = m_state;
            result_3.m_from = other.m_to + 1;
            result_3.m_to = m_to;
        }
    }
    else if (other.Contains(*this))
    {
        if (m_state == other.m_state)
        {
            result_1.m_state = other.m_state;
            result_1.m_from = other.m_from;
            result_1.m_to = other.m_to;
        }
        else
        {
            result_1.m_state = other.m_state;
            result_1.m_from = other.m_from;
            result_1.m_to = m_from - 1;

            result_2.m_state = m_state;
            result_2.m_from = m_from;
            result_2.m_to = m_to;

            result_3.m_state = other.m_state;
            result_3.m_from = m_to + 1;
            result_3.m_to = other.m_to;
        }
    }
    else if (ContainsFrom(other))
    {
        if (m_state == other.m_state)
        {
            other.m_state = result_1.m_state = m_state;
            other.m_from = result_1.m_from = m_from;
            other.m_to = result_1.m_to = other.m_to;
        }
        else
        {
            result_1.m_state = m_state;
            result_1.m_from = m_from;
            result_1.m_to = other.m_from - 1;

            result_2.m_state = other.m_state;
            result_2.m_from = other.m_from;
            result_2.m_to = other.m_to;
        }

    }
    else if (ContainsTo(other))
    {
        if (m_state == other.m_state)
        {
            other.m_state = result_1.m_state = m_state;
            other.m_from = result_1.m_from = other.m_from;
            other.m_to = result_1.m_to = m_to;
        }
        else
        {
            result_2.m_state = other.m_state;
            result_2.m_from = other.m_from;
            result_2.m_to = other.m_to;

            result_3.m_state = m_state;
            result_3.m_from = other.m_to + 1;
            result_3.m_to = m_to;
        }
    }
    if (result_1.IsValid())
    {
        results.push_back(result_1);
    }
    if (result_2.IsValid())
    {
        results.push_back(result_2);
    }
    if (result_3.IsValid())
    {
        results.push_back(result_3);
    }
    return results.size();
}

bool RowRange::IsMatchedNeighbour(const RowRange & other) const
{
    return (m_state == other.m_state && (m_from == other.m_to + 1 || m_to == other.m_from - 1));
}

bool RowRange::Contains(const RowRange & other) const
{
    return (m_from <= other.m_from && m_to >= other.m_to);
}

bool RowRange::ContainsFrom(const RowRange & other) const
{
    return (m_from <= other.m_from && m_to >= other.m_from);
}

bool RowRange::ContainsTo(const RowRange & other) const
{
    return (m_from <= other.m_to && m_to >= other.m_to);
}

bool RowRange::operator < (const RowRange & r) const
{
    return m_from < r.m_from;
}
//  ===========================================================================

CPageLoadedView::CPageLoadedView()
{
    m_total = 0;
}

CPageLoadedView::~CPageLoadedView()
{
}

void CPageLoadedView::SetTotal(int total)
{
    m_total = total;
    m_loaded.clear();
    RowRange range(1, m_total, RowRange::S_BLANK);
    m_loaded.push_back(range);
}

int CPageLoadedView::GetTotal() const
{
    return m_total;
}

void CPageLoadedView::SetRequested(int from, int to)
{
    if (from < 1 && to < 1)
    {
        return;
    }
    InsertItem(from, to, RowRange::S_REQUESTED);
}

void CPageLoadedView::SetLoaded(int from, int to)
{
    if (from >= 1 && to <= m_total)
    {
        InsertItem(from, to, RowRange::S_LOADED);
    }
}

void CPageLoadedView::InsertItem(int from, int to, RowRange::STATE state)
{
    if (from <= 0 && to <= 0)
        return;

    ATLASSERT(from >= 1 && to <= m_total);
    RowRange range(from, to, state);

Again:
    int i = 0;
    for(RowRangeVector::iterator itr = m_loaded.begin(); itr != m_loaded.end(); ++itr)
    {
        RowRangeVector results;
        if (range.m_state != itr->m_state && range.Contains(*itr))
        {	//Consume it
            itr->m_state = range.m_state;
        }
        else if (itr->SplitMerge(range, results))
        {
            m_loaded.erase(itr);
            for(RowRangeVector::iterator itr = results.begin(); itr != results.end(); ++itr)
            {
                m_loaded.push_back(*itr);
            }
            std::sort(m_loaded.begin(), m_loaded.end());
            goto Again;
        }
        if (itr != m_loaded.begin())
        {
            if (m_loaded[i].m_state == itr->m_state)
            {
                m_loaded[i].m_to = itr->m_to;
                m_loaded.erase(itr);
                goto Again;
            }
            ++i;
        }
    }
    std::sort(m_loaded.begin(), m_loaded.end());
}

bool CPageLoadedView::IsFullyLoaded() const
{
    if (m_loaded.size() == 1)
    {
        return (m_loaded[0].m_state == RowRange::S_LOADED);
    }
    return false;
}

void CPageLoadedView::DoPaint(WTL::CDCHandle & dc, CRect rect) const
{
    DWORD win_color = GetSysColor(COLOR_3DFACE);
    DWORD page_color = GetSysColor(COLOR_WINDOW);
    DWORD requested_color = GetSysColor(COLOR_INACTIVECAPTION);
    DWORD loaded_color = GetSysColor(COLOR_ACTIVECAPTION);

    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(loaded_color, &wHue, &wLuminance, &wSaturation);
    wHue = 80;
    DWORD fully_loaded_color = ColorHLSToRGB(wHue, wLuminance, wSaturation);

    dc.DrawEdge(rect, BDR_SUNKENOUTER, BF_RECT);
    rect.DeflateRect(1, 1, 1, 1);
    dc.FillSolidRect(rect, win_color);
    RowRangeVector::const_iterator itr = m_loaded.begin(); 
    if (m_loaded.size() == 1 && itr->m_state == RowRange::S_LOADED)
    {
        dc.FillSolidRect(rect, fully_loaded_color);
    }
    else
    {
        int previous_bottom = rect.top;
        for(; itr != m_loaded.end(); ++itr)
        {
            CRect r = rect;	//For Width;
            r.top = rect.top + MulDiv(itr->m_from, rect.Height(), m_total) - 1;
            if (r.top > previous_bottom)
            {
                r.top = previous_bottom;
            }
            r.bottom = rect.top + MulDiv(itr->m_to, rect.Height(), m_total);
            switch (itr->m_state)
            {
            case RowRange::S_BLANK:
                dc.FillSolidRect(r, page_color);
                break;
            case RowRange::S_REQUESTED:
                dc.FillSolidRect(r, requested_color);
                break;
            case RowRange::S_LOADED:
                dc.FillSolidRect(r, loaded_color);
                break;
            }
            previous_bottom = r.bottom;
        }
    }
}
