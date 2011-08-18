#pragma once

#include "layout.h"
#include "canvas.h"

class CLayout : public ILayout, public CUnknown
{
protected:
	CComPtr<IGraph> m_graph;
	CMeta & m_meta;
	CComPtr<CCanvas> m_canvas;

public:
	IMPLEMENT_CUNKNOWN;

	CLayout(IGraph * graph, CMeta & meta, CCanvas * canvas);
	~CLayout();

	virtual void init();
	virtual void append(IGraphItem * item, const GraphTypes::PointF & defaultPos);
	virtual void remove(IGraphItem * item);
	virtual void append(IGraphEdge * e);
	virtual void remove(IGraphEdge * e);
};
