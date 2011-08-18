#pragma once

#include "layoutImpl.h"
#include "GraphTypes.h"
#include "Thread.h"
#include "Canvas.h"

class CFruchtermanReingoldLayoutImpl : public CLayout, public IPositionHelper
{
private:
	double m_width;
	double m_height;

public:
	CFruchtermanReingoldLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas);
	~CFruchtermanReingoldLayoutImpl();
	void SetBoundry(double width, double height);
	LAYOUTEVENT pulse(const GraphTypes::RectF & extent);
	void GetPosition(IGraphVertex * v, double & x, double & y);
	void SetPosition(IGraphVertex * v, double x, double y);
};
