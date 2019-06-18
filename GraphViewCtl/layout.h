#pragma once

#include "clib.h"
#include "GraphContainer.h"
#include "MetaContainer.h"
#include "GraphTypes.h"

enum LAYOUTEVENT
{
    LAYOUTEVENT_UNKNOWN,
    LAYOUTEVENT_ITERATION,
    LAYOUTEVENT_ABORTED,
    LAYOUTEVENT_FINISHED
};

__interface ILayout : public IUnknown
{
    void init();		
    LAYOUTEVENT pulse(const GraphTypes::RectF & extent);		
    void halt();

    void append(IGraphItem * item, const GraphTypes::PointF & p);
    void remove(IGraphItem * item);
    void append(IGraphEdge * e);
    void remove(IGraphEdge * e);
};

