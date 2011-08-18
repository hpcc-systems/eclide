#include "stdafx.h"

#include "timelineLayout.h"
#include "renderer.h"
#include "utilDateTime.h"
#include "TimelineCtrl.h"

using namespace boost::posix_time;
using namespace boost::gregorian;

//--------------------------------------------------------------------
//
//	comparison functors

typedef IGraphEdge* IGraphEdgePtr;
struct CompareTimes
{
	bool operator() (const IGraphEdgePtr& lhs, const IGraphEdgePtr& rhs)
	{
		time_period lspan = lhs->GetDateRange();
		ptime lbegin=lspan.begin();
		ptime lend=lspan.end();

		time_period rspan = rhs->GetDateRange();
		ptime rbegin=rspan.begin();
		ptime rend=rspan.end();

		if(lbegin<rbegin)
		{
			return true;
		}
		else if(lbegin>rbegin)
		{
			return false;
		}
		else // lbegin==rbegin
		{
			if(lend<=rend)
				return true;
			else
				return false;

		}
	}
};

struct CompareCatsLabelsTimes
{
	bool operator() (const IGraphEdgePtr& lhs, const IGraphEdgePtr& rhs)
	{
		const CCategory& lcat=lhs->GetCategory();
		const CCategory& rcat=rhs->GetCategory();

		if(lcat<rcat)
		{
			return true;
		}
		else if(!(lcat==rcat))
		{
			return false;
		}
		else // ==
		{

			const std::_tstring& lstr=lhs->GetLabel();
			const std::_tstring& rstr=rhs->GetLabel();

			if(lstr<rstr)
			{
				return true;
			}
			else if(lstr>rstr)
			{
				return false;
			}
			else // ==
			{
				time_period lspan = lhs->GetDateRange();
				ptime lbegin=lspan.begin();
				ptime lend=lspan.end();

				time_period rspan = rhs->GetDateRange();
				ptime rbegin=rspan.begin();
				ptime rend=rspan.end();

				if(lbegin<rbegin)
				{
					return true;
				}
				else if(lbegin>rbegin)
				{
					return false;
				}
				else // lbegin==rbegin
				{
					if(lend<=rend)
						return true;
					else
						return false;
				}
			}
		}
	} // operator()
};

CGraphTimelineLayoutImpl::CGraphTimelineLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas) : CLayout(graph, meta, canvas)
{
}

LAYOUTEVENT CGraphTimelineLayoutImpl::pulse(const GraphTypes::RectF & extent)
{
	ptime time1(date(2000,1,1), hours(0));
	ptime time2(date(2000,1,1), hours(0));
	time_period totalSpan(time1, time2);
	IGraphEdgeVector evec;
	evec.reserve(m_graph->GetEdgeCount());

	// calc total time span and prepare vector for sorted edges

	bool first = true;
	CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator();
	for ( ; itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IGraphEdge> e = itr->Get();
		ATLASSERT(e);
		evec.push_back(e.p);
		if (first)
		{
			first = false;
			totalSpan = e->GetDateRange();
		}
		else
		{
			totalSpan=totalSpan.span(e->GetDateRange());
#ifdef _DEBUG
			ATLASSERT(totalSpan.begin()<=e->GetDateRange().begin());
			std::string dsp=to_simple_string(e->GetDateRange());
			std::string tsp=to_simple_string(totalSpan);
#endif

		}
	}

	std::sort(evec.begin(), evec.end(), CompareCatsLabelsTimes());

	// set positions for sorted edges

	ATLASSERT(!totalSpan.is_null());
	ATLASSERT(totalSpan.begin()<=totalSpan.end());

	FILETIME ftbegin;
	FILETIME ftend;

	m_canvas->m_timeline->GetTimeRange(ftbegin, ftend);

	FILETIME ftp1;
	FILETIME ftp2;

	GraphTypes::PointF p1, p2;
	IGraphEdgeVector::iterator itrevec=evec.begin();	
	while(itrevec!=evec.end())
	{
		IGraphEdge* e = *itrevec;
		++itrevec;

		ATLASSERT(e);
		time_period edgeSpan = e->GetDateRange();

		ptime beginE=edgeSpan.begin();
		ptime endE=edgeSpan.end();
		ATLASSERT(beginE<=endE);

		PtimeToFileTime(beginE, ftp1);
		PtimeToFileTime(endE, ftp2);

		p1.x=m_canvas->ScreenPosFromTime(ftp1, ftbegin, ftend);
		p2.x=m_canvas->ScreenPosFromTime(ftp2, ftbegin, ftend);

		CComQIPtr<ILegacyRenderer> lr;
		CComQIPtr<ITimelineEdgeRenderer> ter=m_canvas->GetRenderer(e);
		if(ter)
		{
			ter->SetFromPosition(p1, GraphTypes::screen);
			ter->SetToPosition(p2, GraphTypes::screen);

			GraphTypes::RectF bbox;
			ter->GetBoundsBox(bbox);

			p1.y+=bbox.Height+16; // for no overlap margin 36
			p2.y=p1.y;

		}
	}

	return LAYOUTEVENT_FINISHED;
}

void CGraphTimelineLayoutImpl::halt()
{
}
