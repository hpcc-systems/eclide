#include "stdafx.h"
#include "resource.h"

#include "layoutImpl.h"
#include "thread.h"

using namespace GraphTypes;

CLayout::CLayout(IGraph * graph, CMeta & meta, CCanvas * canvas) : m_graph(graph), m_meta(meta), m_canvas(canvas)
{
}

CLayout::~CLayout()
{
}

//static PointF CenterPoint(0.0f, 0.0f);

void CLayout::init()
{
}

void CLayout::append(IGraphItem * item, const PointF & p)
{
}

void CLayout::remove(IGraphItem * item)
{
}

void CLayout::append(IGraphEdge * e)
{
}

void CLayout::remove(IGraphEdge * e)
{
}

