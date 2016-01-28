#include "StdAfx.h"
#include "resource.h"

#include "fruchtermanreingold.h"
#include "layoutImpl.h"
#include "Renderer.h"

using namespace GraphTypes;

CFruchtermanReingoldLayoutImpl::CFruchtermanReingoldLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas) : CLayout(graph, meta, canvas)
{
    m_width = 10000;
    m_height = 10000;
}

CFruchtermanReingoldLayoutImpl::~CFruchtermanReingoldLayoutImpl()
{
}

void CFruchtermanReingoldLayoutImpl::SetBoundry(double width, double height)
{
    m_width = std::max<double>(width, 10000);
    m_height = std::max<double>(height, 10000);
}

LAYOUTEVENT CFruchtermanReingoldLayoutImpl::pulse(const GraphTypes::RectF & extent)
{
#if BOOST_VERSION >= 103300
    m_graph->fruchterman_reingold_force_directed_layout(this, m_width, m_height);
#endif
    return LAYOUTEVENT_FINISHED;
}

void CFruchtermanReingoldLayoutImpl::GetPosition(IGraphVertex * v, double & x, double & y)
{
    CComPtr<IRenderer> ir=m_canvas->GetRenderer(v);
    ATLASSERT(ir);
    GraphTypes::PointF pt;
    ir->GetPosition(pt);
    x = pt.x;
    y = pt.y;
}

void CFruchtermanReingoldLayoutImpl::SetPosition(IGraphVertex * v, double x, double y)
{
    CComPtr<IRenderer> ir=m_canvas->GetRenderer(v);
    ATLASSERT(ir);
    ir->SetPosition(GraphTypes::PointF((float)x, (float)y));
}


