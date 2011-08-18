#pragma once

#include "layoutImpl.h"

#define M_PI 3.14159265358979323846f

class CGraphCircularLayoutImpl : public CLayout
{
public:
	CGraphCircularLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas) : CLayout(graph, meta, canvas)
	{
	}

	LAYOUTEVENT pulse(const GraphTypes::RectF & extent)
	{
		float gamma=0;
		int i;
		float rayon;
		float sizeTm;
		size_t verticy_count=0;
		CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, TRISTATE_TRUE, TRISTATE_FALSE);
		for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
		{
			verticy_count++;
		}

		if (verticy_count < 2)
		{
			rayon=0;
		}
		else
		{
			rayon = (500.0f * (float)verticy_count) / 64;
			gamma = (float)2 * M_PI / (float)verticy_count;
			float x1,y1,x2,y2;
			i=0;
			x1=rayon*cos(gamma*i)+rayon*2;
			y1=rayon*sin(gamma*i)+rayon*2;
			i=1;
			x2=rayon*cos(gamma*i)+rayon*2;
			y2=rayon*sin(gamma*i)+rayon*2;
			sizeTm=sqrt( (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
			sizeTm=sizeTm/2;
		}

		i = 0;
		for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
		{
			CComPtr<IRenderer> ir=m_canvas->GetRenderer(itr->Get());
			ATLASSERT(ir);
			ir->SetPosition(GraphTypes::PointF(rayon*cos(gamma*i), rayon*sin(gamma*i)));
			++i;
		}
		return LAYOUTEVENT_FINISHED;
	}

	void halt()
	{
	}
};

